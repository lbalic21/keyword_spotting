#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_timer.h"
#include "esp_log.h"
#include <string.h>
#include "audio_pipeline.h"
#include "i2s_stream.h"
#include "board.h"

#include "Application.hpp"
#include "Configuration.hpp"
#include "AudioRecorder/AudioRecorder.hpp"
#include "FeatureGenerator/FeatureGenerator.hpp"
#include "NeuralNetwork/NeuralNetwork.hpp"
#include "CommandRecognizer/CommandRecognizer.hpp"
#include "Commands/PrintCommand.hpp"
#include "Commands/BlankCommand.hpp"
#include "Commands/Command.hpp"

#include "TestSamples/testFrame.h"
#include "TestSamples/yes.h"
#include "TestSamples/no.h"
#include "TestSamples/yes_esp.h"
#include "TestSamples/up.h"
#include "TestSamples/down.h"
#include "TestSamples/happy.h"


static const char *TAG = "MAIN";

/**
 * @brief Function that tests 1 second of audio data and classifies it.
 * 
 */
void testingFacility(int16_t* audioData);

/******************************************************************************/
/********************** STATIC OBJECTS INITIALIZATION *************************/
/******************************************************************************/

static AudioRecorder audioRecorder(SAMPLE_RATE);
static FeatureGenerator featureGenerator;
static NeuralNetwork network;
static CommandRecognizer recognizer;

/******************************************************************************/
/************************** BUFFERS INITIALIZATION ****************************/
/******************************************************************************/

int16_t newSamples[STEP_SIZE];
int16_t audioFrame[WINDOW_SIZE] = {0}; 
float featureSlice[NUMBER_OF_MFCCS];
float featureImage[NUMBER_OF_FEATURES] = {0};


/******************************************************************************/
/****************************** MAIN APPLICATION ******************************/
/******************************************************************************/

void Application(void)
{
    ESP_LOGI(TAG, "Application started");

    ESP_LOGI(TAG, "Time slices in a second: %d", NUMBER_OF_TIME_SLICES);
    ESP_LOGI(TAG, "Number of MFCCs in one time slice: %d", NUMBER_OF_MFCCS);
    ESP_LOGI(TAG, "Number of features in one image: %d", NUMBER_OF_FEATURES);

    // Create and add commands
    BlankCommand command_back("BACKGROUND");
    PrintCommand command_no("NO");
    BlankCommand command_unknown("UNKNOWN");
    PrintCommand command_yes("YES");
    PrintCommand command_left("LEFT");
    PrintCommand command_right("RIGHT");
    PrintCommand command_down("DOWN");
    PrintCommand command_up("UP");
    PrintCommand command_on("ON");
    PrintCommand command_off("OFF");

    recognizer.addCommand(&command_back);
    recognizer.addCommand(&command_left);
    recognizer.addCommand(&command_no);
    recognizer.addCommand(&command_right);
    recognizer.addCommand(&command_unknown);
    recognizer.addCommand(&command_yes);

    recognizer.getNumOfCommands();

    /******************************************************************************/
    /************************** STARTING AUDIO RECORDER ***************************/
    /******************************************************************************/

    audioRecorder.set();
    audioRecorder.start();

    /******************************************************************************/
    /****************************** MAIN SYSTEM LOOP ******************************/
    /******************************************************************************/

    /* tracks the number of new feature slices before invoking the network */
    uint32_t numberOfNewSlices = 0; 

    ESP_LOGI(TAG, "Starting the main system loop");

    int64_t start, end, startLoop, endLoop;

    while(1)
    {   
        startLoop = esp_timer_get_time();
        /* static images testing, uncomment this if you want to test static audio data */
        //testingFacility(yes_esp);
        //ESP_LOGI(TAG, "LOOP");
        

        /**********************************************************************/
        /*********************** ACQUIRING NEW SAMPLES ************************/
        /**********************************************************************/

        uint32_t bytesRead = audioRecorder.getSamples(newSamples, STEP_SIZE);
        //ESP_LOGI(TAG, "Samples retrieved: %ld (%ld bytes)", bytesRead / 2, bytesRead);
        if((bytesRead / 2) < STEP_SIZE)
        {
            //ESP_LOGE(TAG, "Did not get enough samples");
            continue;
        }

        /**
        * Sliding window with overlap. Audio frame holds WINDOW_SIZE samples. Each
        * update shifts "Samples to Keep" to the start of the buffer, discards 
        * "Old Samples," and makes space at the end for "New Samples."
        *
        * Before:
        * -------------------------------------------------------------
        * | Old Samples           | Samples to Keep                   |
        * -------------------------------------------------------------
        * |<----- STEP_SIZE ----->|<---- WINDOW_SIZE - STEP_SIZE ---->|
        * -------------------------------------------------------------
        *
        * After:
        * -------------------------------------------------------------
        * | Kept Samples                      | New Samples           |
        * -------------------------------------------------------------
        * |<---- WINDOW_SIZE - STEP_SIZE ---->|<----- STEP_SIZE ----->|
        * -------------------------------------------------------------
        */
        memcpy(audioFrame, audioFrame + STEP_SIZE, (WINDOW_SIZE - STEP_SIZE) * sizeof(int16_t));
        memcpy(audioFrame + WINDOW_SIZE - STEP_SIZE, newSamples, STEP_SIZE * sizeof(int16_t));


        /**********************************************************************/
        /************************ GENERATING FEATURES *************************/
        /**********************************************************************/
    
        start = esp_timer_get_time();
        bool success = featureGenerator.generateFeatures(audioFrame, featureSlice); 
        end = esp_timer_get_time();
        //printf("Time taken for feature generation: %lld us\n", (end - start));

        if(!success)
        {
            ESP_LOGE(TAG, "Generating feature failed");
            continue;
        }


        /**
         * The same process, as was shown earlier with raw audio samples, occurs
         * here. One time slice that has NUMBER_OF_MFCCS features is copied to the
         * begginning of the buffer and a new feature slice is added to the end of
         * the buffer completing the whole feature image that can be fed to the
         * neural network.
         */

        memcpy(featureImage, featureImage + NUMBER_OF_MFCCS, sizeof(float) * (NUMBER_OF_FEATURES - NUMBER_OF_MFCCS));
        memcpy(featureImage + NUMBER_OF_FEATURES - NUMBER_OF_MFCCS, featureSlice, sizeof(float) * NUMBER_OF_MFCCS);

        numberOfNewSlices++;


        /**********************************************************************/
        /******************** INVOKING THE NEURAL NETWORK *********************/
        /**********************************************************************/

        if(numberOfNewSlices == NUMBER_OF_NEW_SLICES_BEFORE_INVOKING)
        {
            //ESP_LOGI(TAG, "Invoking NN!");
            start = esp_timer_get_time();
            network.giveFeaturesToModel(featureImage, NUMBER_OF_FEATURES);
            success = network.invoke();
            end = esp_timer_get_time();
            //printf("Time taken for invoking: %lld us\n", (end - start));
            if(!success)
            {
                ESP_LOGE(TAG, "Model invoking failed");
                continue;
            }
            numberOfNewSlices = 0;

            /*******************************************************************/
            /*********************** RECOGNIZING COMMANDS **********************/
            /*******************************************************************/

            recognizer.recognize(network.outputData);
        }
    
        endLoop = esp_timer_get_time();
        //printf("Time taken for loop: %lld us\n", (endLoop - startLoop));
        vTaskDelay(1);
    }
}

void testingFacility(int16_t* audioData)
{
    uint32_t pointerInTestData = 0;
    uint32_t loopTracker = 0;
    uint32_t pointer = 0;

    while(1)
    {
        if(loopTracker == 0)
        {
            memcpy(audioFrame, audioData, WINDOW_SIZE * 2);
            pointerInTestData += WINDOW_SIZE;
        }
        else
        {
            memcpy(audioFrame, audioFrame + STEP_SIZE, (WINDOW_SIZE - STEP_SIZE) * 2);
            memcpy(audioFrame + WINDOW_SIZE - STEP_SIZE, audioData + pointerInTestData , STEP_SIZE * 2);
            pointerInTestData += STEP_SIZE;
        }
        featureGenerator.generateFeatures(audioFrame, featureSlice); 
        for(int i = 0; i < NUMBER_OF_MFCCS; i++)
        {
            featureImage[pointer++] = featureSlice[i];
        }
        loopTracker++;
        //printf("Loop tracker %ld\n", loopTracker);
        //printf("Pointer %ld", pointer);
        if(loopTracker == NUMBER_OF_TIME_SLICES)
        {
            network.giveFeaturesToModel(featureImage, NUMBER_OF_FEATURES);
            bool success = network.invoke();
            if(!success)
            {
                ESP_LOGE(TAG, "Model invoking failed");
                continue;
            }
            else{
                ESP_LOGI(TAG, "Invoke worked!");
            }
            for(int c = 0; c < network.numberOfClasses; c++)
            {
                printf("%d %f\n", c, network.outputData[c]);
            }
            
            while(1)
            {
                vTaskDelay(100);
            }
        }
    }
}
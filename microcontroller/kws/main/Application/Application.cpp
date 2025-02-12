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
 * @brief Function that tests 1 second of audio data and classifies it
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
    /* static images testing, uncomment this if you want to test static audio data */
    //testingFacility(no);

    ESP_LOGI(TAG, "Application started");
    ESP_LOGI(TAG, "Time slices in a second: %d", NUMBER_OF_TIME_SLICES);
    ESP_LOGI(TAG, "Number of MFCCs in one time slice: %d", NUMBER_OF_MFCCS);
    ESP_LOGI(TAG, "Number of features in one image: %d", NUMBER_OF_FEATURES);

    /**********************************************************************/
    /****************************** COMMANDS ******************************/
    /**********************************************************************/

    BlankCommand command_back("BACKGROUND", 1, 0.7);
    PrintCommand command_left("LEFT", 5, 0.8);
    PrintCommand command_no("NO", 3, 0.80);
    PrintCommand command_right("RIGHT", 3, 0.85);
    BlankCommand command_unknown("UNKNOWN", 1, 0.7);
    PrintCommand command_yes("YES", 5, 0.85);
    PrintCommand command_zero("ZERO", 3, 0.85);

    recognizer.addCommand(&command_back);
    recognizer.addCommand(&command_left);
    recognizer.addCommand(&command_no);
    recognizer.addCommand(&command_right);
    recognizer.addCommand(&command_unknown);
    recognizer.addCommand(&command_yes);
    recognizer.addCommand(&command_zero);

    recognizer.getNumOfCommands();
  
    /******************************************************************************/
    /****************************** MAIN SYSTEM LOOP ******************************/
    /******************************************************************************/

    uint32_t numberOfNewSlices = 0;   /* tracks the number of new feature slices before invoking the network */ 
    //int u =  0;                       /* not enough samples counter, for testing purposes */
    while(1)
    {   
        //int64_t startLoop = esp_timer_get_time();
        
        /**********************************************************************/
        /*********************** ACQUIRING NEW SAMPLES ************************/
        /**********************************************************************/

        //int64_t startGetSamples = esp_timer_get_time();
        uint32_t bytesRead = audioRecorder.getSamples(newSamples, STEP_SIZE);
        //int64_t endGetSamples = esp_timer_get_time();
        //printf("GetSamples-> (start:%lld, end: %lld, time: %lld) us\n", startGetSamples, endGetSamples, (endGetSamples - startGetSamples));
        //ESP_LOGI(TAG, "Samples retrieved: %ld (%ld bytes)", bytesRead / 2, bytesRead);
        if((bytesRead / 2) < STEP_SIZE)
        {
            //printf("Did not get enough samples %d\n", u++);
            vTaskDelay(1);
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
    
        //int64_t startGen = esp_timer_get_time();
        bool success = featureGenerator.generateFeatures(audioFrame, featureSlice); 
        //int64_t endGen = esp_timer_get_time();
        //printf("FeatureGeneration-> (start:%lld, end: %lld, time: %lld) us\n", startGen, endGen, (endGen - startGen));
        

        if(!success)
        {
            ESP_LOGE(TAG, "Generating feature failed");
            vTaskDelay(1);
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
            //int64_t startNN = esp_timer_get_time();
            network.giveFeaturesToModel(featureImage, NUMBER_OF_FEATURES);
            success = network.invoke();
            //int64_t endNN = esp_timer_get_time();
            //printf("NN Invoke-> (start:%lld, end: %lld, time: %lld) us\n", startNN, endNN, (endNN - startNN));
        
            if(!success)
            {
                ESP_LOGE(TAG, "Model invoking failed");
                vTaskDelay(1);
                continue;
            }
            numberOfNewSlices = 0;

            /*******************************************************************/
            /*********************** RECOGNIZING COMMANDS **********************/
            /*******************************************************************/

            //int64_t startRec = esp_timer_get_time();
            recognizer.recognize(network.outputData);
            //int64_t endRec = esp_timer_get_time();
            //printf("Recognize-> (start:%lld, end: %lld, time: %lld) us\n", startRec, endRec, (endRec - startRec));
        }
    
        //int64_t endLoop = esp_timer_get_time();
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
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
#include "NeuralNetwork/NeuralNetwork.hpp"
#include "CommandRecognizer/CommandRecognizer.hpp"
#include "CommandResponder/CommandResponder.hpp"

#if USE_FLOAT == 1
#include "FeatureGeneratorFloat/FeatureGeneratorFloat.hpp"
#include "FeatureGeneratorFloat/WindowFloat.hpp"
#include "FeatureGeneratorFloat/FFTFloat.hpp"
#include "FeatureGeneratorFloat/MelSpectrogramFloat.hpp"
#include "FeatureGeneratorFloat/DCT.hpp"
#else
#include "FeatureGenerator/FeatureGenerator.hpp"
#include "FeatureGenerator/Window.hpp"
#include "FeatureGenerator/FFT.hpp"
#include "FeatureGenerator/MelSpectrogram.hpp"
#endif

#include "testFrame.h"
#include "yes.h"
#include "no.h"


static const char *TAG = "MAIN";

/******************************************************************************/
/********************** STATIC OBJECTS INITIALIZATION *************************/
/******************************************************************************/

static AudioRecorder audioRecorder(SAMPLE_RATE);

#if USE_FLOAT == 1
static WindowFloat hannWindow;
static FFTFloat fft;
static MelSpectrogramFloat melSpectrogram;
static DCT dct;
static FeatureGeneratorFloat featureGenerator(&hannWindow, &fft, &melSpectrogram, &dct);
#else
static Window hannWindow;
static FFT fft;
static MelSpectrogram melSpectrogram;
static FeatureGenerator featureGenerator(&hannWindow, &fft, &melSpectrogram);
#endif

static NeuralNetwork network;
//static CommandRecognizer recognizer;
//static CommandResponder responder;


/******************************************************************************/
/****************************** MAIN APPLICATION ******************************/
/******************************************************************************/

void Application(void)
{
    ESP_LOGI(TAG, "Application started");

    ESP_LOGI(TAG, "Time slices in a second: %d", NUMBER_OF_TIME_SLICES);
    ESP_LOGI(TAG, "Number of MFCCs in one time slice: %d", NUMBER_OF_MFCCS);
    ESP_LOGI(TAG, "Number of features in one image: %d", NUMBER_OF_FEATURES);

    /******************************************************************************/
    /************************** BUFFERS INITIALIZATION ****************************/
    /******************************************************************************/

    int16_t newSamples[STEP_SIZE];
    int16_t audioFrame[WINDOW_SIZE] = {0}; 

    int8_t featureSlice[NUMBER_OF_MFCCS];
    int8_t featureImage[NUMBER_OF_FEATURES] = {0};

    uint32_t pointerInTestData = 0;
    uint32_t loopTracker = 0;


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

    while(1)
    {
        while(1){

            if(loopTracker == 0)
            {
                memcpy(audioFrame, yes, WINDOW_SIZE * 2);
                pointerInTestData += WINDOW_SIZE;
            }
            else
            {
                memcpy(audioFrame, audioFrame + STEP_SIZE, (WINDOW_SIZE - STEP_SIZE) * 2);
                memcpy(audioFrame + WINDOW_SIZE - STEP_SIZE, yes + pointerInTestData , STEP_SIZE * 2);
                pointerInTestData += STEP_SIZE;
            }

            featureGenerator.generateFeatures(audioFrame, featureSlice); 

            memcpy(featureImage, featureImage + NUMBER_OF_MFCCS, NUMBER_OF_FEATURES - NUMBER_OF_MFCCS);
            memcpy(featureImage + NUMBER_OF_FEATURES - NUMBER_OF_MFCCS, featureSlice, NUMBER_OF_MFCCS);

            loopTracker++;

            //printf("Loop tracker %ld\n", loopTracker);
            //printf("Pointer %ld", pointerInTestData);

            if(loopTracker == NUMBER_OF_TIME_SLICES)
            {
                network.giveFeaturesToModel(featureImage, NUMBER_OF_FEATURES);
                bool success = network.invoke(featureImage);
                if(!success)
                {
                    ESP_LOGE(TAG, "Model invoking failed");
                    continue;
                }
                network.printOutput();

                while(1)
                {
                    vTaskDelay(100);
                }
            }

        }

        //ESP_LOGI(TAG, "LOOP");
        int64_t start = esp_timer_get_time();

        /**********************************************************************/
        /*********************** ACQUIRING NEW SAMPLES ************************/
        /**********************************************************************/

        uint32_t bytesRead = audioRecorder.getSamples(newSamples, STEP_SIZE);
        //ESP_LOGI(TAG, "Samples retrieved: %ld (%ld bytes)", bytesRead / 2, bytesRead);
        if((bytesRead / 2) < STEP_SIZE)
        {
            ESP_LOGE(TAG, "Did not get enough samples");
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
        memcpy(audioFrame, audioFrame + STEP_SIZE, (WINDOW_SIZE - STEP_SIZE) * 2);
        memcpy(audioFrame + WINDOW_SIZE - STEP_SIZE, newSamples, STEP_SIZE * 2);


        /**********************************************************************/
        /************************ GENERATING FEATURES *************************/
        /**********************************************************************/
    
        int64_t start1 = esp_timer_get_time();
        bool success = featureGenerator.generateFeatures(testFrame, featureSlice); 
        int64_t end1 = esp_timer_get_time();
        //printf("Time taken for feature generation: %lld us\n", (end1 - start1));

        if(!success)
        {
            ESP_LOGE(TAG, "Generating feature failed");
            continue;
        }

        while(1)
        {
            vTaskDelay(100);
        }

        /**
         * The same process, as was shown earlier with raw audio samples, occurs
         * here. One time slice that has NUMBER_OF_MFCCS features is copied to the
         * begginning of the buffer and a new feature slice is added to the end of
         * the buffer completing the whole feature image that can be fed to the
         * neural network.
         */

        memcpy(featureImage, featureImage + NUMBER_OF_MFCCS, NUMBER_OF_FEATURES - NUMBER_OF_MFCCS);
        memcpy(featureImage + NUMBER_OF_FEATURES - NUMBER_OF_MFCCS, featureSlice, NUMBER_OF_MFCCS);

        numberOfNewSlices++;


        /**********************************************************************/
        /******************** INVOKING THE NEURAL NETWORK *********************/
        /**********************************************************************/

        if(numberOfNewSlices == NUMBER_OF_NEW_SLICES_BEFORE_INVOKING)
        {
            ESP_LOGI(TAG, "Invoking NN!");
            network.giveFeaturesToModel(featureImage, NUMBER_OF_FEATURES);
            success = network.invoke(featureImage);
            if(!success)
            {
                ESP_LOGE(TAG, "Model invoking failed");
                continue;
            }
            network.printOutput();
            numberOfNewSlices = 0;
        }


        /**********************************************************************/
        /*********************** RECOGNIZING COMMANDS *************************/
        /**********************************************************************/
        



        /**********************************************************************/
        /********************** RESPONDING TO A COMMAND ***********************/
        /**********************************************************************/
        

        int64_t end = esp_timer_get_time();
        printf("Time taken for loop: %lld us\n", (end - start));
        vTaskDelay(1);
    }
}
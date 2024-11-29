#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
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
#else
#include "FeatureGenerator/FeatureGenerator.hpp"
#include "FeatureGenerator/Window.hpp"
#include "FeatureGenerator/FFT.hpp"
#include "FeatureGenerator/MelSpectrogram.hpp"
#endif

int16_t testSamples[512] = {
    -6927, -6556, -6013, -5513, -4946, -4403, -3912, -3422,
    -2936, -2364, -1688, -875,  102,   1214,  2310,  3328,
    4240,  4946,  5400,  5736,  6083,  6518,  7011,  7794,
    8734,  9637,  10523, 11208, 11599, 11648, 11538, 11343,
    11122, 11143, 11391, 11834, 12297, 12747, 12994, 12876,
    12428, 11705, 10913, 10098, 9504,  9161,  9100,  9222,
    9333,  9370,  9113,  8594,  7842,  6900,  5956,  5146,
    4522,  4067,  3786,  3558,  3224,  2729,  2050,  1213,
    256,   -674,  -1378, -2045, -2499, -2783, -3078, -3451,
    -3940, -4508, -5239, -5943, -6605, -7099, -7460, -7692,
    -7746, -7852, -7913, -8085, -8310, -8544, -8776, -8885,
    -8912, -8792, -8588, -8342, -8083, -7886, -7730, -7589,
    -7468, -7231, -6965, -6598, -6155, -5738, -5355, -5017,
    -4756, -4639, -4422, -4231, -3902, -3448, -2941, -2309,
    -1731, -1195, -815,  -550,  -393,  -241,  -96,   117,
    475,   926,   1441,  1962,  2413,  2755,  3022,  3145,
    3240,  3342,  3501,  3713,  4026,  4411,  4771,  5128,
    5350,  5469,  5498,  5383,  5268,  5152,  5007,  5011,
    5038,  5094,  5177,  5244,  5243,  5099,  4897,  4631,
    4273,  3903,  3550,  3248,  2963,  2734,  2569,  2344,
    2074,  1770,  1394,  938,   478,   79,    -266,  -549,
    -682,  -741,  -805,  -825,  -875,  -1013, -1215, -1458,
    -1711, -1893, -2027, -2061, -1874, -1661, -1389, -1067,
    -790,  -527,  -409,  -319,  -200,  -111,  18,    234,
    536,   913,   1320,  1701,  2070,  2395,  2645,  2842,
    3053,  3190,  3340,  3571,  3803,  4038,  4325,  4633,
    4899,  5155,  5354,  5489,  5561,  5524,  5445,  5376,
    5311,  5226,  5221,  5262,  5284,  5298,  5251,  5136,
    4908,  4597,  4200,  3818,  3401,  3068,  2820,  2593,
    2402,  2216,  1938,  1617,  1193,  689,   255,   -238,
    -626,  -926,  -1105, -1241, -1422, -1534, -1733, -2001,
    -2339, -2686, -3016, -3307, -3542, -3688, -3740, -3813,
    -3808, -3888, -3972, -4092, -4322, -4492, -4744, -4949,
    -5146, -5341, -5449, -5549, -5648, -5719, -5734, -5837,
    -5944, -6085, -6318, -6478, -6725, -6887, -7000, -7078,
    -7078, -7092, -7096, -7066, -7160, -7342, -7483, -7770,
    -8030, -8191, -8289, -8332, -8240, -8071, -7947, -7938,
    -7934, -8055, -8382, -8645, -8832, -8989, -9054, -8771,
    -8474, -8122, -7667, -7185, -6836, -6514, -6093, -5745,
    -5323, -4775, -4156, -3516, -2772, -2065, -1306, -557,
    355,   1320,  2327,  3538,  4661,  5747,  6697,  7523,
    8066,  8515,  8906,  9309,  9904,  10548, 11397, 12258,
    12980, 13510, 13667, 13592, 13297, 12846, 12478, 12284,
    12309, 12504, 12801, 12988, 13022, 12666, 11982, 11153,
    10174, 9306,  8617,  8264,  8110,  8101,  8088,  7982,
    7556,  6879,  6049,  5031,  4103,  3252,  2675,  2246,
    1944,  1670,  1226,  658,   -69,   -996,  -2008, -2856,
    -3677, -4288, -4728, -5083, -5390, -5799, -6259, -6861,
    -7473, -8055, -8577, -8956, -9081, -9117, -9135, -8964,
    -8889, -8877, -8825, -8819, -8747, -8571, -8297, -7899,
    -7451, -6989, -6496, -6166, -5789, -5411, -5139, -4704,
    -4250, -3747, -3201, -2719, -2257, -1947, -1708, -1516,
    -1377, -1166, -855,  -493,  -32,   466,   876,   1193,
    1424,  1504,  1514,  1528,  1575,  1711,  1921,  2207,
    2500,  2756,  2894,  2914,  2868,  2728,  2571,  2458,
    2427,  2492,  2547,  2692,  2776,  2799,  2687,  2434,
    2108,  1660,  1241,  812,   519,   344,   258,   270,
    327,   357,   309,   182,   -35,   -328,  -689,  -1026,
    -1265, -1437, -1442, -1339, -1222, -1037, -920,  -881,
    -928,  -1044, -1193, -1273, -1296, -1180, -936,  -650,
    -264,  79,    317,   474,   510,   458,   422,   413,
    479,   708,   1027,  1440,  1901,  2328,  2773,  3103,
    3332,  3538,  3684,  3806,  3978,  4191,  4473,  4794,
    5160,  5516,  5854,  6119,  6335,  6533,  6636,  6724
};

static const char *TAG = "MAIN";

/******************************************************************************/
/********************** STATIC OBJECTS INITIALIZATION *************************/
/******************************************************************************/

static AudioRecorder audioRecorder(SAMPLE_RATE);

#if USE_FLOAT == 1
static WindowFloat hannWindow;
static FFTFloat fft;
static MelSpectrogramFloat melSpectrogram;
static FeatureGeneratorFloat featureGenerator(&hannWindow, &fft, &melSpectrogram);
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

    #if USE_FLOAT == 1
    float featureSlice[NUMBER_OF_MFCCS];
    float featureImage[NUMBER_OF_FEATURES] = {0};
    #else
    int8_t featureSlice[NUMBER_OF_MFCCS];
    int8_t featureImage[NUMBER_OF_FEATURES] = {0};
    #endif


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
        ESP_LOGI(TAG, "LOOP");

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

        bool success = featureGenerator.generateFeatures(audioFrame, featureSlice);
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

        memcpy(featureImage, featureImage + NUMBER_OF_MFCCS, NUMBER_OF_FEATURES - NUMBER_OF_MFCCS);
        memcpy(featureImage + NUMBER_OF_FEATURES - NUMBER_OF_MFCCS, featureSlice, NUMBER_OF_MFCCS);

        numberOfNewSlices++;


        /**********************************************************************/
        /******************** INVOKING THE NEURAL NETWORK *********************/
        /**********************************************************************/

        if(numberOfNewSlices == NUMBER_OF_NEW_SLICES_BEFORE_INVOKING)
        {
            ESP_LOGI(TAG, "NN invoked!");
            success = network.invoke(featureImage);
            if(!success)
            {
                ESP_LOGE(TAG, "Model invoking failed");
                continue;
            }
            numberOfNewSlices = 0;
        }


        /**********************************************************************/
        /*********************** RECOGNIZING COMMANDS *************************/
        /**********************************************************************/




        /**********************************************************************/
        /********************** RESPONDING TO A COMMAND ***********************/
        /**********************************************************************/
        


        vTaskDelay(1);
    }
}
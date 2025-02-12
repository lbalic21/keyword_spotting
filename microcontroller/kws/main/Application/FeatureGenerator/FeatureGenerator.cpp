#include "FeatureGenerator.hpp"

static const char *TAG = "Feature Generator";

bool FeatureGenerator::generateFeatures(int16_t* audioFrame, float* featureSlice)
{
    /*****************************************************************/
    /*************************** WINDOW ******************************/
    /*****************************************************************/

    window.apply(audioFrame, this->audioWindow);
    
    /*****************************************************************/
    /*************************** FFT *********************************/
    /*****************************************************************/

    fft.compute(this->audioWindow, this->spectrogram);

    //for(int i = 0; i < NUMBER_OF_SPECTROGRAM_BINS; i++)
    ////{
    ////    printf("FFT[%d] = %f\n", i, spectrogram[i]);
    //}

    /*****************************************************************/
    /*********************** MEL SPECTRO *****************************/
    /*****************************************************************/

    melSpectrogram.generate(this->spectrogram, this->melSpectro);

    //for(size_t i = 0; i < NUMBER_OF_MEL_BINS; i++)
    //{
    //    printf("MEL[%d] = %f\n", i, melSpectro[i]);
    //}

    /*****************************************************************/
    /*************************** DCT *********************************/
    /*****************************************************************/

    dct.compute(this->melSpectro, featureSlice);

    return true;
}
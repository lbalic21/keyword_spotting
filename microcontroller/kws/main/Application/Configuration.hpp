#ifndef _CONFIGURATION_H_
#define _CONFIGURATION_H_

/* Application configuration */

#define SAMPLE_RATE                 (16000)
#define WINDOW_SIZE                 (512)
#define STEP_SIZE                   (256)
#define WINDOW_BITS                 (12)
#define NUMBER_OF_TIME_SLICES       ((int)((SAMPLE_RATE - WINDOW_SIZE) / STEP_SIZE) + 1)
#define NUMBER_OF_SPECTROGRAM_BINS  ((WINDOW_SIZE / 2) + 1)
#define NUMBER_OF_MEL_BINS          (40)
#define NUMBER_OF_MFCCS             (13)
#define MAX_INFERENCES_PER_SECOND   (55)

#endif /* _CONFIGURATION_H_ */
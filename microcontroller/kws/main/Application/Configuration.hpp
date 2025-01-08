#ifndef _CONFIGURATION_H_
#define _CONFIGURATION_H_

/* Application configuration */

#define USE_FLOAT                               (1)
#define SAMPLE_RATE                             (16000)
#define WINDOW_SIZE                             (512)
#define STEP_SIZE                               (256)
#define WINDOW_BITS                             (12)
#define NUMBER_OF_TIME_SLICES                   ((int)((SAMPLE_RATE - WINDOW_SIZE) / STEP_SIZE) + 1)
#define NUMBER_OF_SPECTROGRAM_BINS              ((WINDOW_SIZE / 2) + 1)
#define NUMBER_OF_MEL_BINS                      (40)
#define LOWER_BAND_LIMIT                        (80.0)
#define UPPER_BAND_LIMIT                        (7600.0)
#define MEL_BREAK_FREQUENCY_HERTZ               (700.0)
#define MEL_HIGH_FREQUENCY_Q                    (1127.0)
#define NUMBER_OF_MFCCS                         (13)
#define NUMBER_OF_FEATURES                      (NUMBER_OF_TIME_SLICES * NUMBER_OF_MFCCS)
#define NUMBER_OF_NEW_SLICES_BEFORE_INVOKING    (10)

#endif /* _CONFIGURATION_H_ */
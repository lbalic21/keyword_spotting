#ifndef _CONFIGURATION_H_
#define _CONFIGURATION_H_

/* Application configuration */

#define SAMPLE_RATE                             (16000)
#define WINDOW_SIZE                             (512)
#define STEP_SIZE                               (384)
#define NUMBER_OF_TIME_SLICES                   ((int)((SAMPLE_RATE - WINDOW_SIZE) / STEP_SIZE) + 1)
#define NUMBER_OF_SPECTROGRAM_BINS              ((WINDOW_SIZE / 2) + 1)
#define NUMBER_OF_MEL_BINS                      (40)
#define LOWER_BAND_LIMIT                        (80.0)
#define UPPER_BAND_LIMIT                        (7600.0)
#define NUMBER_OF_MFCCS                         (12)
#define NUMBER_OF_FEATURES                      (NUMBER_OF_TIME_SLICES * NUMBER_OF_MFCCS)
#define NUMBER_OF_NEW_SLICES_BEFORE_INVOKING    (5)
#define ACTIVATION_THRESHOLD                    (0.70)
#define COOL_DOWN_PERIOD_MS                     (1500)
#define MAX_COMMANDS                            (10)

#endif /* _CONFIGURATION_H_ */
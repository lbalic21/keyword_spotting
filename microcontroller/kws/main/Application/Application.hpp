#ifndef _Application_
#define _Application_

#ifdef __cplusplus
extern "C" {
#endif

/* Application configuration */

#define SAMPLE_RATE                 (16000)
#define WINDOW_SIZE                 (512)
#define STEP_SIZE                   (256)
#define NUMBER_OF_SPECTROGRAM_BINS  ((WINDOW_SIZE / 2) + 1)
#define NUMBER_OF_MEL_BINS          (40)
#define NUMBER_OF_MFCCS             (13)
#define MAX_INFERENCES_PER_SECOND   (30)


void Application(void);

#ifdef __cplusplus
}
#endif

#endif  /* _Application_ */
#ifndef _Application_
#define _Application_

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Main function
 * 
 */
void Application(void);

/**
 * @brief Function that tests 1 second of audio data and classifies it
 * 
 */
void testingFacility(int16_t* audioData);

#ifdef __cplusplus
}
#endif

#endif  /* _Application_ */
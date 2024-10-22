#ifndef _Audio_Recorder_
#define _Audio_Recorder_

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include <string.h>
#include "audio_pipeline.h"
#include "i2s_stream.h"
#include "board.h"
#include "freertos/ringbuf.h"

class AudioRecorder
{
    private:
        uint32_t sampleRate;
        RingbufHandle_t ringBuffer;
        TaskHandle_t captureAudioHandle;

        static void captureAudioTask(void* pvParameters);

    public:
        AudioRecorder(uint32_t sampleRate) : sampleRate(sampleRate), ringBuffer(NULL), captureAudioHandle(NULL) {}
        ~AudioRecorder();
        void set(void);
        void start(void);
        uint32_t getSamples(int16_t* samples, uint32_t numOfSamples);
};

#endif /* _Audio_Recorder_ */

#ifndef _Audio_Recorder_
#define _Audio_Recorder_

#ifdef __cplusplus
extern "C" {
#endif

#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "audio_pipeline.h"
#include "i2s_stream.h"
#include "board.h"
#include "freertos/ringbuf.h"

class AudioRecorder
{
    private:
        audio_element_handle_t i2s_stream_reader;
        RingbufHandle_t ringBuffer;
        TaskHandle_t captureAudioHandle;
        
        void captureAudioTask();

    public:
        AudioRecorder();
        ~AudioRecorder();
        uint32_t getSamples(int16_t* samples, uint32_t numOfSamples);
};



#ifdef __cplusplus
}
#endif

#endif /* _Audio_Recorder_ */
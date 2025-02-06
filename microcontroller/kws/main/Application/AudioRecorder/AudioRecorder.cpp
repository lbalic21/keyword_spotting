#include "AudioRecorder.hpp"

static const char *TAG = "Audio Recorder";

AudioRecorder::AudioRecorder(uint32_t sampleRate)
{
    this->sampleRate = sampleRate;

    
    ESP_LOGI(TAG, "[1] Initializing ring buffer");
    ringBuffer = xRingbufferCreate(1024 * 16, RINGBUF_TYPE_BYTEBUF);
    if (ringBuffer == NULL)
    {
        ESP_LOGE(TAG, "Creating ring buffer failed");
        while(1);
    }

    BaseType_t result = xTaskCreate(
        captureAudioTask,            // Function pointer
        "CaptureAudioTask",          // Task name
        1024 * 4,                   // Stack size (32 KB)
        this,                        // Parameters passed to the task
        10,                          // Task priority
        &(this->captureAudioHandle));   // Handle for the created task

    if(result != pdPASS) 
    {
        ESP_LOGE(TAG, "Creating audio capturing task failed");
        while(1);
    }
}

void AudioRecorder::captureAudioTask(void* pvParameters)
{
    ESP_LOGI(TAG, "Task is running on core: %d\n", xPortGetCoreID());
    AudioRecorder* recorder = static_cast<AudioRecorder*>(pvParameters); 

    audio_board_handle_t board_handle = audio_board_init();
    audio_hal_ctrl_codec(board_handle->audio_hal, AUDIO_HAL_CODEC_MODE_ENCODE, AUDIO_HAL_CTRL_START);

    ESP_LOGI(TAG, "Creating i2s stream to read audio data from codec chip");
    i2s_config_t i2s = {
        .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_RX),
        .sample_rate = (uint32_t)recorder->sampleRate,
        .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,
        .channel_format = I2S_CHANNEL_FMT_ONLY_LEFT,
        .communication_format = I2S_COMM_FORMAT_STAND_I2S,
        .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
        .dma_buf_count = 3,
        .dma_buf_len = 256 * sizeof(int16_t),
        .use_apll = false,
        .tx_desc_auto_clear = false, 
    };
    audio_element_handle_t i2s_stream_reader;
    i2s_stream_cfg_t i2s_cfg = I2S_STREAM_CFG_DEFAULT();
    i2s_cfg.type = AUDIO_STREAM_READER;
    i2s_cfg.i2s_port = I2S_NUM_0;
    i2s_cfg.i2s_config = i2s;
    i2s_stream_reader = i2s_stream_init(&i2s_cfg);

    ESP_LOGI(TAG, "Creating audio pipeline");
    audio_pipeline_handle_t pipeline;
    audio_pipeline_cfg_t pipeline_cfg = DEFAULT_AUDIO_PIPELINE_CONFIG();
    pipeline = audio_pipeline_init(&pipeline_cfg);
    mem_assert(pipeline);
    
    audio_pipeline_register(pipeline, i2s_stream_reader, "i2s");
    audio_pipeline_run(pipeline);
    
    ESP_LOGI(TAG, "[4] Storing data into the ring buffer indefinitely");
    int16_t *buffer = (int16_t*)malloc(512);
    
    while (1) {
        int read_len = audio_element_input(i2s_stream_reader, (char *)buffer, 512);
        //for(int i = 0; i < read_len/2; i++)
        //{
        //    printf("%d - %d\n", i, buffer[i]);
        //}
        if (read_len > 0) {
            xRingbufferSend(recorder->ringBuffer, buffer, read_len, portMAX_DELAY);
        }
    }
}

uint32_t AudioRecorder::getSamples(int16_t* samples, size_t numOfSamples)
{
    size_t bytesNeeded = numOfSamples * 2;

    size_t bytesInTheBuffer;
    vRingbufferGetInfo(this->ringBuffer, NULL, NULL, NULL, NULL, &bytesInTheBuffer);
    if(bytesInTheBuffer < bytesNeeded)
    {
        //ESP_LOGW(TAG, "Not enough data in the ring buffer, only %d bytes", bytesInTheBuffer);
        return 0;
    }
    //printf("In the buffer: %d bytes\n", bytesInTheBuffer);

    // Retrieve the data from the ring buffer
    size_t bytesRetrieved;
    uint16_t* data = NULL;
    data = (uint16_t*)xRingbufferReceiveUpTo(this->ringBuffer, &bytesRetrieved, portMAX_DELAY, bytesNeeded);
    if(data != NULL) 
    {
        memcpy(samples, data, bytesRetrieved / 2);
        vRingbufferReturnItem(this->ringBuffer, (void*)data);

        //if wraparound happened
        if(bytesRetrieved < bytesNeeded)
        {
            samples += bytesRetrieved / 2;

            size_t newBytesRetrieved;
            data = NULL;
            data = (uint16_t*)xRingbufferReceiveUpTo(this->ringBuffer, &newBytesRetrieved, portMAX_DELAY, bytesNeeded - bytesRetrieved);
            if(data == NULL)
            {
                ESP_LOGE(TAG, "Data is NULL");
            }

            memcpy(samples, data, newBytesRetrieved / 2);
            vRingbufferReturnItem(this->ringBuffer, (void*)data);

            if((bytesRetrieved + newBytesRetrieved) != bytesNeeded)
            {
                ESP_LOGE(TAG, "Not all data retrieved - (%d + %d) != %d", bytesRetrieved, newBytesRetrieved, bytesNeeded);
                while(1);
            }
            return bytesRetrieved + newBytesRetrieved;
        }

        if(bytesRetrieved != bytesNeeded)
        {
            ESP_LOGE(TAG, "Not all data retrieved - %d != %d", bytesRetrieved, bytesNeeded);
            while(1);
        }
        return bytesRetrieved;
    }
    return 0; 
}
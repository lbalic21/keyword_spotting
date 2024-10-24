#include "AudioRecorder.hpp"

static const char *TAG = "Audio Recorder";

void AudioRecorder::set(void)
{
    ESP_LOGI(TAG, "Starting codec chip");
    audio_board_handle_t board_handle = audio_board_init();
    audio_hal_ctrl_codec(board_handle->audio_hal, AUDIO_HAL_CODEC_MODE_ENCODE, AUDIO_HAL_CTRL_START);
    
    ESP_LOGI(TAG, "Creating i2s stream to read audio data from codec chip");
    i2s_config_t i2s = {
        .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_RX),
        .sample_rate = (uint32_t)this->sampleRate,
        .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT, // change to 16 bits per sample
        .channel_format = I2S_CHANNEL_FMT_ONLY_RIGHT, // Although the microphone is mono, the I2S data is 2-channel
        .communication_format = I2S_COMM_FORMAT_STAND_I2S,
        .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1, // Default interrupt priority
        .dma_buf_count = 3,
        .dma_buf_len = 300,
        .use_apll = false,
        .tx_desc_auto_clear = false, // Auto clear tx descriptor on underflow
    };
     
    audio_element_handle_t i2s_stream_reader;
    i2s_stream_cfg_t i2s_cfg = I2S_STREAM_CFG_DEFAULT();
    i2s_cfg.type = AUDIO_STREAM_READER;
    i2s_cfg.i2s_port = I2S_NUM_0;
    i2s_cfg.i2s_config = i2s;
    i2s_stream_reader = i2s_stream_init(&i2s_cfg);
}

void AudioRecorder::start(void)
{
    ringBuffer = xRingbufferCreate(1024 * 16, RINGBUF_TYPE_BYTEBUF);
    if (ringBuffer == NULL) {
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

static uint32_t samplesSum = 0;

void AudioRecorder::captureAudioTask(void* pvParameters)
{
    ESP_LOGI(TAG, "Task is running on core: %d\n", xPortGetCoreID());
    size_t wantedNumOfBytes = 512;
    AudioRecorder* recorder = static_cast<AudioRecorder*>(pvParameters); 
    int16_t i2sReadBuffer[256];
    size_t bytesRead;

    while (1) {
        //UBaseType_t stackHighWaterMark = uxTaskGetStackHighWaterMark(NULL);
        //ESP_LOGI(TAG, "Stack high watermark: %d", stackHighWaterMark);

        i2s_read(I2S_NUM_0, i2sReadBuffer, sizeof(i2sReadBuffer), &bytesRead, portMAX_DELAY);
        ESP_LOGI(TAG, "Bytes read: %d", bytesRead);

        // Print the raw audio samples in hexadecimal
        /*
        for (int i = 0; i < bytesRead / sizeof(int16_t); i++) {
            printf("%d\n", i2s_read_buff[i]);
        }
        */

        //samplesSum += bytesRead;
        //ESP_LOGI(TAG, "SAMPLES: %ld", samplesSum);

        if(bytesRead < wantedNumOfBytes)
        {
            ESP_LOGE(TAG, "Wanted %d bytes - retrieved %dbytes", wantedNumOfBytes, bytesRead);
            while(1);
        }

        // Send data to the ring buffer
        if (xRingbufferSend(recorder->ringBuffer, (void*)i2sReadBuffer, bytesRead, portMAX_DELAY) != pdTRUE) {
            ESP_LOGE(TAG, "Failed to send data to the ring buffer");
            while(1);
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
        ESP_LOGE(TAG, "Not enough data in the ring buffer, only %d bytes", bytesInTheBuffer);
        return 0;
    }
    ESP_LOGI(TAG, "In the buffer: %d bytes", bytesInTheBuffer);

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
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
    ringBuffer = xRingbufferCreate(1024 * 32, RINGBUF_TYPE_BYTEBUF);
    if (ringBuffer == NULL) {
        ESP_LOGE(TAG, "Creating ring buffer failed");
        while(1);
    }

    BaseType_t value = xTaskCreate(
        captureAudioTask,            // Function pointer
        "CaptureAudioTask",          // Task name
        1024 * 32,                   // Stack size (32 KB)
        this,                        // Parameters passed to the task
        10,                          // Task priority
        &(this->captureAudioHandle));   // Handle to the created task
    
    if(value != pdPASS) 
    {
        ESP_LOGE(TAG, "Creating audio capturing task failed");
        while(1);
    }
}

void AudioRecorder::captureAudioTask(void* pvParameters)
{
    AudioRecorder* recorder = static_cast<AudioRecorder*>(pvParameters); 
    int16_t i2sReadBuffer[512];
    size_t bytesRead;

    while (1) {
        i2s_read(I2S_NUM_0, i2sReadBuffer, sizeof(i2sReadBuffer), &bytesRead, portMAX_DELAY);
        ESP_LOGI(TAG, "Bytes read: %d", bytesRead);

        // Print the raw audio samples in hexadecimal
        /*
        for (int i = 0; i < bytesRead / sizeof(int16_t); i++) {
            printf("%d\n", i2s_read_buff[i]);
        }
        */

        // Send data to the ring buffer
        if (xRingbufferSend(recorder->ringBuffer, (void*)i2sReadBuffer, bytesRead, pdMS_TO_TICKS(100)) != pdTRUE) {
            ESP_LOGE(TAG, "Failed to send data to the ring buffer");
        }


        vTaskDelay(pdMS_TO_TICKS(10)); 
    }
}

uint32_t AudioRecorder::getSamples(int16_t* samples, uint32_t numOfSamples)
{
    size_t buffer_size;
    // Retrieve the data from the ring buffer
    uint8_t* data = (uint8_t*)xRingbufferReceive(ringBuffer, &buffer_size, portMAX_DELAY);
    
    if (data != NULL) {
        uint32_t sampleCount = buffer_size / sizeof(int16_t);
        if (sampleCount > numOfSamples) {
            sampleCount = numOfSamples; // Limit to the requested number of samples
        }
        memcpy(samples, data, sampleCount * sizeof(int16_t)); // Copy samples to the provided buffer
        vRingbufferReturnItem(ringBuffer, (void*)data); // Return the buffer back to the ring buffer
        return sampleCount; // Return the number of samples retrieved
    }
    return 0; // No samples retrieved
}
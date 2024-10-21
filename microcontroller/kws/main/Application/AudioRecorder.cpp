#include "AudioRecorder.hpp"

static const char *TAG = "Audio Recorder";

AudioRecorder::AudioRecorder()
{
    ESP_LOGI(TAG, "Starting codec chip");
    audio_board_handle_t board_handle = audio_board_init();
    audio_hal_ctrl_codec(board_handle->audio_hal, AUDIO_HAL_CODEC_MODE_ENCODE, AUDIO_HAL_CTRL_START);
    
    ESP_LOGI(TAG, "Creating i2s stream to read audio data from codec chip");
    i2s_config_t i2s = {
        .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_RX),
        .sample_rate = (uint32_t)16000,
        .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT, // change to 16 bits per sample
        .channel_format = I2S_CHANNEL_FMT_ONLY_RIGHT, // Although the microphone is mono, the I2S data is 2-channel
        .communication_format = I2S_COMM_FORMAT_STAND_I2S,
        .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1, // Default interrupt priority
        .dma_buf_count = 3,
        .dma_buf_len = 300,
        .use_apll = false,
        .tx_desc_auto_clear = false, // Auto clear tx descriptor on underflow
    };
     
    i2s_stream_cfg_t i2s_cfg = I2S_STREAM_CFG_DEFAULT();
    i2s_cfg.type = AUDIO_STREAM_READER;
    i2s_cfg.i2s_port = I2S_NUM_0;
    i2s_cfg.i2s_config = i2s;
    i2s_stream_reader = i2s_stream_init(&i2s_cfg);

    ringBuffer = xRingbufferCreate(1024 * 32, RINGBUF_TYPE_BYTEBUF);
    if(ringBuffer == NULL) 
    {
        ESP_LOGE(TAG, "Creating ring buffer failed");
    }

    if(xTaskCreate(this->captureAudioTask, "CaptureAudioTask", 1024 * 32, NULL, 10, &(this->captureAudioHandle)) != pdPASS)
    {
        ESP_LOGE(TAG, "Creating audio capturing task failed");
    }
}

AudioRecorder::~AudioRecorder()
{
    vTaskDelete(this->captureAudioHandle);
    //delete ring buff and i2c stream
}

void AudioRecorder::captureAudioTask()
{
    uint16_t i2s_read_buff[512];
    size_t bytes_read;

    while (1) {
        i2s_read(I2S_NUM_0, i2s_read_buff, sizeof(i2s_read_buff), &bytes_read, portMAX_DELAY);

        // Print the raw audio samples in hexadecimal
        for (int i = 0; i < bytes_read / sizeof(int16_t); i++) {
            printf("%d\n", i2s_read_buff[i]);
        }
        vTaskDelay(pdMS_TO_TICKS(10)); 
    }

}

uint32_t AudioRecorder::getSamples(int16_t* samples, uint32_t numOfSamples)
{
   return 0;
}
#include "NeuralNetwork.hpp"
#include "esp_log.h"

static const char *TAG = "NeuralNetwork";

NeuralNetwork::NeuralNetwork()
{
  ESP_LOGI(TAG, "NN initialization");
  this->model = tflite::GetModel(g_model);
  if (model->version() != TFLITE_SCHEMA_VERSION)
  {
    ESP_LOGE(TAG, "Model provided is schema version %ld not equal to supported version %d.", model->version(), TFLITE_SCHEMA_VERSION);
    return;
  }
}

bool NeuralNetwork::invoke(int8_t* featureImage)
{
  return true;
}
#include "NeuralNetwork.hpp"

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

  if (resolver.AddConv2D() != kTfLiteOk) {
    while(1);
    return;
  }
  if (resolver.AddFullyConnected() != kTfLiteOk) {
    while(1);
    return;
  }
  if (resolver.AddSoftmax() != kTfLiteOk) {
    while(1);
    return;
  }
  if(resolver.AddReshape() != kTfLiteOk)
  {
    while(1);
    return;
  }
  if (resolver.AddMaxPool2D() != kTfLiteOk) {
    while(1);
    return;
  }

  // Build an interpreter to run the model with.
  static tflite::MicroInterpreter static_interpreter(
      model, resolver, tensor_arena, TENSOR_ARENA_SIZE);
  interpreter = &static_interpreter;

  // Allocate memory from the tensor_arena for the model's tensors.
  TfLiteStatus allocate_status = interpreter->AllocateTensors();
  if (allocate_status != kTfLiteOk) {
    ESP_LOGE(TAG, "Allocating Tensors failed: %d", allocate_status);
    while(1);
  }

  // Get information about the memory area to use for the model's input.
  model_input = interpreter->input(0);

  if((model_input->dims->size != 4) || 
     (model_input->dims->data[0] != 1) ||
     (model_input->dims->data[1] != 61) ||
     (model_input->type != 1)) 
  {
    ESP_LOGE(TAG, "Bad input tensor parameters in model");
    ESP_LOGE(TAG, "Expected dims->size: 2, Actual: %d", model_input->dims->size);
    ESP_LOGE(TAG, "Expected dims->data[0]: 1, Actual: %d", model_input->dims->data[0]);
    ESP_LOGE(TAG, "Expected dims->data[1]: %d, Actual: %d", NUMBER_OF_FEATURES, model_input->dims->data[1]);
    ESP_LOGE(TAG, "Expected type: %d (kTfLiteInt8), Actual: %d", 1, model_input->type);

    while(1);
  }
  model_input_buffer = tflite::GetTensorData<float>(model_input);
  ESP_LOGW(TAG, "PROCESS");


  ESP_LOGI(TAG, "Model input shape: [%d, %d, %d, %d]",
         model_input->dims->data[0],
         model_input->dims->data[1],
         model_input->dims->data[2],
         model_input->dims->data[3]);

}

void NeuralNetwork::giveFeaturesToModel(float* features, size_t numberOfFeatures)
{
  for(size_t i = 0; i < numberOfFeatures; i++)
  {
    model_input_buffer[i] = features[i];
  }
}

bool NeuralNetwork::invoke(void)
{
  TfLiteStatus invoke_status = interpreter->Invoke();
  if(invoke_status != kTfLiteOk) {
    ESP_LOGE(TAG, "Invoke failed");
    return false;
  }
  return true;
}

void NeuralNetwork::printOutput()
{
  TfLiteTensor* output = interpreter->output(0);
  float* output_data = tflite::GetTensorData<float>(output);
  size_t output_size = output->dims->data[1]; // Size of the output

  // Process the output data
  for (size_t i = 0; i < output_size; i++)
  {
    ESP_LOGI(TAG, "Output[%d]: %f", i, output_data[i]);
  }

  int max_index = 0;
  float max_value = output_data[0];
  for (size_t i = 1; i < output_size; i++) {
    if (output_data[i] > max_value) {
        max_value = output_data[i];
        max_index = i;
    }
}
ESP_LOGI(TAG, "Predicted class index: %d", max_index);
}
#include "NeuralNetwork.hpp"

static const char *TAG = "NeuralNetwork";

NeuralNetwork::NeuralNetwork()
{
  printf("NN initialization");
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
  if (resolver.AddQuantize() != kTfLiteOk) {
    while(1);
    return;
  }
  if (resolver.AddDequantize() != kTfLiteOk) {
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
     (model_input->dims->data[1] != NUMBER_OF_TIME_SLICES) ||
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

  TfLiteTensor* output = interpreter->output(0);
  this->numberOfClasses = output->dims->data[1]; 
  ESP_LOGI(TAG, "Number of classes: %d", numberOfClasses);
  this->outputData = (float*)malloc(sizeof(float) * numberOfClasses);

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
    printf("Invoke failed");
    return false;
  }
  TfLiteTensor* output = interpreter->output(0);
  outputData = tflite::GetTensorData<float>(output);
  return true;
}
# Keyword spotting system 
![ESP32-S3](https://img.shields.io/badge/Platform-ESP32--S3-blue) ![C++](https://img.shields.io/badge/Language-C++-red) ![Python](https://img.shields.io/badge/NeuralNetworkTraining-Python-yellow) ![Status](https://img.shields.io/badge/Status-Active-brightgreen)

## 📌 Overview
Keyword spotting (KWS) system is a lightweight and efficient system designed for the ESP32-S3. It processes audio input, extracts MFCC features and runs inference on a neural network to detect commands and trigger predefined actions.

## ✨ Features
-  **Efficient Audio Command Recognition**: Detects and executes commands based on neural network inference.
-  **Low-Latency Execution**: Real-time response upon detecting a valid command.
-  **Optimized for Embedded systems**: Lightweight and fast.
-  **Customizable Command Set**: Easily choose which words trigger actions.
-  **Easy addition of new action**: Attach new actions to specific word command.

## 🏗 Repository Structure
```
/
 ├── docs       # Masters thesis written on this theme
 │
 ├── microcontroller/ kws    # ESP32-S3 implementation (C++)
 │
 ├── neural_network/              # Training and evaluation of the CNN model (Python)
 │   ├── models                   # Trained models
 │   ├── kws_network.ipynb        # Jupyter notebook used for neural network model training
 │
 ├── recordings_playroom      # testing facility for audio recordings
/
```

## 🛠 Implementation on ESP32-S3 (C++)

### Overview
Espressif IoT Development Framework (ESP-IDF) 5.1.2 is used for the development 
of this project.
Follow the ESP-IDF installation guide for ESP32-S3 [here](https://docs.espressif.com/projects/esp-idf/en/latest/esp32s3/get-started/index.html).

The microcontroller captures audio, extracts MFCC features, and runs inference using the trained CNN model. Trained model is the result of the process that happens in Jupyter Notebook (neural_network/kws_network.ipynb).

### Adding Wanted Commands

Commad that the neural network is trained on should be added just like it is shown below. "Backgorund" and "Unknown" are always added no matter the choosing of other commands and they should be instantiated as BlankCommand object. Commands MUST be added alphabetically for the system to work properly.

```
BlankCommand command_back("BACKGROUND", 1, 0.7);
PrintCommand command_left("LEFT", 3, 0.8);
PrintCommand command_no("NO", 3, 0.85);
PrintCommand command_right("RIGHT", 4, 0.8);
BlankCommand command_unknown("UNKNOWN", 1, 0.7);

recognizer.addCommand(&command_back);
recognizer.addCommand(&command_left);
recognizer.addCommand(&command_no);
recognizer.addCommand(&command_right);
recognizer.addCommand(&command_unknown);
```

### Writing custom jobs
After the command is recognized, a specific job is triggered. What type of job is
triggered depends on the type of command. BlankCommand does nothing, PrintCommand prints the name of heard command. Custom commands can be written, they only need to follow Command interface. Implementation of a custom command must inherit Command class and only implement virtual function 'virtual void execute(void)'. PrintCommand class is shown below and can be used as template for other custom commands implementations.

```
#include "Command.hpp"

class PrintCommand : public Command
{
    public:
        PrintCommand(const char* commandName, int historySize, float threshold) : Command(commandName, historySize, threshold) {}
        virtual void execute(void);
};
```


## 🚀 Neural Network Training (Python)

### Overview
The neural network extracts Mel Frequency Cepstral Coefficients (MFCC) from audio,
builds a Convolutional Neural Network (CNN) and trains it to recognize different
audio commands. Everything that is needed preinstalled is Python 3.9.

###  Dataset 
For the neural network training Google Speech Commands V2 dataset is used (https://www.kaggle.com/datasets/sylkaladin/speech-commands-v2). 

###  Training the Model
The whole training process is done with Jupyter Notebook found in neural_network folder.
In the beggining there is a configuration part that enables user to play with different
parameters of feature extraction and netwotk training process. Also, there is an option to choose which commands the neural network will be trained on.

```
# Configuration

TEST_DATASET_SIZE = 0.1
VALIDATION_DATASET_SIZE = 0.1
EPOCHS = 50
LEARNING_RATE = 0.005
BATCH_SIZE = 32
SAMPLE_RATE = 16000
WINDOW_SIZE = 512
STEP_SIZE = 384
LOWER_BAND_LIMIT = 80.0
UPPER_BAND_LIMIT = 7600.0
NUMBER_OF_MEL_BINS = 40
NUMBER_OF_MFCCS = 13

wanted_commands = ["yes", "no", "left", "right", "zero"]
```
Notebook automatically installs everything that is needed, downloads the dataset,
makes wanted categories and trains the network.

###  Model Export
After training, the model will be saved in `models/`. Also, smaller version of the model
ready to run on microcontroller is saved as model.cc. This file should be included into ESP-IDF microcontroller project.

## 📄 TODOs & Improvements
- Make integer MFCC extraction on the microcontroller, currently floating point arithmetics is used (not a problem on ESP32-S3, but might be on different microcontrollers that don't have FPU)
- Add custom commands (multiple 1 second audio clips of someone saying wanted word should be recorded)

## 📜 License
This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

# Keyword spotting system 
![ESP32-S3](https://img.shields.io/badge/Platform-ESP32--S3-blue) ![C++](https://img.shields.io/badge/Language-C++-red) ![Python](https://img.shields.io/badge/NeuralNetworkTraining-Python-yellow) ![Status](https://img.shields.io/badge/Status-Active-brightgreen)

## 📌 Overview
Keyword spotting (KWS) system is a lightweight and efficient system designed for the ESP32-S3. It processes audio input, extracts MFCC features, and runs inference on a neural network to detect commands and trigger predefined actions.

## ✨ Features
-  **Efficient Audio Command Recognition**: Detects and executes commands based on neural network inference.
-  **Customizable Command Set**: Easily choose which words trigger actions.
-  **Easy addition of new action**: Attach new actions to specific word command.
-  **Optimized for ESP32-S3**: Lightweight and performant for embedded systems.
-  **Low-Latency Execution**: Immediate response upon detecting a valid command.

## 🏗 Repository Structure
```
/
 ├── docs       # Masters thesis written on this theme
 │
 ├── microcontroller/ kws    # ESP32-S3 implementation (C++)
 │   ├── src/                # Source code for command recognition
 │   ├── models/             # Deployed models for inference
 │
 ├── neural_network/              # Training and evaluation of the CNN model (Python)
 │   ├── models                   # Trained models
 │   ├── kws_network.ipynb        # Jupyter notebook used for neural network model training
 │
 ├── recordings_playroom      # testing facility
```

## 🛠 Installation & Setup

### 1️⃣ Clone the Repository
```sh
git clone https://github.com/your-username/your-repo.git
cd your-repo
```

### 2️⃣ Install Dependencies
- **Neural Network (Python)**: Install dependencies in `neural_network/`
  ```sh
  cd neural_network
  pip install -r requirements.txt
  ```
- **Microcontroller (C++)**: Follow the ESP-IDF installation guide for ESP32-S3 [here](https://docs.espressif.com/projects/esp-idf/en/latest/esp32s3/get-started/index.html).

### 3️⃣ Build & Flash to ESP32-S3
```sh
cd microcontroller
idf.py set-target esp32s3
idf.py build flash monitor
```

## 🚀 Neural Network Training (Python)

### 📌 Overview
The neural network processes spectrograms of recorded audio and learns to classify them into predefined commands.

### 🛠 Dataset Preparation
Place your dataset in `dataset/`. Ensure it is structured correctly with labeled audio files.

### 🔧 Training the Model
Run the following command to train the model:
```sh
python train.py --epochs 10 --batch_size 32
```

### 📤 Model Export
After training, the model will be saved in `model/`. Convert it for ESP32 deployment using:
```sh
python export_model.py --model model/best_model.h5 --output model/tflite_model.tflite
```

## 🚀 Microcontroller Implementation (C++)

### 📌 Overview
The microcontroller captures audio, extracts MFCC features, and runs inference using the trained CNN model.

### 🛠 Setup & Installation

#### 1️⃣ Set Up ESP-IDF
Follow the [ESP-IDF installation guide](https://docs.espressif.com/projects/esp-idf/en/latest/esp32s3/get-started/index.html) to install the development environment.

#### 2️⃣ Build & Flash Firmware
Navigate to the `microcontroller/` directory and run:
```sh
idf.py set-target esp32s3
idf.py build flash monitor
```

#### 3️⃣ Deploy the Model
Copy the converted TFLite model (`tflite_model.tflite`) into `models/` before flashing.

### 🚀 Running Inference
The recognizer processes audio and detects commands in real-time. If a command's probability exceeds `ACTIVATION_THRESHOLD`, it triggers an action.

## 📄 TODOs & Improvements
- [ ] Improve dataset preprocessing
- [ ] Optimize inference speed
- [ ] Experiment with different CNN architectures
- [ ] Implement a dynamic threshold for improved accuracy

## 📜 License
This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

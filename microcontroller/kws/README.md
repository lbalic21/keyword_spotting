# Command Recognizer for ESP32-S3

![ESP32-S3](https://img.shields.io/badge/Platform-ESP32--S3-blue) ![C++](https://img.shields.io/badge/Language-C++-red) ![Status](https://img.shields.io/badge/Status-Active-brightgreen)

## 📌 Overview
Command Recognizer is a lightweight and efficient keyword spotting (KWS) system designed for the ESP32-S3. It processes audio input, extracts MFCC features, and runs inference on a neural network to detect commands and trigger predefined actions.

## ✨ Features
- 🚀 **Efficient Audio Command Recognition**: Detects and executes commands based on neural network inference.
- 🎧 **Customizable Command Set**: Easily add and manage multiple commands.
- ⏳ **Cool-Down Mechanism**: Prevents rapid re-triggering of commands.
- ⚡ **Optimized for ESP32-S3**: Lightweight and performant for embedded systems.
- 📡 **Low-Latency Execution**: Immediate response upon detecting a valid command.

## 🛠️ Installation & Setup

### 1️⃣ Clone the Repository
```sh
git clone https://github.com/your-username/your-repo.git
cd your-repo
```

### 2️⃣ Install ESP-IDF (if not installed)
Follow the official [ESP-IDF installation guide](https://docs.espressif.com/projects/esp-idf/en/latest/esp32s3/get-started/index.html).

### 3️⃣ Build & Flash to ESP32-S3
```sh
idf.py set-target esp32s3
idf.py build flash monitor
```

## 🚀 Usage

### ✅ Adding Commands
To add a new command, subclass `Command` and implement the `execute()` method. Then, register it using `CommandRecognizer::addCommand()`.

```cpp
class CustomCommand : public Command {
public:
    void execute(float probability) override {
        printf("Custom Command Triggered with probability: %f\n", probability);
    }
};

// Usage
CommandRecognizer recognizer;
recognizer.addCommand(new CustomCommand());
```

### ✅ Recognizing Commands
- The system continuously processes audio and runs inference using a neural network.
- If a class's probability exceeds the `ACTIVATION_THRESHOLD`, the corresponding command is executed.
- A **cool-down period** ensures commands are not repeatedly triggered.

```cpp
bool CommandRecognizer::recognize(int numberOfClasses, float* outputData) {
    if (((esp_timer_get_time() - lastRecognizeTime) / 1000) < COOL_OF_PERIOD_MS) {
        return false;
    }
    for (uint32_t i = 0; i < numberOfClasses; i++) {
        if (outputData[i] > ACTIVATION_THRESHOLD) {
            invokeCommand(i, outputData[i]);
            lastRecognizeTime = esp_timer_get_time();
            return true;
        }
    }
    return false;
}
```

### ✅ Invoking Commands
```cpp
void CommandRecognizer::invokeCommand(uint32_t commandIndex, float probability) {
    if (commandIndex < commandCount) {
        commands[commandIndex]->execute(probability);
    }
}
```

### ✅ Getting the Number of Commands
```cpp
void CommandRecognizer::getNumOfCommands() {
    printf("Number of commands: %d\n", commandCount);
}
```

## 🛠️ Code Structure
```
/src
 ├── CommandRecognizer.cpp  # Core logic for recognizing commands
 ├── CommandRecognizer.hpp  # Header file defining the recognizer class
 ├── main.cpp               # Entry point for the application
 ├── models/                # Neural network models for KWS
 ├── audio/                 # Raw audio processing utilities
```

## 📜 TODOs & Improvements
- [ ] Improve memory efficiency for command storage
- [ ] Implement dynamic command registration
- [ ] Add support for additional neural network architectures

## 🤝 Contributing
Contributions are welcome! Feel free to:
- Submit issues for bug reports and feature requests.
- Fork the repo and submit pull requests with improvements.

## 📝 License
This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## 📩 Contact
For questions or discussions, feel free to reach out via GitHub Issues or email.

---

Happy coding! 🚀


set(COMPONENT_SRCS 
"main.c"
"Application/Application.cpp"
"Application/AudioRecorder/AudioRecorder.cpp"
"Application/testFrame.cpp"
"Application/yes.cpp"
"Application/no.cpp"
"Application/yes_esp.cpp"

#"Application/FeatureGenerator/FeatureGenerator.cpp"
#"Application/FeatureGenerator/Window.cpp"
#"Application/FeatureGenerator/FFT.cpp"
#"Application/FeatureGenerator/MelSpectrogram.cpp"
"Application/FeatureGeneratorFloat/DCT.cpp"

"Application/FeatureGeneratorFloat/FeatureGeneratorFloat.cpp"
"Application/FeatureGeneratorFloat/WindowFloat.cpp"
"Application/FeatureGeneratorFloat/FFTFloat.cpp"
"Application/FeatureGeneratorFloat/MelSpectrogramFloat.cpp"

"Application/NeuralNetwork/model.cc"
"Application/NeuralNetwork/NeuralNetwork.cpp"
"Application/CommandRecognizer/CommandRecognizer.cpp"
"Application/CommandResponder/CommandResponder.cpp"

)
set(COMPONENT_ADD_INCLUDEDIRS 
.                          
Application
)

register_component()
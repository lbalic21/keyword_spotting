set(COMPONENT_SRCS 
"main.c"
"Application/Application.cpp"
"Application/AudioRecorder/AudioRecorder.cpp"
"Application/TestSamples/testFrame.cpp"
"Application/TestSamples/yes.cpp"
"Application/TestSamples/no.cpp"
"Application/TestSamples/yes_esp.cpp"
"Application/TestSamples/up.cpp"
"Application/TestSamples/down.cpp"
"Application/TestSamples/happy.cpp"

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
"Application/CommandRecognizer/PrintCommand.cpp"
"Application/CommandRecognizer/NoCommand.cpp"

)
set(COMPONENT_ADD_INCLUDEDIRS 
.                          
Application
)

register_component()
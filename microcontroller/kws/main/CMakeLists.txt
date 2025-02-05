set(COMPONENT_SRCS 
"main.c"
"Application/Application.cpp"
"Application/AudioRecorder/AudioRecorder.cpp"

"Application/FeatureGenerator/FeatureGenerator.cpp"
"Application/FeatureGenerator/Window.cpp"
"Application/FeatureGenerator/FFT.cpp"
"Application/FeatureGenerator/MelSpectrogram.cpp"
"Application/FeatureGenerator/DCT.cpp"

"Application/NeuralNetwork/model.cc"
"Application/NeuralNetwork/NeuralNetwork.cpp"
"Application/CommandRecognizer/CommandRecognizer.cpp"
"Application/Commands/Command.cpp"
"Application/Commands/BlankCommand.cpp"
"Application/Commands/PrintCommand.cpp"

"Application/TestSamples/testFrame.cpp"
"Application/TestSamples/yes.cpp"
"Application/TestSamples/no.cpp"
"Application/TestSamples/yes_esp.cpp"
"Application/TestSamples/up.cpp"
"Application/TestSamples/down.cpp"
"Application/TestSamples/happy.cpp"
)
set(COMPONENT_ADD_INCLUDEDIRS 
.                          
Application
)

register_component()
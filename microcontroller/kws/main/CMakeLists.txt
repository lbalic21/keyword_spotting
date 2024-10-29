set(COMPONENT_SRCS 
"main.c"
"Application/Application.cpp"
"Application/AudioRecorder.cpp"
"Application/FeatureGenerator.cpp"
"Application/Window.cpp"
"Application/FFT.cpp"
)
set(COMPONENT_ADD_INCLUDEDIRS 
.                          
Application
)

register_component()
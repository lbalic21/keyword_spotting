set(COMPONENT_SRCS 
"main.c"
"Application/Application.cpp"
"Application/AudioRecorder.cpp"
"Application/FeatureGenerator.cpp"
)
set(COMPONENT_ADD_INCLUDEDIRS 
.                          
Application
)

register_component()
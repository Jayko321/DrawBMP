cmake_minimum_required(VERSION 3.28)

project(DrawBMP 
    DESCRIPTION "Simple application to draw monochrome BMP files to console"
    LANGUAGES CXX 
    VERSION 0.1)

add_executable("${PROJECT_NAME}"
    src/main.cpp
    src/BMPImage.h            
    src/BMPImage.cpp
)

target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_17)
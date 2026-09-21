cmake_minimum_required(VERSION 3.16)
project(ASCIIArt VERSION 2.0 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(OpenCV REQUIRED COMPONENTS core imgcodecs videoio imgproc)

add_executable(ascii-art
    src/main.cpp src/Arguments.cpp src/AsciiConverter.cpp
    src/Renderer.cpp src/Terminal.cpp src/VideoRenderer.cpp)

target_include_directories(ascii-art PRIVATE include ${OpenCV_INCLUDE_DIRS})
target_link_libraries(ascii-art PRIVATE ${OpenCV_LIBS})

if(MSVC)
    target_compile_options(ascii-art PRIVATE /W4 /O2)
else()
    target_compile_options(ascii-art PRIVATE -Wall -Wextra -Wpedantic -O3)
endif()

if(WIN32)
    target_compile_definitions(ascii-art PRIVATE _CRT_SECURE_NO_WARNINGS)
endif()

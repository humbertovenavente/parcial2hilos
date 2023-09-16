cmake_minimum_required(VERSION 3.0)
project(parcial2)

# Find the packages you need (e.g., OpenCV and wxWidgets)
find_package(OpenCV REQUIRED)
find_package(wxWidgets REQUIRED COMPONENTS core base)
include(${wxWidgets_USE_FILE})

# Add your source files
add_executable(parcial2 gui.cpp imageProcessing.cpp imageProcessing.h)

# Link libraries
target_link_libraries(parcial2 ${OpenCV_LIBS} ${wxWidgets_LIBRARIES})

# Set C++ standard (e.g., C++11)
set(CMAKE_CXX_STANDARD 11)

# Optionally, set an output directory for the binary
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

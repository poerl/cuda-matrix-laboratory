cmake_minimum_required(VERSION 3.27.0)
project(
    vote-gen-benchmark-gui 
    VERSION 0.1.0 
    LANGUAGES C CXX
)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# Automoc init
find_package(Qt6 REQUIRED COMPONENTS Core)
find_package(OpenMP REQUIRED)

if(ENABLE_CUDA)
    enable_language(CUDA)
    
    set(CMAKE_CUDA_STANDARD 20)
    set(CMAKE_CUDA_STANDARD_REQUIRED ON)
    set(CMAKE_CUDA_EXTENSIONS OFF)
    
    set(CMAKE_CUDA_RUNTIME_LIBRARY Shared)
    set(CMAKE_CUDA_ARCHITECTURES native)
    
    find_package(CUDAToolkit REQUIRED)
    add_compile_definitions(CUDA_ENABLED)
    message(STATUS "CUDA enabled. Building with cuda support.")
else()
    message(STATUS "CUDA disabled. Building without cuda support.")
endif()

add_subdirectory(modules/api)

add_subdirectory(modules/gui)
add_subdirectory(modules/model)
add_subdirectory(modules/presenter)

add_executable(vote-gen-benchmark-gui main.cpp)

target_link_libraries(vote-gen-benchmark-gui PUBLIC 
    api
    gui
    model
    presenter
)

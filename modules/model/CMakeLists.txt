set(MODEL_SOURCES
    src/AbstractComputer.cpp
    src/CpuComputer.cpp
    src/Model.cpp
)

if(ENABLE_CUDA)
    list(APPEND MODEL_SOURCES
        src/CudaComputer.cu
    )
else()
endif()

add_library(model STATIC
    ${MODEL_SOURCES}
    include/Array2D.h
    include/FileWorker.h

    src/Array2D.cpp
    src/FileWorker.cpp
)

target_include_directories(model PUBLIC include)

target_link_libraries(model PUBLIC
    api
    OpenMP::OpenMP_CXX
)

if(ENABLE_CUDA)
    target_link_libraries(model PUBLIC CUDA::toolkit)
endif()
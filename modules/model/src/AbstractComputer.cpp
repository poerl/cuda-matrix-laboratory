#include "AbstractComputer.h"

#include "CpuComputer.h"
#include <cstdint>
#include <stdexcept>
#include <vector>
#ifdef CUDA_ENABLED
    #include <cuda_runtime.h>
    #include "CudaComputer.h"
#endif // CUDA_ENABLED

#include <memory>

std::unique_ptr<AbstractComputer> AbstractComputerUtils::createAbstractComputer() {
#ifdef CUDA_ENABLED
    int device_count = 0;
    cudaError_t err = cudaGetDeviceCount(&device_count);

    if (err == cudaSuccess && device_count > 0) {
        return std::make_unique<CudaComputer>();
    }
#endif // CUDA_ENABLED
    return std::make_unique<CpuComputer>();
}
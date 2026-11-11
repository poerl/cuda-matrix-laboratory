#pragma once

#include "AbstractComputer.h"

#include <cstddef>
#include <cstdint>
#include <random>

#include <cuda_runtime.h>

class CudaComputer : public AbstractComputer {
private:
    cudaDeviceProp deviceProps_{};
    uint32_t blockSizeThreads_ = 256;
    uint64_t masterSeed_ = std::random_device{}();

public:
    CudaComputer();
    ~CudaComputer() override = default;

    CudaComputer(const CudaComputer&) = delete;
    CudaComputer& operator=(const CudaComputer&) = delete;

    std::unique_ptr<IArray2D> genMatrix(size_t rows, size_t cols) override;
    std::unique_ptr<IArray2D> mulMatrixes(const IArray2D* matrixA, const IArray2D* matrixB) override;

    void setBlockSizeThreads(uint32_t nThreads) override;
    uint32_t getBlockSizeThreads() override;
    uint32_t getCapBlockSizeThreads() override;

    std::string getName() const override;
    bool isCPU() const override { return false; }

private:
    static void checkCuda(cudaError_t error, const char* operation);
};

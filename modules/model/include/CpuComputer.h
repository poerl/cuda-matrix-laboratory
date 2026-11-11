#pragma once

#include "AbstractComputer.h"
#include "IArray2D.h"

#include <cstddef>
#include <random>

class CpuComputer : public AbstractComputer {
private:
    unsigned masterSeed = std::random_device{}();
    size_t numThreads = 1;
public:
    std::unique_ptr<IArray2D> genMatrix(size_t rows, size_t cols) override;
    std::unique_ptr<IArray2D> mulMatrixes(const IArray2D* matrixA, const IArray2D* matrixB) override;

    std::string getName() const override;
    uint32_t getBlockSizeThreads() override;
    uint32_t getCapBlockSizeThreads() override;
    bool isCPU() const override {return true;};
    void setBlockSizeThreads(uint32_t nThreads) override;
};
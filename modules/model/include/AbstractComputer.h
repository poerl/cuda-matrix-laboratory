#pragma once

#include <cstdint>
#include <memory>
#include <span>
#include <string>
#include <vector>

#include "dtos.h"
#include "IArray2D.h"

class AbstractComputer {
protected:
    
public:
    virtual ~AbstractComputer() = default;
public:
    virtual std::unique_ptr<IArray2D> genMatrix(size_t rows, size_t cols) = 0;
    virtual std::unique_ptr<IArray2D> mulMatrixes(const IArray2D* matrixA, const IArray2D* matrixB) = 0;

    virtual void setBlockSizeThreads(uint32_t nThreads) = 0;
    virtual uint32_t getBlockSizeThreads() = 0;
    virtual uint32_t getCapBlockSizeThreads() = 0;
    virtual std::string getName() const = 0;
    virtual bool isCPU() const = 0;
protected:
    
};

namespace AbstractComputerUtils {
    std::unique_ptr<AbstractComputer> createAbstractComputer();
}
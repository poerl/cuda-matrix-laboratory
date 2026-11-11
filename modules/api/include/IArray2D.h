#pragma once

#include <cstddef>
#include <vector>
#include <memory>

class IArray2D {
public:
    virtual ~IArray2D() = default;
    
    virtual size_t getRows() const = 0;
    virtual size_t getCols() const = 0;
    virtual std::vector<double>& getData() = 0;
    virtual const std::vector<double>& getData() const = 0;
    
    virtual double& at(size_t row, size_t col) = 0;
    virtual const double& at(size_t row, size_t col) const = 0;
    
    virtual void resize(size_t rows, size_t cols) = 0;
    virtual void fill(double value) = 0;
};
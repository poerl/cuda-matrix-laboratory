// src/Array2D.h
#pragma once

#include "IArray2D.h"
#include <memory>

class Array2D : public IArray2D {
private:
    size_t rows_;
    size_t cols_;
    std::vector<double> data_;

public:
    Array2D(size_t rows = 0, size_t cols = 0);
    Array2D(size_t rows, size_t cols, double fillValue);
    
    size_t getRows() const override;
    size_t getCols() const override;
    std::vector<double>& getData() override;
    const std::vector<double>& getData() const override;
    
    double& at(size_t row, size_t col) override;
    const double& at(size_t row, size_t col) const override;
    
    void resize(size_t rows, size_t cols) override;
    void fill(double value) override;
    
    static std::unique_ptr<Array2D> create(size_t rows, size_t cols);
    static std::unique_ptr<Array2D> create(size_t rows, size_t cols, double fillValue);
};
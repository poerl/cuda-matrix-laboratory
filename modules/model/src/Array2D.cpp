#include "Array2D.h"
#include <stdexcept>

Array2D::Array2D(size_t rows, size_t cols) 
    : rows_(rows), cols_(cols), data_(rows * cols, 0.0) {
}

Array2D::Array2D(size_t rows, size_t cols, double fillValue)
    : rows_(rows), cols_(cols), data_(rows * cols, fillValue) {
}

size_t Array2D::getRows() const {
    return rows_;
}

size_t Array2D::getCols() const {
    return cols_;
}

std::vector<double>& Array2D::getData() {
    return data_;
}

const std::vector<double>& Array2D::getData() const {
    return data_;
}

double& Array2D::at(size_t row, size_t col) {
    if (row >= rows_ || col >= cols_) {
        throw std::out_of_range("Array2D index out of range");
    }
    return data_[row * cols_ + col];
}

const double& Array2D::at(size_t row, size_t col) const {
    if (row >= rows_ || col >= cols_) {
        throw std::out_of_range("Array2D index out of range");
    }
    return data_[row * cols_ + col];
}

void Array2D::resize(size_t rows, size_t cols) {
    rows_ = rows;
    cols_ = cols;
    data_.resize(rows * cols, 0.0);
}

void Array2D::fill(double value) {
    std::fill(data_.begin(), data_.end(), value);
}

std::unique_ptr<Array2D> Array2D::create(size_t rows, size_t cols) {
    return std::make_unique<Array2D>(rows, cols);
}

std::unique_ptr<Array2D> Array2D::create(size_t rows, size_t cols, double fillValue) {
    return std::make_unique<Array2D>(rows, cols, fillValue);
}
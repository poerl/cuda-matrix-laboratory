#include "CpuComputer.h"
#include "dtos.h"
#include "IArray2D.h"
#include "Array2D.h"

#include <cstdint>
#include <format>
#include <omp.h>
#include <random>
#include <vector>
#include <memory>

    

std::unique_ptr<IArray2D> CpuComputer::genMatrix(size_t rows, size_t cols) {
    auto matrix = Array2D::create(rows, cols);
    
    thread_local std::mt19937 gen(masterSeed + omp_get_thread_num());
    thread_local std::uniform_real_distribution<double> distrib(-1000.0, 1000.0);

    #pragma omp parallel for num_threads(numThreads) collapse(2)
    for (size_t i = 0; i < rows; ++i) {
        for (size_t j = 0; j < cols; ++j) {
            matrix->at(i, j) = distrib(gen);
        }
    }
    
    return matrix;
}

std::unique_ptr<IArray2D> CpuComputer::mulMatrixes(const IArray2D* A, const IArray2D* B) {
    if (A->getCols() != B->getRows()) {
        throw std::invalid_argument("Matrix dimensions mismatch for multiplication");
    }
    
    size_t rows = A->getRows();
    size_t cols = B->getCols();
    size_t common = A->getCols();
    
    auto result = Array2D::create(rows, cols, 0.0);
    
    #pragma omp parallel for num_threads(numThreads) collapse(2)
    for (size_t i = 0; i < rows; ++i) {
        for (size_t j = 0; j < cols; ++j) {
            double sum = 0.0;
            for (size_t k = 0; k < common; ++k) {
                sum += A->at(i, k) * B->at(k, j);
            }
            result->at(i, j) = sum;
        }
    }
    
    return result;
}

std::string CpuComputer::getName() const {
    return "CpuComputer";
}

void CpuComputer::setBlockSizeThreads(uint32_t nThreads) {
    if (nThreads == 0 || nThreads > omp_get_max_threads()) {
        throw std::invalid_argument(std::format("Thread count for cpu must be in [1..{}]", omp_get_max_threads()));
    }
    numThreads = nThreads;
}

uint32_t CpuComputer::getBlockSizeThreads() {
    return numThreads;
}

uint32_t CpuComputer::getCapBlockSizeThreads() {
    return omp_get_max_threads();
}
#include "FileWorker.h"
#include "Array2D.h"

#include <fstream>
#include <stdexcept>

std::unique_ptr<IArray2D> FileWorker::readMatrix(const std::string& filename,
                                                    size_t expectedRows,
                                                    size_t expectedCols) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Cannot open file: " + filename);
    }
    
    auto matrix = Array2D::create(expectedRows, expectedCols);
    
    for (size_t i = 0; i < expectedRows; ++i) {
        for (size_t j = 0; j < expectedCols; ++j) {
            double value;
            if (!(file >> value)) {
                throw std::runtime_error("Error reading matrix from file: " + filename);
            }
            matrix->at(i, j) = value;
        }
    }
    
    return matrix;
}

void FileWorker::writeMatrix(const std::string& filename,
                            const IArray2D* matrix) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Cannot open file for writing: " + filename);
    }
    
    file.precision(10);
    
    for (size_t i = 0; i < matrix->getRows(); ++i) {
        for (size_t j = 0; j < matrix->getCols(); ++j) {
            file << matrix->at(i, j);
            if (j < matrix->getCols() - 1) {
                file << " ";
            }
        }
        file << "\n";
    }
}

std::unique_ptr<FileWorker> FileWorker::create() {
    return std::make_unique<FileWorker>();
}
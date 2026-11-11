#pragma once

#include "IArray2D.h"
#include <memory>
#include <string>

class IFileWorker {
public:
    virtual ~IFileWorker() = default;
    
    virtual std::unique_ptr<IArray2D> readMatrix(const std::string& filename, 
                                                   size_t expectedRows, 
                                                   size_t expectedCols) = 0;
    virtual void writeMatrix(const std::string& filename, 
                            const IArray2D* matrix) = 0;
};
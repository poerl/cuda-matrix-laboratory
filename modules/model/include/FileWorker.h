#pragma once

#include "IFileWorker.h"

class FileWorker : public IFileWorker {
public:
    std::unique_ptr<IArray2D> readMatrix(const std::string& filename,
                                          size_t expectedRows,
                                          size_t expectedCols) override;
    
    void writeMatrix(const std::string& filename,
                    const IArray2D* matrix) override;
    
    static std::unique_ptr<FileWorker> create();
};
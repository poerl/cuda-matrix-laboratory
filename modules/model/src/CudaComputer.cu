#include "CudaComputer.h"
#include "Array2D.h"

#include <cuda_runtime.h>
#include <curand_kernel.h>
#include <stdexcept>
#include <string>


__global__ void generateMatrixKernel(
    double* output,
    size_t elementCount,
    unsigned long long seed
) {
    size_t idx = blockIdx.x * blockDim.x + threadIdx.x;
    if (idx >= elementCount) return;
    
    curandStatePhilox4_32_10_t state;
    curand_init(seed, idx, 0, &state);
    output[idx] = curand_uniform_double(&state) * 2000.0 - 1000.0;
}

constexpr int TILE_SIZE = 16;

__global__ void multiplyMatricesTiledKernel(
    const double* A,
    const double* B,
    double* C,
    int rowsA,
    int colsA,
    int colsB
) {
    __shared__ double tileA[TILE_SIZE][TILE_SIZE];
    __shared__ double tileB[TILE_SIZE][TILE_SIZE];

    int row = blockIdx.y * TILE_SIZE + threadIdx.y;
    int col = blockIdx.x * TILE_SIZE + threadIdx.x;

    double sum = 0.0;
    int tileCount = (colsA + TILE_SIZE - 1) / TILE_SIZE;

    for (int t = 0; t < tileCount; t++) {
        int aCol = t * TILE_SIZE + threadIdx.x;
        int bRow = t * TILE_SIZE + threadIdx.y;

        tileA[threadIdx.y][threadIdx.x] = 
            (row < rowsA && aCol < colsA) ? A[row * colsA + aCol] : 0.0;
        
        tileB[threadIdx.y][threadIdx.x] = 
            (bRow < colsA && col < colsB) ? B[bRow * colsB + col] : 0.0;

        __syncthreads();

        #pragma unroll
        for (int k = 0; k < TILE_SIZE; k++) {
            sum += tileA[threadIdx.y][k] * tileB[k][threadIdx.x];
        }

        __syncthreads();
    }

    if (row < rowsA && col < colsB) {
        C[row * colsB + col] = sum;
    }
}


CudaComputer::CudaComputer() {
    int deviceCount = 0;
    checkCuda(cudaGetDeviceCount(&deviceCount), "cudaGetDeviceCount");
    
    if (deviceCount == 0) {
        throw std::runtime_error("No CUDA-capable GPU found");
    }

    cudaSetDevice(0);
    checkCuda(cudaGetDeviceProperties(&deviceProps_, 0), "cudaGetDeviceProperties");
    
    blockSizeThreads_ = std::min(256u, static_cast<uint32_t>(deviceProps_.maxThreadsPerBlock));
}

std::unique_ptr<IArray2D> CudaComputer::genMatrix(size_t rows, size_t cols) {
    auto matrix = Array2D::create(rows, cols);
    if (rows == 0 || cols == 0) return matrix;

    size_t elementCount = rows * cols;
    double* deviceData = nullptr;
    
    checkCuda(cudaMalloc(&deviceData, elementCount * sizeof(double)), "cudaMalloc");

    int gridSize = (elementCount + blockSizeThreads_ - 1) / blockSizeThreads_;
    
    generateMatrixKernel<<<gridSize, blockSizeThreads_>>>(
        deviceData, elementCount, masterSeed_
    );
    
    checkCuda(cudaDeviceSynchronize(), "generateMatrixKernel");
    checkCuda(cudaMemcpy(matrix->getData().data(), deviceData, 
                         elementCount * sizeof(double), cudaMemcpyDeviceToHost), "cudaMemcpy");
    
    cudaFree(deviceData);
    return matrix;
}

std::unique_ptr<IArray2D> CudaComputer::mulMatrixes(
    const IArray2D* matrixA,
    const IArray2D* matrixB
) {
    if (!matrixA || !matrixB) {
        throw std::invalid_argument("Matrix pointer is null");
    }
    if (matrixA->getCols() != matrixB->getRows()) {
        throw std::invalid_argument("Matrix dimensions mismatch");
    }

    int rowsA = matrixA->getRows();
    int colsA = matrixA->getCols();
    int colsB = matrixB->getCols();
    
    auto result = Array2D::create(rowsA, colsB, 0.0);
    if (rowsA == 0 || colsB == 0) return result;

    size_t sizeA = rowsA * colsA * sizeof(double);
    size_t sizeB = colsA * colsB * sizeof(double);
    size_t sizeC = rowsA * colsB * sizeof(double);

    double *deviceA = nullptr, *deviceB = nullptr, *deviceC = nullptr;
    
    checkCuda(cudaMalloc(&deviceA, sizeA), "cudaMalloc A");
    checkCuda(cudaMalloc(&deviceB, sizeB), "cudaMalloc B");
    checkCuda(cudaMalloc(&deviceC, sizeC), "cudaMalloc C");

    checkCuda(cudaMemcpy(deviceA, matrixA->getData().data(), sizeA, cudaMemcpyHostToDevice), "copy A");
    checkCuda(cudaMemcpy(deviceB, matrixB->getData().data(), sizeB, cudaMemcpyHostToDevice), "copy B");

    dim3 block(TILE_SIZE, TILE_SIZE);
    dim3 grid((colsB + TILE_SIZE - 1) / TILE_SIZE, (rowsA + TILE_SIZE - 1) / TILE_SIZE);

    multiplyMatricesTiledKernel<<<grid, block>>>(deviceA, deviceB, deviceC, rowsA, colsA, colsB);
    
    checkCuda(cudaDeviceSynchronize(), "multiplyMatricesTiledKernel");
    checkCuda(cudaMemcpy(result->getData().data(), deviceC, sizeC, cudaMemcpyDeviceToHost), "copy C");

    cudaFree(deviceA);
    cudaFree(deviceB);
    cudaFree(deviceC);
    
    return result;
}

void CudaComputer::setBlockSizeThreads(uint32_t nThreads) {
    if (nThreads == 0 || nThreads > static_cast<uint32_t>(deviceProps_.maxThreadsPerBlock)) {
        throw std::invalid_argument("Invalid block thread count");
    }
    blockSizeThreads_ = nThreads;
}

uint32_t CudaComputer::getBlockSizeThreads() const {
    return blockSizeThreads_;
}

uint32_t CudaComputer::getCapBlockSizeThreads() const {
    return deviceProps_.maxThreadsPerBlock;
}

std::string CudaComputer::getName() const {
    return "CudaComputer";
}

void CudaComputer::checkCuda(cudaError_t error, const char* operation) {
    if (error != cudaSuccess) {
        throw std::runtime_error(std::string(operation) + ": " + cudaGetErrorString(error));
    }
}
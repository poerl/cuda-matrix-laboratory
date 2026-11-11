#include "CudaComputer.h"
#include "Array2D.h"

#include <cuda_runtime.h>
#include <curand_kernel.h>
#include <algorithm>
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
    int colsB,
    int tilesX,
    int tilesY
) {
    __shared__ double tileA[TILE_SIZE][TILE_SIZE];
    __shared__ double tileB[TILE_SIZE][TILE_SIZE];

    const int tx = threadIdx.x;
    const int ty = threadIdx.y;
    const int tileCount = (colsA + TILE_SIZE - 1) / TILE_SIZE;
    const int totalTiles = tilesX * tilesY;

    for (int tileIdx = blockIdx.x; tileIdx < totalTiles; tileIdx += gridDim.x) {
        const int tileRow = tileIdx / tilesX;
        const int tileCol = tileIdx % tilesX;

        const int row = tileRow * TILE_SIZE + ty;
        const int col = tileCol * TILE_SIZE + tx;

        double sum = 0.0;

        for (int t = 0; t < tileCount; t++) {
            const int aCol = t * TILE_SIZE + tx;
            const int bRow = t * TILE_SIZE + ty;

            tileA[ty][tx] =
                (row < rowsA && aCol < colsA) ? A[row * colsA + aCol] : 0.0;

            tileB[ty][tx] =
                (bRow < colsA && col < colsB) ? B[bRow * colsB + col] : 0.0;

            __syncthreads();

            #pragma unroll
            for (int k = 0; k < TILE_SIZE; k++) {
                sum += tileA[ty][k] * tileB[k][tx];
            }

            __syncthreads();
        }

        if (row < rowsA && col < colsB) {
            C[row * colsB + col] = sum;
        }

        __syncthreads();
    }
}


CudaComputer::CudaComputer() {
    int deviceCount = 0;
    checkCuda(cudaGetDeviceCount(&deviceCount), "cudaGetDeviceCount");

    if (deviceCount == 0) {
        throw std::runtime_error("No CUDA-capable GPU found");
    }

    checkCuda(cudaSetDevice(0), "cudaSetDevice");
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

    checkCuda(cudaGetLastError(), "generateMatrixKernel launch");
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

    const int tilesX = (colsB + TILE_SIZE - 1) / TILE_SIZE;
    const int tilesY = (rowsA + TILE_SIZE - 1) / TILE_SIZE;
    const size_t totalTiles = static_cast<size_t>(tilesX) * tilesY;

    // Сколько блоков GPU держит одновременно (оценка по потокам на SM).
    constexpr unsigned TILE_THREADS = TILE_SIZE * TILE_SIZE; // 256
    unsigned blocksPerSM = deviceProps_.maxThreadsPerMultiProcessor / TILE_THREADS;
    if (blocksPerSM == 0) blocksPerSM = 1;
    const size_t residentCapacity =
        static_cast<size_t>(deviceProps_.multiProcessorCount) * blocksPerSM;

    size_t blocks = residentCapacity * blockSizeThreads_ / TILE_THREADS;
    blocks = std::max<size_t>(1, std::min(blocks, totalTiles));

    dim3 block(TILE_SIZE, TILE_SIZE);
    dim3 grid(static_cast<unsigned>(blocks));

    multiplyMatricesTiledKernel<<<grid, block>>>(
        deviceA, deviceB, deviceC, rowsA, colsA, colsB, tilesX, tilesY
    );

    checkCuda(cudaGetLastError(), "multiplyMatricesTiledKernel launch");
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
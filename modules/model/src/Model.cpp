#include "Model.h"
#include "FileWorker.h"

#include <chrono>
#include <cstddef>
#include <cstdint>
#include <random>
#include <stdexcept>
#include <iostream>

Model::Model(size_t heightA, size_t widthA, size_t widthB) {
    if (heightA <= 0)
        throw std::invalid_argument("Height should be more than zero.");
    if (widthA <= 0)
        throw std::invalid_argument("Width should be more than zero.");
    if (widthB <= 0)
        throw std::invalid_argument("Width should be more than zero.");
    
    this->heightA = heightA;
    this->widthA = widthA;
    this->widthB = widthB;

    blockThreadCap = computer->getCapBlockSizeThreads();

    setReady();
}

std::string Model::getComputerName() {
    return computer->getName();
}

void Model::setOnBlockCallback(const std::function<void()> callback) {
    callbacks_block.push_back(callback);
}

void Model::setOnReadyCallback(const std::function<void()> callback) {
    callbacks_ready.push_back(callback);
}

void Model::setOnLastTimeUpdateCallback(const std::function<void()> callback) {
    callbacks_timeUpdate.push_back(callback);
}

void Model::setOnStatusUpdateCallback(const std::function<void()> callback) {
    callbacks_statusUpdate.push_back(callback);
}

void Model::setOnTimesUpdateCallback(const std::function<void()> callback) {
    callbacks_benchTimesUpdate.push_back(callback);
}

size_t Model::setHeightA(size_t height) {
    if (ready && height > 0) {
        heightA = height;
    }
    return heightA;
}

size_t Model::setWidthA(size_t width) {
    if (ready && width > 0) {
        widthA = width;
    }
    return widthA;
}

size_t Model::setWidthB(size_t width) {
    if (ready && width > 0) {
        widthB = width;
    }
    return widthB;
}

const std::string& Model::setFilePathA(const std::string& filePath){
    if (ready && !filePath.empty()){
        unsetReady();
        readMatrixA(filePath);
        setReady();
    }
    return filePathA;
}

const std::string& Model::setFilePathB(const std::string& filePath){
    if (ready && !filePath.empty()){
        unsetReady();
        readMatrixB(filePath);
        setReady();
    }
    return filePathB;
}

const std::string& Model::setFilePathO(const std::string& filePath){
    if (ready && !filePath.empty()){
        filePathO = filePath;
    }
    return filePathO;
}

const std::string& Model::setStatus(const std::string& status){
    if (ready){
        this->status = status;
    }
    return this->status;
}

size_t Model::setBlockSize(size_t size) {
    if (ready && size > 0 && size <= blockThreadCap) {
        try {
            computer->setBlockSizeThreads(size);
        } catch (const std::invalid_argument& e) {
            return blockSize;
        }
        blockSize = size;
    }

    return blockSize;
}

size_t Model::getHeightA() const{
    return heightA;
}

size_t Model::getWidthA() const{
    return widthA;
}

size_t Model::getWidthB() const{
    return widthB;
}

const std::string& Model::getFilePathA() const{
    return filePathA;
}

const std::string& Model::getFilePathB() const{
    return filePathB;
}

const std::string& Model::getFilePathO() const{
    return filePathO;
}

const std::string& Model::getStatus() const{
    return status;
}

size_t Model::getBlockSize() const {
    return blockSize;
}

size_t Model::getBlockSizeCap() const {
    return blockThreadCap;
}

uint64_t Model::getLastTimeNS() const {
    return lastTimeNS;
}

std::span<const BenchNode> Model::getBenchNodes() const { return benchNodes; }

bool Model::isReady() const { return ready; }

void Model::genA(){
    unsetReady();
    matrixA = computer->genMatrix(heightA, widthA);
    status = "Сгенерирована матрица A";
    invoke(callbacks_statusUpdate);
    setReady();
}

void Model::genB(){
    unsetReady();
    matrixB = computer->genMatrix(widthA, widthB);
    status = "Сгенерирована матрица B";
    invoke(callbacks_statusUpdate);
    setReady();
}

void Model::start() {
    if (!ready)
        return;
    unsetReady();
    auto start = std::chrono::high_resolution_clock::now();

    status = "Запущены вычисления...";
    invoke(callbacks_statusUpdate);

    try{
        matrixO = computer->mulMatrixes(matrixA.get(), matrixB.get());
    } catch (const std::invalid_argument& e){
        status = "Ошибка: " + std::string(e.what());
        invoke(callbacks_statusUpdate);

        setReady();
        return;
    }

    status = "Итоговая матрица посчитана";
    invoke(callbacks_statusUpdate);

    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);
    lastTimeNS = static_cast<uint64_t>(duration.count());

    invoke(callbacks_timeUpdate);

    writeMatrixO();

    setReady();
}

void Model::stop() {
    if (!ready)
        stopRequested = true;
}

void Model::benchmark() {
    if (!ready)
        return;

    benchNodes.clear();
    static const size_t blockThreads[] {1024, 512, 256, 128, 64, 32};
    unsetReady();
   
    if (computer->isCPU()) {
        for (int i = computer->getCapBlockSizeThreads(); i > 0; i--) {
            computer->setBlockSizeThreads(i);

            auto start = std::chrono::high_resolution_clock::now();

            status = "Запущены вычисления на кол-ве потоков: " + std::to_string(i);
            invoke(callbacks_statusUpdate);

            matrixO = computer->mulMatrixes(matrixA.get(), matrixB.get());

            auto end = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);
            lastTimeNS = static_cast<uint64_t>(duration.count());

            auto result = BenchNode {};
            result.time = lastTimeNS;
            result.threads = i;
            benchNodes.push_back(result);

            invoke(callbacks_timeUpdate);
            
            if (stopRequested)
                break;
        }
        invoke(callbacks_benchTimesUpdate);
    } else {
        for (size_t threads : blockThreads) {
            if (threads > blockThreadCap)
                continue;
            computer->setBlockSizeThreads(threads);

            auto start = std::chrono::high_resolution_clock::now();

            status = "Запущены вычисления на размере блоков: " + std::to_string(threads);
            invoke(callbacks_statusUpdate);

            matrixO = computer->mulMatrixes(matrixA.get(), matrixB.get());

            auto end = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);
            lastTimeNS = static_cast<uint64_t>(duration.count());

            auto result = BenchNode {};
            result.time = lastTimeNS;
            result.threads = threads;
            benchNodes.push_back(result);

            invoke(callbacks_timeUpdate);

            if (stopRequested)
                break;
        }
        invoke(callbacks_benchTimesUpdate);
    }

    status = "Бенчмарк завершён.";
    invoke(callbacks_statusUpdate);

    computer->setBlockSizeThreads(blockSize);
    setReady();
}

void Model::invoke(std::vector<std::function<void()>> &callbacks) {
    for (auto &cb : callbacks)
        cb();
}

void Model::setReady() {
    if (!ready) {
        ready = true;
        stopRequested = false;
        invoke(callbacks_ready);
    }
}

void Model::unsetReady() {
    if (ready) {
        ready = false;
        invoke(callbacks_block);
    }
}

void Model::readMatrixA(const std::string& filePath){
    try{
        matrixA = fileWorker->readMatrix(filePath, heightA, widthA);
        status = "Прочтена матрица A";
        invoke(callbacks_statusUpdate);
        filePathA = filePath;

    } catch(const std::runtime_error& e){
        status = "Ошибка: " + std::string(e.what());
        invoke(callbacks_statusUpdate);
    }
}

void Model::readMatrixB(const std::string& filePath){
    try{
        matrixB = fileWorker->readMatrix(filePath, widthA, widthB);
        status = "Прочтена матрица B";
        invoke(callbacks_statusUpdate);
        filePathB = filePath;

    } catch(const std::runtime_error& e){
        status = "Ошибка: " + std::string(e.what());
        invoke(callbacks_statusUpdate);
    }
}

void Model::writeMatrixO(){
    try{
        fileWorker->writeMatrix(filePathO, matrixO.get());
        status = "Записана итоговая матрица";
        invoke(callbacks_statusUpdate);

    } catch(const std::runtime_error& e){
        status = "Ошибка: " + std::string(e.what());
        invoke(callbacks_statusUpdate);
    }
}
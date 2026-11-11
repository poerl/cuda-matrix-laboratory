#pragma once

#include <cstddef>
#include <cstdint>
#include <functional>
#include <memory>
#include <random>
#include <string>
#include <vector>

#include "AbstractComputer.h"
#include "IModel.h"
#include "FileWorker.h"
#include "IArray2D.h"
#include "dtos.h"


class Model : public IModel {
    private:
        std::vector<std::function<void()>> callbacks_block;
        std::vector<std::function<void()>> callbacks_ready;
        std::vector<std::function<void()>> callbacks_timeUpdate;
        std::vector<std::function<void()>> callbacks_statusUpdate;
        std::vector<std::function<void()>> callbacks_benchTimesUpdate;

        std::string status = "418";

        size_t heightA = 0;
        size_t widthA = 0;
        size_t widthB = 0;

        std::unique_ptr<IArray2D> matrixA;
        std::unique_ptr<IArray2D> matrixB;
        std::unique_ptr<IArray2D> matrixO;

        std::unique_ptr<FileWorker> fileWorker = FileWorker::create();


        std::string filePathA = "";
        std::string filePathB = "";
        std::string filePathO = "";
        
        size_t blockSize = 1;
        size_t blockThreadCap = 1;

        std::vector<BenchNode> benchNodes{};

        bool ready = false;
        bool stopRequested = false;
        uint64_t lastTimeNS = 0;

        std::unique_ptr<AbstractComputer> computer = AbstractComputerUtils::createAbstractComputer();

    public: // constructors
        Model(size_t heightA = 100, size_t widthA = 100, size_t widthB = 100);
    public: // public methods
        std::string getComputerName();
    public: // inherited methods
        void setOnBlockCallback(const std::function<void()> callback) override;

        void setOnReadyCallback(const std::function<void()> callback) override;

        void setOnLastTimeUpdateCallback(const std::function<void()> callback) override;

        void setOnStatusUpdateCallback(const std::function<void()> callback) override;

        void setOnTimesUpdateCallback(const std::function<void()> callback) override;

        size_t setHeightA(size_t height) override;
        size_t setWidthA(size_t width) override;
        size_t setWidthB(size_t width) override;

        const std::string& setFilePathA(const std::string& filePath) override;
        const std::string& setFilePathB(const std::string& filePath) override;
        const std::string& setFilePathO(const std::string& filePath) override;

        const std::string& setStatus(const std::string& status) override;

        size_t setBlockSize(size_t size) override;


        size_t getHeightA() const override;
        size_t getWidthA() const override;
        size_t getWidthB() const override;

        const std::string& getFilePathA() const override;
        const std::string& getFilePathB() const override;
        const std::string& getFilePathO() const override;

        const std::string& getStatus() const override;

        size_t getBlockSize() const override;

        size_t getBlockSizeCap() const override;

        uint64_t getLastTimeNS() const override;

        std::span<const BenchNode> getBenchNodes() const override;

        virtual bool isReady() const override;

        void genA() override;
        void genB() override;

        void start() override;

        void stop() override;

        void benchmark() override;

    private:
        void invoke(std::vector<std::function<void()>> &callbacks);

        void setReady();

        void unsetReady();

        void readMatrixA(const std::string& filePath);
        void readMatrixB(const std::string& filePath);

        void writeMatrixO();
};

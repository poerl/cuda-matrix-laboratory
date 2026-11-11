#pragma once

#include <cstddef>
#include <cstdint>
#include <functional>
#include <span>
#include <string>

#include "dtos.h"

class IModel {
    public:
        virtual ~IModel() = default;
        
        // Callbacks
        virtual void setOnBlockCallback(const std::function<void()> callback) = 0;
        virtual void setOnReadyCallback(const std::function<void()> callback) = 0;

        virtual void setOnLastTimeUpdateCallback(const std::function<void()> callback) = 0;
        virtual void setOnStatusUpdateCallback(const std::function<void()> callback) = 0;
        
        virtual void setOnTimesUpdateCallback(const std::function<void()> callback) = 0;

        // Methods
        // Setters
        virtual size_t setHeightA(size_t height) = 0;
        virtual size_t setWidthA(size_t width) = 0;
        virtual size_t setWidthB(size_t width) = 0;

        virtual const std::string& setFilePathA(const std::string& filePath) = 0;
        virtual const std::string& setFilePathB(const std::string& filePath) = 0;
        virtual const std::string& setFilePathO(const std::string& filePath) = 0;

        virtual const std::string& setStatus(const std::string& status) = 0;

        virtual size_t setBlockSize(size_t size) = 0;

        // Getters
        virtual size_t getHeightA() const = 0;
        virtual size_t getWidthA() const = 0;
        virtual size_t getWidthB() const = 0;

        virtual const std::string& getFilePathA() const = 0;
        virtual const std::string& getFilePathB() const = 0;
        virtual const std::string& getFilePathO() const = 0;

        virtual const std::string& getStatus() const = 0;

        virtual size_t getBlockSize() const = 0;
        virtual size_t getBlockSizeCap() const = 0;

        virtual uint64_t getLastTimeNS() const = 0;
        virtual std::span<const BenchNode> getBenchNodes() const = 0;

        virtual bool isReady() const = 0;

        virtual void genA() = 0;
        virtual void genB() = 0;

        virtual void start() = 0;
        virtual void stop() = 0;
        virtual void benchmark() = 0;
};
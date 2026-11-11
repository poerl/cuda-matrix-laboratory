#pragma once

#include <qobject.h>

#include <cstddef>
#include <span>
#include <string>

#include "dtos.h"

class IMainView {
    public:
        virtual ~IMainView() = default;

        // Signals
        std::function<void(size_t)> sigHeightAChanged;
        std::function<void(size_t)> sigWidthAChanged;
        std::function<void(size_t)> sigWidthBChanged;

        std::function<void(size_t)> sigBlockSizeChanged;
        
        std::function<void()> sigStartRequested;
        std::function<void()> sigStopRequested;
        std::function<void()> sigBenchRequested;

        std::function<void()> sigGenARequested;
        std::function<void()> sigGenBRequested;

        std::function<void(const std::string& filePath)> sigFilePathAChanged;
        std::function<void(const std::string& filePath)> sigFilePathBChanged;
        std::function<void(const std::string& filePath)> sigFilePathOChanged;


        // Slots
        virtual void onLastTimeUpdate(uint64_t timeNS) = 0;
        virtual void onBlock() = 0;
        virtual void onReady() = 0;

        virtual void onStatusUpdate(const std::string& status) = 0;

        virtual void onTimesDataUpdate(std::span<const BenchNode> benches) = 0;


        // Setters
        virtual void setHeightA(size_t height) = 0;
        virtual void setWidthA(size_t width) = 0;
        virtual void setWidthB(size_t width) = 0;

        virtual void setFilePathA(const std::string& filePath) = 0;
        virtual void setFilePathB(const std::string& filePath) = 0;
        virtual void setFilePathO(const std::string& filePath) = 0;

        virtual void setBlockSize(size_t size) = 0;
        virtual void setBlocksMaximum(size_t max) = 0;

        virtual void showWindow() = 0;
};
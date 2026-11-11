#pragma once

#include <cstdint>
#include <cstddef>

#include <QObject>
#include <qtmetamacros.h>

#include "IModel.h"
#include "IMainView.h"

class Presenter: public QObject {
    Q_OBJECT
    public:
        Presenter(IModel *model,IMainView *mainView, QObject *parent = nullptr);

        void show();

    private:
        IModel *model;
        IMainView *mainView;

        void sigHeightAChanged(size_t height);
        void sigWidthAChanged(size_t width);
        void sigWidthBChanged(size_t width);

        void sigBlockSizeChanged(size_t size);
        
        void sigStartRequested();
        void sigStopRequested();
        void sigBenchRequested();
        
        void sigGenARequested();
        void sigGenBRequested();

        void sigFilePathAChanged(const std::string& filePath);
        void sigFilePathBChanged(const std::string& filePath);
        void sigFilePathOChanged(const std::string& filePath);
        
        void connectToViewCallbacks();
        void connectToModelCallbacks();
        void fillStartData();
    };
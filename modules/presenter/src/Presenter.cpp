#include "Presenter.h"

#include <cstdint>
#include <cstddef>

#include <QObject>
#include <QtConcurrent>
#include <qtmetamacros.h>

#include "IModel.h"
#include "IMainView.h"

Presenter::Presenter(IModel *model,IMainView *mainView, QObject *parent) : QObject(parent) {
    this->model = model;
    this->mainView = mainView;

    connectToViewCallbacks();
    connectToModelCallbacks();
    fillStartData();
}


void Presenter::show(){
    mainView->showWindow();
}


void Presenter::connectToViewCallbacks(){
    mainView->sigHeightAChanged = [this](size_t height){
        this->sigHeightAChanged(height);
    };
    mainView->sigWidthAChanged = [this](size_t width){
        this->sigWidthAChanged(width);
    };
    mainView->sigWidthBChanged = [this](size_t width){
        this->sigWidthBChanged(width);
    };

    mainView->sigBlockSizeChanged = [this](size_t size){
        this->sigBlockSizeChanged(size);
    };

    mainView->sigStartRequested = [this](){
        this->sigStartRequested();
    };
    mainView->sigStopRequested = [this](){
        this->sigStopRequested();
    };
    mainView->sigBenchRequested = [this](){
        this->sigBenchRequested();
    };

    mainView->sigGenARequested = [this](){
        this->sigGenARequested();
    };
    mainView->sigGenBRequested = [this](){
        this->sigGenBRequested();
    };

    mainView->sigFilePathAChanged = [this](const std::string& filePath){
        this->sigFilePathAChanged(filePath);
    };
    mainView->sigFilePathBChanged = [this](const std::string& filePath){
        this->sigFilePathBChanged(filePath);
    };
    mainView->sigFilePathOChanged = [this](const std::string& filePath){
        this->sigFilePathOChanged(filePath);
    };
}

void Presenter::connectToModelCallbacks() {
    model->setOnBlockCallback([this]() {
        QMetaObject::invokeMethod(this, [this]() { mainView->onBlock(); });
    });

    model->setOnReadyCallback([this]() {
        QMetaObject::invokeMethod(this, [this]() { mainView->onReady(); });
    });

    model->setOnLastTimeUpdateCallback([this]() {
        QMetaObject::invokeMethod(this, [this]() {
            mainView->onLastTimeUpdate(model->getLastTimeNS());
        });
    });

    model->setOnStatusUpdateCallback([this]() {
        QMetaObject::invokeMethod(this, [this]() {
            mainView->onStatusUpdate(model->getStatus());
        });
    });

    model->setOnTimesUpdateCallback([this]() {
        QMetaObject::invokeMethod(this, [this]() {
            mainView->onTimesDataUpdate(model->getBenchNodes());
        });
    });
}



void Presenter::fillStartData(){
    mainView->setHeightA(model->getHeightA());
    mainView->setWidthA(model->getWidthA());
    mainView->setWidthB(model->getWidthB());
    
    mainView->setBlocksMaximum(model->getBlockSizeCap());
    mainView->setBlockSize(model->getBlockSize());

    mainView->setFilePathA(model->getFilePathA());
    mainView->setFilePathB(model->getFilePathB());
    mainView->setFilePathO(model->getFilePathO());

    mainView->onStatusUpdate(model->getStatus());
}

void Presenter::sigHeightAChanged(size_t height){
    mainView->setHeightA(model->setHeightA(height));
}

void Presenter::sigWidthAChanged(size_t width){
    mainView->setWidthA(model->setWidthA(width));
}

void Presenter::sigWidthBChanged(size_t width){
    mainView->setWidthB(model->setWidthB(width));
}

void Presenter::sigBlockSizeChanged(size_t size){
    mainView->setBlockSize(model->setBlockSize(size));
}


void Presenter::sigStartRequested(){
    if (!model->isReady())
        return;
    mainView->onBlock();
    QThreadPool::globalInstance()->start([this]() {
        model->start();
    });
}

void Presenter::sigStopRequested(){
    model->stop();
}

void Presenter::sigBenchRequested(){
    if (!model->isReady())
        return;
    mainView->onBlock();
    QThreadPool::globalInstance()->start([this]() {
        model->benchmark();
    });
}

void Presenter::sigGenARequested(){
    if (!model->isReady())
        return;
    mainView->onBlock();
    QThreadPool::globalInstance()->start([this]() {
        model->genA();
    });
}

void Presenter::sigGenBRequested(){
    if (!model->isReady())
        return;
    mainView->onBlock();
    QThreadPool::globalInstance()->start([this]() {
        model->genB();
    });
}

void Presenter::sigFilePathAChanged(const std::string& filePath){
    mainView->setFilePathA(model->setFilePathA(filePath));
}

void Presenter::sigFilePathBChanged(const std::string& filePath){
    mainView->setFilePathB(model->setFilePathB(filePath));
}

void Presenter::sigFilePathOChanged(const std::string& filePath){
    mainView->setFilePathO(model->setFilePathO(filePath));
}

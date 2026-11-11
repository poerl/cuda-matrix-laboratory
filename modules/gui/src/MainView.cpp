#include "MainView.h"

#include <cstdint>
#include <string>

#include <QMainWindow>
#include <QWidget>
#include <QTabWidget>
#include <QVBoxLayout>
#include <QString>
#include <QCloseEvent>

#include "dtos.h"

MainView::MainView(QString label, QWidget* parent) : QMainWindow(parent) {
    setWindowTitle(label);

    tabWidget = new QTabWidget(this);
    
    inputWidget = new InputWidget(tabWidget);
    perfWidget = new QWidget(tabWidget);

    perfLayout = new QVBoxLayout(perfWidget);
    perfChart = new PerformanceChart(perfWidget, "Время работы потоков в мс.");

    initUI();
    connectSignals();
}


void MainView::initUI(){
    setCentralWidget(tabWidget);

    tabWidget->addTab(inputWidget, "Матрицы");
    tabWidget->addTab(perfWidget, "Бэнчмарк");

    perfLayout->addWidget(perfChart);
    perfChart->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
}


void MainView::connectSignals(){
    connect(inputWidget, &InputWidget::sigBlockSizeChanged, this, [this](size_t size) {
        if (sigBlockSizeChanged) {
            sigBlockSizeChanged(size);
        }
    });

    connect(inputWidget, &InputWidget::sigStartRequested, this, [this]() {
        if (sigStartRequested) {
            sigStartRequested();
        }
    });
    connect(inputWidget, &InputWidget::sigStopRequested, this, [this]() {
        if (sigStopRequested) {
            sigStopRequested();
        }
    });
    connect(inputWidget, &InputWidget::sigBenchRequested, this, [this]() {
        if (sigBenchRequested) {
            sigBenchRequested();
        }
    });

    connect(inputWidget, &InputWidget::sigGenARequested, this, [this]() {
        if (sigGenARequested) {
            sigGenARequested();
        }
    });
    connect(inputWidget, &InputWidget::sigGenBRequested, this, [this]() {
        if (sigGenBRequested) {
            sigGenBRequested();
        }
    });

    connect(inputWidget, &InputWidget::sigHeightAChanged, this, [this](size_t height) {
        if (sigHeightAChanged) {
            sigHeightAChanged(height);
        }
    });
    connect(inputWidget, &InputWidget::sigWidthAChanged, this, [this](size_t width) {
        if (sigWidthAChanged) {
            sigWidthAChanged(width);
        }
    });
    connect(inputWidget, &InputWidget::sigWidthBChanged, this, [this](size_t width) {
        if (sigWidthBChanged) {
            sigWidthBChanged(width);
        }
    });

    connect(inputWidget, &InputWidget::sigFilePathAChanged, this, [this](const std::string& filePath) {
        if (sigFilePathAChanged) {
            sigFilePathAChanged(filePath);
        }
    });
    connect(inputWidget, &InputWidget::sigFilePathBChanged, this, [this](const std::string& filePath) {
        if (sigFilePathBChanged) {
            sigFilePathBChanged(filePath);
        }
    });
    connect(inputWidget, &InputWidget::sigFilePathOChanged, this, [this](const std::string& filePath) {
        if (sigFilePathOChanged) {
            sigFilePathOChanged(filePath);
        }
    });
}


void MainView::showWindow(){
    show();
}


void MainView::closeEvent(QCloseEvent *event){
    if (sigStopRequested) {
            sigStopRequested();
    }
    event->accept();
}


void MainView::onLastTimeUpdate(uint64_t timeNS){
    inputWidget->onLastTimeUpdate(timeNS);
}

void MainView::onBlock(){
    inputWidget->onBlock();
}

void MainView::onReady(){
    inputWidget->onReady();
}

void MainView::onStatusUpdate(const std::string& status){
    inputWidget->onStatusUpdate(status);
}

void MainView::onTimesDataUpdate(std::span<const BenchNode> benches){
    perfChart->onUpdateData(benches);
}

void MainView::setBlockSize(size_t size){
    inputWidget->setBlockSize(size);
}

void MainView::setBlocksMaximum(size_t max){
    inputWidget->setBlocksMaximum(max);
}        

void MainView::setHeightA(size_t height){
    inputWidget->setHeightA(height);
}

void MainView::setWidthA(size_t width){
    inputWidget->setWidthA(width);
}

void MainView::setWidthB(size_t width){
    inputWidget->setWidthB(width);
}

void MainView::setFilePathA(const std::string& filePath){
    inputWidget->setFilePathA(filePath);
}

void MainView::setFilePathB(const std::string& filePath){
    inputWidget->setFilePathB(filePath);
}

void MainView::setFilePathO(const std::string& filePath){
    inputWidget->setFilePathO(filePath);
}
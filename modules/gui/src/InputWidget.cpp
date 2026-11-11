#include "InputWidget.h"

#include <cstddef>
#include <cstdint>
#include <string>

#include <QComboBox>
#include <QDoubleSpinBox>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QSpinBox>
#include <QWidget>

#include "MatrixWidget.h"
#include "size_selector.h"

InputWidget::InputWidget(QWidget *parent): QWidget(parent){
    mainLayout = new QVBoxLayout(this);
    sizesLayout = new QGridLayout();
    matrixesLayout = new QGridLayout();
    blockSizeLayout = new QHBoxLayout();
    buttonsLayout = new QHBoxLayout();
    lastTimeLayout = new QHBoxLayout();

    heightA = new SizeSelector(1'000, this);
    widthA = new SizeSelector(1'000, this);
    widthB = new SizeSelector(1'000, this);

    matrixA = new MatrixWidget(this);
    matrixB = new MatrixWidget(this);
    matrixO = new MatrixWidget(this, false);

    blockSize = new QSpinBox(this);

    start = new QPushButton("Старт", this);
    stop = new QPushButton("Стоп", this);
    bench = new QPushButton("Бэнч", this);
    
    status = new QLabel("Велкоме ту матрих мультиплуер", this);
    lastTime = new QLabel("0", this);

    initUI();
    connectSignals();
}


void InputWidget::initUI(){
    sizesLayout->addWidget(new QLabel("Размеры матриц", this), 0, 0, 1, 5);
    sizesLayout->addWidget(heightA, 1, 0);
    sizesLayout->addWidget(new QLabel("X", this), 1, 1);
    sizesLayout->addWidget(widthA, 1, 2);
    sizesLayout->addWidget(new QLabel("X", this), 1, 3);
    sizesLayout->addWidget(widthB, 1, 4);

    matrixesLayout->addWidget(new QLabel("Матрица A", this), 0, 0);
    matrixesLayout->addWidget(matrixA, 1, 0);
    matrixesLayout->addWidget(new QLabel("Матрица B", this), 0, 1);
    matrixesLayout->addWidget(matrixB, 1, 1);

    blockSizeLayout->addWidget(new QLabel("Кол-во блоков(потоков)", this));
    blockSizeLayout->addWidget(blockSize);

    buttonsLayout->addWidget(start);
    buttonsLayout->addWidget(stop);
    buttonsLayout->addWidget(bench);

    lastTimeLayout->addWidget(new QLabel("Время подсчёта: ", this));
    lastTimeLayout->addWidget(lastTime);

    QWidget *sizesWidget = new QWidget(this);
    sizesWidget->setLayout(sizesLayout);
    QWidget *matrixesWidget = new QWidget(this);
    matrixesWidget->setLayout(matrixesLayout);
    QWidget *blockSizeWidget = new QWidget(this);
    blockSizeWidget->setLayout(blockSizeLayout);
    QWidget *buttonsWidget = new QWidget(this);
    buttonsWidget->setLayout(buttonsLayout);
    QWidget *lastTimeWidget = new QWidget(this);
    lastTimeWidget->setLayout(lastTimeLayout);
    
    mainLayout->addWidget(sizesWidget);
    mainLayout->addWidget(matrixesWidget);
    mainLayout->addWidget(new QLabel("Матрица вывода", this));
    mainLayout->addWidget(matrixO);
    mainLayout->addWidget(blockSizeWidget);
    mainLayout->addWidget(buttonsWidget);
    mainLayout->addWidget(status);
    mainLayout->addWidget(lastTimeWidget);

    mainLayout->setAlignment(Qt::AlignCenter);

    setLayout(mainLayout);

    blockSize->setMinimum(1);
    blockSize->setMaximum(1024);
    blockSize->setValue(32);
}


void InputWidget::connectSignals(){
    connect(heightA, &SizeSelector::sig_size_changed, this, &InputWidget::sigHeightAChanged);
    connect(widthA, &SizeSelector::sig_size_changed, this, &InputWidget::sigWidthAChanged);
    connect(widthB, &SizeSelector::sig_size_changed, this, &InputWidget::sigWidthBChanged);

    connect(matrixA, &MatrixWidget::sigGenRequested, this, &InputWidget::sigGenARequested);
    connect(matrixB, &MatrixWidget::sigGenRequested, this, &InputWidget::sigGenBRequested);

    connect(matrixA, &MatrixWidget::sigFilePathChanged, this, &InputWidget::sigFilePathAChanged);
    connect(matrixB, &MatrixWidget::sigFilePathChanged, this, &InputWidget::sigFilePathBChanged);
    connect(matrixO, &MatrixWidget::sigFilePathChanged, this, &InputWidget::sigFilePathOChanged);

    connect(blockSize, &QSpinBox::editingFinished, this, &InputWidget::onBlockSizeChanged);

    connect(start, &QPushButton::clicked, this, &InputWidget::sigStartRequested);
    connect(stop, &QPushButton::clicked, this, &InputWidget::sigStopRequested);
    connect(bench, &QPushButton::clicked, this, &InputWidget::sigBenchRequested);

}


void InputWidget::onBlockSizeChanged(){
    emit sigBlockSizeChanged(static_cast<size_t>(blockSize->value()));
}

void InputWidget::onLastTimeUpdate(uint64_t timeNS){
    lastTime->setText(
        QString::number(static_cast<double>(timeNS) / 1'000'000'000.0)
    );
}

void InputWidget::onStatusUpdate(const std::string& status){
    this->status->setText(QString::fromStdString(status));
}


void InputWidget::onBlock(){
        heightA->setEnabled(false);
        widthA->setEnabled(false);
        widthB->setEnabled(false);

        matrixA->onBlock();
        matrixB->onBlock();
        matrixO->onBlock();

        blockSize->setEnabled(false);
        start->setEnabled(false);
        bench->setEnabled(false);
        lastTime->setEnabled(false);

        stop->setEnabled(true);
}

void InputWidget::onReady(){
        heightA->setEnabled(true);
        widthA->setEnabled(true);
        widthB->setEnabled(true);

        matrixA->onReady();
        matrixB->onReady();
        matrixO->onReady();

        blockSize->setEnabled(true);
        start->setEnabled(true);
        bench->setEnabled(true);
        lastTime->setEnabled(true);

        stop->setEnabled(false);
}


void InputWidget::setHeightA(size_t height){
    heightA->setSize(height);
}
        
void InputWidget::setWidthA(size_t width){
    widthA->setSize(width);
}
void InputWidget::setWidthB(size_t width){
    widthB->setSize(width);
}

void InputWidget::setFilePathA(const std::string& filePath){
    matrixA->setFilePath(filePath);
}
void InputWidget::setFilePathB(const std::string& filePath){
    matrixB->setFilePath(filePath);
}
void InputWidget::setFilePathO(const std::string& filePath){
    matrixO->setFilePath(filePath);
}

void InputWidget::setBlockSize(size_t size){
    blockSize->setValue(size);
}

void InputWidget::setBlocksMaximum(size_t max){
    blockSize->setMaximum(static_cast<int>(max));
}
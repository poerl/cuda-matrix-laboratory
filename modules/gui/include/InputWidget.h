#pragma once

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

class InputWidget: public QWidget{
    Q_OBJECT
    public:
        InputWidget(QWidget *parent = nullptr);

        void setHeightA(size_t height);
        void setWidthA(size_t width);
        void setWidthB(size_t width);

        void setFilePathA(const std::string& filePath);
        void setFilePathB(const std::string& filePath);
        void setFilePathO(const std::string& filePath);

        void setBlockSize(size_t size);
        void setBlocksMaximum(size_t max);
    
    signals:
        void sigBlockSizeChanged(size_t size);
        
        void sigStartRequested();
        void sigStopRequested();
        void sigBenchRequested();

        void sigGenARequested();
        void sigGenBRequested();
        
        void sigHeightAChanged(size_t height);
        void sigWidthAChanged(size_t width);
        void sigWidthBChanged(size_t width);

        void sigFilePathAChanged(const std::string& filePath);
        void sigFilePathBChanged(const std::string& filePath);
        void sigFilePathOChanged(const std::string& filePath);
    
    public slots:
        void onLastTimeUpdate(uint64_t timeNS);
        void onBlock();
        void onReady();

        void onStatusUpdate(const std::string& status);

    private slots:
        void onBlockSizeChanged();
        
    private:
        QVBoxLayout *mainLayout;
        QGridLayout *sizesLayout;
        QGridLayout *matrixesLayout;
        QHBoxLayout *blockSizeLayout;
        QHBoxLayout *buttonsLayout;
        QHBoxLayout *lastTimeLayout;

        SizeSelector *heightA;
        SizeSelector *widthA;
        SizeSelector *widthB;
        
        MatrixWidget *matrixA;
        MatrixWidget *matrixB;
        MatrixWidget *matrixO;

        QSpinBox *blockSize;

        QPushButton *start;
        QPushButton *stop;
        QPushButton *bench;

        QLabel *status;
        QLabel *lastTime;

        void initUI();
        void connectSignals();
};
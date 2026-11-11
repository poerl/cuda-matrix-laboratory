#include "MatrixWidget.h"

#include <QFileDialog>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QWidget>
#include <QString>

#include <string>

MatrixWidget::MatrixWidget(QWidget *parent, bool canGenerate): QWidget(parent){
    layout = new QVBoxLayout(this);

    if(canGenerate){
        generate = new QPushButton("Генерировать", this);
        openFileDialog = new QPushButton("Читать из файла", this);
    }
    else{
        generate = nullptr;
        openFileDialog = new QPushButton("Сохранить в файл", this);
    }

    filePathLabel = new QLabel("Путь до выбранного файла", this);

    this->canGenerate = canGenerate;

    initUI();
    connectSignals();
}


void MatrixWidget::initUI(){
    if(canGenerate){
        layout->addWidget(generate);
    }

    layout->addWidget(openFileDialog);
    layout->addWidget(filePathLabel);
}


void MatrixWidget::connectSignals(){
    if(canGenerate){
        connect(generate, &QPushButton::clicked, this, &MatrixWidget::onGenRequested);
    }
    connect(openFileDialog, &QPushButton::clicked, this, &MatrixWidget::onChooseFile);
}


void MatrixWidget::setFilePath(const std::string& filePath){
    filePathLabel->setText(QString::fromStdString(filePath));
}


void MatrixWidget::onBlock(){
    if(canGenerate){
        generate->setEnabled(false);
    }

    openFileDialog->setEnabled(false);
}

void MatrixWidget::onReady(){
    if(canGenerate){
        generate->setEnabled(true);
    }
    
    openFileDialog->setEnabled(true);
}


void MatrixWidget::onChooseFile(){
    if(canGenerate){
        filePathLabel->setText(QFileDialog::getOpenFileName(this, "Открыть", QDir::homePath()));
    }
    else{
        filePathLabel->setText(QFileDialog::getOpenFileName(this, "Сохранить", QDir::homePath()));
    }
    filePathLabel->setEnabled(true);
    emit sigFilePathChanged(filePathLabel->text().toStdString());
}

void MatrixWidget::onGenRequested(){
    filePathLabel->setEnabled(false);
    emit sigGenRequested();
}
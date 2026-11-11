#pragma once

#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QWidget>

#include <string>

class MatrixWidget: public QWidget{
    Q_OBJECT
    public:
        MatrixWidget(QWidget *parent = nullptr, bool canGenerate = true);

        void setFilePath(const std::string& filePath);

    signals:
        void sigGenRequested();
        void sigFilePathChanged(const std::string& filePath);


    public slots:
        void onBlock();
        void onReady();

    private slots:
        void onChooseFile();
        void onGenRequested();

    private:
        bool canGenerate;

        QVBoxLayout *layout;

        QPushButton *generate;
        QPushButton *openFileDialog;
        QLabel *filePathLabel;

        void initUI();
        void connectSignals();
};
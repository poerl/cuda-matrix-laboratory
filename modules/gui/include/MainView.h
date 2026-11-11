#pragma once

#include <cstddef>
#include <cstdint>
#include <span>

#include <QMainWindow>
#include <QWidget>
#include <QTabWidget>
#include <QVBoxLayout>
#include <QString>
#include <QCloseEvent>

#include "IMainView.h"
#include "InputWidget.h"
#include "performance_chart.h"

class MainView: public QMainWindow, public IMainView{
    Q_OBJECT
    public:
        MainView(QString label, QWidget* parent = nullptr);

        // Setters
        void setHeightA(size_t height) override;
        void setWidthA(size_t width) override;
        void setWidthB(size_t width) override;

        void setFilePathA(const std::string& filePath) override;
        void setFilePathB(const std::string& filePath) override;
        void setFilePathO(const std::string& filePath) override;

        void setBlockSize(size_t size) override;
        void setBlocksMaximum(size_t max) override;

        void showWindow() override;
    
    public slots:
        void onLastTimeUpdate(uint64_t timeNS) override;
        void onBlock() override;
        void onReady() override;
        void onStatusUpdate(const std::string& status) override;
        void onTimesDataUpdate(std::span<const BenchNode> benches) override;

    private:
        QTabWidget *tabWidget;
        InputWidget *inputWidget;
        QWidget *perfWidget;

        QVBoxLayout *perfLayout;
        PerformanceChart *perfChart;

        void initUI();
        void connectSignals();
        void closeEvent(QCloseEvent *event) override;
};
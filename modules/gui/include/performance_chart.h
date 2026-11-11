#pragma once

#include <cstdint>
#include <qchartview.h>
#include <qchart.h>
#include <qbarseries.h>
#include <qbarset.h>
#include <qbarcategoryaxis.h>
#include <qvalueaxis.h>
#include <qstring.h>
#include <vector>

#include "dtos.h"

// using std::vector;

class PerformanceChart: public QChartView{
    Q_OBJECT
public:
    PerformanceChart(QWidget *parent = nullptr, const QString &title = "Performance chart");
    void setData(std::span<const BenchNode>& timesData);
private:
    QChart *m_chart;
    QBarSeries *m_series;
    QBarSet *m_set;
    QBarCategoryAxis *m_axisX;
    QValueAxis *m_axisY;
    bool hasSetData;
public slots:
    void onUpdateData(std::span<const BenchNode>& timesData);
};
#include <algorithm>
#include <cstdint>
#include <performance_chart.h>

#include "dtos.h"

PerformanceChart::PerformanceChart(QWidget *parent, const QString &title) 
    : QChartView(parent)
{
    m_chart = new QChart();
    m_chart -> setTitle(title);
    m_chart->setAnimationOptions(QChart::SeriesAnimations);

    m_series = new QBarSeries();
    m_set = new QBarSet("Values");
    m_series->append(m_set);
    m_chart->addSeries(m_series);

    m_axisX = new QBarCategoryAxis();
    m_chart->addAxis(m_axisX, Qt::AlignBottom);
    m_series->attachAxis(m_axisX);
    
    m_axisY = new QValueAxis();
    m_chart->addAxis(m_axisY, Qt::AlignLeft);
    m_series->attachAxis(m_axisY);

    hasSetData = false;

    setChart(m_chart);
    setRenderHint(QPainter::Antialiasing);
    setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Preferred);
}

void PerformanceChart::setData(std::span<const BenchNode>& timesData){
    uint64_t maxTime = 0;
    for(size_t i = 0; i < timesData.size(); i ++){
        if(i == 0 || timesData[i].time > maxTime)
            maxTime = timesData[i].time;
    }

    m_axisY -> setRange(0, maxTime / 1'000'000.0);

    for(int i = 0; i < timesData.size(); i ++){
        m_set -> append(timesData[i].time / 1'000'000.0);
        m_axisX->append(QString::number(timesData[i].threads));
    }
    hasSetData = true;
}

void PerformanceChart::onUpdateData(std::span<const BenchNode>& timesData){
    if(!hasSetData)
        setData(timesData);
    
    for(int i = 0; i < timesData.size(); i ++){
        m_set -> replace(i, timesData[i].time / 1'000'000.0);
    }

    uint64_t maxTime = 0;
    for(size_t i = 0; i < timesData.size(); i ++){
        if(i == 0 || timesData[i].time > maxTime)
            maxTime = timesData[i].time;
    }
    m_axisY -> setRange(0, maxTime / 1'000'000.0);
}
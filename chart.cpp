#include "chart.h"

Chart::Chart():QChart()
{
    yAxis = new QValueAxis();
    loadAxis = new QValueAxis();
    xAxis = new QValueAxis();
    lineView = new QChartView(this);
    lineSeries = new QLineSeries();
    refValueSeries = new QLineSeries();
    loadMotorSeries = new QLineSeries();
}

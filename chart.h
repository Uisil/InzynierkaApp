#ifndef CHART_H
#define CHART_H

#include <QChart>
#include <QCategoryAxis>
#include <QLineSeries>
#include <QValueAxis>
#include <QChartView>
#include <QPointF>
#include <QList>

class Chart : public QChart
{

public:
    Chart();
    QValueAxis *yAxis;
    QValueAxis *loadAxis;
    QValueAxis *xAxis;
    QChartView *lineView;
    QLineSeries *lineSeries;
    QLineSeries *refValueSeries;
    QLineSeries *loadMotorSeries;
    QList<QPointF> pointSeries;
    QList<QPointF> refPointSeries;
    QList<QPointF> loadMotorPointSeries;
};

#endif // CHART_H

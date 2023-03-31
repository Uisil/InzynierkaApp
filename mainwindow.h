#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QBarSet>
#include <QChart>
#include <QChartView>
#include <QBarSeries>
#include <QBarCategoryAxis>
#include <QValueAxis>
#include <QGridLayout>
#include <QLineSeries>
#include <QPointF>
#include <QList>
#include <QCategoryAxis>

#include <QSerialPortInfo>
#include <QDateTime>
#include <QSerialPort>
#include <QRadioButton>
#include <QTimer>
#include <QFile>
#include <QTextStream>

#include "chart.h"

#include <cstdlib>
#include <stdlib.h>
#include <string>


QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    QSerialPort *port;
    QDateTime *czas;
    Chart *lineChartCurr;
    QGridLayout *vBoxCurr;
    Chart *lineChartSpeed;
    QGridLayout *vBoxSpeed;
    Chart *lineChartPos;
    QGridLayout *vBoxPos;

    QTimer *timeThread;
    QList<QSerialPortInfo> devices;
    QList<QSerialPortInfo> devicesPrev;

    bool read = false;
    bool dataRxCplt = false;

    float timeValue[8000] = {0};
    float measurValue[8000] = {0};
    float measurValueSpeed[8000] = {0};
    float measurValuePos[8000] = {0};
    uint16_t byteReadNo = 0;

    int idxValue = 0;
    uint8_t idx = 0;
    uint8_t tmp[4+4*sizeof(float)] = {0};

private slots:

    void readFromProc();
    void measurCplt();

    void on_przyciskStart_clicked();

    void on_przyciskStop_clicked();

    void on_przyciskAktu_clicked();

    void on_przyciskAktuPos_clicked();

    void on_przycisAktuSpeed_clicked();

    void on_przyciskAutoPos_clicked();

    void on_przyciskAuto_clicked();

    void on_przyciskAutoSpeed_clicked();

    void on_przyciskStartSpeed_clicked();

    void on_przyciskStartPos_clicked();

    void on_SwitchLoadMotor_clicked();

    void on_SwitchLoadMotorSpeed_clicked();

    void on_SwitchLoadMotorPos_clicked();

    void on_trybSter_currentIndexChanged(int index);

    void on_numberRefValueCurr_valueChanged(int arg1);

    void on_numberLoadValueSpeed_valueChanged(int arg1);

    void on_numberLoadValueCurr_valueChanged(int arg1);

    void on_numberRefValueSpeed_valueChanged(int arg1);

    void on_numberRefValuePos_valueChanged(int arg1);

    void on_numberLoadValuePos_valueChanged(int arg1);

    void timer();
    void on_actiondane_pomiarowe_triggered();

private:
    Ui::MainWindow *ui;
    // dodawanie logów na terminalu
    void addLog(QString mess);

    //funkcje odpowiedzialne za wykresy
    void chartCreate();
    void chartInit();
    void chartShow();
    void chartScale(Chart *lineChart, float minValue = 0, float maxValue = 10000 , float minTime = 0, float maxTime = 10000);
    void chartSeriesChanged(Chart *lineChart);
    void addRefValueSeries(Chart *lineChart);
    void addLoadMotorSeries(Chart *lineChart);
    void lineInit(Chart *lineChart, float minValue,float maxValue,float minTime,float maxTime);
    void chartCurrInit();
    void chartSpeedInit();
    void chartPosInit();
    void lineEdit(Chart *lineChart, bool rewSeries, float minValue,float maxValue,float minTime,float maxTime);
    void calcMinMax(Chart *lineChart);
    void mainClear();

    // funkcje ustawiające comboboxy i łączce wartosci tych od trybu sterowania
    void comboBoxSet();
    void comboBoxConnect();

    //funkcje określające port
    void portOpen();
    void portOpenAuto();
    void portSetParameters();

    // funkcja odczytu danych przychodzących
    void dataFrameRead();

    //funkcja ogólna od wysyłki ramki danych
    void writeParameters();

    //funkcje odpowiedzialne za przygotowanie ramek i ich wysyłke
    void motorParameterSend(uint8_t *parametersToSend);
    void refCurrPatternParametersSend(uint8_t *parametersToSend);
    void loadMotorCurrPatternParametersSend(uint8_t *parametersToSend);
    void refSpeedPatternParametersSend(uint8_t *parametersToSend);
    void loadMotorSpeedPatternParametersSend(uint8_t *parametersToSend);
    void refPosPatternParametersSend(uint8_t *parametersToSend);
    void loadMotorPosPatternParametersSend(uint8_t *parametersToSend);
    void currentPIDParametersSend(uint8_t *parametersToSend);
    void speedPIDParametersSend(uint8_t *parametersToSend);
    void posPIDParametersSend(uint8_t *parametersToSend);

    void blockSimTime();
    void blockUnusedPatternRefValueCurr(int arg1);
    void blockUnusedPatternLoadMotorCurr(int arg1);
    void blockUnusedPatternRefValueSpeed(int arg1);
    void blockUnusedPatternLoadMotorSpeed(int arg1);
    void blockUnusedPatternRefValuePos(int arg1);
    void blockUnusedPatternLoadMotorPos(int arg1);
    void onOffLoadMotorCurr();
    void onOffLoadMotorSpeed();
    void onOffLoadMotorPos();

    void writeData();
};
#endif // MAINWINDOW_H

#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    timeThread = new QTimer(this);
    connect(timeThread,SIGNAL(timeout()),this,SLOT(timer()));
    timeThread->start(500);

    this -> port = new QSerialPort(this);
    chartCreate();
    chartInit();
    chartShow();

    comboBoxSet();
    comboBoxConnect();

    blockSimTime();
    blockUnusedPatternRefValueCurr(1);
    blockUnusedPatternLoadMotorCurr(1);
    blockUnusedPatternLoadMotorSpeed(1);
    blockUnusedPatternRefValueSpeed(1);
    blockUnusedPatternRefValuePos(1);
    blockUnusedPatternLoadMotorPos(1);

    onOffLoadMotorCurr();
    onOffLoadMotorSpeed();
    onOffLoadMotorPos();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::lineInit(Chart *lineChart, float minValue,float maxValue,float minTime,float maxTime)
{
    // funkcja inicjująca wykres z biblioteki QChart
    lineChart->lineSeries -> append(lineChart->pointSeries);
    lineChart->refValueSeries->append(lineChart->refPointSeries);
    lineChart->loadMotorSeries->append(lineChart->loadMotorPointSeries);

    lineChart->addSeries(lineChart->lineSeries);
    lineChart->addSeries(lineChart->refValueSeries);
    lineChart->addSeries(lineChart->loadMotorSeries);
    lineChart->setTitle("Pierwszy rysunek");
    lineChart->setTheme(QChart::ChartThemeDark);

    lineChart->yAxis->setRange(minValue,maxValue);
    lineChart->yAxis->setTitleText("oś Y");
    lineChart->addAxis(lineChart->yAxis,Qt::AlignLeft);
    lineChart->lineSeries->attachAxis(lineChart->yAxis);
    lineChart->refValueSeries->attachAxis(lineChart->yAxis);
    lineChart->loadMotorSeries->attachAxis(lineChart->yAxis);

    lineChart -> xAxis->setRange(minTime,maxTime);
    lineChart -> xAxis->setTitleText("oś X");
    lineChart->addAxis(lineChart->xAxis,Qt::AlignBottom);
    lineChart->lineSeries->attachAxis(lineChart->xAxis);
    lineChart->refValueSeries->attachAxis(lineChart->xAxis);
    lineChart->loadMotorSeries->attachAxis(lineChart->xAxis);

    lineChart->lineView->setRenderHint(QPainter::Antialiasing);

}

void MainWindow::chartCurrInit()
{
    lineChartCurr->lineSeries->setName("prąd zmierzony");
    lineChartCurr->refValueSeries->setName("wartość zadana prądu");
    lineChartCurr->loadMotorSeries->setName("zadany moment obciążenia");

    lineChartCurr->lineSeries -> append(lineChartCurr->pointSeries);
    lineChartCurr->refValueSeries->append(lineChartCurr->refPointSeries);
    lineChartCurr->loadMotorSeries->append(lineChartCurr->loadMotorPointSeries);
    lineChartCurr->loadMotorSeries->setVisible(false);

    lineChartCurr->addSeries(lineChartCurr->lineSeries);
    lineChartCurr->addSeries(lineChartCurr->refValueSeries);
    lineChartCurr->addSeries(lineChartCurr->loadMotorSeries);
    lineChartCurr->setTitle("Wykres prądu");
    lineChartCurr->setTheme(QChart::ChartThemeLight);

    lineChartCurr->yAxis->setRange(-5000,5000);
    lineChartCurr->yAxis->setTitleText("prąd [A]");
    lineChartCurr->addAxis(lineChartCurr->yAxis,Qt::AlignLeft);
    lineChartCurr->lineSeries->attachAxis(lineChartCurr->yAxis);
    lineChartCurr->refValueSeries->attachAxis(lineChartCurr->yAxis);
    //lineChartCurr->loadMotorSeries->attachAxis(lineChartCurr->yAxis);

    lineChartCurr->loadAxis->setRange(0,0.10);
    lineChartCurr->loadAxis->setTitleText("moment obciążenia [Nm]");
    lineChartCurr->addAxis(lineChartCurr->loadAxis,Qt::AlignRight);
    lineChartCurr->loadMotorSeries->attachAxis(lineChartCurr->loadAxis);
    lineChartCurr->loadAxis->setVisible(false);

    lineChartCurr -> xAxis->setRange(-5000,5000);
    lineChartCurr -> xAxis->setTitleText("czas [s]");
    lineChartCurr->addAxis(lineChartCurr->xAxis,Qt::AlignBottom);
    lineChartCurr->lineSeries->attachAxis(lineChartCurr->xAxis);
    lineChartCurr->refValueSeries->attachAxis(lineChartCurr->xAxis);
    lineChartCurr->loadMotorSeries->attachAxis(lineChartCurr->xAxis);

    lineChartCurr->legend()->setAlignment(Qt::AlignBottom);

    lineChartCurr->lineView->setRenderHint(QPainter::Antialiasing);
}

void MainWindow::chartSpeedInit()
{
    lineChartSpeed->lineSeries->setName("predkość zmierzona");
    lineChartSpeed->refValueSeries->setName("wartość zadana prędkości");
    lineChartSpeed->loadMotorSeries->setName("zadany moment obciążenia");
    lineChartSpeed->loadMotorSeries->setVisible(false);

    lineChartSpeed->lineSeries -> append(lineChartSpeed->pointSeries);
    lineChartSpeed->refValueSeries->append(lineChartSpeed->refPointSeries);
    lineChartSpeed->loadMotorSeries->append(lineChartSpeed->loadMotorPointSeries);

    lineChartSpeed->addSeries(lineChartSpeed->lineSeries);
    lineChartSpeed->addSeries(lineChartSpeed->refValueSeries);
    lineChartSpeed->addSeries(lineChartSpeed->loadMotorSeries);
    lineChartSpeed->setTitle("Wykres prędkości");
    lineChartSpeed->setTheme(QChart::ChartThemeLight);

    lineChartSpeed->yAxis->setRange(-5000,5000);
    lineChartSpeed->yAxis->setTitleText("prędkość [rpm]");
    lineChartSpeed->addAxis(lineChartSpeed->yAxis,Qt::AlignLeft);
    lineChartSpeed->lineSeries->attachAxis(lineChartSpeed->yAxis);
    lineChartSpeed->refValueSeries->attachAxis(lineChartSpeed->yAxis);
    //lineChartSpeed->loadMotorSeries->attachAxis(lineChartSpeed->yAxis);

    lineChartSpeed -> xAxis->setRange(-5000,5000);
    lineChartSpeed -> xAxis->setTitleText("czas [s]");
    lineChartSpeed->addAxis(lineChartSpeed->xAxis,Qt::AlignBottom);
    lineChartSpeed->lineSeries->attachAxis(lineChartSpeed->xAxis);
    lineChartSpeed->refValueSeries->attachAxis(lineChartSpeed->xAxis);
    lineChartSpeed->loadMotorSeries->attachAxis(lineChartSpeed->xAxis);

    lineChartSpeed->loadAxis->setRange(0,0.10);
    lineChartSpeed->loadAxis->setTitleText("moment obciążenia [Nm]");
    lineChartSpeed->addAxis(lineChartSpeed->loadAxis,Qt::AlignRight);
    lineChartSpeed->loadMotorSeries->attachAxis(lineChartSpeed->loadAxis);
    lineChartSpeed->loadAxis->setVisible(false);


    lineChartSpeed->legend()->setAlignment(Qt::AlignBottom);

    lineChartSpeed->lineView->setRenderHint(QPainter::Antialiasing);
}

void MainWindow::chartPosInit()
{
    lineChartPos->lineSeries->setName("pozycja zmierzona");
    lineChartPos->refValueSeries->setName("wartość zadana pozycji");
    lineChartPos->loadMotorSeries->setName("zadany moment obciążenia");

    lineChartPos->lineSeries -> append(lineChartPos->pointSeries);
    lineChartPos->refValueSeries->append(lineChartPos->refPointSeries);
    lineChartPos->loadMotorSeries->append(lineChartPos->loadMotorPointSeries);
    lineChartPos->loadMotorSeries->setVisible(false);

    lineChartPos->addSeries(lineChartPos->lineSeries);
    lineChartPos->addSeries(lineChartPos->refValueSeries);
    lineChartPos->addSeries(lineChartPos->loadMotorSeries);
    lineChartPos->setTitle("Wykres pozycji wału silnika");
    lineChartPos->setTheme(QChart::ChartThemeLight);

    lineChartPos->yAxis->setRange(-5000,5000);
    lineChartPos->yAxis->setTitleText("obroty");
    lineChartPos->addAxis(lineChartPos->yAxis,Qt::AlignLeft);
    lineChartPos->lineSeries->attachAxis(lineChartPos->yAxis);
    lineChartPos->refValueSeries->attachAxis(lineChartPos->yAxis);
    //lineChartPos->loadMotorSeries->attachAxis(lineChartPos->yAxis);

    lineChartPos -> xAxis->setRange(-5000,5000);
    lineChartPos -> xAxis->setTitleText("czas [s]");
    lineChartPos->addAxis(lineChartPos->xAxis,Qt::AlignBottom);
    lineChartPos->lineSeries->attachAxis(lineChartPos->xAxis);
    lineChartPos->refValueSeries->attachAxis(lineChartPos->xAxis);
    lineChartPos->loadMotorSeries->attachAxis(lineChartPos->xAxis);

    lineChartPos->loadAxis->setRange(0,0.10);
    lineChartPos->loadAxis->setTitleText("moment obciążenia [Nm]");
    lineChartPos->addAxis(lineChartPos->loadAxis,Qt::AlignRight);
    lineChartPos->loadMotorSeries->attachAxis(lineChartPos->loadAxis);
    lineChartPos->loadAxis->setVisible(false);


    lineChartPos->legend()->setAlignment(Qt::AlignBottom);

    lineChartPos->lineView->setRenderHint(QPainter::Antialiasing);
}

void MainWindow::addLog(QString mess)
{
    // funkcja dodająca  wiadomość do terminala
    ui->textEdit->append(czas->currentDateTime().toString("dd:mm:yy hh:mm:ss") + "\t" + mess);
}

void MainWindow::readFromProc()
{
    // funkcja odczytująca dane z buffora
    while(port->isReadable() && (port->bytesAvailable() >= 20) && read == true)
    {
        port->read((char *)&tmp[idx], 1);

        if( idx == 0 )
        {
            if(tmp[0] != 0xAA)
                idx = 0;
            else
                idx++;
        }
        else if( idx == 1 )
        {
            if(tmp[1] != 0x55)
                idx = 0;
            else
                idx++;
        }
        else if(idx == 3)
        {
            byteReadNo = (tmp[2]<<8)+tmp[3];
            idx++;
        }
        else if( idx == 19 )
        {
            idx=0;
            dataFrameRead();
        }
        else
            idx++;
    }
}

void MainWindow::measurCplt()
{
    //funkcja aktualizujaca wykresy po przyjściu nowych danych
    chartSeriesChanged(lineChartCurr);
    chartSeriesChanged(lineChartSpeed);
    chartSeriesChanged(lineChartPos);
    //port->close();
}

void MainWindow::writeParameters()
{
    portOpenAuto();
    if(this->port->isOpen() && this->port->isWritable())
    {
        /* budowa ramki danych: mode, m.valueLoad, m.simTime,c_c.Kp, c_c.Ti, c_c.Td, c_c.Kff, c_c.Kaw, m.refCurr
         * s_c.Kp, s_c.Ti, s_c.Td, s_c.Kff, s_c.Kaw, m.refSpeed
         * p_c.Kp, p_c.Ti, p_c.Td, p_c.Kff, p_c.Kaw, m.refPos */
        uint8_t parametersToSend[35*4+3] = {0};

        mainClear();
        read = true;
        dataRxCplt = false;

        parametersToSend[0] = 16;
        parametersToSend[1] = 18;

        motorParameterSend(parametersToSend);
        currentPIDParametersSend(parametersToSend);
        speedPIDParametersSend(parametersToSend);
        posPIDParametersSend(parametersToSend);

        this->port->write((char *)parametersToSend,35*4+3);
    }
    else
    {
        this->addLog("Nie moge wyslac parametru!");
    }
}

void MainWindow::calcMinMax(Chart *lineChart)
{
    //funkcja obliczająca maksymalny oraz minimalny punkt serii danych
    float maxValue = 0;
    float minValue = 0;
    float maxTime = 0;
    float minTime = 0;
    for(int i = 1;i<=idxValue-1;i++)
    {
        if(lineChart->pointSeries[i].y()>maxValue) maxValue = lineChart->pointSeries[i].y();
        if(lineChart->pointSeries[i].y()<minValue) minValue = lineChart->pointSeries[i].y();
        if(timeValue[i]>maxTime) maxTime = timeValue[i];
        if(timeValue[i]<minTime) minTime = timeValue[i];
    }
    if(maxValue>0)
    {
        maxValue = maxValue*1.1;
    }
    else
    {
        maxValue = maxValue*0.9;
    }
    if(minValue>0)
    {
        minValue = minValue*0.9;
    }
    else
    {
        minValue = minValue*1.1;
    }
    chartScale(lineChart,minValue, maxValue, minTime, maxTime);
}

void MainWindow::mainClear()
{
    /* funkcja czyszcząca serie danych
     * wywoływana przed uruchomieniem kolejnego eksperymentu
     * funkcją writeParameters() */
    idxValue = 0;
    idx = 0;

    lineChartCurr->pointSeries.clear();
    lineChartSpeed->pointSeries.clear();
    lineChartPos->pointSeries.clear();

    lineChartCurr->refPointSeries.clear();
    lineChartSpeed->refPointSeries.clear();
    lineChartPos->refPointSeries.clear();

    lineChartCurr->loadMotorPointSeries.clear();
    lineChartSpeed->loadMotorPointSeries.clear();
    lineChartPos->loadMotorPointSeries.clear();

    port->clear();


}

void MainWindow::chartCreate()
{
    /*funkcja tworząca nowe obiekty wykresów
     * oraz obiekty widgetów odpowiednio umieszczonych w GroupBox'ie */
    this -> lineChartCurr = new Chart();
    this -> vBoxCurr = new QGridLayout();
    this -> lineChartSpeed = new Chart();
    this -> vBoxSpeed = new QGridLayout();
    this -> lineChartPos = new Chart();
    this -> vBoxPos = new QGridLayout();
}

void MainWindow::chartInit()
{
    /* funkcja inicjalizująca wykresy oraz ustawiajaca OpenGL
     * w celu optymalizacji czasu wykreślania wykresów */
    lineChartCurr -> lineSeries ->setUseOpenGL();
    lineChartSpeed -> lineSeries ->setUseOpenGL();
    lineChartPos -> lineSeries ->setUseOpenGL();
    chartCurrInit();
    chartSpeedInit();
    chartPosInit();
}

void MainWindow::chartShow()
{
    /* funkcja dodająca wykresy do widoku
     * i rozmieszczjąca je w siatce group boxa */
    this -> vBoxCurr -> addWidget(lineChartCurr->lineView);
    ui->groupBox_8->setLayout(vBoxCurr);
    this -> vBoxSpeed -> addWidget(lineChartSpeed->lineView);
    ui->groupBox_7->setLayout(vBoxSpeed);
    this -> vBoxPos -> addWidget(lineChartPos->lineView);
    ui->groupBox_13->setLayout(vBoxPos);
}

void MainWindow::chartScale(Chart *lineChart, float minValue, float maxValue, float minTime, float maxTime)
{
    // funkcja skalująca wykres
    lineChart -> yAxis -> setRange(minValue,maxValue);
    lineChart -> xAxis -> setRange(minTime, maxTime);
}

void MainWindow::chartSeriesChanged(Chart *lineChart)
{
    // funkcja wykreślająca nowe wykresy
    lineChart -> lineSeries -> clear();
    lineChart->refValueSeries->clear();
    lineChart->loadMotorSeries->clear();

    if(lineChart == lineChartCurr)
    {
        lineChart->lineSeries ->append(lineChart->pointSeries);
        if(ui->trybSter->currentIndex() == 1)
        {
            addRefValueSeries(lineChart);
            addLoadMotorSeries(lineChart);
        }
    }
    if(lineChart == lineChartSpeed)
    {
        lineChart->lineSeries ->append(lineChart->pointSeries);
        if(ui->trybSter->currentIndex() == 2)
        {
            addRefValueSeries(lineChart);
            addLoadMotorSeries(lineChart);
        }
    }
    if(lineChart == lineChartPos)
    {
        lineChart->lineSeries ->append(lineChart->pointSeries);
        if(ui->trybSter->currentIndex() == 3)
        {
            addRefValueSeries(lineChart);
            addLoadMotorSeries(lineChart);
        }
    }
}

void MainWindow::addRefValueSeries(Chart *lineChart)
{
    lineChart->refValueSeries->clear();

    if(lineChart == lineChartCurr)
    {
        switch(ui->numberRefValueCurr->value())
        {
        case 1:
            lineChart->refPointSeries << QPointF(0,0);
            lineChart->refPointSeries << QPointF((float)ui->refCurrTime->value()/1000,0);
            lineChart->refPointSeries << QPointF((float)ui->refCurrTime->value()/1000,ui->valueCurrRef->value());
            lineChart->refPointSeries << QPointF((float)ui->valueCurrTs->value()/1000,ui->valueCurrRef->value());
            lineChart->refPointSeries << QPointF((float)ui->valueCurrTs->value()/1000,0);
            lineChart->refPointSeries << QPointF((float)ui->valueCurrTs->value()/1000,0);
            lineChart->refPointSeries << QPointF((float)ui->valueCurrTs->value()/1000,0);
            lineChart->refPointSeries << QPointF((float)ui->valueCurrTs->value()/1000,0);
            lineChart->refPointSeries << QPointF((float)ui->valueCurrTs->value()/1000,0);
            lineChart->refPointSeries << QPointF((float)ui->valueCurrTs->value()/1000,0);
            lineChart->refPointSeries << QPointF((float)ui->valueCurrTs->value()/1000,0);
            break;
        case 2:
            lineChart->refPointSeries << QPointF(0,0);
            lineChart->refPointSeries << QPointF((float)ui->refCurrTime->value()/1000,0);
            lineChart->refPointSeries << QPointF((float)ui->refCurrTime->value()/1000,ui->valueCurrRef->value());
            lineChart->refPointSeries << QPointF((float)ui->refCurrTime2->value()/1000,ui->valueCurrRef->value());
            lineChart->refPointSeries << QPointF((float)ui->refCurrTime2->value()/1000,ui->valueCurrRef2->value());
            lineChart->refPointSeries << QPointF((float)ui->valueCurrTs->value()/1000,ui->valueCurrRef2->value());
            lineChart->refPointSeries << QPointF((float)ui->valueCurrTs->value()/1000,0);
            lineChart->refPointSeries << QPointF((float)ui->valueCurrTs->value()/1000,0);
            lineChart->refPointSeries << QPointF((float)ui->valueCurrTs->value()/1000,0);
            lineChart->refPointSeries << QPointF((float)ui->valueCurrTs->value()/1000,0);
            lineChart->refPointSeries << QPointF((float)ui->valueCurrTs->value()/1000,0);
            break;
        case 3:
            lineChart->refPointSeries << QPointF(0,0);
            lineChart->refPointSeries << QPointF((float)ui->refCurrTime->value()/1000,0);
            lineChart->refPointSeries << QPointF((float)ui->refCurrTime->value()/1000,ui->valueCurrRef->value());
            lineChart->refPointSeries << QPointF((float)ui->refCurrTime2->value()/1000,ui->valueCurrRef->value());
            lineChart->refPointSeries << QPointF((float)ui->refCurrTime2->value()/1000,ui->valueCurrRef2->value());
            lineChart->refPointSeries << QPointF((float)ui->refCurrTime3->value()/1000,ui->valueCurrRef2->value());
            lineChart->refPointSeries << QPointF((float)ui->refCurrTime3->value()/1000,ui->valueCurrRef3->value());
            lineChart->refPointSeries << QPointF((float)ui->valueCurrTs->value()/1000,ui->valueCurrRef3->value());
            lineChart->refPointSeries << QPointF((float)ui->valueCurrTs->value()/1000,0);
            lineChart->refPointSeries << QPointF((float)ui->valueCurrTs->value()/1000,0);
            lineChart->refPointSeries << QPointF((float)ui->valueCurrTs->value()/1000,0);
            break;
        case 4:
            lineChart->refPointSeries << QPointF(0,0);
            lineChart->refPointSeries << QPointF((float)ui->refCurrTime->value()/1000,0);
            lineChart->refPointSeries << QPointF((float)ui->refCurrTime->value()/1000,ui->valueCurrRef->value());
            lineChart->refPointSeries << QPointF((float)ui->refCurrTime2->value()/1000,ui->valueCurrRef->value());
            lineChart->refPointSeries << QPointF((float)ui->refCurrTime2->value()/1000,ui->valueCurrRef2->value());
            lineChart->refPointSeries << QPointF((float)ui->refCurrTime3->value()/1000,ui->valueCurrRef2->value());
            lineChart->refPointSeries << QPointF((float)ui->refCurrTime3->value()/1000,ui->valueCurrRef3->value());
            lineChart->refPointSeries << QPointF((float)ui->refCurrTime4->value()/1000,ui->valueCurrRef3->value());
            lineChart->refPointSeries << QPointF((float)ui->refCurrTime4->value()/1000,ui->valueCurrRef4->value());
            lineChart->refPointSeries << QPointF((float)ui->valueCurrTs->value()/1000,ui->valueCurrRef4->value());
            lineChart->refPointSeries << QPointF((float)ui->valueCurrTs->value()/1000,0);
            break;
        case 5:
            lineChart->refPointSeries << QPointF(0,0);
            lineChart->refPointSeries << QPointF((float)ui->refCurrTime->value()/1000,0);
            lineChart->refPointSeries << QPointF((float)ui->refCurrTime->value()/1000,ui->valueCurrRef->value());
            lineChart->refPointSeries << QPointF((float)ui->refCurrTime2->value()/1000,ui->valueCurrRef->value());
            lineChart->refPointSeries << QPointF((float)ui->refCurrTime2->value()/1000,ui->valueCurrRef2->value());
            lineChart->refPointSeries << QPointF((float)ui->refCurrTime3->value()/1000,ui->valueCurrRef2->value());
            lineChart->refPointSeries << QPointF((float)ui->refCurrTime3->value()/1000,ui->valueCurrRef3->value());
            lineChart->refPointSeries << QPointF((float)ui->refCurrTime4->value()/1000,ui->valueCurrRef3->value());
            lineChart->refPointSeries << QPointF((float)ui->refCurrTime4->value()/1000,ui->valueCurrRef4->value());
            lineChart->refPointSeries << QPointF((float)ui->refCurrTime5->value()/1000,ui->valueCurrRef4->value());
            lineChart->refPointSeries << QPointF((float)ui->refCurrTime5->value()/1000,ui->valueCurrRef5->value());
            break;
        default:
            break;
        }
    }
    if(lineChart == lineChartSpeed)
    {
        switch(ui->numberRefValueSpeed->value())
        {
        case 1:
            lineChart->refPointSeries << QPointF(0,0);
            lineChart->refPointSeries << QPointF((float)ui->refSpeedTime->value()/1000,0);
            lineChart->refPointSeries << QPointF((float)ui->refSpeedTime->value()/1000,ui->valueSpeedRef->value());
            lineChart->refPointSeries << QPointF((float)ui->valueSpeedTs->value()/1000,ui->valueSpeedRef->value());
            lineChart->refPointSeries << QPointF((float)ui->valueSpeedTs->value()/1000,0);
            lineChart->refPointSeries << QPointF((float)ui->valueSpeedTs->value()/1000,0);
            lineChart->refPointSeries << QPointF((float)ui->valueSpeedTs->value()/1000,0);
            lineChart->refPointSeries << QPointF((float)ui->valueSpeedTs->value()/1000,0);
            lineChart->refPointSeries << QPointF((float)ui->valueSpeedTs->value()/1000,0);
            lineChart->refPointSeries << QPointF((float)ui->valueSpeedTs->value()/1000,0);
            lineChart->refPointSeries << QPointF((float)ui->valueSpeedTs->value()/1000,0);
            break;
        case 2:
            lineChart->refPointSeries << QPointF(0,0);
            lineChart->refPointSeries << QPointF((float)ui->refSpeedTime->value()/1000,0);
            lineChart->refPointSeries << QPointF((float)ui->refSpeedTime->value()/1000,ui->valueSpeedRef->value());
            lineChart->refPointSeries << QPointF((float)ui->refSpeedTime2->value()/1000,ui->valueSpeedRef->value());
            lineChart->refPointSeries << QPointF((float)ui->refSpeedTime2->value()/1000,ui->valueSpeedRef2->value());
            lineChart->refPointSeries << QPointF((float)ui->valueSpeedTs->value()/1000,ui->valueSpeedRef2->value());
            lineChart->refPointSeries << QPointF((float)ui->valueSpeedTs->value()/1000,0);
            lineChart->refPointSeries << QPointF((float)ui->valueSpeedTs->value()/1000,0);
            lineChart->refPointSeries << QPointF((float)ui->valueSpeedTs->value()/1000,0);
            lineChart->refPointSeries << QPointF((float)ui->valueSpeedTs->value()/1000,0);
            lineChart->refPointSeries << QPointF((float)ui->valueSpeedTs->value()/1000,0);
            break;
        case 3:
            lineChart->refPointSeries << QPointF(0,0);
            lineChart->refPointSeries << QPointF((float)ui->refSpeedTime->value()/1000,0);
            lineChart->refPointSeries << QPointF((float)ui->refSpeedTime->value()/1000,ui->valueSpeedRef->value());
            lineChart->refPointSeries << QPointF((float)ui->refSpeedTime2->value()/1000,ui->valueSpeedRef->value());
            lineChart->refPointSeries << QPointF((float)ui->refSpeedTime2->value()/1000,ui->valueSpeedRef2->value());
            lineChart->refPointSeries << QPointF((float)ui->refSpeedTime3->value()/1000,ui->valueSpeedRef2->value());
            lineChart->refPointSeries << QPointF((float)ui->refSpeedTime3->value()/1000,ui->valueSpeedRef3->value());
            lineChart->refPointSeries << QPointF((float)ui->valueSpeedTs->value()/1000,ui->valueSpeedRef3->value());
            lineChart->refPointSeries << QPointF((float)ui->valueSpeedTs->value()/1000,0);
            lineChart->refPointSeries << QPointF((float)ui->valueSpeedTs->value()/1000,0);
            lineChart->refPointSeries << QPointF((float)ui->valueSpeedTs->value()/1000,0);
            break;
        case 4:
            lineChart->refPointSeries << QPointF(0,0);
            lineChart->refPointSeries << QPointF((float)ui->refSpeedTime->value()/1000,0);
            lineChart->refPointSeries << QPointF((float)ui->refSpeedTime->value()/1000,ui->valueSpeedRef->value());
            lineChart->refPointSeries << QPointF((float)ui->refSpeedTime2->value()/1000,ui->valueSpeedRef->value());
            lineChart->refPointSeries << QPointF((float)ui->refSpeedTime2->value()/1000,ui->valueSpeedRef2->value());
            lineChart->refPointSeries << QPointF((float)ui->refSpeedTime3->value()/1000,ui->valueSpeedRef2->value());
            lineChart->refPointSeries << QPointF((float)ui->refSpeedTime3->value()/1000,ui->valueSpeedRef3->value());
            lineChart->refPointSeries << QPointF((float)ui->refSpeedTime4->value()/1000,ui->valueSpeedRef3->value());
            lineChart->refPointSeries << QPointF((float)ui->refSpeedTime4->value()/1000,ui->valueSpeedRef4->value());
            lineChart->refPointSeries << QPointF((float)ui->valueSpeedTs->value()/1000,ui->valueSpeedRef4->value());
            lineChart->refPointSeries << QPointF((float)ui->valueSpeedTs->value()/1000,0);
            break;
        case 5:
            lineChart->refPointSeries << QPointF(0,0);
            lineChart->refPointSeries << QPointF((float)ui->refSpeedTime->value()/1000,0);
            lineChart->refPointSeries << QPointF((float)ui->refSpeedTime->value()/1000,ui->valueSpeedRef->value());
            lineChart->refPointSeries << QPointF((float)ui->refSpeedTime2->value()/1000,ui->valueSpeedRef->value());
            lineChart->refPointSeries << QPointF((float)ui->refSpeedTime2->value()/1000,ui->valueSpeedRef2->value());
            lineChart->refPointSeries << QPointF((float)ui->refSpeedTime3->value()/1000,ui->valueSpeedRef2->value());
            lineChart->refPointSeries << QPointF((float)ui->refSpeedTime3->value()/1000,ui->valueSpeedRef3->value());
            lineChart->refPointSeries << QPointF((float)ui->refSpeedTime4->value()/1000,ui->valueSpeedRef3->value());
            lineChart->refPointSeries << QPointF((float)ui->refSpeedTime4->value()/1000,ui->valueSpeedRef4->value());
            lineChart->refPointSeries << QPointF((float)ui->refSpeedTime5->value()/1000,ui->valueSpeedRef4->value());
            lineChart->refPointSeries << QPointF((float)ui->refSpeedTime5->value()/1000,ui->valueSpeedRef5->value());
            break;
        default:
            break;
        }
    }
    if(lineChart == lineChartPos)
    {
        switch(ui->numberRefValuePos->value())
        {
        case 1:
            lineChart->refPointSeries << QPointF(0,0);
            lineChart->refPointSeries << QPointF((float)ui->refPosTime->value()/1000,0);
            lineChart->refPointSeries << QPointF((float)ui->refPosTime->value()/1000,ui->valuePosRef->value());
            lineChart->refPointSeries << QPointF((float)ui->valuePosTs->value()/1000,ui->valuePosRef->value());
            lineChart->refPointSeries << QPointF((float)ui->valuePosTs->value()/1000,0);
            lineChart->refPointSeries << QPointF((float)ui->valuePosTs->value()/1000,0);
            lineChart->refPointSeries << QPointF((float)ui->valuePosTs->value()/1000,0);
            lineChart->refPointSeries << QPointF((float)ui->valuePosTs->value()/1000,0);
            lineChart->refPointSeries << QPointF((float)ui->valuePosTs->value()/1000,0);
            lineChart->refPointSeries << QPointF((float)ui->valuePosTs->value()/1000,0);
            lineChart->refPointSeries << QPointF((float)ui->valuePosTs->value()/1000,0);
            break;
        case 2:
            lineChart->refPointSeries << QPointF(0,0);
            lineChart->refPointSeries << QPointF((float)ui->refPosTime->value()/1000,0);
            lineChart->refPointSeries << QPointF((float)ui->refPosTime->value()/1000,ui->valuePosRef->value());
            lineChart->refPointSeries << QPointF((float)ui->refPosTime2->value()/1000,ui->valuePosRef->value());
            lineChart->refPointSeries << QPointF((float)ui->refPosTime2->value()/1000,ui->valuePosRef2->value());
            lineChart->refPointSeries << QPointF((float)ui->valuePosTs->value()/1000,ui->valuePosRef2->value());
            lineChart->refPointSeries << QPointF((float)ui->valuePosTs->value()/1000,0);
            lineChart->refPointSeries << QPointF((float)ui->valuePosTs->value()/1000,0);
            lineChart->refPointSeries << QPointF((float)ui->valuePosTs->value()/1000,0);
            lineChart->refPointSeries << QPointF((float)ui->valuePosTs->value()/1000,0);
            lineChart->refPointSeries << QPointF((float)ui->valuePosTs->value()/1000,0);
            break;
        case 3:
            lineChart->refPointSeries << QPointF(0,0);
            lineChart->refPointSeries << QPointF((float)ui->refPosTime->value()/1000,0);
            lineChart->refPointSeries << QPointF((float)ui->refPosTime->value()/1000,ui->valuePosRef->value());
            lineChart->refPointSeries << QPointF((float)ui->refPosTime2->value()/1000,ui->valuePosRef->value());
            lineChart->refPointSeries << QPointF((float)ui->refPosTime2->value()/1000,ui->valuePosRef2->value());
            lineChart->refPointSeries << QPointF((float)ui->refPosTime3->value()/1000,ui->valuePosRef2->value());
            lineChart->refPointSeries << QPointF((float)ui->refPosTime3->value()/1000,ui->valuePosRef3->value());
            lineChart->refPointSeries << QPointF((float)ui->valuePosTs->value()/1000,ui->valuePosRef3->value());
            lineChart->refPointSeries << QPointF((float)ui->valuePosTs->value()/1000,0);
            lineChart->refPointSeries << QPointF((float)ui->valuePosTs->value()/1000,0);
            lineChart->refPointSeries << QPointF((float)ui->valuePosTs->value()/1000,0);
            break;
        case 4:
            lineChart->refPointSeries << QPointF(0,0);
            lineChart->refPointSeries << QPointF((float)ui->refPosTime->value()/1000,0);
            lineChart->refPointSeries << QPointF((float)ui->refPosTime->value()/1000,ui->valuePosRef->value());
            lineChart->refPointSeries << QPointF((float)ui->refPosTime2->value()/1000,ui->valuePosRef->value());
            lineChart->refPointSeries << QPointF((float)ui->refPosTime2->value()/1000,ui->valuePosRef2->value());
            lineChart->refPointSeries << QPointF((float)ui->refPosTime3->value()/1000,ui->valuePosRef2->value());
            lineChart->refPointSeries << QPointF((float)ui->refPosTime3->value()/1000,ui->valuePosRef3->value());
            lineChart->refPointSeries << QPointF((float)ui->refPosTime4->value()/1000,ui->valuePosRef3->value());
            lineChart->refPointSeries << QPointF((float)ui->refPosTime4->value()/1000,ui->valuePosRef4->value());
            lineChart->refPointSeries << QPointF((float)ui->valuePosTs->value()/1000,ui->valuePosRef4->value());
            lineChart->refPointSeries << QPointF((float)ui->valuePosTs->value()/1000,0);
            break;
        case 5:
            lineChart->refPointSeries << QPointF(0,0);
            lineChart->refPointSeries << QPointF((float)ui->refPosTime->value()/1000,0);
            lineChart->refPointSeries << QPointF((float)ui->refPosTime->value()/1000,ui->valuePosRef->value());
            lineChart->refPointSeries << QPointF((float)ui->refPosTime2->value()/1000,ui->valuePosRef->value());
            lineChart->refPointSeries << QPointF((float)ui->refPosTime2->value()/1000,ui->valuePosRef2->value());
            lineChart->refPointSeries << QPointF((float)ui->refPosTime3->value()/1000,ui->valuePosRef2->value());
            lineChart->refPointSeries << QPointF((float)ui->refPosTime3->value()/1000,ui->valuePosRef3->value());
            lineChart->refPointSeries << QPointF((float)ui->refPosTime4->value()/1000,ui->valuePosRef3->value());
            lineChart->refPointSeries << QPointF((float)ui->refPosTime4->value()/1000,ui->valuePosRef4->value());
            lineChart->refPointSeries << QPointF((float)ui->refPosTime5->value()/1000,ui->valuePosRef4->value());
            lineChart->refPointSeries << QPointF((float)ui->refPosTime5->value()/1000,ui->valuePosRef5->value());
            break;
        default:
            break;
        }
    }

    lineChart->refValueSeries ->append(lineChart->refPointSeries);
}

void MainWindow::addLoadMotorSeries(Chart *lineChart)
{
    lineChart->loadMotorSeries->clear();

    if(lineChart == lineChartCurr)
    {
        if(ui->SwitchLoadMotor->isChecked())
        {
            switch(ui->numberLoadValueCurr->value())
            {
            case 1:
                lineChart->loadMotorPointSeries << QPointF(0,0);
                lineChart->loadMotorPointSeries << QPointF((float)ui->motorLoadCurrTime->value()/1000,0);
                lineChart->loadMotorPointSeries << QPointF((float)ui->motorLoadCurrTime->value()/1000,ui->motorLoadCurr->value());
                lineChart->loadMotorPointSeries << QPointF((float)ui->valueCurrTs->value()/1000,ui->motorLoadCurr->value());
                lineChart->loadMotorPointSeries << QPointF((float)ui->valueCurrTs->value()/1000,0);
                lineChart->loadMotorPointSeries << QPointF((float)ui->valueCurrTs->value()/1000,0);
                lineChart->loadMotorPointSeries << QPointF((float)ui->valueCurrTs->value()/1000,0);
                lineChart->loadMotorPointSeries << QPointF((float)ui->valueCurrTs->value()/1000,0);
                lineChart->loadMotorPointSeries << QPointF((float)ui->valueCurrTs->value()/1000,0);
                lineChart->loadMotorPointSeries << QPointF((float)ui->valueCurrTs->value()/1000,0);
                lineChart->loadMotorPointSeries << QPointF((float)ui->valueCurrTs->value()/1000,0);
                break;
            case 2:
                lineChart->loadMotorPointSeries << QPointF(0,0);
                lineChart->loadMotorPointSeries << QPointF((float)ui->motorLoadCurrTime->value()/1000,0);
                lineChart->loadMotorPointSeries << QPointF((float)ui->motorLoadCurrTime->value()/1000,ui->motorLoadCurr->value());
                lineChart->loadMotorPointSeries << QPointF((float)ui->motorLoadCurrTime2->value()/1000,ui->motorLoadCurr->value());
                lineChart->loadMotorPointSeries << QPointF((float)ui->motorLoadCurrTime2->value()/1000,ui->motorLoadCurr2->value());
                lineChart->loadMotorPointSeries << QPointF((float)ui->valueCurrTs->value()/1000,ui->motorLoadCurr2->value());
                lineChart->loadMotorPointSeries << QPointF((float)ui->valueCurrTs->value()/1000,0);
                lineChart->loadMotorPointSeries << QPointF((float)ui->valueCurrTs->value()/1000,0);
                lineChart->loadMotorPointSeries << QPointF((float)ui->valueCurrTs->value()/1000,0);
                lineChart->loadMotorPointSeries << QPointF((float)ui->valueCurrTs->value()/1000,0);
                lineChart->loadMotorPointSeries << QPointF((float)ui->valueCurrTs->value()/1000,0);
                break;
            case 3:
                lineChart->loadMotorPointSeries << QPointF(0,0);
                lineChart->loadMotorPointSeries << QPointF((float)ui->motorLoadCurrTime->value()/1000,0);
                lineChart->loadMotorPointSeries << QPointF((float)ui->motorLoadCurrTime->value()/1000,ui->motorLoadCurr->value());
                lineChart->loadMotorPointSeries << QPointF((float)ui->motorLoadCurrTime2->value()/1000,ui->motorLoadCurr->value());
                lineChart->loadMotorPointSeries << QPointF((float)ui->motorLoadCurrTime2->value()/1000,ui->motorLoadCurr2->value());
                lineChart->loadMotorPointSeries << QPointF((float)ui->motorLoadCurrTime3->value()/1000,ui->motorLoadCurr2->value());
                lineChart->loadMotorPointSeries << QPointF((float)ui->motorLoadCurrTime3->value()/1000,ui->motorLoadCurr3->value());
                lineChart->loadMotorPointSeries << QPointF((float)ui->valueCurrTs->value()/1000,ui->motorLoadCurr3->value());
                lineChart->loadMotorPointSeries << QPointF((float)ui->valueCurrTs->value()/1000,0);
                lineChart->loadMotorPointSeries << QPointF((float)ui->valueCurrTs->value()/1000,0);
                lineChart->loadMotorPointSeries << QPointF((float)ui->valueCurrTs->value()/1000,0);
                break;
            case 4:
                lineChart->loadMotorPointSeries << QPointF(0,0);
                lineChart->loadMotorPointSeries << QPointF((float)ui->motorLoadCurrTime->value()/1000,0);
                lineChart->loadMotorPointSeries << QPointF((float)ui->motorLoadCurrTime->value()/1000,ui->motorLoadCurr->value());
                lineChart->loadMotorPointSeries << QPointF((float)ui->motorLoadCurrTime2->value()/1000,ui->motorLoadCurr->value());
                lineChart->loadMotorPointSeries << QPointF((float)ui->motorLoadCurrTime2->value()/1000,ui->motorLoadCurr2->value());
                lineChart->loadMotorPointSeries << QPointF((float)ui->motorLoadCurrTime3->value()/1000,ui->motorLoadCurr2->value());
                lineChart->loadMotorPointSeries << QPointF((float)ui->motorLoadCurrTime3->value()/1000,ui->motorLoadCurr3->value());
                lineChart->loadMotorPointSeries << QPointF((float)ui->motorLoadCurrTime4->value()/1000,ui->motorLoadCurr3->value());
                lineChart->loadMotorPointSeries << QPointF((float)ui->motorLoadCurrTime4->value()/1000,ui->motorLoadCurr4->value());
                lineChart->loadMotorPointSeries << QPointF((float)ui->valueCurrTs->value()/1000,ui->motorLoadCurr4->value());
                lineChart->loadMotorPointSeries << QPointF((float)ui->valueCurrTs->value()/1000,0);
                break;
            case 5:
                lineChart->loadMotorPointSeries << QPointF(0,0);
                lineChart->loadMotorPointSeries << QPointF((float)ui->motorLoadCurrTime->value()/1000,0);
                lineChart->loadMotorPointSeries << QPointF((float)ui->motorLoadCurrTime->value()/1000,ui->motorLoadCurr->value());
                lineChart->loadMotorPointSeries << QPointF((float)ui->motorLoadCurrTime2->value()/1000,ui->motorLoadCurr->value());
                lineChart->loadMotorPointSeries << QPointF((float)ui->motorLoadCurrTime2->value()/1000,ui->motorLoadCurr2->value());
                lineChart->loadMotorPointSeries << QPointF((float)ui->motorLoadCurrTime3->value()/1000,ui->motorLoadCurr2->value());
                lineChart->loadMotorPointSeries << QPointF((float)ui->motorLoadCurrTime3->value()/1000,ui->motorLoadCurr3->value());
                lineChart->loadMotorPointSeries << QPointF((float)ui->motorLoadCurrTime4->value()/1000,ui->motorLoadCurr3->value());
                lineChart->loadMotorPointSeries << QPointF((float)ui->motorLoadCurrTime4->value()/1000,ui->motorLoadCurr4->value());
                lineChart->loadMotorPointSeries << QPointF((float)ui->motorLoadCurrTime5->value()/1000,ui->motorLoadCurr4->value());
                lineChart->loadMotorPointSeries << QPointF((float)ui->motorLoadCurrTime5->value()/1000,ui->motorLoadCurr5->value());
                break;
            default:
                break;
            }
        }
        else
        {
            lineChart->loadMotorPointSeries << QPointF((float)ui->valueCurrTs->value(),0);
            lineChart->loadMotorPointSeries << QPointF((float)ui->valueCurrTs->value()/1000,0);
            lineChart->loadMotorPointSeries << QPointF((float)ui->valueCurrTs->value()/1000,0);
            lineChart->loadMotorPointSeries << QPointF((float)ui->valueCurrTs->value()/1000,0);
            lineChart->loadMotorPointSeries << QPointF((float)ui->valueCurrTs->value()/1000,0);
            lineChart->loadMotorPointSeries << QPointF((float)ui->valueCurrTs->value()/1000,0);
            lineChart->loadMotorPointSeries << QPointF((float)ui->valueCurrTs->value()/1000,0);
            lineChart->loadMotorPointSeries << QPointF((float)ui->valueCurrTs->value()/1000,0);
            lineChart->loadMotorPointSeries << QPointF((float)ui->valueCurrTs->value()/1000,0);
        }
    }
    if(lineChart == lineChartSpeed)
    {
        if(ui->SwitchLoadMotorSpeed->isChecked())
        {
            switch(ui->numberLoadValueSpeed->value())
            {
            case 1:
                lineChart->loadMotorPointSeries << QPointF(0,0);
                lineChart->loadMotorPointSeries << QPointF((float)ui->motorLoadSpeedTime->value()/1000,0);
                lineChart->loadMotorPointSeries << QPointF((float)ui->motorLoadSpeedTime->value()/1000,ui->motorLoadSpeed->value());
                lineChart->loadMotorPointSeries << QPointF((float)ui->valueSpeedTs->value()/1000,ui->motorLoadSpeed->value());
                lineChart->loadMotorPointSeries << QPointF((float)ui->valueSpeedTs->value()/1000,0);
                lineChart->loadMotorPointSeries << QPointF((float)ui->valueSpeedTs->value()/1000,0);
                lineChart->loadMotorPointSeries << QPointF((float)ui->valueSpeedTs->value()/1000,0);
                lineChart->loadMotorPointSeries << QPointF((float)ui->valueSpeedTs->value()/1000,0);
                lineChart->loadMotorPointSeries << QPointF((float)ui->valueSpeedTs->value()/1000,0);
                lineChart->loadMotorPointSeries << QPointF((float)ui->valueSpeedTs->value()/1000,0);
                lineChart->loadMotorPointSeries << QPointF((float)ui->valueSpeedTs->value()/1000,0);
                break;
            case 2:
                lineChart->loadMotorPointSeries << QPointF(0,0);
                lineChart->loadMotorPointSeries << QPointF((float)ui->motorLoadSpeedTime->value()/1000,0);
                lineChart->loadMotorPointSeries << QPointF((float)ui->motorLoadSpeedTime->value()/1000,ui->motorLoadSpeed->value());
                lineChart->loadMotorPointSeries << QPointF((float)ui->motorLoadSpeedTime2->value()/1000,ui->motorLoadSpeed->value());
                lineChart->loadMotorPointSeries << QPointF((float)ui->motorLoadSpeedTime2->value()/1000,ui->motorLoadSpeed2->value());
                lineChart->loadMotorPointSeries << QPointF((float)ui->valueSpeedTs->value()/1000,ui->motorLoadSpeed2->value());
                lineChart->loadMotorPointSeries << QPointF((float)ui->valueSpeedTs->value()/1000,0);
                lineChart->loadMotorPointSeries << QPointF((float)ui->valueSpeedTs->value()/1000,0);
                lineChart->loadMotorPointSeries << QPointF((float)ui->valueSpeedTs->value()/1000,0);
                lineChart->loadMotorPointSeries << QPointF((float)ui->valueSpeedTs->value()/1000,0);
                lineChart->loadMotorPointSeries << QPointF((float)ui->valueSpeedTs->value()/1000,0);
                break;
            case 3:
                lineChart->loadMotorPointSeries << QPointF(0,0);
                lineChart->loadMotorPointSeries << QPointF((float)ui->motorLoadSpeedTime->value()/1000,0);
                lineChart->loadMotorPointSeries << QPointF((float)ui->motorLoadSpeedTime->value()/1000,ui->motorLoadSpeed->value());
                lineChart->loadMotorPointSeries << QPointF((float)ui->motorLoadSpeedTime2->value()/1000,ui->motorLoadSpeed->value());
                lineChart->loadMotorPointSeries << QPointF((float)ui->motorLoadSpeedTime2->value()/1000,ui->motorLoadSpeed2->value());
                lineChart->loadMotorPointSeries << QPointF((float)ui->motorLoadSpeedTime3->value()/1000,ui->motorLoadSpeed2->value());
                lineChart->loadMotorPointSeries << QPointF((float)ui->motorLoadSpeedTime3->value()/1000,ui->motorLoadSpeed3->value());
                lineChart->loadMotorPointSeries << QPointF((float)ui->valueSpeedTs->value()/1000,ui->motorLoadSpeed3->value());
                lineChart->loadMotorPointSeries << QPointF((float)ui->valueSpeedTs->value()/1000,0);
                lineChart->loadMotorPointSeries << QPointF((float)ui->valueSpeedTs->value()/1000,0);
                lineChart->loadMotorPointSeries << QPointF((float)ui->valueSpeedTs->value()/1000,0);
                break;
            case 4:
                lineChart->loadMotorPointSeries << QPointF(0,0);
                lineChart->loadMotorPointSeries << QPointF((float)ui->motorLoadSpeedTime->value()/1000,0);
                lineChart->loadMotorPointSeries << QPointF((float)ui->motorLoadSpeedTime->value()/1000,ui->motorLoadSpeed->value());
                lineChart->loadMotorPointSeries << QPointF((float)ui->motorLoadSpeedTime2->value()/1000,ui->motorLoadSpeed->value());
                lineChart->loadMotorPointSeries << QPointF((float)ui->motorLoadSpeedTime2->value()/1000,ui->motorLoadSpeed2->value());
                lineChart->loadMotorPointSeries << QPointF((float)ui->motorLoadSpeedTime3->value()/1000,ui->motorLoadSpeed2->value());
                lineChart->loadMotorPointSeries << QPointF((float)ui->motorLoadSpeedTime3->value()/1000,ui->motorLoadSpeed3->value());
                lineChart->loadMotorPointSeries << QPointF((float)ui->motorLoadSpeedTime4->value()/1000,ui->motorLoadSpeed3->value());
                lineChart->loadMotorPointSeries << QPointF((float)ui->motorLoadSpeedTime4->value()/1000,ui->motorLoadSpeed4->value());
                lineChart->loadMotorPointSeries << QPointF((float)ui->valueSpeedTs->value()/1000,ui->motorLoadSpeed4->value());
                lineChart->loadMotorPointSeries << QPointF((float)ui->valueSpeedTs->value()/1000,0);
                break;
            case 5:
                lineChart->loadMotorPointSeries << QPointF(0,0);
                lineChart->loadMotorPointSeries << QPointF((float)ui->motorLoadSpeedTime->value()/1000,0);
                lineChart->loadMotorPointSeries << QPointF((float)ui->motorLoadSpeedTime->value()/1000,ui->motorLoadSpeed->value());
                lineChart->loadMotorPointSeries << QPointF((float)ui->motorLoadSpeedTime2->value()/1000,ui->motorLoadSpeed->value());
                lineChart->loadMotorPointSeries << QPointF((float)ui->motorLoadSpeedTime2->value()/1000,ui->motorLoadSpeed2->value());
                lineChart->loadMotorPointSeries << QPointF((float)ui->motorLoadSpeedTime3->value()/1000,ui->motorLoadSpeed2->value());
                lineChart->loadMotorPointSeries << QPointF((float)ui->motorLoadSpeedTime3->value()/1000,ui->motorLoadSpeed3->value());
                lineChart->loadMotorPointSeries << QPointF((float)ui->motorLoadSpeedTime4->value()/1000,ui->motorLoadSpeed3->value());
                lineChart->loadMotorPointSeries << QPointF((float)ui->motorLoadSpeedTime4->value()/1000,ui->motorLoadSpeed4->value());
                lineChart->loadMotorPointSeries << QPointF((float)ui->motorLoadSpeedTime5->value()/1000,ui->motorLoadSpeed4->value());
                lineChart->loadMotorPointSeries << QPointF((float)ui->motorLoadSpeedTime5->value()/1000,ui->motorLoadSpeed5->value());
                break;
            default:
                break;
            }
        }
        else
        {
            lineChart->loadMotorPointSeries << QPointF((float)ui->valueSpeedTs->value(),0);
            lineChart->loadMotorPointSeries << QPointF((float)ui->valueSpeedTs->value()/1000,0);
            lineChart->loadMotorPointSeries << QPointF((float)ui->valueSpeedTs->value()/1000,0);
            lineChart->loadMotorPointSeries << QPointF((float)ui->valueSpeedTs->value()/1000,0);
            lineChart->loadMotorPointSeries << QPointF((float)ui->valueSpeedTs->value()/1000,0);
            lineChart->loadMotorPointSeries << QPointF((float)ui->valueSpeedTs->value()/1000,0);
            lineChart->loadMotorPointSeries << QPointF((float)ui->valueSpeedTs->value()/1000,0);
            lineChart->loadMotorPointSeries << QPointF((float)ui->valueSpeedTs->value()/1000,0);
            lineChart->loadMotorPointSeries << QPointF((float)ui->valueSpeedTs->value()/1000,0);
        }
    }
    if(lineChart == lineChartPos)
    {
        if(ui->SwitchLoadMotorPos->isChecked())
        {
            switch(ui->numberLoadValuePos->value())
            {
            case 1:
                lineChart->loadMotorPointSeries << QPointF(0,0);
                lineChart->loadMotorPointSeries << QPointF((float)ui->motorLoadPosTime->value()/1000,0);
                lineChart->loadMotorPointSeries << QPointF((float)ui->motorLoadPosTime->value()/1000,ui->motorLoadPos->value());
                lineChart->loadMotorPointSeries << QPointF((float)ui->valuePosTs->value()/1000,ui->motorLoadPos->value());
                lineChart->loadMotorPointSeries << QPointF((float)ui->valuePosTs->value()/1000,0);
                lineChart->loadMotorPointSeries << QPointF((float)ui->valuePosTs->value()/1000,0);
                lineChart->loadMotorPointSeries << QPointF((float)ui->valuePosTs->value()/1000,0);
                lineChart->loadMotorPointSeries << QPointF((float)ui->valuePosTs->value()/1000,0);
                lineChart->loadMotorPointSeries << QPointF((float)ui->valuePosTs->value()/1000,0);
                lineChart->loadMotorPointSeries << QPointF((float)ui->valuePosTs->value()/1000,0);
                lineChart->loadMotorPointSeries << QPointF((float)ui->valuePosTs->value()/1000,0);
                break;
            case 2:
                lineChart->loadMotorPointSeries << QPointF(0,0);
                lineChart->loadMotorPointSeries << QPointF((float)ui->motorLoadPosTime->value()/1000,0);
                lineChart->loadMotorPointSeries << QPointF((float)ui->motorLoadPosTime->value()/1000,ui->motorLoadPos->value());
                lineChart->loadMotorPointSeries << QPointF((float)ui->motorLoadPosTime2->value()/1000,ui->motorLoadPos->value());
                lineChart->loadMotorPointSeries << QPointF((float)ui->motorLoadPosTime2->value()/1000,ui->motorLoadPos2->value());
                lineChart->loadMotorPointSeries << QPointF((float)ui->valuePosTs->value()/1000,ui->motorLoadPos2->value());
                lineChart->loadMotorPointSeries << QPointF((float)ui->valuePosTs->value()/1000,0);
                lineChart->loadMotorPointSeries << QPointF((float)ui->valuePosTs->value()/1000,0);
                lineChart->loadMotorPointSeries << QPointF((float)ui->valuePosTs->value()/1000,0);
                lineChart->loadMotorPointSeries << QPointF((float)ui->valuePosTs->value()/1000,0);
                lineChart->loadMotorPointSeries << QPointF((float)ui->valuePosTs->value()/1000,0);
                break;
            case 3:
                lineChart->loadMotorPointSeries << QPointF(0,0);
                lineChart->loadMotorPointSeries << QPointF((float)ui->motorLoadPosTime->value()/1000,0);
                lineChart->loadMotorPointSeries << QPointF((float)ui->motorLoadPosTime->value()/1000,ui->motorLoadPos->value());
                lineChart->loadMotorPointSeries << QPointF((float)ui->motorLoadPosTime2->value()/1000,ui->motorLoadPos->value());
                lineChart->loadMotorPointSeries << QPointF((float)ui->motorLoadPosTime2->value()/1000,ui->motorLoadPos2->value());
                lineChart->loadMotorPointSeries << QPointF((float)ui->motorLoadPosTime3->value()/1000,ui->motorLoadPos2->value());
                lineChart->loadMotorPointSeries << QPointF((float)ui->motorLoadPosTime3->value()/1000,ui->motorLoadPos3->value());
                lineChart->loadMotorPointSeries << QPointF((float)ui->valuePosTs->value()/1000,ui->motorLoadPos3->value());
                lineChart->loadMotorPointSeries << QPointF((float)ui->valuePosTs->value()/1000,0);
                lineChart->loadMotorPointSeries << QPointF((float)ui->valuePosTs->value()/1000,0);
                lineChart->loadMotorPointSeries << QPointF((float)ui->valuePosTs->value()/1000,0);
                break;
            case 4:
                lineChart->loadMotorPointSeries << QPointF(0,0);
                lineChart->loadMotorPointSeries << QPointF((float)ui->motorLoadPosTime->value()/1000,0);
                lineChart->loadMotorPointSeries << QPointF((float)ui->motorLoadPosTime->value()/1000,ui->motorLoadPos->value());
                lineChart->loadMotorPointSeries << QPointF((float)ui->motorLoadPosTime2->value()/1000,ui->motorLoadPos->value());
                lineChart->loadMotorPointSeries << QPointF((float)ui->motorLoadPosTime2->value()/1000,ui->motorLoadPos2->value());
                lineChart->loadMotorPointSeries << QPointF((float)ui->motorLoadPosTime3->value()/1000,ui->motorLoadPos2->value());
                lineChart->loadMotorPointSeries << QPointF((float)ui->motorLoadPosTime3->value()/1000,ui->motorLoadPos3->value());
                lineChart->loadMotorPointSeries << QPointF((float)ui->motorLoadPosTime4->value()/1000,ui->motorLoadPos3->value());
                lineChart->loadMotorPointSeries << QPointF((float)ui->motorLoadPosTime4->value()/1000,ui->motorLoadPos4->value());
                lineChart->loadMotorPointSeries << QPointF((float)ui->valuePosTs->value()/1000,ui->motorLoadPos4->value());
                lineChart->loadMotorPointSeries << QPointF((float)ui->valuePosTs->value()/1000,0);
                break;
            case 5:
                lineChart->loadMotorPointSeries << QPointF(0,0);
                lineChart->loadMotorPointSeries << QPointF((float)ui->motorLoadPosTime->value()/1000,0);
                lineChart->loadMotorPointSeries << QPointF((float)ui->motorLoadPosTime->value()/1000,ui->motorLoadPos->value());
                lineChart->loadMotorPointSeries << QPointF((float)ui->motorLoadPosTime2->value()/1000,ui->motorLoadPos->value());
                lineChart->loadMotorPointSeries << QPointF((float)ui->motorLoadPosTime2->value()/1000,ui->motorLoadPos2->value());
                lineChart->loadMotorPointSeries << QPointF((float)ui->motorLoadPosTime3->value()/1000,ui->motorLoadPos2->value());
                lineChart->loadMotorPointSeries << QPointF((float)ui->motorLoadPosTime3->value()/1000,ui->motorLoadPos3->value());
                lineChart->loadMotorPointSeries << QPointF((float)ui->motorLoadPosTime4->value()/1000,ui->motorLoadPos3->value());
                lineChart->loadMotorPointSeries << QPointF((float)ui->motorLoadPosTime4->value()/1000,ui->motorLoadPos4->value());
                lineChart->loadMotorPointSeries << QPointF((float)ui->motorLoadPosTime5->value()/1000,ui->motorLoadPos4->value());
                lineChart->loadMotorPointSeries << QPointF((float)ui->motorLoadPosTime5->value()/1000,ui->motorLoadPos5->value());
                break;
            default:
                break;
            }
        }
        else
        {
            lineChart->loadMotorPointSeries << QPointF((float)ui->valuePosTs->value(),0);
            lineChart->loadMotorPointSeries << QPointF((float)ui->valuePosTs->value()/1000,0);
            lineChart->loadMotorPointSeries << QPointF((float)ui->valuePosTs->value()/1000,0);
            lineChart->loadMotorPointSeries << QPointF((float)ui->valuePosTs->value()/1000,0);
            lineChart->loadMotorPointSeries << QPointF((float)ui->valuePosTs->value()/1000,0);
            lineChart->loadMotorPointSeries << QPointF((float)ui->valuePosTs->value()/1000,0);
            lineChart->loadMotorPointSeries << QPointF((float)ui->valuePosTs->value()/1000,0);
            lineChart->loadMotorPointSeries << QPointF((float)ui->valuePosTs->value()/1000,0);
            lineChart->loadMotorPointSeries << QPointF((float)ui->valuePosTs->value()/1000,0);
        }
    }

    lineChart->loadMotorSeries ->append(lineChart->loadMotorPointSeries);
}

void MainWindow::comboBoxSet()
{
    //funkcja wpisująca wartości do odpowiednich combo box'ów

    //elementy skladowe comboboxa od trybu sterowania
    ui -> trybSter -> addItem("Testowy");
    ui -> trybSter -> addItem("Regulator prądu");
    ui -> trybSter -> addItem("Regulator prędkości");
    ui -> trybSter -> addItem("Regulator położenia");

    //elementy skladowe comboboxa od trybu sterowania prędkości
    ui -> trybSterSpeed -> addItem("Testowy");
    ui -> trybSterSpeed -> addItem("Regulator prądu");
    ui -> trybSterSpeed -> addItem("Regulator prędkości");
    ui -> trybSterSpeed -> addItem("Regulator położenia");

    //elementy skladowe comboboxa od trybu sterowania położenia
    ui -> trybSterPos -> addItem("Testowy");
    ui -> trybSterPos -> addItem("Regulator prądu");
    ui -> trybSterPos -> addItem("Regulator prędkości");
    ui -> trybSterPos -> addItem("Regulator położenia");
}

void MainWindow::comboBoxConnect()
{
    /*funkcja łącząca widget'y opisujące to samo
     *ale dla czytelności znajdujące się w różnych kartach */

    //łączenie comboxów w różnych zakładkach regulatorów
    connect(ui->trybSter,SIGNAL(currentIndexChanged(int)),ui->trybSterSpeed,SLOT(setCurrentIndex(int)));
    connect(ui->trybSter,SIGNAL(currentIndexChanged(int)),ui->trybSterPos,SLOT(setCurrentIndex(int)));
    connect(ui->trybSterSpeed,SIGNAL(currentIndexChanged(int)),ui->trybSter,SLOT(setCurrentIndex(int)));
    connect(ui->trybSterSpeed,SIGNAL(currentIndexChanged(int)),ui->trybSterPos,SLOT(setCurrentIndex(int)));
    connect(ui->trybSterPos,SIGNAL(currentIndexChanged(int)),ui->trybSterSpeed,SLOT(setCurrentIndex(int)));
    connect(ui->trybSterPos,SIGNAL(currentIndexChanged(int)),ui->trybSter,SLOT(setCurrentIndex(int)));

    //łączenie spinboxów od czasu symulacji
    connect(ui->valueCurrTs,SIGNAL(valueChanged(int)),ui->valuePosTs,SLOT(setValue(int)));
    connect(ui->valueCurrTs,SIGNAL(valueChanged(int)),ui->valueSpeedTs,SLOT(setValue(int)));
    connect(ui->valueSpeedTs,SIGNAL(valueChanged(int)),ui->valueCurrTs,SLOT(setValue(int)));
    connect(ui->valueSpeedTs,SIGNAL(valueChanged(int)),ui->valuePosTs,SLOT(setValue(int)));
    connect(ui->valuePosTs,SIGNAL(valueChanged(int)),ui->valueSpeedTs,SLOT(setValue(int)));
    connect(ui->valuePosTs,SIGNAL(valueChanged(int)),ui->valueCurrTs,SLOT(setValue(int)));

    //łączenie przycisków on/off od obciążenia
    connect(ui->SwitchLoadMotor,SIGNAL(clicked(bool)),ui->SwitchLoadMotorPos,SLOT(toggle()));
    connect(ui->SwitchLoadMotor,SIGNAL(clicked(bool)),ui->SwitchLoadMotorSpeed,SLOT(toggle()));
    connect(ui->SwitchLoadMotorSpeed,SIGNAL(clicked(bool)),ui->SwitchLoadMotor,SLOT(toggle()));
    connect(ui->SwitchLoadMotorSpeed,SIGNAL(clicked(bool)),ui->SwitchLoadMotorPos,SLOT(toggle()));
    connect(ui->SwitchLoadMotorPos,SIGNAL(clicked(bool)),ui->SwitchLoadMotorSpeed,SLOT(toggle()));
    connect(ui->SwitchLoadMotorPos,SIGNAL(clicked(bool)),ui->SwitchLoadMotor,SLOT(toggle()));
}

void MainWindow::portOpenAuto()
{
    if(!port->isOpen())
    {
        if(port->open(QSerialPort::ReadWrite))
        {
            portSetParameters();
        }
        else
        {
            this->addLog("Otwarcie portu niepowiodlo sie");
        }
    }
}

void MainWindow::portSetParameters()
{
    // funkcja ustawiająca parametrry połączenia na porcie USB
    port->setBaudRate(QSerialPort::Baud115200);
    port->setDataBits(QSerialPort::Data8);
    port->setParity(QSerialPort::NoParity);
    port->setStopBits(QSerialPort::OneStop);
    port->setFlowControl(QSerialPort::NoFlowControl);

    port->setReadBufferSize(8000*18);
    port->clear();
    this->addLog("Otwarcie portu powiodlo sie");
    connect(this->port,SIGNAL(readyRead()),this,SLOT(readFromProc()));
}

void MainWindow::dataFrameRead()
{
    // funkcja odczytująca z ramki danych wartości punktów pomiarowych
    if(!dataRxCplt)
    {
        float time;
        float curr;
        float vel;
        float pos;
        uint8_t *ptr;
        ptr = (uint8_t *)&time;
        for(uint8_t i = 0; i < sizeof(float); i++) ptr[i] = tmp[4+0*sizeof(float) + i];
        ptr = (uint8_t *)&curr;
        for(uint8_t i = 0; i < sizeof(float); i++) ptr[i] = tmp[4+1*sizeof(float) + i];
        ptr = (uint8_t *)&vel;
        for(uint8_t i = 0; i < sizeof(float); i++) ptr[i] = tmp[4+2*sizeof(float) + i];
        ptr = (uint8_t *)&pos;
        for(uint8_t i = 0; i < sizeof(float); i++) ptr[i] = tmp[4+3*sizeof(float) + i];


        lineChartCurr -> pointSeries << QPointF(time,curr);
        lineChartSpeed -> pointSeries << QPointF(time,vel);
        lineChartPos -> pointSeries << QPointF(time,pos);

        timeValue[idxValue] = time;
        idxValue++;
        if(idxValue >= byteReadNo)
        {
            dataRxCplt = true;
            addLog("Wykresy zostały zaktualizowane");
            measurCplt();
            //read = false;
        }
    }
}

void MainWindow::motorParameterSend(uint8_t *parametersToSend)
{
    /* funkcja wpisująca parametry ogólne
     * do ramki danych do wysyłki */
    uint8_t *ptr;

    parametersToSend[2] = ui->trybSter->currentIndex();

    switch(ui->trybSter->currentIndex())
    {
    case 0:
            /* nieobsługiwane ponieważ jest to tryb testowy
             * sterowany z poziomu mikroprocesora */
        break;
    case 1:
        loadMotorCurrPatternParametersSend(parametersToSend);
        refCurrPatternParametersSend(parametersToSend);
        break;
    case 2:
        loadMotorSpeedPatternParametersSend(parametersToSend);
        refSpeedPatternParametersSend(parametersToSend);
        break;
    case 3:
        loadMotorPosPatternParametersSend(parametersToSend);
        refPosPatternParametersSend(parametersToSend);
        break;
    default:
        break;
    }
    int parameterNo = 20;
    float tmp = (float)ui->valueCurrTs->value();
    ptr = (uint8_t *) &tmp;
    for(uint8_t i = 0;i < sizeof(float);i++) parametersToSend[3+parameterNo*sizeof(float)+i] = ptr[i];
}



void MainWindow::refCurrPatternParametersSend(uint8_t *parametersToSend)
{
    uint8_t *ptr;
    int parameterNo = 5;
    float tmp;

    switch(ui->numberRefValueCurr->value())
    {
    case 1:
        // wpisanie do buffora wartości referencyjnych
        parameterNo = 10;
        tmp = (float)ui->valueCurrRef->value();
        ptr = (uint8_t *) &tmp;
        for(uint8_t i = 0;i < sizeof(float);i++) parametersToSend[3+parameterNo*sizeof(float)+i] = ptr[i];

        parameterNo = 11;
        tmp = 0;
        for(uint8_t i = 0;i < sizeof(float);i++) parametersToSend[3+parameterNo*sizeof(float)+i] = ptr[i];

        parameterNo = 12;
        tmp = 0;
        for(uint8_t i = 0;i < sizeof(float);i++) parametersToSend[3+parameterNo*sizeof(float)+i] = ptr[i];

        parameterNo = 13;
        tmp = 0;
        for(uint8_t i = 0;i < sizeof(float);i++) parametersToSend[3+parameterNo*sizeof(float)+i] = ptr[i];

        parameterNo = 14;
        tmp = 0;
        for(uint8_t i = 0;i < sizeof(float);i++) parametersToSend[3+parameterNo*sizeof(float)+i] = ptr[i];

        //wpisanie do buffora odpowiednich czasów
        parameterNo = 15;
        tmp = (float)ui->refCurrTime->value();
        for(uint8_t i = 0;i < sizeof(float);i++) parametersToSend[3+parameterNo*sizeof(float)+i] = ptr[i];

        parameterNo = 16;
        tmp = (float)ui->valueCurrTs->value();
        for(uint8_t i = 0;i < sizeof(float);i++) parametersToSend[3+parameterNo*sizeof(float)+i] = ptr[i];

        parameterNo = 17;
        tmp = (float)ui->valueCurrTs->value();
        for(uint8_t i = 0;i < sizeof(float);i++) parametersToSend[3+parameterNo*sizeof(float)+i] = ptr[i];

        parameterNo = 18;
        tmp = (float)ui->valueCurrTs->value();
        for(uint8_t i = 0;i < sizeof(float);i++) parametersToSend[3+parameterNo*sizeof(float)+i] = ptr[i];

        parameterNo = 19;
        tmp = (float)ui->valueCurrTs->value();
        for(uint8_t i = 0;i < sizeof(float);i++) parametersToSend[3+parameterNo*sizeof(float)+i] = ptr[i];
        break;
    case 2:
        // wpisanie do buffora wartości referencyjnych
        parameterNo = 10;
        tmp = (float)ui->valueCurrRef->value();
        ptr = (uint8_t *) &tmp;
        for(uint8_t i = 0;i < sizeof(float);i++) parametersToSend[3+parameterNo*sizeof(float)+i] = ptr[i];

        parameterNo = 11;
        tmp = (float)ui->valueCurrRef2->value();
        for(uint8_t i = 0;i < sizeof(float);i++) parametersToSend[3+parameterNo*sizeof(float)+i] = ptr[i];

        parameterNo = 12;
        tmp = 0;
        for(uint8_t i = 0;i < sizeof(float);i++) parametersToSend[3+parameterNo*sizeof(float)+i] = ptr[i];

        parameterNo = 13;
        tmp = 0;
        for(uint8_t i = 0;i < sizeof(float);i++) parametersToSend[3+parameterNo*sizeof(float)+i] = ptr[i];

        parameterNo = 14;
        tmp = 0;
        for(uint8_t i = 0;i < sizeof(float);i++) parametersToSend[3+parameterNo*sizeof(float)+i] = ptr[i];

        //wpisanie do buffora odpowiednich czasów
        parameterNo = 15;
        tmp = (float)ui->refCurrTime->value();
        for(uint8_t i = 0;i < sizeof(float);i++) parametersToSend[3+parameterNo*sizeof(float)+i] = ptr[i];

        parameterNo = 16;
        tmp = (float)ui->refCurrTime2->value();
        for(uint8_t i = 0;i < sizeof(float);i++) parametersToSend[3+parameterNo*sizeof(float)+i] = ptr[i];

        parameterNo = 17;
        tmp = (float)ui->valueCurrTs->value();
        for(uint8_t i = 0;i < sizeof(float);i++) parametersToSend[3+parameterNo*sizeof(float)+i] = ptr[i];

        parameterNo = 18;
        tmp = (float)ui->valueCurrTs->value();
        for(uint8_t i = 0;i < sizeof(float);i++) parametersToSend[3+parameterNo*sizeof(float)+i] = ptr[i];

        parameterNo = 19;
        tmp = (float)ui->valueCurrTs->value();
        for(uint8_t i = 0;i < sizeof(float);i++) parametersToSend[3+parameterNo*sizeof(float)+i] = ptr[i];
        break;
    case 3:
        // wpisanie do buffora wartości referencyjnych
        parameterNo = 10;
        tmp = (float)ui->valueCurrRef->value();
        ptr = (uint8_t *) &tmp;
        for(uint8_t i = 0;i < sizeof(float);i++) parametersToSend[3+parameterNo*sizeof(float)+i] = ptr[i];

        parameterNo = 11;
        tmp = (float)ui->valueCurrRef2->value();
        for(uint8_t i = 0;i < sizeof(float);i++) parametersToSend[3+parameterNo*sizeof(float)+i] = ptr[i];

        parameterNo = 12;
        tmp = (float)ui->valueCurrRef3->value();
        for(uint8_t i = 0;i < sizeof(float);i++) parametersToSend[3+parameterNo*sizeof(float)+i] = ptr[i];

        parameterNo = 13;
        tmp = 0;
        for(uint8_t i = 0;i < sizeof(float);i++) parametersToSend[3+parameterNo*sizeof(float)+i] = ptr[i];

        parameterNo = 14;
        tmp = 0;
        for(uint8_t i = 0;i < sizeof(float);i++) parametersToSend[3+parameterNo*sizeof(float)+i] = ptr[i];

        //wpisanie do buffora odpowiednich czasów
        parameterNo = 15;
        tmp = (float)ui->refCurrTime->value();
        for(uint8_t i = 0;i < sizeof(float);i++) parametersToSend[3+parameterNo*sizeof(float)+i] = ptr[i];

        parameterNo = 16;
        tmp = (float)ui->refCurrTime2->value();
        for(uint8_t i = 0;i < sizeof(float);i++) parametersToSend[3+parameterNo*sizeof(float)+i] = ptr[i];

        parameterNo = 17;
        tmp = (float)ui->refCurrTime3->value();
        for(uint8_t i = 0;i < sizeof(float);i++) parametersToSend[3+parameterNo*sizeof(float)+i] = ptr[i];

        parameterNo = 18;
        tmp = (float)ui->valueCurrTs->value();
        for(uint8_t i = 0;i < sizeof(float);i++) parametersToSend[3+parameterNo*sizeof(float)+i] = ptr[i];

        parameterNo = 19;
        tmp = (float)ui->valueCurrTs->value();
        for(uint8_t i = 0;i < sizeof(float);i++) parametersToSend[3+parameterNo*sizeof(float)+i] = ptr[i];
        break;
    case 4:
        // wpisanie do buffora wartości referencyjnych
        parameterNo = 10;
        tmp = (float)ui->valueCurrRef->value();
        ptr = (uint8_t *) &tmp;
        for(uint8_t i = 0;i < sizeof(float);i++) parametersToSend[3+parameterNo*sizeof(float)+i] = ptr[i];

        parameterNo = 11;
        tmp = (float)ui->valueCurrRef2->value();
        for(uint8_t i = 0;i < sizeof(float);i++) parametersToSend[3+parameterNo*sizeof(float)+i] = ptr[i];

        parameterNo = 12;
        tmp = (float)ui->valueCurrRef3->value();
        for(uint8_t i = 0;i < sizeof(float);i++) parametersToSend[3+parameterNo*sizeof(float)+i] = ptr[i];

        parameterNo = 13;
        tmp = (float)ui->valueCurrRef4->value();
        for(uint8_t i = 0;i < sizeof(float);i++) parametersToSend[3+parameterNo*sizeof(float)+i] = ptr[i];

        parameterNo = 14;
        tmp = 0;
        for(uint8_t i = 0;i < sizeof(float);i++) parametersToSend[3+parameterNo*sizeof(float)+i] = ptr[i];

        //wpisanie do buffora odpowiednich czasów
        parameterNo = 15;
        tmp = (float)ui->refCurrTime->value();
        for(uint8_t i = 0;i < sizeof(float);i++) parametersToSend[3+parameterNo*sizeof(float)+i] = ptr[i];

        parameterNo = 16;
        tmp = (float)ui->refCurrTime2->value();
        for(uint8_t i = 0;i < sizeof(float);i++) parametersToSend[3+parameterNo*sizeof(float)+i] = ptr[i];

        parameterNo = 17;
        tmp = (float)ui->refCurrTime3->value();
        for(uint8_t i = 0;i < sizeof(float);i++) parametersToSend[3+parameterNo*sizeof(float)+i] = ptr[i];

        parameterNo = 18;
        tmp = (float)ui->refCurrTime4->value();
        for(uint8_t i = 0;i < sizeof(float);i++) parametersToSend[3+parameterNo*sizeof(float)+i] = ptr[i];

        parameterNo = 19;
        tmp = (float)ui->valueCurrTs->value();
        for(uint8_t i = 0;i < sizeof(float);i++) parametersToSend[3+parameterNo*sizeof(float)+i] = ptr[i];
        break;
    case 5:
        // wpisanie do buffora wartości referencyjnych
        parameterNo = 10;
        tmp = (float)ui->valueCurrRef->value();
        ptr = (uint8_t *) &tmp;
        for(uint8_t i = 0;i < sizeof(float);i++) parametersToSend[3+parameterNo*sizeof(float)+i] = ptr[i];

        parameterNo = 11;
        tmp = (float)ui->valueCurrRef2->value();
        for(uint8_t i = 0;i < sizeof(float);i++) parametersToSend[3+parameterNo*sizeof(float)+i] = ptr[i];

        parameterNo = 12;
        tmp = (float)ui->valueCurrRef3->value();
        for(uint8_t i = 0;i < sizeof(float);i++) parametersToSend[3+parameterNo*sizeof(float)+i] = ptr[i];

        parameterNo = 13;
        tmp = (float)ui->valueCurrRef4->value();
        for(uint8_t i = 0;i < sizeof(float);i++) parametersToSend[3+parameterNo*sizeof(float)+i] = ptr[i];

        parameterNo = 14;
        tmp = (float)ui->valueCurrRef5->value();
        for(uint8_t i = 0;i < sizeof(float);i++) parametersToSend[3+parameterNo*sizeof(float)+i] = ptr[i];

        //wpisanie do buffora odpowiednich czasów
        parameterNo = 15;
        tmp = (float)ui->refCurrTime->value();
        for(uint8_t i = 0;i < sizeof(float);i++) parametersToSend[3+parameterNo*sizeof(float)+i] = ptr[i];

        parameterNo = 16;
        tmp = (float)ui->refCurrTime2->value();
        for(uint8_t i = 0;i < sizeof(float);i++) parametersToSend[3+parameterNo*sizeof(float)+i] = ptr[i];

        parameterNo = 17;
        tmp = (float)ui->refCurrTime3->value();
        for(uint8_t i = 0;i < sizeof(float);i++) parametersToSend[3+parameterNo*sizeof(float)+i] = ptr[i];

        parameterNo = 18;
        tmp = (float)ui->refCurrTime4->value();
        for(uint8_t i = 0;i < sizeof(float);i++) parametersToSend[3+parameterNo*sizeof(float)+i] = ptr[i];

        parameterNo = 19;
        tmp = (float)ui->refCurrTime5->value();
        for(uint8_t i = 0;i < sizeof(float);i++) parametersToSend[3+parameterNo*sizeof(float)+i] = ptr[i];
        break;
    default:
        break;
    }

}

void MainWindow::loadMotorCurrPatternParametersSend(uint8_t *parametersToSend)
{
    uint8_t *ptr;
    int parameterNo = 0;
    float tmp;

    // wpisanie do buffora wartości obciążenia
    if(ui->SwitchLoadMotor->isChecked())
    {
        switch(ui->numberLoadValueCurr->value())
        {
        case 1:

            parameterNo = 0;
            tmp = (float)ui->motorLoadCurr->value();
            tmp = 1.379*log(ui->motorLoadCurr->value())+44.99;
            if(tmp>43 || tmp<38)
                tmp = 0;
            ptr = (uint8_t *) &tmp;
            for(uint8_t i = 0;i < sizeof(float);i++) parametersToSend[3+parameterNo*sizeof(float)+i] = ptr[i];

            parameterNo = 1;
            tmp = 0;
            for(uint8_t i = 0;i < sizeof(float);i++) parametersToSend[3+parameterNo*sizeof(float)+i] = ptr[i];

            parameterNo = 2;
            tmp = 0;
            for(uint8_t i = 0;i < sizeof(float);i++) parametersToSend[3+parameterNo*sizeof(float)+i] = ptr[i];

            parameterNo = 3;
            tmp = 0;
            for(uint8_t i = 0;i < sizeof(float);i++) parametersToSend[3+parameterNo*sizeof(float)+i] = ptr[i];

            parameterNo = 4;
            tmp = 0;
            for(uint8_t i = 0;i < sizeof(float);i++) parametersToSend[3+parameterNo*sizeof(float)+i] = ptr[i];

            //wpisanie do buffora odpowiednich czasów
            parameterNo = 5;
            tmp = (float)ui->motorLoadCurrTime->value();
            ptr = (uint8_t *) &tmp;
            for(uint8_t i = 0;i < sizeof(float);i++) parametersToSend[3+parameterNo*sizeof(float)+i] = ptr[i];

            parameterNo = 6;
            tmp = (float)ui->valueCurrTs->value();
            for(uint8_t i = 0;i < sizeof(float);i++) parametersToSend[3+parameterNo*sizeof(float)+i] = ptr[i];

            parameterNo = 7;
            tmp = (float)ui->valueCurrTs->value();
            for(uint8_t i = 0;i < sizeof(float);i++) parametersToSend[3+parameterNo*sizeof(float)+i] = ptr[i];

            parameterNo = 8;
            tmp = (float)ui->valueCurrTs->value();
            for(uint8_t i = 0;i < sizeof(float);i++) parametersToSend[3+parameterNo*sizeof(float)+i] = ptr[i];

            parameterNo = 9;
            tmp = (float)ui->valueCurrTs->value();
            for(uint8_t i = 0;i < sizeof(float);i++) parametersToSend[3+parameterNo*sizeof(float)+i] = ptr[i];
            break;
        case 2:

            //wpisanie do buffora odpowiednich czasów
            parameterNo = 0;
            tmp = (float)ui->motorLoadCurr->value();
            tmp = 1.379*log(ui->motorLoadCurr->value())+44.99;
            if(tmp>43 || tmp<38)
                tmp = 0;
            ptr = (uint8_t *) &tmp;
            for(uint8_t i = 0;i < sizeof(float);i++) parametersToSend[3+parameterNo*sizeof(float)+i] = ptr[i];

            parameterNo = 1;
            tmp = (float)ui->motorLoadCurr2->value();
            tmp = 1.379*log(ui->motorLoadCurr2->value())+44.99;
            if(tmp>43 || tmp<38)
                tmp = 0;
            for(uint8_t i = 0;i < sizeof(float);i++) parametersToSend[3+parameterNo*sizeof(float)+i] = ptr[i];

            parameterNo = 2;
            tmp = 0;
            for(uint8_t i = 0;i < sizeof(float);i++) parametersToSend[3+parameterNo*sizeof(float)+i] = ptr[i];

            parameterNo = 3;
            tmp = 0;
            for(uint8_t i = 0;i < sizeof(float);i++) parametersToSend[3+parameterNo*sizeof(float)+i] = ptr[i];

            parameterNo = 4;
            tmp = 0;
            for(uint8_t i = 0;i < sizeof(float);i++) parametersToSend[3+parameterNo*sizeof(float)+i] = ptr[i];

            //wpisanie do buffora odpowiednich czasów
            parameterNo = 5;
            tmp = (float)ui->motorLoadCurrTime->value();
            ptr = (uint8_t *) &tmp;
            for(uint8_t i = 0;i < sizeof(float);i++) parametersToSend[3+parameterNo*sizeof(float)+i] = ptr[i];

            parameterNo = 6;
            tmp = (float)ui->motorLoadCurrTime2->value();
            for(uint8_t i = 0;i < sizeof(float);i++) parametersToSend[3+parameterNo*sizeof(float)+i] = ptr[i];

            parameterNo = 7;
            tmp = (float)ui->valueCurrTs->value();
            for(uint8_t i = 0;i < sizeof(float);i++) parametersToSend[3+parameterNo*sizeof(float)+i] = ptr[i];

            parameterNo = 8;
            tmp = (float)ui->valueCurrTs->value();
            for(uint8_t i = 0;i < sizeof(float);i++) parametersToSend[3+parameterNo*sizeof(float)+i] = ptr[i];

            parameterNo = 9;
            tmp = (float)ui->valueCurrTs->value();
            for(uint8_t i = 0;i < sizeof(float);i++) parametersToSend[3+parameterNo*sizeof(float)+i] = ptr[i];
            break;
        case 3:

            //wpisanie do buffora odpowiednich czasów
            parameterNo = 0;
            tmp = (float)ui->motorLoadCurr->value();
            tmp = 1.379*log(ui->motorLoadCurr->value())+44.99;
            if(tmp>43 || tmp<38)
                tmp = 0;
            ptr = (uint8_t *) &tmp;
            for(uint8_t i = 0;i < sizeof(float);i++) parametersToSend[3+parameterNo*sizeof(float)+i] = ptr[i];

            parameterNo = 1;
            tmp = (float)ui->motorLoadCurr2->value();
            tmp = 1.379*log(ui->motorLoadCurr2->value())+44.99;
            if(tmp>43 || tmp<38)
                tmp = 0;
            for(uint8_t i = 0;i < sizeof(float);i++) parametersToSend[3+parameterNo*sizeof(float)+i] = ptr[i];

            parameterNo = 2;
            tmp = (float)ui->motorLoadCurr3->value();
            tmp = 1.379*log(ui->motorLoadCurr3->value())+44.99;
            if(tmp>43 || tmp<38)
                tmp = 0;
            for(uint8_t i = 0;i < sizeof(float);i++) parametersToSend[3+parameterNo*sizeof(float)+i] = ptr[i];

            parameterNo = 3;
            tmp = 0;
            for(uint8_t i = 0;i < sizeof(float);i++) parametersToSend[3+parameterNo*sizeof(float)+i] = ptr[i];

            parameterNo = 4;
            tmp = 0;
            for(uint8_t i = 0;i < sizeof(float);i++) parametersToSend[3+parameterNo*sizeof(float)+i] = ptr[i];

            //wpisanie do buffora odpowiednich czasów
            parameterNo = 5;
            tmp = (float)ui->motorLoadCurrTime->value();
            ptr = (uint8_t *) &tmp;
            for(uint8_t i = 0;i < sizeof(float);i++) parametersToSend[3+parameterNo*sizeof(float)+i] = ptr[i];

            parameterNo = 6;
            tmp = (float)ui->motorLoadCurrTime2->value();
            for(uint8_t i = 0;i < sizeof(float);i++) parametersToSend[3+parameterNo*sizeof(float)+i] = ptr[i];

            parameterNo = 7;
            tmp = (float)ui->motorLoadCurrTime3->value();
            for(uint8_t i = 0;i < sizeof(float);i++) parametersToSend[3+parameterNo*sizeof(float)+i] = ptr[i];

            parameterNo = 8;
            tmp = (float)ui->valueCurrTs->value();
            for(uint8_t i = 0;i < sizeof(float);i++) parametersToSend[3+parameterNo*sizeof(float)+i] = ptr[i];

            parameterNo = 9;
            tmp = (float)ui->valueCurrTs->value();
            for(uint8_t i = 0;i < sizeof(float);i++) parametersToSend[3+parameterNo*sizeof(float)+i] = ptr[i];
            break;
        case 4:
            //wpisanie do buffora odpowiednich czasów
            parameterNo = 0;
            tmp = (float)ui->motorLoadCurr->value();
            tmp = 1.379*log(ui->motorLoadCurr->value())+44.99;
            if(tmp>43 || tmp<38)
                tmp = 0;
            ptr = (uint8_t *) &tmp;
            for(uint8_t i = 0;i < sizeof(float);i++) parametersToSend[3+parameterNo*sizeof(float)+i] = ptr[i];

            parameterNo = 1;
            tmp = (float)ui->motorLoadCurr2->value();
            tmp = 1.379*log(ui->motorLoadCurr2->value())+44.99;
            if(tmp>43 || tmp<38)
                tmp = 0;
            for(uint8_t i = 0;i < sizeof(float);i++) parametersToSend[3+parameterNo*sizeof(float)+i] = ptr[i];

            parameterNo = 2;
            tmp = (float)ui->motorLoadCurr3->value();
            tmp = 1.379*log(ui->motorLoadCurr3->value())+44.99;
            if(tmp>43 || tmp<38)
                tmp = 0;
            for(uint8_t i = 0;i < sizeof(float);i++) parametersToSend[3+parameterNo*sizeof(float)+i] = ptr[i];

            parameterNo = 3;
            tmp = (float)ui->motorLoadCurr4->value();
            tmp = 1.379*log(ui->motorLoadCurr4->value())+44.99;
            if(tmp>43 || tmp<38)
                tmp = 0;
            for(uint8_t i = 0;i < sizeof(float);i++) parametersToSend[3+parameterNo*sizeof(float)+i] = ptr[i];

            parameterNo = 4;
            tmp = 0;
            for(uint8_t i = 0;i < sizeof(float);i++) parametersToSend[3+parameterNo*sizeof(float)+i] = ptr[i];

            //wpisanie do buffora odpowiednich czasów
            parameterNo = 5;
            tmp = (float)ui->motorLoadCurrTime->value();
            ptr = (uint8_t *) &tmp;
            for(uint8_t i = 0;i < sizeof(float);i++) parametersToSend[3+parameterNo*sizeof(float)+i] = ptr[i];

            parameterNo = 6;
            tmp = (float)ui->motorLoadCurrTime2->value();
            for(uint8_t i = 0;i < sizeof(float);i++) parametersToSend[3+parameterNo*sizeof(float)+i] = ptr[i];

            parameterNo = 7;
            tmp = (float)ui->motorLoadCurrTime3->value();
            for(uint8_t i = 0;i < sizeof(float);i++) parametersToSend[3+parameterNo*sizeof(float)+i] = ptr[i];

            parameterNo = 8;
            tmp = (float)ui->motorLoadCurrTime4->value();
            for(uint8_t i = 0;i < sizeof(float);i++) parametersToSend[3+parameterNo*sizeof(float)+i] = ptr[i];

            parameterNo = 9;
            tmp = (float)ui->valueCurrTs->value();
            for(uint8_t i = 0;i < sizeof(float);i++) parametersToSend[3+parameterNo*sizeof(float)+i] = ptr[i];
            break;

        case 5:

            //wpisanie do buffora odpowiednich czasów
            parameterNo = 0;
            tmp = (float)ui->motorLoadCurr->value();
            tmp = 1.379*log(ui->motorLoadCurr->value())+44.99;
            if(tmp>43 || tmp<38)
                tmp = 0;
            ptr = (uint8_t *) &tmp;
            for(uint8_t i = 0;i < sizeof(float);i++) parametersToSend[3+parameterNo*sizeof(float)+i] = ptr[i];

            parameterNo = 1;
            tmp = (float)ui->motorLoadCurr2->value();
            tmp = 1.379*log(ui->motorLoadCurr2->value())+44.99;
            if(tmp>43 || tmp<38)
                tmp = 0;
            for(uint8_t i = 0;i < sizeof(float);i++) parametersToSend[3+parameterNo*sizeof(float)+i] = ptr[i];

            parameterNo = 2;
            tmp = (float)ui->motorLoadCurr3->value();
            tmp = 1.379*log(ui->motorLoadCurr3->value())+44.99;
            if(tmp>43 || tmp<38)
                tmp = 0;
            for(uint8_t i = 0;i < sizeof(float);i++) parametersToSend[3+parameterNo*sizeof(float)+i] = ptr[i];

            parameterNo = 3;
            tmp = (float)ui->motorLoadCurr4->value();
            tmp = 1.379*log(ui->motorLoadCurr4->value())+44.99;
            if(tmp>43 || tmp<38)
                tmp = 0;
            for(uint8_t i = 0;i < sizeof(float);i++) parametersToSend[3+parameterNo*sizeof(float)+i] = ptr[i];

            parameterNo = 4;
            tmp = (float)ui->motorLoadCurr5->value();
            tmp = 1.379*log(ui->motorLoadCurr5->value())+44.99;
            if(tmp>43 || tmp<38)
                tmp = 0;
            for(uint8_t i = 0;i < sizeof(float);i++) parametersToSend[3+parameterNo*sizeof(float)+i] = ptr[i];

            //wpisanie do buffora odpowiednich czasów
            parameterNo = 5;
            tmp = (float)ui->motorLoadCurrTime->value();
            ptr = (uint8_t *) &tmp;
            for(uint8_t i = 0;i < sizeof(float);i++) parametersToSend[3+parameterNo*sizeof(float)+i] = ptr[i];

            parameterNo = 6;
            tmp = (float)ui->motorLoadCurrTime2->value();
            for(uint8_t i = 0;i < sizeof(float);i++) parametersToSend[3+parameterNo*sizeof(float)+i] = ptr[i];

            parameterNo = 7;
            tmp = (float)ui->motorLoadCurrTime3->value();
            for(uint8_t i = 0;i < sizeof(float);i++) parametersToSend[3+parameterNo*sizeof(float)+i] = ptr[i];

            parameterNo = 8;
            tmp = (float)ui->motorLoadCurrTime4->value();
            for(uint8_t i = 0;i < sizeof(float);i++) parametersToSend[3+parameterNo*sizeof(float)+i] = ptr[i];

            parameterNo = 9;
            tmp = (float)ui->motorLoadCurrTime5->value();
            for(uint8_t i = 0;i < sizeof(float);i++) parametersToSend[3+parameterNo*sizeof(float)+i] = ptr[i];
            break;
        default:
            break;
        }
    }
    else
    {
        parameterNo = 0;
        tmp = 0;
        ptr = (uint8_t *) &tmp;
        for(uint8_t i = 0;i < sizeof(float);i++) parametersToSend[3+parameterNo*sizeof(float)+i] = ptr[i];

        parameterNo = 1;
        tmp = 0;
        for(uint8_t i = 0;i < sizeof(float);i++) parametersToSend[3+parameterNo*sizeof(float)+i] = ptr[i];

        parameterNo = 2;
        tmp = 0;
        for(uint8_t i = 0;i < sizeof(float);i++) parametersToSend[3+parameterNo*sizeof(float)+i] = ptr[i];

        parameterNo = 3;
        tmp = 0;
        for(uint8_t i = 0;i < sizeof(float);i++) parametersToSend[3+parameterNo*sizeof(float)+i] = ptr[i];

        parameterNo = 4;
        tmp = 0;
        for(uint8_t i = 0;i < sizeof(float);i++) parametersToSend[3+parameterNo*sizeof(float)+i] = ptr[i];

        //wpisanie do buffora odpowiednich czasów
        parameterNo = 5;
        tmp = 0;
        ptr = (uint8_t *) &tmp;
        for(uint8_t i = 0;i < sizeof(float);i++) parametersToSend[3+parameterNo*sizeof(float)+i] = ptr[i];

        parameterNo = 6;
        tmp = 0;
        for(uint8_t i = 0;i < sizeof(float);i++) parametersToSend[3+parameterNo*sizeof(float)+i] = ptr[i];

        parameterNo = 7;
        tmp = 0;
        for(uint8_t i = 0;i < sizeof(float);i++) parametersToSend[3+parameterNo*sizeof(float)+i] = ptr[i];

        parameterNo = 8;
        tmp = 0;
        for(uint8_t i = 0;i < sizeof(float);i++) parametersToSend[3+parameterNo*sizeof(float)+i] = ptr[i];

        parameterNo = 9;
        tmp = 0;
        for(uint8_t i = 0;i < sizeof(float);i++) parametersToSend[3+parameterNo*sizeof(float)+i] = ptr[i];

    }
}

void MainWindow::refSpeedPatternParametersSend(uint8_t *parametersToSend)
{
    uint8_t *ptr;
    int parameterNo = 5;
    float tmp;

    switch(ui->numberRefValueSpeed->value())
    {
    case 1:
        // wpisanie do buffora wartości referencyjnych
        parameterNo = 10;
        tmp = (float)ui->valueSpeedRef->value();
        ptr = (uint8_t *) &tmp;
        for(uint8_t i = 0;i < sizeof(float);i++) parametersToSend[3+parameterNo*sizeof(float)+i] = ptr[i];

        parameterNo = 11;
        tmp = 0;
        for(uint8_t i = 0;i < sizeof(float);i++) parametersToSend[3+parameterNo*sizeof(float)+i] = ptr[i];

        parameterNo = 12;
        tmp = 0;
        for(uint8_t i = 0;i < sizeof(float);i++) parametersToSend[3+parameterNo*sizeof(float)+i] = ptr[i];

        parameterNo = 13;
        tmp = 0;
        for(uint8_t i = 0;i < sizeof(float);i++) parametersToSend[3+parameterNo*sizeof(float)+i] = ptr[i];

        parameterNo = 14;
        tmp = 0;
        for(uint8_t i = 0;i < sizeof(float);i++) parametersToSend[3+parameterNo*sizeof(float)+i] = ptr[i];

        //wpisanie do buffora odpowiednich czasów
        parameterNo = 15;
        tmp = (float)ui->refSpeedTime->value();
        for(uint8_t i = 0;i < sizeof(float);i++) parametersToSend[3+parameterNo*sizeof(float)+i] = ptr[i];

        parameterNo = 16;
        tmp = (float)ui->valueSpeedTs->value();
        for(uint8_t i = 0;i < sizeof(float);i++) parametersToSend[3+parameterNo*sizeof(float)+i] = ptr[i];

        parameterNo = 17;
        tmp = (float)ui->valueSpeedTs->value();
        for(uint8_t i = 0;i < sizeof(float);i++) parametersToSend[3+parameterNo*sizeof(float)+i] = ptr[i];

        parameterNo = 18;
        tmp = (float)ui->valueSpeedTs->value();
        for(uint8_t i = 0;i < sizeof(float);i++) parametersToSend[3+parameterNo*sizeof(float)+i] = ptr[i];

        parameterNo = 19;
        tmp = (float)ui->valueSpeedTs->value();
        for(uint8_t i = 0;i < sizeof(float);i++) parametersToSend[3+parameterNo*sizeof(float)+i] = ptr[i];
        break;
    case 2:
        // wpisanie do buffora wartości referencyjnych
        parameterNo = 10;
        tmp = (float)ui->valueSpeedRef->value();
        ptr = (uint8_t *) &tmp;
        for(uint8_t i = 0;i < sizeof(float);i++) parametersToSend[3+parameterNo*sizeof(float)+i] = ptr[i];

        parameterNo = 11;
        tmp = (float)ui->valueSpeedRef2->value();
        for(uint8_t i = 0;i < sizeof(float);i++) parametersToSend[3+parameterNo*sizeof(float)+i] = ptr[i];

        parameterNo = 12;
        tmp = 0;
        for(uint8_t i = 0;i < sizeof(float);i++) parametersToSend[3+parameterNo*sizeof(float)+i] = ptr[i];

        parameterNo = 13;
        tmp = 0;
        for(uint8_t i = 0;i < sizeof(float);i++) parametersToSend[3+parameterNo*sizeof(float)+i] = ptr[i];

        parameterNo = 14;
        tmp = 0;
        for(uint8_t i = 0;i < sizeof(float);i++) parametersToSend[3+parameterNo*sizeof(float)+i] = ptr[i];

        //wpisanie do buffora odpowiednich czasów
        parameterNo = 15;
        tmp = (float)ui->refSpeedTime->value();
        for(uint8_t i = 0;i < sizeof(float);i++) parametersToSend[3+parameterNo*sizeof(float)+i] = ptr[i];

        parameterNo = 16;
        tmp = (float)ui->refSpeedTime2->value();
        for(uint8_t i = 0;i < sizeof(float);i++) parametersToSend[3+parameterNo*sizeof(float)+i] = ptr[i];

        parameterNo = 17;
        tmp = (float)ui->valueSpeedTs->value();
        for(uint8_t i = 0;i < sizeof(float);i++) parametersToSend[3+parameterNo*sizeof(float)+i] = ptr[i];

        parameterNo = 18;
        tmp = (float)ui->valueSpeedTs->value();
        for(uint8_t i = 0;i < sizeof(float);i++) parametersToSend[3+parameterNo*sizeof(float)+i] = ptr[i];

        parameterNo = 19;
        tmp = (float)ui->valueSpeedTs->value();
        for(uint8_t i = 0;i < sizeof(float);i++) parametersToSend[3+parameterNo*sizeof(float)+i] = ptr[i];
        break;
    case 3:
        // wpisanie do buffora wartości referencyjnych
        parameterNo = 10;
        tmp = (float)ui->valueSpeedRef->value();
        ptr = (uint8_t *) &tmp;
        for(uint8_t i = 0;i < sizeof(float);i++) parametersToSend[3+parameterNo*sizeof(float)+i] = ptr[i];

        parameterNo = 11;
        tmp = (float)ui->valueSpeedRef2->value();
        for(uint8_t i = 0;i < sizeof(float);i++) parametersToSend[3+parameterNo*sizeof(float)+i] = ptr[i];

        parameterNo = 12;
        tmp = (float)ui->valueSpeedRef3->value();
        for(uint8_t i = 0;i < sizeof(float);i++) parametersToSend[3+parameterNo*sizeof(float)+i] = ptr[i];

        parameterNo = 13;
        tmp = 0;
        for(uint8_t i = 0;i < sizeof(float);i++) parametersToSend[3+parameterNo*sizeof(float)+i] = ptr[i];

        parameterNo = 14;
        tmp = 0;
        for(uint8_t i = 0;i < sizeof(float);i++) parametersToSend[3+parameterNo*sizeof(float)+i] = ptr[i];

        //wpisanie do buffora odpowiednich czasów
        parameterNo = 15;
        tmp = (float)ui->refSpeedTime->value();
        for(uint8_t i = 0;i < sizeof(float);i++) parametersToSend[3+parameterNo*sizeof(float)+i] = ptr[i];

        parameterNo = 16;
        tmp = (float)ui->refSpeedTime2->value();
        for(uint8_t i = 0;i < sizeof(float);i++) parametersToSend[3+parameterNo*sizeof(float)+i] = ptr[i];

        parameterNo = 17;
        tmp = (float)ui->refSpeedTime3->value();
        for(uint8_t i = 0;i < sizeof(float);i++) parametersToSend[3+parameterNo*sizeof(float)+i] = ptr[i];

        parameterNo = 18;
        tmp = (float)ui->valueSpeedTs->value();
        for(uint8_t i = 0;i < sizeof(float);i++) parametersToSend[3+parameterNo*sizeof(float)+i] = ptr[i];

        parameterNo = 19;
        tmp = (float)ui->valueSpeedTs->value();
        for(uint8_t i = 0;i < sizeof(float);i++) parametersToSend[3+parameterNo*sizeof(float)+i] = ptr[i];
        break;
    case 4:
        // wpisanie do buffora wartości referencyjnych
        parameterNo = 10;
        tmp = (float)ui->valueSpeedRef->value();
        ptr = (uint8_t *) &tmp;
        for(uint8_t i = 0;i < sizeof(float);i++) parametersToSend[3+parameterNo*sizeof(float)+i] = ptr[i];

        parameterNo = 11;
        tmp = (float)ui->valueSpeedRef2->value();
        for(uint8_t i = 0;i < sizeof(float);i++) parametersToSend[3+parameterNo*sizeof(float)+i] = ptr[i];

        parameterNo = 12;
        tmp = (float)ui->valueSpeedRef3->value();
        for(uint8_t i = 0;i < sizeof(float);i++) parametersToSend[3+parameterNo*sizeof(float)+i] = ptr[i];

        parameterNo = 13;
        tmp = (float)ui->valueSpeedRef4->value();
        for(uint8_t i = 0;i < sizeof(float);i++) parametersToSend[3+parameterNo*sizeof(float)+i] = ptr[i];

        parameterNo = 14;
        tmp = 0;
        for(uint8_t i = 0;i < sizeof(float);i++) parametersToSend[3+parameterNo*sizeof(float)+i] = ptr[i];

        //wpisanie do buffora odpowiednich czasów
        parameterNo = 15;
        tmp = (float)ui->refSpeedTime->value();
        for(uint8_t i = 0;i < sizeof(float);i++) parametersToSend[3+parameterNo*sizeof(float)+i] = ptr[i];

        parameterNo = 16;
        tmp = (float)ui->refSpeedTime2->value();
        for(uint8_t i = 0;i < sizeof(float);i++) parametersToSend[3+parameterNo*sizeof(float)+i] = ptr[i];

        parameterNo = 17;
        tmp = (float)ui->refSpeedTime3->value();
        for(uint8_t i = 0;i < sizeof(float);i++) parametersToSend[3+parameterNo*sizeof(float)+i] = ptr[i];

        parameterNo = 18;
        tmp = (float)ui->refSpeedTime4->value();
        for(uint8_t i = 0;i < sizeof(float);i++) parametersToSend[3+parameterNo*sizeof(float)+i] = ptr[i];

        parameterNo = 19;
        tmp = (float)ui->valueSpeedTs->value();
        for(uint8_t i = 0;i < sizeof(float);i++) parametersToSend[3+parameterNo*sizeof(float)+i] = ptr[i];
        break;
    case 5:
        // wpisanie do buffora wartości referencyjnych
        parameterNo = 10;
        tmp = (float)ui->valueSpeedRef->value();
        ptr = (uint8_t *) &tmp;
        for(uint8_t i = 0;i < sizeof(float);i++) parametersToSend[3+parameterNo*sizeof(float)+i] = ptr[i];

        parameterNo = 11;
        tmp = (float)ui->valueSpeedRef2->value();
        for(uint8_t i = 0;i < sizeof(float);i++) parametersToSend[3+parameterNo*sizeof(float)+i] = ptr[i];

        parameterNo = 12;
        tmp = (float)ui->valueSpeedRef3->value();
        for(uint8_t i = 0;i < sizeof(float);i++) parametersToSend[3+parameterNo*sizeof(float)+i] = ptr[i];

        parameterNo = 13;
        tmp = (float)ui->valueSpeedRef4->value();
        for(uint8_t i = 0;i < sizeof(float);i++) parametersToSend[3+parameterNo*sizeof(float)+i] = ptr[i];

        parameterNo = 14;
        tmp = (float)ui->valueSpeedRef5->value();
        for(uint8_t i = 0;i < sizeof(float);i++) parametersToSend[3+parameterNo*sizeof(float)+i] = ptr[i];

        //wpisanie do buffora odpowiednich czasów
        parameterNo = 15;
        tmp = (float)ui->refSpeedTime->value();
        for(uint8_t i = 0;i < sizeof(float);i++) parametersToSend[3+parameterNo*sizeof(float)+i] = ptr[i];

        parameterNo = 16;
        tmp = (float)ui->refSpeedTime2->value();
        for(uint8_t i = 0;i < sizeof(float);i++) parametersToSend[3+parameterNo*sizeof(float)+i] = ptr[i];

        parameterNo = 17;
        tmp = (float)ui->refSpeedTime3->value();
        for(uint8_t i = 0;i < sizeof(float);i++) parametersToSend[3+parameterNo*sizeof(float)+i] = ptr[i];

        parameterNo = 18;
        tmp = (float)ui->refSpeedTime4->value();
        for(uint8_t i = 0;i < sizeof(float);i++) parametersToSend[3+parameterNo*sizeof(float)+i] = ptr[i];

        parameterNo = 19;
        tmp = (float)ui->refSpeedTime5->value();
        for(uint8_t i = 0;i < sizeof(float);i++) parametersToSend[3+parameterNo*sizeof(float)+i] = ptr[i];
        break;
    default:
        break;
    }
}

void MainWindow::loadMotorSpeedPatternParametersSend(uint8_t *parametersToSend)
{
    uint8_t *ptr;
    int parameterNo = 0;
    float tmp;
    // wpisanie do buffora wartości obciążenia
    if(ui->SwitchLoadMotorSpeed->isChecked())
    {
        switch(ui->numberLoadValueSpeed->value())
        {
        case 1:

            parameterNo = 0;
            tmp = (float)ui->motorLoadSpeed->value();
            tmp = 1.379*log(ui->motorLoadSpeed->value())+44.99;
            if(tmp>43 || tmp<38)
                tmp = 0;
            ptr = (uint8_t *) &tmp;
            for(uint8_t i = 0;i < sizeof(float);i++) parametersToSend[3+parameterNo*sizeof(float)+i] = ptr[i];

            parameterNo = 1;
            tmp = 0;
            for(uint8_t i = 0;i < sizeof(float);i++) parametersToSend[3+parameterNo*sizeof(float)+i] = ptr[i];

            parameterNo = 2;
            tmp = 0;
            for(uint8_t i = 0;i < sizeof(float);i++) parametersToSend[3+parameterNo*sizeof(float)+i] = ptr[i];

            parameterNo = 3;
            tmp = 0;
            for(uint8_t i = 0;i < sizeof(float);i++) parametersToSend[3+parameterNo*sizeof(float)+i] = ptr[i];

            parameterNo = 4;
            tmp = 0;
            for(uint8_t i = 0;i < sizeof(float);i++) parametersToSend[3+parameterNo*sizeof(float)+i] = ptr[i];

            //wpisanie do buffora odpowiednich czasów
            parameterNo = 5;
            tmp = (float)ui->motorLoadSpeedTime->value();
            ptr = (uint8_t *) &tmp;
            for(uint8_t i = 0;i < sizeof(float);i++) parametersToSend[3+parameterNo*sizeof(float)+i] = ptr[i];

            parameterNo = 6;
            tmp = (float)ui->valueSpeedTs->value();
            for(uint8_t i = 0;i < sizeof(float);i++) parametersToSend[3+parameterNo*sizeof(float)+i] = ptr[i];

            parameterNo = 7;
            tmp = (float)ui->valueSpeedTs->value();
            for(uint8_t i = 0;i < sizeof(float);i++) parametersToSend[3+parameterNo*sizeof(float)+i] = ptr[i];

            parameterNo = 8;
            tmp = (float)ui->valueSpeedTs->value();
            for(uint8_t i = 0;i < sizeof(float);i++) parametersToSend[3+parameterNo*sizeof(float)+i] = ptr[i];

            parameterNo = 9;
            tmp = (float)ui->valueSpeedTs->value();
            for(uint8_t i = 0;i < sizeof(float);i++) parametersToSend[3+parameterNo*sizeof(float)+i] = ptr[i];

            break;
        case 2:

            parameterNo = 0;
            tmp = (float)ui->motorLoadSpeed->value();
            tmp = 1.379*log(ui->motorLoadSpeed->value())+44.99;
            if(tmp>43 || tmp<38)
                tmp = 0;
            ptr = (uint8_t *) &tmp;
            for(uint8_t i = 0;i < sizeof(float);i++) parametersToSend[3+parameterNo*sizeof(float)+i] = ptr[i];

            parameterNo = 1;
            tmp = (float)ui->motorLoadSpeed2->value();
            tmp = 1.379*log(ui->motorLoadSpeed2->value())+44.99;
            if(tmp>43 || tmp<38)
                tmp = 0;
            for(uint8_t i = 0;i < sizeof(float);i++) parametersToSend[3+parameterNo*sizeof(float)+i] = ptr[i];

            parameterNo = 2;
            tmp = 0;
            for(uint8_t i = 0;i < sizeof(float);i++) parametersToSend[3+parameterNo*sizeof(float)+i] = ptr[i];

            parameterNo = 3;
            tmp = 0;
            for(uint8_t i = 0;i < sizeof(float);i++) parametersToSend[3+parameterNo*sizeof(float)+i] = ptr[i];

            parameterNo = 4;
            tmp = 0;
            for(uint8_t i = 0;i < sizeof(float);i++) parametersToSend[3+parameterNo*sizeof(float)+i] = ptr[i];

            //wpisanie do buffora odpowiednich czasów
            parameterNo = 5;
            tmp = (float)ui->motorLoadSpeedTime->value();
            ptr = (uint8_t *) &tmp;
            for(uint8_t i = 0;i < sizeof(float);i++) parametersToSend[3+parameterNo*sizeof(float)+i] = ptr[i];

            parameterNo = 6;
            tmp = (float)ui->motorLoadSpeedTime2->value();
            for(uint8_t i = 0;i < sizeof(float);i++) parametersToSend[3+parameterNo*sizeof(float)+i] = ptr[i];

            parameterNo = 7;
            tmp = (float)ui->valueSpeedTs->value();
            for(uint8_t i = 0;i < sizeof(float);i++) parametersToSend[3+parameterNo*sizeof(float)+i] = ptr[i];

            parameterNo = 8;
            tmp = (float)ui->valueSpeedTs->value();
            for(uint8_t i = 0;i < sizeof(float);i++) parametersToSend[3+parameterNo*sizeof(float)+i] = ptr[i];

            parameterNo = 9;
            tmp = (float)ui->valueSpeedTs->value();
            for(uint8_t i = 0;i < sizeof(float);i++) parametersToSend[3+parameterNo*sizeof(float)+i] = ptr[i];
            break;
        case 3:

            parameterNo = 0;
            tmp = (float)ui->motorLoadSpeed->value();
            tmp = 1.379*log(ui->motorLoadSpeed->value())+44.99;
            if(tmp>43 || tmp<38)
                tmp = 0;
            ptr = (uint8_t *) &tmp;
            for(uint8_t i = 0;i < sizeof(float);i++) parametersToSend[3+parameterNo*sizeof(float)+i] = ptr[i];

            parameterNo = 1;
            tmp = (float)ui->motorLoadSpeed2->value();
            tmp = 1.379*log(ui->motorLoadSpeed2->value())+44.99;
            if(tmp>43 || tmp<38)
                tmp = 0;
            for(uint8_t i = 0;i < sizeof(float);i++) parametersToSend[3+parameterNo*sizeof(float)+i] = ptr[i];

            parameterNo = 2;
            tmp = (float)ui->motorLoadSpeed3->value();
            tmp = 1.379*log(ui->motorLoadSpeed3->value())+44.99;
            if(tmp>43 || tmp<38)
                tmp = 0;
            for(uint8_t i = 0;i < sizeof(float);i++) parametersToSend[3+parameterNo*sizeof(float)+i] = ptr[i];

            parameterNo = 3;
            tmp = 0;
            for(uint8_t i = 0;i < sizeof(float);i++) parametersToSend[3+parameterNo*sizeof(float)+i] = ptr[i];

            parameterNo = 4;
            tmp = 0;
            for(uint8_t i = 0;i < sizeof(float);i++) parametersToSend[3+parameterNo*sizeof(float)+i] = ptr[i];


            //wpisanie do buffora odpowiednich czasów
            parameterNo = 5;
            tmp = (float)ui->motorLoadSpeedTime->value();
            ptr = (uint8_t *) &tmp;
            for(uint8_t i = 0;i < sizeof(float);i++) parametersToSend[3+parameterNo*sizeof(float)+i] = ptr[i];

            parameterNo = 6;
            tmp = (float)ui->motorLoadSpeedTime2->value();
            for(uint8_t i = 0;i < sizeof(float);i++) parametersToSend[3+parameterNo*sizeof(float)+i] = ptr[i];

            parameterNo = 7;
            tmp = (float)ui->motorLoadSpeedTime3->value();
            for(uint8_t i = 0;i < sizeof(float);i++) parametersToSend[3+parameterNo*sizeof(float)+i] = ptr[i];

            parameterNo = 8;
            tmp = (float)ui->valueSpeedTs->value();
            for(uint8_t i = 0;i < sizeof(float);i++) parametersToSend[3+parameterNo*sizeof(float)+i] = ptr[i];

            parameterNo = 9;
            tmp = (float)ui->valueSpeedTs->value();
            for(uint8_t i = 0;i < sizeof(float);i++) parametersToSend[3+parameterNo*sizeof(float)+i] = ptr[i];
            break;
        case 4:

            parameterNo = 0;
            tmp = (float)ui->motorLoadSpeed->value();
            tmp = 1.379*log(ui->motorLoadSpeed->value())+44.99;
            if(tmp>43 || tmp<38)
                tmp = 0;
            ptr = (uint8_t *) &tmp;
            for(uint8_t i = 0;i < sizeof(float);i++) parametersToSend[3+parameterNo*sizeof(float)+i] = ptr[i];

            parameterNo = 1;
            tmp = (float)ui->motorLoadSpeed2->value();
            tmp = 1.379*log(ui->motorLoadSpeed2->value())+44.99;
            if(tmp>43 || tmp<38)
                tmp = 0;
            for(uint8_t i = 0;i < sizeof(float);i++) parametersToSend[3+parameterNo*sizeof(float)+i] = ptr[i];

            parameterNo = 2;
            tmp = (float)ui->motorLoadSpeed3->value();
            tmp = 1.379*log(ui->motorLoadSpeed3->value())+44.99;
            if(tmp>43 || tmp<38)
                tmp = 0;
            for(uint8_t i = 0;i < sizeof(float);i++) parametersToSend[3+parameterNo*sizeof(float)+i] = ptr[i];

            parameterNo = 3;
            tmp = (float)ui->motorLoadSpeed4->value();
            tmp = 1.379*log(ui->motorLoadSpeed4->value())+44.99;
            if(tmp>43 || tmp<38)
                tmp = 0;
            for(uint8_t i = 0;i < sizeof(float);i++) parametersToSend[3+parameterNo*sizeof(float)+i] = ptr[i];

            parameterNo = 4;
            tmp = 0;
            for(uint8_t i = 0;i < sizeof(float);i++) parametersToSend[3+parameterNo*sizeof(float)+i] = ptr[i];

            //wpisanie do buffora odpowiednich czasów
            parameterNo = 5;
            tmp = (float)ui->motorLoadSpeedTime->value();
            ptr = (uint8_t *) &tmp;
            for(uint8_t i = 0;i < sizeof(float);i++) parametersToSend[3+parameterNo*sizeof(float)+i] = ptr[i];

            parameterNo = 6;
            tmp = (float)ui->motorLoadSpeedTime2->value();
            for(uint8_t i = 0;i < sizeof(float);i++) parametersToSend[3+parameterNo*sizeof(float)+i] = ptr[i];

            parameterNo = 7;
            tmp = (float)ui->motorLoadSpeedTime3->value();
            for(uint8_t i = 0;i < sizeof(float);i++) parametersToSend[3+parameterNo*sizeof(float)+i] = ptr[i];

            parameterNo = 8;
            tmp = (float)ui->motorLoadSpeedTime4->value();
            for(uint8_t i = 0;i < sizeof(float);i++) parametersToSend[3+parameterNo*sizeof(float)+i] = ptr[i];

            parameterNo = 9;
            tmp = (float)ui->valueSpeedTs->value();
            for(uint8_t i = 0;i < sizeof(float);i++) parametersToSend[3+parameterNo*sizeof(float)+i] = ptr[i];
            break;
        case 5:

            parameterNo = 0;
            tmp = (float)ui->motorLoadSpeed->value();
            tmp = 1.379*log(ui->motorLoadSpeed->value())+44.99;
            if(tmp>43 || tmp<38)
                tmp = 0;
            ptr = (uint8_t *) &tmp;
            for(uint8_t i = 0;i < sizeof(float);i++) parametersToSend[3+parameterNo*sizeof(float)+i] = ptr[i];

            parameterNo = 1;
            tmp = (float)ui->motorLoadSpeed2->value();
            tmp = 1.379*log(ui->motorLoadSpeed2->value())+44.99;
            if(tmp>43 || tmp<38)
                tmp = 0;
            for(uint8_t i = 0;i < sizeof(float);i++) parametersToSend[3+parameterNo*sizeof(float)+i] = ptr[i];

            parameterNo = 2;
            tmp = (float)ui->motorLoadSpeed3->value();
            tmp = 1.379*log(ui->motorLoadSpeed3->value())+44.99;
            if(tmp>43 || tmp<38)
                tmp = 0;
            for(uint8_t i = 0;i < sizeof(float);i++) parametersToSend[3+parameterNo*sizeof(float)+i] = ptr[i];

            parameterNo = 3;
            tmp = (float)ui->motorLoadSpeed4->value();
            tmp = 1.379*log(ui->motorLoadSpeed4->value())+44.99;
            if(tmp>43 || tmp<38)
                tmp = 0;
            for(uint8_t i = 0;i < sizeof(float);i++) parametersToSend[3+parameterNo*sizeof(float)+i] = ptr[i];

            parameterNo = 4;
            tmp = (float)ui->motorLoadSpeed5->value();
            tmp = 1.379*log(ui->motorLoadSpeed5->value())+44.99;
            if(tmp>43 || tmp<38)
                tmp = 0;
            for(uint8_t i = 0;i < sizeof(float);i++) parametersToSend[3+parameterNo*sizeof(float)+i] = ptr[i];

            //wpisanie do buffora odpowiednich czasów
            parameterNo = 5;
            tmp = (float)ui->motorLoadSpeedTime->value();
            ptr = (uint8_t *) &tmp;
            for(uint8_t i = 0;i < sizeof(float);i++) parametersToSend[3+parameterNo*sizeof(float)+i] = ptr[i];

            parameterNo = 6;
            tmp = (float)ui->motorLoadSpeedTime2->value();
            for(uint8_t i = 0;i < sizeof(float);i++) parametersToSend[3+parameterNo*sizeof(float)+i] = ptr[i];

            parameterNo = 7;
            tmp = (float)ui->motorLoadSpeedTime3->value();
            for(uint8_t i = 0;i < sizeof(float);i++) parametersToSend[3+parameterNo*sizeof(float)+i] = ptr[i];

            parameterNo = 8;
            tmp = (float)ui->motorLoadSpeedTime4->value();
            for(uint8_t i = 0;i < sizeof(float);i++) parametersToSend[3+parameterNo*sizeof(float)+i] = ptr[i];

            parameterNo = 9;
            tmp = (float)ui->motorLoadSpeedTime5->value();
            for(uint8_t i = 0;i < sizeof(float);i++) parametersToSend[3+parameterNo*sizeof(float)+i] = ptr[i];
            break;
        default:
            break;
        }
    }
    else
    {
        parameterNo = 0;
        tmp = 0;
        ptr = (uint8_t *) &tmp;
        for(uint8_t i = 0;i < sizeof(float);i++) parametersToSend[3+parameterNo*sizeof(float)+i] = ptr[i];

        parameterNo = 1;
        tmp = 0;
        for(uint8_t i = 0;i < sizeof(float);i++) parametersToSend[3+parameterNo*sizeof(float)+i] = ptr[i];

        parameterNo = 2;
        tmp = 0;
        for(uint8_t i = 0;i < sizeof(float);i++) parametersToSend[3+parameterNo*sizeof(float)+i] = ptr[i];

        parameterNo = 3;
        tmp = 0;
        for(uint8_t i = 0;i < sizeof(float);i++) parametersToSend[3+parameterNo*sizeof(float)+i] = ptr[i];

        parameterNo = 4;
        tmp = 0;
        for(uint8_t i = 0;i < sizeof(float);i++) parametersToSend[3+parameterNo*sizeof(float)+i] = ptr[i];


        //wpisanie do buffora odpowiednich czasów
        parameterNo = 5;
        tmp = 0;
        ptr = (uint8_t *) &tmp;
        for(uint8_t i = 0;i < sizeof(float);i++) parametersToSend[3+parameterNo*sizeof(float)+i] = ptr[i];

        parameterNo = 6;
        tmp = 0;
        for(uint8_t i = 0;i < sizeof(float);i++) parametersToSend[3+parameterNo*sizeof(float)+i] = ptr[i];

        parameterNo = 7;
        tmp = 0;
        for(uint8_t i = 0;i < sizeof(float);i++) parametersToSend[3+parameterNo*sizeof(float)+i] = ptr[i];

        parameterNo = 8;
        tmp = 0;
        for(uint8_t i = 0;i < sizeof(float);i++) parametersToSend[3+parameterNo*sizeof(float)+i] = ptr[i];

        parameterNo = 9;
        tmp = 0;
        for(uint8_t i = 0;i < sizeof(float);i++) parametersToSend[3+parameterNo*sizeof(float)+i] = ptr[i];
    }
}

void MainWindow::refPosPatternParametersSend(uint8_t *parametersToSend)
{
    uint8_t *ptr;

    // wpisanie do buffora wartości referencyjnych
    int parameterNo = 5;
    float tmp;

    switch(ui->numberRefValuePos->value())
    {
    case 1:
        // wpisanie do buffora wartości referencyjnych
        parameterNo = 10;
        tmp = (float)ui->valuePosRef->value();
        ptr = (uint8_t *) &tmp;
        for(uint8_t i = 0;i < sizeof(float);i++) parametersToSend[3+parameterNo*sizeof(float)+i] = ptr[i];

        parameterNo = 11;
        tmp = 0;
        for(uint8_t i = 0;i < sizeof(float);i++) parametersToSend[3+parameterNo*sizeof(float)+i] = ptr[i];

        parameterNo = 12;
        tmp = 0;
        for(uint8_t i = 0;i < sizeof(float);i++) parametersToSend[3+parameterNo*sizeof(float)+i] = ptr[i];

        parameterNo = 13;
        tmp = 0;
        for(uint8_t i = 0;i < sizeof(float);i++) parametersToSend[3+parameterNo*sizeof(float)+i] = ptr[i];

        parameterNo = 14;
        tmp = 0;
        for(uint8_t i = 0;i < sizeof(float);i++) parametersToSend[3+parameterNo*sizeof(float)+i] = ptr[i];

        //wpisanie do buffora odpowiednich czasów
        parameterNo = 15;
        tmp = (float)ui->refPosTime->value();
        for(uint8_t i = 0;i < sizeof(float);i++) parametersToSend[3+parameterNo*sizeof(float)+i] = ptr[i];

        parameterNo = 16;
        tmp = (float)ui->valuePosTs->value();
        for(uint8_t i = 0;i < sizeof(float);i++) parametersToSend[3+parameterNo*sizeof(float)+i] = ptr[i];

        parameterNo = 17;
        tmp = (float)ui->valuePosTs->value();
        for(uint8_t i = 0;i < sizeof(float);i++) parametersToSend[3+parameterNo*sizeof(float)+i] = ptr[i];

        parameterNo = 18;
        tmp = (float)ui->valuePosTs->value();
        for(uint8_t i = 0;i < sizeof(float);i++) parametersToSend[3+parameterNo*sizeof(float)+i] = ptr[i];

        parameterNo = 19;
        tmp = (float)ui->valuePosTs->value();
        for(uint8_t i = 0;i < sizeof(float);i++) parametersToSend[3+parameterNo*sizeof(float)+i] = ptr[i];
        break;
    case 2:
        // wpisanie do buffora wartości referencyjnych
        parameterNo = 10;
        tmp = (float)ui->valuePosRef->value();
        ptr = (uint8_t *) &tmp;
        for(uint8_t i = 0;i < sizeof(float);i++) parametersToSend[3+parameterNo*sizeof(float)+i] = ptr[i];

        parameterNo = 11;
        tmp = (float)ui->valuePosRef2->value();
        for(uint8_t i = 0;i < sizeof(float);i++) parametersToSend[3+parameterNo*sizeof(float)+i] = ptr[i];

        parameterNo = 12;
        tmp = 0;
        for(uint8_t i = 0;i < sizeof(float);i++) parametersToSend[3+parameterNo*sizeof(float)+i] = ptr[i];

        parameterNo = 13;
        tmp = 0;
        for(uint8_t i = 0;i < sizeof(float);i++) parametersToSend[3+parameterNo*sizeof(float)+i] = ptr[i];

        parameterNo = 14;
        tmp = 0;
        for(uint8_t i = 0;i < sizeof(float);i++) parametersToSend[3+parameterNo*sizeof(float)+i] = ptr[i];

        //wpisanie do buffora odpowiednich czasów
        parameterNo = 15;
        tmp = (float)ui->refPosTime->value();
        for(uint8_t i = 0;i < sizeof(float);i++) parametersToSend[3+parameterNo*sizeof(float)+i] = ptr[i];

        parameterNo = 16;
        tmp = (float)ui->refPosTime2->value();
        for(uint8_t i = 0;i < sizeof(float);i++) parametersToSend[3+parameterNo*sizeof(float)+i] = ptr[i];

        parameterNo = 17;
        tmp = (float)ui->valuePosTs->value();
        for(uint8_t i = 0;i < sizeof(float);i++) parametersToSend[3+parameterNo*sizeof(float)+i] = ptr[i];

        parameterNo = 18;
        tmp = (float)ui->valuePosTs->value();
        for(uint8_t i = 0;i < sizeof(float);i++) parametersToSend[3+parameterNo*sizeof(float)+i] = ptr[i];

        parameterNo = 19;
        tmp = (float)ui->valuePosTs->value();
        for(uint8_t i = 0;i < sizeof(float);i++) parametersToSend[3+parameterNo*sizeof(float)+i] = ptr[i];
        break;
    case 3:
        // wpisanie do buffora wartości referencyjnych
        parameterNo = 10;
        tmp = (float)ui->valuePosRef->value();
        ptr = (uint8_t *) &tmp;
        for(uint8_t i = 0;i < sizeof(float);i++) parametersToSend[3+parameterNo*sizeof(float)+i] = ptr[i];

        parameterNo = 11;
        tmp = (float)ui->valuePosRef2->value();
        for(uint8_t i = 0;i < sizeof(float);i++) parametersToSend[3+parameterNo*sizeof(float)+i] = ptr[i];

        parameterNo = 12;
        tmp = (float)ui->valuePosRef3->value();
        for(uint8_t i = 0;i < sizeof(float);i++) parametersToSend[3+parameterNo*sizeof(float)+i] = ptr[i];

        parameterNo = 13;
        tmp = 0;
        for(uint8_t i = 0;i < sizeof(float);i++) parametersToSend[3+parameterNo*sizeof(float)+i] = ptr[i];

        parameterNo = 14;
        tmp = 0;
        for(uint8_t i = 0;i < sizeof(float);i++) parametersToSend[3+parameterNo*sizeof(float)+i] = ptr[i];

        //wpisanie do buffora odpowiednich czasów
        parameterNo = 15;
        tmp = (float)ui->refPosTime->value();
        for(uint8_t i = 0;i < sizeof(float);i++) parametersToSend[3+parameterNo*sizeof(float)+i] = ptr[i];

        parameterNo = 16;
        tmp = (float)ui->refPosTime2->value();
        for(uint8_t i = 0;i < sizeof(float);i++) parametersToSend[3+parameterNo*sizeof(float)+i] = ptr[i];

        parameterNo = 17;
        tmp = (float)ui->refPosTime3->value();
        for(uint8_t i = 0;i < sizeof(float);i++) parametersToSend[3+parameterNo*sizeof(float)+i] = ptr[i];

        parameterNo = 18;
        tmp = (float)ui->valuePosTs->value();
        for(uint8_t i = 0;i < sizeof(float);i++) parametersToSend[3+parameterNo*sizeof(float)+i] = ptr[i];

        parameterNo = 19;
        tmp = (float)ui->valuePosTs->value();
        for(uint8_t i = 0;i < sizeof(float);i++) parametersToSend[3+parameterNo*sizeof(float)+i] = ptr[i];
        break;
    case 4:
        // wpisanie do buffora wartości referencyjnych
        parameterNo = 10;
        tmp = (float)ui->valuePosRef->value();
        ptr = (uint8_t *) &tmp;
        for(uint8_t i = 0;i < sizeof(float);i++) parametersToSend[3+parameterNo*sizeof(float)+i] = ptr[i];

        parameterNo = 11;
        tmp = (float)ui->valuePosRef2->value();
        for(uint8_t i = 0;i < sizeof(float);i++) parametersToSend[3+parameterNo*sizeof(float)+i] = ptr[i];

        parameterNo = 12;
        tmp = (float)ui->valuePosRef3->value();
        for(uint8_t i = 0;i < sizeof(float);i++) parametersToSend[3+parameterNo*sizeof(float)+i] = ptr[i];

        parameterNo = 13;
        tmp = (float)ui->valuePosRef4->value();
        for(uint8_t i = 0;i < sizeof(float);i++) parametersToSend[3+parameterNo*sizeof(float)+i] = ptr[i];

        parameterNo = 14;
        tmp = 0;
        for(uint8_t i = 0;i < sizeof(float);i++) parametersToSend[3+parameterNo*sizeof(float)+i] = ptr[i];

        //wpisanie do buffora odpowiednich czasów
        parameterNo = 15;
        tmp = (float)ui->refPosTime->value();
        for(uint8_t i = 0;i < sizeof(float);i++) parametersToSend[3+parameterNo*sizeof(float)+i] = ptr[i];

        parameterNo = 16;
        tmp = (float)ui->refPosTime2->value();
        for(uint8_t i = 0;i < sizeof(float);i++) parametersToSend[3+parameterNo*sizeof(float)+i] = ptr[i];

        parameterNo = 17;
        tmp = (float)ui->refPosTime3->value();
        for(uint8_t i = 0;i < sizeof(float);i++) parametersToSend[3+parameterNo*sizeof(float)+i] = ptr[i];

        parameterNo = 18;
        tmp = (float)ui->refPosTime4->value();
        for(uint8_t i = 0;i < sizeof(float);i++) parametersToSend[3+parameterNo*sizeof(float)+i] = ptr[i];

        parameterNo = 19;
        tmp = (float)ui->valuePosTs->value();
        for(uint8_t i = 0;i < sizeof(float);i++) parametersToSend[3+parameterNo*sizeof(float)+i] = ptr[i];
        break;
    case 5:
        // wpisanie do buffora wartości referencyjnych
        parameterNo = 10;
        tmp = (float)ui->valuePosRef->value();
        ptr = (uint8_t *) &tmp;
        for(uint8_t i = 0;i < sizeof(float);i++) parametersToSend[3+parameterNo*sizeof(float)+i] = ptr[i];

        parameterNo = 11;
        tmp = (float)ui->valuePosRef2->value();
        for(uint8_t i = 0;i < sizeof(float);i++) parametersToSend[3+parameterNo*sizeof(float)+i] = ptr[i];

        parameterNo = 12;
        tmp = (float)ui->valuePosRef3->value();
        for(uint8_t i = 0;i < sizeof(float);i++) parametersToSend[3+parameterNo*sizeof(float)+i] = ptr[i];

        parameterNo = 13;
        tmp = (float)ui->valuePosRef4->value();
        for(uint8_t i = 0;i < sizeof(float);i++) parametersToSend[3+parameterNo*sizeof(float)+i] = ptr[i];

        parameterNo = 14;
        tmp = (float)ui->valuePosRef5->value();
        for(uint8_t i = 0;i < sizeof(float);i++) parametersToSend[3+parameterNo*sizeof(float)+i] = ptr[i];

        //wpisanie do buffora odpowiednich czasów
        parameterNo = 15;
        tmp = (float)ui->refPosTime->value();
        for(uint8_t i = 0;i < sizeof(float);i++) parametersToSend[3+parameterNo*sizeof(float)+i] = ptr[i];

        parameterNo = 16;
        tmp = (float)ui->refPosTime2->value();
        for(uint8_t i = 0;i < sizeof(float);i++) parametersToSend[3+parameterNo*sizeof(float)+i] = ptr[i];

        parameterNo = 17;
        tmp = (float)ui->refPosTime3->value();
        for(uint8_t i = 0;i < sizeof(float);i++) parametersToSend[3+parameterNo*sizeof(float)+i] = ptr[i];

        parameterNo = 18;
        tmp = (float)ui->refPosTime4->value();
        for(uint8_t i = 0;i < sizeof(float);i++) parametersToSend[3+parameterNo*sizeof(float)+i] = ptr[i];

        parameterNo = 19;
        tmp = (float)ui->refPosTime5->value();
        for(uint8_t i = 0;i < sizeof(float);i++) parametersToSend[3+parameterNo*sizeof(float)+i] = ptr[i];
        break;
    default:
        break;
    }
}

void MainWindow::loadMotorPosPatternParametersSend(uint8_t *parametersToSend)
{
    uint8_t *ptr;
    int parameterNo = 0;
    float tmp;

    // wpisanie do buffora wartości obciążenia
    if(ui->SwitchLoadMotorPos->isChecked())
    {
        switch(ui->numberLoadValuePos->value())
        {
        case 1:

            parameterNo = 0;
            tmp = (float)ui->motorLoadPos->value();
            tmp = 1.379*log(ui->motorLoadPos->value())+44.99;
            if(tmp>43 || tmp<38)
                tmp = 0;
            ptr = (uint8_t *) &tmp;
            for(uint8_t i = 0;i < sizeof(float);i++) parametersToSend[3+parameterNo*sizeof(float)+i] = ptr[i];

            parameterNo = 1;
            tmp = 0;
            for(uint8_t i = 0;i < sizeof(float);i++) parametersToSend[3+parameterNo*sizeof(float)+i] = ptr[i];

            parameterNo = 2;
            tmp = 0;
            for(uint8_t i = 0;i < sizeof(float);i++) parametersToSend[3+parameterNo*sizeof(float)+i] = ptr[i];

            parameterNo = 3;
            tmp = 0;
            for(uint8_t i = 0;i < sizeof(float);i++) parametersToSend[3+parameterNo*sizeof(float)+i] = ptr[i];

            parameterNo = 4;
            tmp = 0;
            for(uint8_t i = 0;i < sizeof(float);i++) parametersToSend[3+parameterNo*sizeof(float)+i] = ptr[i];


            //wpisanie do buffora odpowiednich czasów
            parameterNo = 5;
            tmp = (float)ui->motorLoadPosTime->value();
            ptr = (uint8_t *) &tmp;
            for(uint8_t i = 0;i < sizeof(float);i++) parametersToSend[3+parameterNo*sizeof(float)+i] = ptr[i];

            parameterNo = 6;
            tmp = (float)ui->valuePosTs->value();
            for(uint8_t i = 0;i < sizeof(float);i++) parametersToSend[3+parameterNo*sizeof(float)+i] = ptr[i];

            parameterNo = 7;
            tmp = (float)ui->valuePosTs->value();
            for(uint8_t i = 0;i < sizeof(float);i++) parametersToSend[3+parameterNo*sizeof(float)+i] = ptr[i];

            parameterNo = 8;
            tmp = (float)ui->valuePosTs->value();
            for(uint8_t i = 0;i < sizeof(float);i++) parametersToSend[3+parameterNo*sizeof(float)+i] = ptr[i];

            parameterNo = 9;
            tmp = (float)ui->valuePosTs->value();
            for(uint8_t i = 0;i < sizeof(float);i++) parametersToSend[3+parameterNo*sizeof(float)+i] = ptr[i];
            break;
        case 2:

            parameterNo = 0;
            tmp = (float)ui->motorLoadPos->value();
            tmp = 1.379*log(ui->motorLoadPos->value())+44.99;
            if(tmp>43 || tmp<38)
                tmp = 0;
            ptr = (uint8_t *) &tmp;
            for(uint8_t i = 0;i < sizeof(float);i++) parametersToSend[3+parameterNo*sizeof(float)+i] = ptr[i];

            parameterNo = 1;
            tmp = (float)ui->motorLoadPos2->value();
            tmp = 1.379*log(ui->motorLoadPos2->value())+44.99;
            if(tmp>43 || tmp<38)
                tmp = 0;
            for(uint8_t i = 0;i < sizeof(float);i++) parametersToSend[3+parameterNo*sizeof(float)+i] = ptr[i];

            parameterNo = 2;
            tmp = 0;
            for(uint8_t i = 0;i < sizeof(float);i++) parametersToSend[3+parameterNo*sizeof(float)+i] = ptr[i];

            parameterNo = 3;
            tmp = 0;
            for(uint8_t i = 0;i < sizeof(float);i++) parametersToSend[3+parameterNo*sizeof(float)+i] = ptr[i];

            parameterNo = 4;
            tmp = 0;
            for(uint8_t i = 0;i < sizeof(float);i++) parametersToSend[3+parameterNo*sizeof(float)+i] = ptr[i];

            //wpisanie do buffora odpowiednich czasów
            parameterNo = 5;
            tmp = (float)ui->motorLoadPosTime->value();
            ptr = (uint8_t *) &tmp;
            for(uint8_t i = 0;i < sizeof(float);i++) parametersToSend[3+parameterNo*sizeof(float)+i] = ptr[i];

            parameterNo = 6;
            tmp = (float)ui->motorLoadPosTime2->value();
            for(uint8_t i = 0;i < sizeof(float);i++) parametersToSend[3+parameterNo*sizeof(float)+i] = ptr[i];

            parameterNo = 7;
            tmp = (float)ui->valuePosTs->value();
            for(uint8_t i = 0;i < sizeof(float);i++) parametersToSend[3+parameterNo*sizeof(float)+i] = ptr[i];

            parameterNo = 8;
            tmp = (float)ui->valuePosTs->value();
            for(uint8_t i = 0;i < sizeof(float);i++) parametersToSend[3+parameterNo*sizeof(float)+i] = ptr[i];

            parameterNo = 9;
            tmp = (float)ui->valuePosTs->value();
            for(uint8_t i = 0;i < sizeof(float);i++) parametersToSend[3+parameterNo*sizeof(float)+i] = ptr[i];

            break;
        case 3:

            parameterNo = 0;
            tmp = (float)ui->motorLoadPos->value();
            tmp = 1.379*log(ui->motorLoadPos->value())+44.99;
            if(tmp>43 || tmp<38)
                tmp = 0;
            ptr = (uint8_t *) &tmp;
            for(uint8_t i = 0;i < sizeof(float);i++) parametersToSend[3+parameterNo*sizeof(float)+i] = ptr[i];

            parameterNo = 1;
            tmp = (float)ui->motorLoadPos2->value();
            tmp = 1.379*log(ui->motorLoadPos2->value())+44.99;
            if(tmp>43 || tmp<38)
                tmp = 0;
            for(uint8_t i = 0;i < sizeof(float);i++) parametersToSend[3+parameterNo*sizeof(float)+i] = ptr[i];

            parameterNo = 2;
            tmp = (float)ui->motorLoadPos3->value();
            tmp = 1.379*log(ui->motorLoadPos3->value())+44.99;
            if(tmp>43 || tmp<38)
                tmp = 0;
            for(uint8_t i = 0;i < sizeof(float);i++) parametersToSend[3+parameterNo*sizeof(float)+i] = ptr[i];

            parameterNo = 3;
            tmp = 0;
            for(uint8_t i = 0;i < sizeof(float);i++) parametersToSend[3+parameterNo*sizeof(float)+i] = ptr[i];

            parameterNo = 4;
            tmp = 0;
            for(uint8_t i = 0;i < sizeof(float);i++) parametersToSend[3+parameterNo*sizeof(float)+i] = ptr[i];

            //wpisanie do buffora odpowiednich czasów
            parameterNo = 5;
            tmp = (float)ui->motorLoadPosTime->value();
            ptr = (uint8_t *) &tmp;
            for(uint8_t i = 0;i < sizeof(float);i++) parametersToSend[3+parameterNo*sizeof(float)+i] = ptr[i];

            parameterNo = 6;
            tmp = (float)ui->motorLoadPosTime2->value();
            for(uint8_t i = 0;i < sizeof(float);i++) parametersToSend[3+parameterNo*sizeof(float)+i] = ptr[i];

            parameterNo = 7;
            tmp = (float)ui->motorLoadPosTime3->value();
            for(uint8_t i = 0;i < sizeof(float);i++) parametersToSend[3+parameterNo*sizeof(float)+i] = ptr[i];

            parameterNo = 8;
            tmp = (float)ui->valuePosTs->value();
            for(uint8_t i = 0;i < sizeof(float);i++) parametersToSend[3+parameterNo*sizeof(float)+i] = ptr[i];

            parameterNo = 9;
            tmp = (float)ui->valuePosTs->value();
            for(uint8_t i = 0;i < sizeof(float);i++) parametersToSend[3+parameterNo*sizeof(float)+i] = ptr[i];

            break;
        case 4:

            parameterNo = 0;
            tmp = (float)ui->motorLoadPos->value();
            tmp = 1.379*log(ui->motorLoadPos->value())+44.99;
            if(tmp>43 || tmp<38)
                tmp = 0;
            ptr = (uint8_t *) &tmp;
            for(uint8_t i = 0;i < sizeof(float);i++) parametersToSend[3+parameterNo*sizeof(float)+i] = ptr[i];

            parameterNo = 1;
            tmp = (float)ui->motorLoadPos2->value();
            tmp = 1.379*log(ui->motorLoadPos2->value())+44.99;
            if(tmp>43 || tmp<38)
                tmp = 0;
            for(uint8_t i = 0;i < sizeof(float);i++) parametersToSend[3+parameterNo*sizeof(float)+i] = ptr[i];

            parameterNo = 2;
            tmp = (float)ui->motorLoadPos3->value();
            tmp = 1.379*log(ui->motorLoadPos3->value())+44.99;
            if(tmp>43 || tmp<38)
                tmp = 0;
            for(uint8_t i = 0;i < sizeof(float);i++) parametersToSend[3+parameterNo*sizeof(float)+i] = ptr[i];

            parameterNo = 3;
            tmp = (float)ui->motorLoadPos4->value();
            tmp = 1.379*log(ui->motorLoadPos4->value())+44.99;
            if(tmp>43 || tmp<38)
                tmp = 0;
            for(uint8_t i = 0;i < sizeof(float);i++) parametersToSend[3+parameterNo*sizeof(float)+i] = ptr[i];

            parameterNo = 4;
            tmp = 0;
            for(uint8_t i = 0;i < sizeof(float);i++) parametersToSend[3+parameterNo*sizeof(float)+i] = ptr[i];

            //wpisanie do buffora odpowiednich czasów
            parameterNo = 5;
            tmp = (float)ui->motorLoadPosTime->value();
            ptr = (uint8_t *) &tmp;
            for(uint8_t i = 0;i < sizeof(float);i++) parametersToSend[3+parameterNo*sizeof(float)+i] = ptr[i];

            parameterNo = 6;
            tmp = (float)ui->motorLoadPosTime2->value();
            for(uint8_t i = 0;i < sizeof(float);i++) parametersToSend[3+parameterNo*sizeof(float)+i] = ptr[i];

            parameterNo = 7;
            tmp = (float)ui->motorLoadPosTime3->value();
            for(uint8_t i = 0;i < sizeof(float);i++) parametersToSend[3+parameterNo*sizeof(float)+i] = ptr[i];

            parameterNo = 8;
            tmp = (float)ui->motorLoadPosTime4->value();
            for(uint8_t i = 0;i < sizeof(float);i++) parametersToSend[3+parameterNo*sizeof(float)+i] = ptr[i];

            parameterNo = 9;
            tmp = (float)ui->valuePosTs->value();
            for(uint8_t i = 0;i < sizeof(float);i++) parametersToSend[3+parameterNo*sizeof(float)+i] = ptr[i];

            break;
        case 5:

            parameterNo = 0;
            tmp = (float)ui->motorLoadPos->value();
            tmp = 1.379*log(ui->motorLoadPos->value())+44.99;
            if(tmp>43 || tmp<38)
                tmp = 0;
            ptr = (uint8_t *) &tmp;
            for(uint8_t i = 0;i < sizeof(float);i++) parametersToSend[3+parameterNo*sizeof(float)+i] = ptr[i];

            parameterNo = 1;
            tmp = (float)ui->motorLoadPos2->value();
            tmp = 1.379*log(ui->motorLoadPos2->value())+44.99;
            if(tmp>43 || tmp<38)
                tmp = 0;
            for(uint8_t i = 0;i < sizeof(float);i++) parametersToSend[3+parameterNo*sizeof(float)+i] = ptr[i];

            parameterNo = 2;
            tmp = (float)ui->motorLoadPos3->value();
            tmp = 1.379*log(ui->motorLoadPos3->value())+44.99;
            if(tmp>43 || tmp<38)
                tmp = 0;
            for(uint8_t i = 0;i < sizeof(float);i++) parametersToSend[3+parameterNo*sizeof(float)+i] = ptr[i];

            parameterNo = 3;
            tmp = (float)ui->motorLoadPos4->value();
            tmp = 1.379*log(ui->motorLoadPos4->value())+44.99;
            if(tmp>43 || tmp<38)
                tmp = 0;
            for(uint8_t i = 0;i < sizeof(float);i++) parametersToSend[3+parameterNo*sizeof(float)+i] = ptr[i];

            parameterNo = 4;
            tmp = (float)ui->motorLoadPos5->value();
            tmp = 1.379*log(ui->motorLoadPos5->value())+44.99;
            if(tmp>43 || tmp<38)
                tmp = 0;
            for(uint8_t i = 0;i < sizeof(float);i++) parametersToSend[3+parameterNo*sizeof(float)+i] = ptr[i];

            //wpisanie do buffora odpowiednich czasów
            parameterNo = 5;
            tmp = (float)ui->motorLoadPosTime->value();
            ptr = (uint8_t *) &tmp;
            for(uint8_t i = 0;i < sizeof(float);i++) parametersToSend[3+parameterNo*sizeof(float)+i] = ptr[i];

            parameterNo = 6;
            tmp = (float)ui->motorLoadPosTime2->value();
            for(uint8_t i = 0;i < sizeof(float);i++) parametersToSend[3+parameterNo*sizeof(float)+i] = ptr[i];

            parameterNo = 7;
            tmp = (float)ui->motorLoadPosTime3->value();
            for(uint8_t i = 0;i < sizeof(float);i++) parametersToSend[3+parameterNo*sizeof(float)+i] = ptr[i];

            parameterNo = 8;
            tmp = (float)ui->motorLoadPosTime4->value();
            for(uint8_t i = 0;i < sizeof(float);i++) parametersToSend[3+parameterNo*sizeof(float)+i] = ptr[i];

            parameterNo = 9;
            tmp = (float)ui->motorLoadPosTime5->value();
            for(uint8_t i = 0;i < sizeof(float);i++) parametersToSend[3+parameterNo*sizeof(float)+i] = ptr[i];

            break;
        default:
            break;
        }
    }
    else
    {

        parameterNo = 0;
        tmp = 0;
        ptr = (uint8_t *) &tmp;
        for(uint8_t i = 0;i < sizeof(float);i++) parametersToSend[3+parameterNo*sizeof(float)+i] = ptr[i];

        parameterNo = 1;
        tmp = 0;
        for(uint8_t i = 0;i < sizeof(float);i++) parametersToSend[3+parameterNo*sizeof(float)+i] = ptr[i];

        parameterNo = 2;
        tmp = 0;
        for(uint8_t i = 0;i < sizeof(float);i++) parametersToSend[3+parameterNo*sizeof(float)+i] = ptr[i];

        parameterNo = 3;
        tmp = 0;
        for(uint8_t i = 0;i < sizeof(float);i++) parametersToSend[3+parameterNo*sizeof(float)+i] = ptr[i];

        parameterNo = 4;
        tmp = 0;
        for(uint8_t i = 0;i < sizeof(float);i++) parametersToSend[3+parameterNo*sizeof(float)+i] = ptr[i];

        //wpisanie do buffora odpowiednich czasów
        parameterNo = 5;
        tmp = 0;
        ptr = (uint8_t *) &tmp;
        for(uint8_t i = 0;i < sizeof(float);i++) parametersToSend[3+parameterNo*sizeof(float)+i] = ptr[i];

        parameterNo = 6;
        tmp = 0;
        for(uint8_t i = 0;i < sizeof(float);i++) parametersToSend[3+parameterNo*sizeof(float)+i] = ptr[i];

        parameterNo = 7;
        tmp = 0;
        for(uint8_t i = 0;i < sizeof(float);i++) parametersToSend[3+parameterNo*sizeof(float)+i] = ptr[i];

        parameterNo = 8;
        tmp = 0;
        for(uint8_t i = 0;i < sizeof(float);i++) parametersToSend[3+parameterNo*sizeof(float)+i] = ptr[i];

        parameterNo = 9;
        tmp = 0;
        for(uint8_t i = 0;i < sizeof(float);i++) parametersToSend[3+parameterNo*sizeof(float)+i] = ptr[i];

    }
}

void MainWindow::currentPIDParametersSend(uint8_t *parametersToSend)
{
    /* funkcja wpisująaca parametry regulatora PID prądu
     * do ramki danych do wysyłki
     * UWAGA: przy dodawaniu parametrów należy skorygować wartości parametrNo
     * także w innych funkcjach od wysyłki */
    uint8_t *ptr;

    int parameterNo = 21;
    float tmp = (float)ui->valueCurrKp->value();
    ptr = (uint8_t *) &tmp;
    for(uint8_t i = 0;i < sizeof(float);i++) parametersToSend[3+parameterNo*sizeof(float)+i] = ptr[i];

    parameterNo = 22;
    tmp = (float)ui->valueCurrTi->value();
    for(uint8_t i = 0;i < sizeof(float);i++) parametersToSend[3+parameterNo*sizeof(float)+i] = ptr[i];

    parameterNo = 23;
    tmp = (float)ui->valueCurrTd->value();
    for(uint8_t i = 0;i < sizeof(float);i++) parametersToSend[3+parameterNo*sizeof(float)+i] = ptr[i];

    parameterNo = 24;
    tmp = (float)ui->valueCurrKw->value();
    for(uint8_t i = 0;i < sizeof(float);i++) parametersToSend[3+parameterNo*sizeof(float)+i] = ptr[i];
}

void MainWindow::speedPIDParametersSend(uint8_t *parametersToSend)
{
    /* funkcja wpisująaca parametry regulatora PID prędkości
     * do ramki danych do wysyłki
     * UWAGA: przy dodawaniu parametrów należy skorygować wartości parametrNo
     * także w innych funkcjach od wysyłki */

    uint8_t *ptr;

    int parameterNo = 25;
    float tmp = (float)ui->valueSpeedKp->value();
    ptr = (uint8_t *) &tmp;
    for(uint8_t i = 0;i < sizeof(float);i++) parametersToSend[3+parameterNo*sizeof(float)+i] = ptr[i];

    parameterNo = 26;
    tmp = (float)ui->valueSpeedTi->value();
    for(uint8_t i = 0;i < sizeof(float);i++) parametersToSend[3+parameterNo*sizeof(float)+i] = ptr[i];

    parameterNo = 27;
    tmp = (float)ui->valueSpeedTd->value();
    for(uint8_t i = 0;i < sizeof(float);i++) parametersToSend[3+parameterNo*sizeof(float)+i] = ptr[i];

    parameterNo = 28;
    tmp = (float)ui->valueSpeedKw->value();
    for(uint8_t i = 0;i < sizeof(float);i++) parametersToSend[3+parameterNo*sizeof(float)+i] = ptr[i];
}

void MainWindow::posPIDParametersSend(uint8_t *parametersToSend)
{
    /* funkcja wpisująaca parametry regulatora PID położenia
     * do ramki danych do wysyłki
     * UWAGA: przy dodawaniu parametrów należy skorygować wartości parametrNo
     * także w innych funkcjach od wysyłki */

    uint8_t *ptr;

    int parameterNo = 29;
    float tmp = (float)ui->valuePosKp->value();
    ptr = (uint8_t *) &tmp;
    for(uint8_t i = 0;i < sizeof(float);i++) parametersToSend[3+parameterNo*sizeof(float)+i] = ptr[i];

    parameterNo = 30;
    tmp = (float)ui->valuePosTi->value();
    for(uint8_t i = 0;i < sizeof(float);i++) parametersToSend[3+parameterNo*sizeof(float)+i] = ptr[i];

    parameterNo = 31;
    tmp = (float)ui->valuePosTd->value();
    for(uint8_t i = 0;i < sizeof(float);i++) parametersToSend[3+parameterNo*sizeof(float)+i] = ptr[i];

    parameterNo = 32;
    tmp = (float)ui->valuePosKff->value();
    for(uint8_t i = 0;i < sizeof(float);i++) parametersToSend[3+parameterNo*sizeof(float)+i] = ptr[i];

    parameterNo = 33;
    tmp = (float)ui->valuePosKff2->value();
    for(uint8_t i = 0;i < sizeof(float);i++) parametersToSend[3+parameterNo*sizeof(float)+i] = ptr[i];

    parameterNo = 34;
    tmp = (float)ui->valuePosKw->value();
    for(uint8_t i = 0;i < sizeof(float);i++) parametersToSend[3+parameterNo*sizeof(float)+i] = ptr[i];
}

void MainWindow::blockSimTime()
{
    if(ui->trybSter->currentIndex() == 0)
    {
        ui->valueCurrTs->setValue(400);
        ui->valueSpeedTs->setValue(400);
        ui->valuePosTs->setValue(400);

        ui->valueCurrTs->setDisabled(true);
        ui->valueSpeedTs->setDisabled(true);
        ui->valuePosTs->setDisabled(true);
    }
    else
    {
        ui->valueCurrTs->setDisabled(false);
        ui->valueSpeedTs->setDisabled(false);
        ui->valuePosTs->setDisabled(false);
    }
}

void MainWindow::blockUnusedPatternRefValueCurr(int arg1)
{
    ui->valueCurrRef->setDisabled(false);
    ui->valueCurrRef2->setDisabled(false);
    ui->valueCurrRef3->setDisabled(false);
    ui->valueCurrRef4->setDisabled(false);
    ui->valueCurrRef5->setDisabled(false);

    ui->refCurrTime->setDisabled(false);
    ui->refCurrTime2->setDisabled(false);
    ui->refCurrTime3->setDisabled(false);
    ui->refCurrTime4->setDisabled(false);
    ui->refCurrTime5->setDisabled(false);
    for(int i=5;i>arg1;i--)
    {
        switch(i)
        {
        case 1:
            ui->valueCurrRef->setDisabled(true);
            ui->refCurrTime->setDisabled(true);
            break;
        case 2:
            ui->valueCurrRef2->setDisabled(true);
            ui->refCurrTime2->setDisabled(true);
            break;
        case 3:
            ui->valueCurrRef3->setDisabled(true);
            ui->refCurrTime3->setDisabled(true);
            break;
        case 4:
            ui->valueCurrRef4->setDisabled(true);
            ui->refCurrTime4->setDisabled(true);
            break;
        case 5:
            ui->valueCurrRef5->setDisabled(true);
            ui->refCurrTime5->setDisabled(true);
            break;
        default:
            break;
        }
    }
}

void MainWindow::blockUnusedPatternLoadMotorCurr(int arg1)
{
    ui->motorLoadCurr->setDisabled(false);
    ui->motorLoadCurr2->setDisabled(false);
    ui->motorLoadCurr3->setDisabled(false);
    ui->motorLoadCurr4->setDisabled(false);
    ui->motorLoadCurr5->setDisabled(false);

    ui->motorLoadCurrTime->setDisabled(false);
    ui->motorLoadCurrTime2->setDisabled(false);
    ui->motorLoadCurrTime3->setDisabled(false);
    ui->motorLoadCurrTime4->setDisabled(false);
    ui->motorLoadCurrTime5->setDisabled(false);
    for(int i=5;i>arg1;i--)
    {
        switch(i)
        {
        case 1:
            ui->motorLoadCurr->setDisabled(true);
            ui->motorLoadCurrTime->setDisabled(true);
            break;
        case 2:
            ui->motorLoadCurr2->setDisabled(true);
            ui->motorLoadCurrTime2->setDisabled(true);
            break;
        case 3:
            ui->motorLoadCurr3->setDisabled(true);
            ui->motorLoadCurrTime3->setDisabled(true);
            break;
        case 4:
            ui->motorLoadCurr4->setDisabled(true);
            ui->motorLoadCurrTime4->setDisabled(true);
            break;
        case 5:
            ui->motorLoadCurr5->setDisabled(true);
            ui->motorLoadCurrTime5->setDisabled(true);
            break;
        default:
            break;
        }
    }
}

void MainWindow::blockUnusedPatternRefValueSpeed(int arg1)
{
    ui->valueSpeedRef->setDisabled(false);
    ui->valueSpeedRef2->setDisabled(false);
    ui->valueSpeedRef3->setDisabled(false);
    ui->valueSpeedRef4->setDisabled(false);
    ui->valueSpeedRef5->setDisabled(false);

    ui->refSpeedTime->setDisabled(false);
    ui->refSpeedTime2->setDisabled(false);
    ui->refSpeedTime3->setDisabled(false);
    ui->refSpeedTime4->setDisabled(false);
    ui->refSpeedTime5->setDisabled(false);
    for(int i=5;i>arg1;i--)
    {
        switch(i)
        {
        case 1:
            ui->valueSpeedRef->setDisabled(true);
            ui->refSpeedTime->setDisabled(true);
            break;
        case 2:
            ui->valueSpeedRef2->setDisabled(true);
            ui->refSpeedTime2->setDisabled(true);
            break;
        case 3:
            ui->valueSpeedRef3->setDisabled(true);
            ui->refSpeedTime3->setDisabled(true);
            break;
        case 4:
            ui->valueSpeedRef4->setDisabled(true);
            ui->refSpeedTime4->setDisabled(true);
            break;
        case 5:
            ui->valueSpeedRef5->setDisabled(true);
            ui->refSpeedTime5->setDisabled(true);
            break;
        default:
            break;
        }
    }
}

void MainWindow::blockUnusedPatternLoadMotorSpeed(int arg1)
{
    ui->motorLoadSpeed->setDisabled(false);
    ui->motorLoadSpeed2->setDisabled(false);
    ui->motorLoadSpeed3->setDisabled(false);
    ui->motorLoadSpeed4->setDisabled(false);
    ui->motorLoadSpeed5->setDisabled(false);

    ui->motorLoadSpeedTime->setDisabled(false);
    ui->motorLoadSpeedTime2->setDisabled(false);
    ui->motorLoadSpeedTime3->setDisabled(false);
    ui->motorLoadSpeedTime4->setDisabled(false);
    ui->motorLoadSpeedTime5->setDisabled(false);
    for(int i=5;i>arg1;i--)
    {
        switch(i)
        {
        case 1:
            ui->motorLoadSpeed->setDisabled(true);
            ui->motorLoadSpeedTime->setDisabled(true);
            break;
        case 2:
            ui->motorLoadSpeed2->setDisabled(true);
            ui->motorLoadSpeedTime2->setDisabled(true);
            break;
        case 3:
            ui->motorLoadSpeed3->setDisabled(true);
            ui->motorLoadSpeedTime3->setDisabled(true);
            break;
        case 4:
            ui->motorLoadSpeed4->setDisabled(true);
            ui->motorLoadSpeedTime4->setDisabled(true);
            break;
        case 5:
            ui->motorLoadSpeed5->setDisabled(true);
            ui->motorLoadSpeedTime5->setDisabled(true);
            break;
        default:
            break;
        }
    }
}

void MainWindow::blockUnusedPatternRefValuePos(int arg1)
{
    ui->valuePosRef->setDisabled(false);
    ui->valuePosRef2->setDisabled(false);
    ui->valuePosRef3->setDisabled(false);
    ui->valuePosRef4->setDisabled(false);
    ui->valuePosRef5->setDisabled(false);

    ui->refPosTime->setDisabled(false);
    ui->refPosTime2->setDisabled(false);
    ui->refPosTime3->setDisabled(false);
    ui->refPosTime4->setDisabled(false);
    ui->refPosTime5->setDisabled(false);
    for(int i=5;i>arg1;i--)
    {
        switch(i)
        {
        case 1:
            ui->valuePosRef->setDisabled(true);
            ui->refPosTime->setDisabled(true);
            break;
        case 2:
            ui->valuePosRef2->setDisabled(true);
            ui->refPosTime2->setDisabled(true);
            break;
        case 3:
            ui->valuePosRef3->setDisabled(true);
            ui->refPosTime3->setDisabled(true);
            break;
        case 4:
            ui->valuePosRef4->setDisabled(true);
            ui->refPosTime4->setDisabled(true);
            break;
        case 5:
            ui->valuePosRef5->setDisabled(true);
            ui->refPosTime5->setDisabled(true);
            break;
        default:
            break;
        }
    }
}

void MainWindow::blockUnusedPatternLoadMotorPos(int arg1)
{
    ui->motorLoadPos->setDisabled(false);
    ui->motorLoadPos2->setDisabled(false);
    ui->motorLoadPos3->setDisabled(false);
    ui->motorLoadPos4->setDisabled(false);
    ui->motorLoadPos5->setDisabled(false);

    ui->motorLoadPosTime->setDisabled(false);
    ui->motorLoadPosTime2->setDisabled(false);
    ui->motorLoadPosTime3->setDisabled(false);
    ui->motorLoadPosTime4->setDisabled(false);
    ui->motorLoadPosTime5->setDisabled(false);
    for(int i=5;i>arg1;i--)
    {
        switch(i)
        {
        case 1:
            ui->motorLoadPos->setDisabled(true);
            ui->motorLoadPosTime->setDisabled(true);
            break;
        case 2:
            ui->motorLoadPos2->setDisabled(true);
            ui->motorLoadPosTime2->setDisabled(true);
            break;
        case 3:
            ui->motorLoadPos3->setDisabled(true);
            ui->motorLoadPosTime3->setDisabled(true);
            break;
        case 4:
            ui->motorLoadPos4->setDisabled(true);
            ui->motorLoadPosTime4->setDisabled(true);
            break;
        case 5:
            ui->motorLoadPos5->setDisabled(true);
            ui->motorLoadPosTime5->setDisabled(true);
            break;
        default:
            break;
        }
    }
}

void MainWindow::onOffLoadMotorCurr()
{
    if(ui->SwitchLoadMotor->isChecked())
    {
        ui->groupBox_21->setDisabled(false);
        ui->groupBox_18->setDisabled(false);
        ui->groupBox_15->setDisabled(false);

        lineChartCurr->loadAxis->setVisible();
        lineChartSpeed->loadAxis->setVisible();
        lineChartPos->loadAxis->setVisible();

        lineChartCurr->loadMotorSeries->setVisible();
        lineChartSpeed->loadMotorSeries->setVisible();
        lineChartPos->loadMotorSeries->setVisible();
    }
    else
    {
        ui->groupBox_21->setDisabled(true);
        ui->groupBox_18->setDisabled(true);
        ui->groupBox_15->setDisabled(true);

        lineChartCurr->loadAxis->setVisible(false);
        lineChartSpeed->loadAxis->setVisible(false);
        lineChartPos->loadAxis->setVisible(false);

        lineChartCurr->loadMotorSeries->setVisible(false);
        lineChartSpeed->loadMotorSeries->setVisible(false);
        lineChartPos->loadMotorSeries->setVisible(false);
    }
}

void MainWindow::onOffLoadMotorSpeed()
{
    if(ui->SwitchLoadMotorSpeed->isChecked())
    {
        ui->groupBox_21->setDisabled(false);
        ui->groupBox_18->setDisabled(false);
        ui->groupBox_15->setDisabled(false);

        lineChartCurr->loadAxis->setVisible();
        lineChartSpeed->loadAxis->setVisible();
        lineChartPos->loadAxis->setVisible();

        lineChartCurr->loadMotorSeries->setVisible();
        lineChartSpeed->loadMotorSeries->setVisible();
        lineChartPos->loadMotorSeries->setVisible();
    }
    else
    {
        ui->groupBox_21->setDisabled(true);
        ui->groupBox_18->setDisabled(true);
        ui->groupBox_15->setDisabled(true);

        lineChartCurr->loadAxis->setVisible(false);
        lineChartSpeed->loadAxis->setVisible(false);
        lineChartPos->loadAxis->setVisible(false);

        lineChartCurr->loadMotorSeries->setVisible(false);
        lineChartSpeed->loadMotorSeries->setVisible(false);
        lineChartPos->loadMotorSeries->setVisible(false);
    }
}

void MainWindow::onOffLoadMotorPos()
{
    if(ui->SwitchLoadMotorPos->isChecked())
    {
        ui->groupBox_21->setDisabled(false);
        ui->groupBox_18->setDisabled(false);
        ui->groupBox_15->setDisabled(false);

        lineChartCurr->loadAxis->setVisible();
        lineChartSpeed->loadAxis->setVisible();
        lineChartPos->loadAxis->setVisible();

        lineChartCurr->loadMotorSeries->setVisible();
        lineChartSpeed->loadMotorSeries->setVisible();
        lineChartPos->loadMotorSeries->setVisible();
    }
    else
    {
        ui->groupBox_21->setDisabled(true);
        ui->groupBox_18->setDisabled(true);
        ui->groupBox_15->setDisabled(true);

        lineChartCurr->loadAxis->setVisible(false);
        lineChartSpeed->loadAxis->setVisible(false);
        lineChartPos->loadAxis->setVisible(false);

        lineChartCurr->loadMotorSeries->setVisible(false);
        lineChartSpeed->loadMotorSeries->setVisible(false);
        lineChartPos->loadMotorSeries->setVisible(false);
    }
}

void MainWindow::writeData()
{
    //dane pomiarowe pradu
    QString mFileName = "C:/silnikApp/dane.txt";

    QFile mFile(mFileName);
    mFile.remove(mFileName);
    if(!mFile.open(QFile::ReadWrite | QFile::Text))
    {
        addLog("Nie mozna otworzyc pliku do zapisu!");
        return;
    }

    QTextStream out(&mFile);
    foreach(QPointF element,lineChartCurr->pointSeries)
    {
      out<<element.x()<<";";
      out<<element.y()<<"\n";
    }

    mFile.flush();
    mFile.close();
    //dane pomiarowe predkosci
    mFileName= "C:/silnikApp/dane_vel.txt";
    QFile mFile2(mFileName);
    mFile2.remove(mFileName);
    if(!mFile2.open(QFile::ReadWrite | QFile::Text))
    {
        addLog("Nie mozna otworzyc pliku do zapisu!");
        return;
    }

    QTextStream out2(&mFile2);
    foreach(QPointF element,lineChartSpeed->pointSeries)
    {
      out2<<element.x()<<";";
      out2<<element.y()<<"\n";
    }

    mFile2.flush();
    mFile2.close();

    //dane pomiarowe predkosci
    mFileName= "C:/silnikApp/dane_pos.txt";
    QFile mFile3(mFileName);
    mFile3.remove(mFileName);
    if(!mFile3.open(QFile::ReadWrite | QFile::Text))
    {
        addLog("Nie mozna otworzyc pliku do zapisu!");
        return;
    }

    QTextStream out3(&mFile3);
    foreach(QPointF element,lineChartPos->pointSeries)
    {
      out3<<element.x()<<";";
      out3<<element.y()<<"\n";
    }

    mFile3.flush();
    mFile3.close();

}

void MainWindow::on_przyciskStart_clicked()
{
    this->writeParameters();
}


void MainWindow::on_przyciskStop_clicked()
{
    this->writeParameters();
}


void MainWindow::on_przyciskAktu_clicked()
{
    chartScale(lineChartCurr,ui->minPrad->value(),ui->maxPrad->value(),ui->minCzas->value(),ui->maxCzas->value());
}


void MainWindow::on_przyciskAktuPos_clicked()
{
    chartScale(lineChartPos,ui->minPos->value(),ui->maxPos->value(),ui->minCzasPos->value(),ui->maxCzasPos->value());
}


void MainWindow::on_przycisAktuSpeed_clicked()
{
    chartScale(lineChartSpeed,ui->minSpeed->value(),ui->maxSpeed->value(),ui->minCzasSpeed->value(),ui->maxCzasSpeed->value());
}


void MainWindow::on_przyciskAutoPos_clicked()
{
    //funkcja obsługująca zdarzenie wciśnięcia przycisku "Autoskalowanie" dla pozycji
    if(dataRxCplt)
    {
        calcMinMax(lineChartPos);
    }
}


void MainWindow::on_przyciskAuto_clicked()
{
    //funkcja obsługująca zdarzenie wciśnięcia przycisku "Autoskalowanie" dla prądu
    if(dataRxCplt)
    {
        calcMinMax(lineChartCurr);
    }
}


void MainWindow::on_przyciskAutoSpeed_clicked()
{
    //funkcja obsługująca zdarzenie wciśnięcia przycisku "Autoskalowanie" dla prędkości
    if(dataRxCplt)
    {
        calcMinMax(lineChartSpeed);
    }
}


void MainWindow::on_przyciskStartSpeed_clicked()
{
    this->writeParameters();
}


void MainWindow::on_przyciskStartPos_clicked()
{
    this->writeParameters();
}


void MainWindow::on_SwitchLoadMotor_clicked()
{
    onOffLoadMotorCurr();
}


void MainWindow::on_SwitchLoadMotorSpeed_clicked()
{
    onOffLoadMotorSpeed();
}


void MainWindow::on_SwitchLoadMotorPos_clicked()
{
    onOffLoadMotorPos();
}


void MainWindow::on_trybSter_currentIndexChanged(int index)
{
    blockSimTime();
}


void MainWindow::on_numberRefValueCurr_valueChanged(int arg1)
{
    blockUnusedPatternRefValueCurr(arg1);
}


void MainWindow::on_numberLoadValueSpeed_valueChanged(int arg1)
{
    blockUnusedPatternLoadMotorSpeed(arg1);
}


void MainWindow::on_numberLoadValueCurr_valueChanged(int arg1)
{
    blockUnusedPatternLoadMotorCurr(arg1);
}


void MainWindow::on_numberRefValueSpeed_valueChanged(int arg1)
{
    blockUnusedPatternRefValueSpeed(arg1);
}


void MainWindow::on_numberRefValuePos_valueChanged(int arg1)
{
    blockUnusedPatternRefValuePos(arg1);
}


void MainWindow::on_numberLoadValuePos_valueChanged(int arg1)
{
    blockUnusedPatternLoadMotorPos(arg1);
}

void MainWindow::timer()
{
    bool state = false;
    devices = QSerialPortInfo::availablePorts();

    if(devices.length() != devicesPrev.length())
    {
        if(!devices.isEmpty())
        {
            for(int i = 0; i<=devices.length()-1;i++)
            {
                QString description = devices.at(i).description();
                if(description == "STMicroelectronics STLink Virtual COM Port")
                {
                    state = true;
                    port->setPortName(devices.at(i).portName());
                }
            }
            if(state)
            {
                addLog("Podłączono sterownik");

            }
            else
            {
                addLog("Odłączono sterownik");
            }
        }
        else
        {
            addLog("Odłączono sterownik");
        }
    }

    devicesPrev = devices;
}


void MainWindow::on_actiondane_pomiarowe_triggered()
{
    writeData();
}


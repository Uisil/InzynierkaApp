#ifndef COMTHREAD_H
#define COMTHREAD_H

#include <QThread>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QMutex>

class ComThread : public QThread
{
public:
    ComThread();
    void run();

    QSerialPort *port;
    QList<QSerialPortInfo> devices;
    QList<QSerialPortInfo> devicesPrev;
signals:
     void availablePortChanged(bool);

public slots:
};

#endif // COMTHREAD_H

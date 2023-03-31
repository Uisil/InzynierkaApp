#include "comthread.h"

ComThread::ComThread()
{

}

void ComThread::run()
{
    while(true)
    {
        bool include = false;
        QMutex mutex;
        mutex.lock();
        devices = QSerialPortInfo::availablePorts();
        mutex.unlock();

        if(devices.length() == devicesPrev.length())
        {
            break;
        }
        if(!devices.isEmpty())
        {
            for(int i = 0; i<=devices.length()-1;i++)
            {
                QString description = devices.at(i).description();
                if(description == "STMicroelectronics STLink Virtual COM Port")
                {
                    include = true;
                }
            }
        }
        devicesPrev = devices;
        //emit availablePortChanged(include);
    }
}

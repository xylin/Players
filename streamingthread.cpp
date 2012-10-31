
#include <QtGui\QtGui>

#include <math.h>

#include "streamingthread.h"


StreamingThread::StreamingThread(QObject *parent)
    : QThread(parent)
{
    restart = false;
    abort = false;

}

StreamingThread::~StreamingThread()
{
    mutex.lock();
    abort = true;
    condition.wakeOne();
    mutex.unlock();

    wait();
}

void StreamingThread::render(double centerX, double centerY, double scaleFactor,
                          QSize resultSize)
{
    QMutexLocker locker(&mutex);

    this->centerX = centerX;
    this->centerY = centerY;
    this->scaleFactor = scaleFactor;
    this->resultSize = resultSize;

    if (!isRunning()) {
        start(LowPriority);
    } else {
        restart = true;
        condition.wakeOne();
    }
}

void StreamingThread::run()
{
    forever {
        mutex.lock();
        QSize resultSize = this->resultSize;
        double scaleFactor = this->scaleFactor;
        double centerX = this->centerX;
        double centerY = this->centerY;
        mutex.unlock();
		
        mutex.lock();

        if (!restart)
            condition.wait(&mutex);
        restart = false;
        mutex.unlock();
    }
}


#include "streamingthread.moc" //last line
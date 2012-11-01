
#ifndef STREAMINGTHREAD_H
#define STREAMINGTHREAD_H

#include <QtCore/QMutex>
#include <QtCore/QSize>
#include <QtCore/QThread>
#include <QtCore/QWaitCondition>

QT_BEGIN_NAMESPACE
class QImage;
QT_END_NAMESPACE

class vrpn_Connection_IP;
class myTracker;


class StreamingThread : public QThread
{
    Q_OBJECT

public:
    StreamingThread(QObject *parent = 0);
    ~StreamingThread();

    void StreamOneFrame(double X, double Y, double Z);

protected:
    void run();

private:   

    QMutex mutex;
    QWaitCondition condition;
    
	double m_dX;
    double m_dY;
    double m_dZ;
    
    bool restart;
    bool abort;    


	vrpn_Connection_IP* m_Connection;
    myTracker* m_serverTracker;
};

#endif

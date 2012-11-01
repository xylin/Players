
#include <QtGui\QtGui>

#include <math.h>

#include <iostream>
#include <fstream>


#include "vrpn_Text.h"
#include "vrpn_Tracker.h"
#include "vrpn_Connection.h"

#include "vrpnSkeletonTracker.h"
#include "streamingthread.h"

using namespace std;

StreamingThread::StreamingThread(QObject *parent)
    : QThread(parent)
{
    restart = false;
    abort = false;

	// Creating the network server
	m_Connection = new vrpn_Connection_IP();

	// Creating the tracker
	m_serverTracker = new myTracker(m_Connection );

	cout << "Created VRPN server." << endl;

}

StreamingThread::~StreamingThread()
{
    mutex.lock();
    abort = true;
    condition.wakeOne();
    mutex.unlock();

    wait();
}

void StreamingThread::StreamOneFrame(double centerX, double centerY, double scaleFactor,
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
	static float angle = 0; 
	
	

    forever {
        mutex.lock();
        QSize resultSize = this->resultSize;
        double scaleFactor = this->scaleFactor;
        double centerX = this->centerX;
        double centerY = this->centerY;

	/*	vector<float> &rvfPos = m_serverTracker->m_vvfWholeSkeleton.at(0);

		angle += 0.001f;

		rvfPos.at(0) =  sinf( angle );
		rvfPos.at(1) = 0;
		rvfPos.at(2) = 0;
		*/
        mutex.unlock();
		
      
		//---------------------
		m_serverTracker->mainloop();

		m_Connection->mainloop();

		cout << "one vrpn loop." << endl;

		// Calling Sleep to let the CPU breathe.
		SleepEx(500,FALSE);
		//---------------------

		  
		
		
		mutex.lock();

        if (!restart)
            condition.wait(&mutex);
        restart = false;
        mutex.unlock();
    }
}


#include "streamingthread.moc" //last line
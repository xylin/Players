
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

void StreamingThread::StreamOneFrame(double X, double Y, double Z)
{
    QMutexLocker locker(&mutex);

    this->m_dX = X;
    this->m_dY = Y;
    this->m_dZ = Z;
 

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
                
		double dX = this->m_dX;
        double dY = this->m_dY;
        double dZ = this->m_dZ;

		vector<float> &rvfPos_HEAD = m_serverTracker->m_vvfWholeSkeleton.at(0);

		angle += 0.001f;

		rvfPos_HEAD.at(0) =  sinf( angle );
		rvfPos_HEAD.at(1) = 0;
		rvfPos_HEAD.at(2) = 0;

		vector<float> &rvfPos_NECK = m_serverTracker->m_vvfWholeSkeleton.at(1);
		
		rvfPos_NECK.at(0) = dX;
		rvfPos_NECK.at(1) = dY;
		rvfPos_NECK.at(2) = dZ;
		
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
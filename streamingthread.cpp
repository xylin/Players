
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

	m_vvdCoord.resize(iSKELETON_NUM);

	for(int i=0; i<iSKELETON_NUM; i++)
	{
		m_vvdCoord.at(i).resize(3);
	}

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

void StreamingThread::StreamOneFrame(vector< vector<double> >& vvdCoord)
{
    QMutexLocker locker(&mutex);

	for(int i=0; i<vvdCoord.size(); i++)
	{
		m_vvdCoord.at(i).at(0) =  vvdCoord.at(i).at(0);   
		m_vvdCoord.at(i).at(1) =  vvdCoord.at(i).at(1);
		m_vvdCoord.at(i).at(2) =  vvdCoord.at(i).at(2);
	}

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
	
    forever 
	{
        mutex.lock();

		for(int i=0; i<m_vvdCoord.size(); i++)
		{
			double dX = m_vvdCoord.at(i).at(0);
			double dY = m_vvdCoord.at(i).at(1);
			double dZ = m_vvdCoord.at(i).at(2);

			vector<float> &rvfPos = m_serverTracker->m_vvfWholeSkeleton.at(i);		

			rvfPos.at(0) = dX;
			rvfPos.at(1) = dY;
			rvfPos.at(2) = dZ;
		}

		/*
		angle += 0.001f;
		
		vector<float> &rvfPos_NECK = m_serverTracker->m_vvfWholeSkeleton.at(1);
		
		rvfPos_NECK.at(0) = sinf(angle);
		rvfPos_NECK.at(1) = dY;
		rvfPos_NECK.at(2) = dZ;*/
		
        mutex.unlock();
		
      
		//---------------------
		m_serverTracker->mainloop();

		m_Connection->mainloop();

		cout << "one vrpn loop." << endl;

		// Calling Sleep to let the CPU breathe.
		SleepEx(200,FALSE);
		//---------------------

		  		
		
		mutex.lock();

        if (!restart)
            condition.wait(&mutex);
        restart = false;
        mutex.unlock();
    }
}


#include "streamingthread.moc" //last line
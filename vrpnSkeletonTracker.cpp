// your tracker class must inherit from the vrpn_Tracker class

#include <math.h>

#include <iostream>
#include <fstream>


#include "vrpn_Text.h"
#include "vrpn_Tracker.h"

#include "vrpnSkeletonTracker.h"

myTracker::myTracker( vrpn_Connection *c ) :
	vrpn_Tracker( "Tracker0", c )
{
	m_vvfWholeSkeleton.resize(iSKELETON_NUM);

	for(int i=0; i<iSKELETON_NUM; i++)
	{
		m_vvfWholeSkeleton.at(i).resize(3);
	}
}

void myTracker::mainloop()
{
	vrpn_gettimeofday(&_timestamp, NULL);

	vrpn_Tracker::timestamp = _timestamp;

	// We will just put a fake data in the position of our tracker
	static float angle = 0; angle += 0.001f;

	for(int i=0; i<iSKELETON_NUM; i++)
	{
		vector<float> &rvfPos = m_vvfWholeSkeleton.at(i);

		// the pos array contains the position value of the tracker
		// XXX Set your values here
		pos[0] = sinf( angle ); 
		pos[1] = rvfPos.at(1);
		pos[2] = rvfPos.at(2);

		// the d_quat array contains the orientation value of the tracker, stored as a quaternion
		// XXX Set your values here
		d_quat[0] = 0.0f;
		d_quat[1] = 0.0f;
		d_quat[2] = 0.0f;
		d_quat[3] = 1.0f;

		char msgbuf[1000];

		d_sensor = i;

		int  len = vrpn_Tracker::encode_to(msgbuf);

		if (d_connection->pack_message(len, _timestamp, position_m_id, d_sender_id, msgbuf,
			vrpn_CONNECTION_LOW_LATENCY))
		{
			fprintf(stderr,"can't write message: tossing\n");
		}

		server_mainloop();
	}
}

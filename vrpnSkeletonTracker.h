
#ifndef VRPNSKELETONTRACKER_H
#define VRPNSKELETONTRACKER_H

#include <vector>

class vrpn_Tracker;

using namespace std;

#define iSKELETON_NUM 24

class myTracker : public vrpn_Tracker
{
public:
	myTracker( vrpn_Connection *c = 0 );
	virtual ~myTracker() {};

	virtual void mainloop();


	std::vector< std::vector<float> > m_vvfWholeSkeleton;

protected:
	struct timeval _timestamp;
};

#endif
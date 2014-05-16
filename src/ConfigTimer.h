#ifndef ECAP_ADAPTER_CONFIGEVENT_H
#define ECAP_ADAPTER_CONFIGEVENT_H

#include "time_utility.h"
#include "ConfigEvent.h"
#include <unistd.h>
#include <signal.h>
#include <set>
#include <boost/shared_ptr.hpp>
#include <boost/data_time/posix_time/posix_time.hpp>

namespace EventTimer {
	
class ConfigTimer {

private:
	typedef multiset<shared_ptr<ConfigEvent> > EventSet;
	EventSet configEvent_set;
	
private:
	void sig_alrm(int signo);
	void setTimer(int nsecs);
	void traverse();
	
public:
	ConfigTimer();
	bool addEvent(shared_ptr<ConfigEvent> event);
	bool delEvent(shared_ptr<ConfigEvent> event);
};

}
#endif
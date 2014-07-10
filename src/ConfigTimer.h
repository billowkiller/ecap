 
#ifndef ECAP_ADAPTER_CONFIGTIMER_H
#define ECAP_ADAPTER_CONFIGTIMER_H

#include "ConfigEvent.h"
#include <time.h>
#include <signal.h>
#include <unistd.h>
#include <map>
#include <boost/shared_ptr.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

using EventTimer::ConfigEvent;
using namespace boost::posix_time;

namespace EventTimer {

class ConfigTimer {

private:
	typedef std::multimap<ptime, boost::shared_ptr<ConfigEvent> > EventMap;
	EventMap configEvents;

	struct sigevent evp;
	struct itimerspec ts;
	timer_t timer;

private:
	ConfigTimer();
	void _update_timer();
	void _set_timer(int);

public:
	void checkEvent();
	static ConfigTimer& instance();
	void addConfig(boost::shared_ptr<ConfigEvent> event);
	bool delConfig(boost::shared_ptr<ConfigEvent> event);
	int getEventsNum();
};

void handle(union sigval v);
}
#endif
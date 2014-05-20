#ifndef ECAP_ADAPTER_CONFIGTIMER_H
#define ECAP_ADAPTER_CONFIGTIMER_H

#include "time_utility.h"
#include "ConfigEvent.h"
#include <unistd.h>
#include <signal.h>
#include <map>
#include <stdexcept>
#include <utility>
#include <boost/shared_ptr.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

using EventTimer::ConfigEvent;
namespace EventTimer {
	
class ConfigTimer {

private:
	typedef std::multimap<ptime, boost::shared_ptr<ConfigEvent>> EventMap;
	EventMap configEvents;
	
private:
	void setTimer(int nsecs);
	ConfigTimer();
	
public:
	static ConfigTimer& instance() {
		static ConfigTimer configTimer;
		return configTimer;
	}
	void checkEvent();
	bool addEvent(boost::shared_ptr<ConfigEvent> &event);
	bool delEvent(boost::shared_ptr<ConfigEvent> &event);
};



void sig_alrm(int signo) {
	ConfigTimer::instance().checkEvent();	
}

}
#endif
#ifndef ECAP_ADAPTER_CONFIGTIMER_H
#define ECAP_ADAPTER_CONFIGTIMER_H

#include "ConfigEvent.h"
#include <unistd.h>
#include <signal.h>
#include <map>
#include <stdexcept>
#include <utility>
#include <boost/shared_ptr.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

using EventTimer::ConfigEvent;
using namespace boost::posix_time;

namespace EventTimer {

class ConfigTimer {

private:
	typedef std::multimap<ptime, boost::shared_ptr<ConfigEvent>> EventMap;
	EventMap configEvents;
	
private:
	void sig_alrm(int signo) {
		ConfigTimer::instance().checkEvent();	
	}
	void setTimer(int nsecs);
	ConfigTimer();
	
public:
	static ConfigTimer& instance();
	void checkEvent();
	bool addEvent(boost::shared_ptr<ConfigEvent> &event);
	bool delEvent(boost::shared_ptr<ConfigEvent> &event);
	
	static boost::shared_ptr<ptime> curtime();
	
	static int seconds_gap(boost::shared_ptr<ptime> time1, boost::shared_ptr<ptime> time2);
	
	static int expected_seconds(boost::shared_ptr<ptime> time);
};

}
#endif
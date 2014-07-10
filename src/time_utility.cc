#include "time_utility.h"
#include "ConfigTimer.h"

boost::shared_ptr<ptime> EventTimer::curtime() {
	return boost::shared_ptr<ptime>(new ptime(second_clock::local_time()));
}

int EventTimer::seconds_gap(const ptime *time1,const ptime *time2) {
	time_duration gap = (*time1)-(*time2);
	return gap.total_seconds();
}

int EventTimer::expected_seconds(const ptime *time) {
	return seconds_gap(time, curtime().get());
}

int EventTimer::expected_seconds(const ptime &time) {
	return expected_seconds(&time);
}

std::string EventTimer::make_time(int secs) {
	boost::shared_ptr<time_duration> t(new time_duration(0,0,secs,0));
	boost::shared_ptr<ptime> time = EventTimer::curtime();
	*time += *t;
	return to_simple_string(*time);
}

bool EventTimer::addEvent(boost::shared_ptr<ConfigEvent> event)
{
	ConfigTimer &timer = ConfigTimer::instance();
	int num = timer.getEventsNum();
	ConfigTimer::instance().addConfig(event);
	return num+1 == timer.getEventsNum();
} 

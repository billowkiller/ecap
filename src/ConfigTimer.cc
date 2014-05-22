#include "ConfigTimer.h"
#include "Timer.h"
#include <functional>

EventTimer::ConfigTimer::ConfigTimer() {
	
}

EventTimer::ConfigTimer& EventTimer::ConfigTimer::instance(){
		static ConfigTimer configTimer;
		return configTimer;
}

void EventTimer::ConfigTimer::checkEvent() {
	ptime * ctime = curtime().get();
	
	for(EventMap::iterator it = configEvents.begin(); it!=configEvents.end();) {
		
		if(it->first <= *ctime) {
			boost::shared_ptr<ConfigEvent> event(it->second);
			event->triggerFunc();
			configEvents.erase(it++);
			if(event->event_status == ConfigEvent::CONFIGOPEN)
				addEvent(event);
		} else
			break;
	}
}

void EventTimer::ConfigTimer::setTimer(int nsecs) {
	
	std::function<void()> func = std::bind(&EventTimer::ConfigTimer::checkEvent, &EventTimer::ConfigTimer::instance()); 
	
	//async
	Timer timer(nsecs, true , func);
}

bool EventTimer::ConfigTimer::addEvent(boost::shared_ptr<ConfigEvent> &event) {
	configEvents.insert(std::make_pair(ptime(event->getCurTime()), event));
}

bool EventTimer::ConfigTimer::delEvent(boost::shared_ptr<ConfigEvent> &event) {
	std::pair<EventMap::iterator, EventMap::iterator> p = configEvents.equal_range(event->getStartTime());
	
	/* if the config has not taken effect */
	for(EventMap::iterator it=p.first; it!=p.second; it++) {
		if(it->second->configID == event->configID) {
			configEvents.erase(it);
			return true;
		}
	}
	
	/* if the config has taken effect */
	p = configEvents.equal_range(event->getEndTime());
	for(EventMap::iterator it=p.first; it!=p.second; it++) {
		if(it->second->configID == event->configID) {
			configEvents.erase(it);
			return true;
		}
	}
	
	return false;
}

boost::shared_ptr<ptime> curtime() {
	return boost::shared_ptr<ptime>(new ptime(second_clock::local_time()));
}

int seconds_gap(boost::shared_ptr<ptime> time1, boost::shared_ptr<ptime> time2) {
	time_duration gap = (*time1)-(*time2);
	return gap.total_seconds();
}

int expected_seconds(boost::shared_ptr<ptime> time) {
	return seconds_gap(time, curtime());
}
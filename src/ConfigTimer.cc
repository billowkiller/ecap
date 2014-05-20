#include "ConfigTimer.h"

EventTimer::ConfigTimer::ConfigTimer() {
	if(signal(SIGALRM, sig_alrm) == SIG_ERR)
		throw std::runtime_error("Cannot register SIGALRM handler function");
	
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
	alarm(nsecs);
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
#include "ConfigTimer"

ConfigTimer::ConfigTimer() {
	if(signal(SIGALRM, sig_alrm) == SIG_ERR)
		throw runtime_error("Cannot register SIGALRM handler function");
	
}

void ConfigTimer::sig_alrm(int signo) {
	
	ptime * ctime = curtime().get();
	
	for(EventSet::iterator it = configEvents.begin(); it!=configEvents.end(); it++) {
		
		if(it->first <= *ctime) {
			it->second->curFunction();
			configEvents.erase(it);
			if(it->seconde->event_status == ConfigEvent::CONFIGOPEN)
				addEvent(it->second);
		} else
			break;
	}
}

void ConfigTimer::setTimer(int nsecs) {
	alarm(nsecs);
}

bool ConfigTimer::addEvent(shared_ptr<ConfigEvent> event) {
	configEvents.insert(event->getCurTime, event);
}

bool ConfigTimer::delEvent(shared_ptr<ConfigEvent> event) {
	pair<EventMap::iterator, EventMap::iterator> p = configEvents.equal_range(event->getStartTime())
	
	/* if the config has not taken effect */
	for(EventMap::iterator it=p.first; it!=p.second; it++) {
		if(it->configID == event->configID) {
			configEvents.erase(it);
			return true;
		}
	}
	
	/* if the config has taken effect */
	p = configEvents.equal_range(event->getEndTime());
	for(EventMap::iterator it=p.first; it!=p.second; it++) {
		if(it->configID == event->configID) {
			configEvents.erase(it);
			return true;
		}
	}
	
	return false;
}
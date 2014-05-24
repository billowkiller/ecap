#include "ConfigTimer.h"
#include "time_utility.h"
#include <assert.h>

EventTimer::ConfigTimer::ConfigTimer() {
	evp.sigev_value.sival_ptr = &timer;
	evp.sigev_notify = SIGEV_THREAD;
	evp.sigev_notify_function = handle;
	evp.sigev_value.sival_int = 3;
	
	int ret = timer_create(CLOCK_REALTIME, &evp, &timer);
	assert(!ret);
}

EventTimer::ConfigTimer& EventTimer::ConfigTimer::instance(){
		static ConfigTimer configTimer;
		return configTimer;
}

void EventTimer::handle(union sigval v) {
	EventTimer::ConfigTimer::instance().checkEvent();
}

void EventTimer::ConfigTimer::checkEvent() {
	boost::shared_ptr<ptime> ctime = curtime();
	
	for(EventMap::iterator it = configEvents.begin(); it!=configEvents.end();) {
		
		if(it->first <= *ctime) {
			boost::shared_ptr<ConfigEvent> event(it->second);
			event->triggerFunc();
			configEvents.erase(it++);
			if(event->event_status == ConfigEvent::CONFIGOPEN)
				addConfig(event);
		} else
			break;
	}
	_update_timer();
}

void
EventTimer::ConfigTimer::_update_timer(){
	int secs = expected_seconds(configEvents.begin()->first);
	_set_timer(secs);
}

void
EventTimer::ConfigTimer::_set_timer(int sec) {
	
	struct itimerspec ts;
	ts.it_interval.tv_sec = 0;
	ts.it_interval.tv_nsec = 0;
	ts.it_value.tv_sec = sec;
	ts.it_value.tv_nsec = 0;
	
	int ret = timer_settime (timer, 0, &ts, NULL);
	assert(!ret);
}

bool EventTimer::ConfigTimer::addConfig(boost::shared_ptr<ConfigEvent> &event) {
	
	if(event->getCurTime() < configEvents.begin()->first){
		_set_timer(expected_seconds(event->getCurTime()));
	}
	
	configEvents.insert(std::make_pair(ptime(event->getCurTime()), event));
}

bool EventTimer::ConfigTimer::delConfig(boost::shared_ptr<ConfigEvent> &event) {
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

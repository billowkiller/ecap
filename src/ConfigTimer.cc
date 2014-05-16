#include "ConfigTimer"

ConfigTimer::ConfigTimer() {
	if(signal(SIGALRM, sig_alrm) == SIG_ERR)
		throw runtime_error("Cannot register SIGALRM handler function");
	
}

void ConfigTimer::sig_alrm(int signo) {
	
	ptime * ctime = curtime().get();
	
	for(EventSet::iterator it = configEvent_set.begin(); it!=configEvent_set.end(); it++) {
		if(it->curMeasureTime <= *ctime) {
			
			configEvent_set.erase(it);
			it->curFunction();
		}
			
	}
}

void ConfigTimer::setTimer(int nsecs) {
	alarm(nsecs);
}

void ConfigTimer::traverse() {
	
	
}

bool ConfigTimer::addEvent(shared_ptr<ConfigEvent> event) {
	
}

bool ConfigTimer::delEvent(shared_ptr<ConfigEvent> event) {
	
}
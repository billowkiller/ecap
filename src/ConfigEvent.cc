#include "ConfigEvent.h"
#include "EventFuncs.h"
/* define ConfigEvent */

EventTimer::ConfigEvent::ConfigEvent(std::string id, std::string start_time, std::string end_time):
configID(id),
event_status(CONFIGCLOSE),
GK_start_time(boost::posix_time::time_from_string(start_time)),
GK_end_time(boost::posix_time::time_from_string(end_time)){

}

const boost::posix_time::ptime & EventTimer::ConfigEvent::getCurTime() const {
	if(event_status == CONFIGCLOSE)
		return GK_start_time;
	else
		return GK_end_time;
}

const boost::posix_time::ptime & EventTimer::ConfigEvent::getStartTime() const {
	return GK_start_time;
}

const boost::posix_time::ptime & EventTimer::ConfigEvent::getEndTime() const {
	return GK_end_time;
}

/* end */


/* define IDConfigEvent */

EventTimer::IDConfigEvent::IDConfigEvent(std::string id, std::string start_time, std::string end_time, std::string sid, std::string rid, std::string action):
EventTimer::ConfigEvent(id, start_time, end_time), PZUserID(sid), PZResourceID(rid), PZAction(action){
}

EventTimer::IDConfigEvent::IDConfigEvent(std::string id, std::string start_time, std::string end_time):IDConfigEvent(id, start_time, end_time, "", "", "") {

}

void EventTimer::IDConfigEvent::setUserID(std::string id) {
	PZUserID = id;
}

void EventTimer::IDConfigEvent::setResourceID(std::string id) {
	PZResourceID = id;
}

void EventTimer::IDConfigEvent::setAction(std::string id) {
	PZAction = id;
}

void EventTimer::IDConfigEvent::setSOA(std::string sid, std::string rid, std::string action) {
	PZUserID = sid;
	PZResourceID = rid;
	PZAction = action;
}

void EventTimer::IDConfigEvent::triggerFunc() {
	if(event_status == CONFIGCLOSE) {
		boost::bind(&EventTimer::EventFuncs::IDAddFunc, _1, _2, _3)
				(PZUserID, PZResourceID, PZAction);
		event_status = CONFIGOPEN;
	}
	else {
		boost::bind(&EventTimer::EventFuncs::IDDelFunc, _1, _2, _3)
				(PZUserID, PZResourceID, PZAction);
		event_status = CONFIGCLOSE;
	}
}

/* end */


/* define KWConfigEvent */

EventTimer::KWConfigEvent::KWConfigEvent(std::string id, std::string start_time, std::string end_time, std::string kw):EventTimer::ConfigEvent(id, start_time, end_time){

	PZControlContents.push_back(kw);
}

EventTimer::KWConfigEvent::KWConfigEvent(std::string id, std::string start_time, std::string end_time):ConfigEvent(id, start_time, end_time) {

}

void EventTimer::KWConfigEvent::addKeyword(std::string kw) {
	PZControlContents.push_back(kw);
}

void EventTimer::KWConfigEvent::addKeywords(std::vector<std::string> kws) {
	PZControlContents.insert(PZControlContents.end()-1, kws.begin(), kws.end());
}

void EventTimer::KWConfigEvent::triggerFunc() {
	if(event_status == CONFIGCLOSE) {
		boost::bind(&EventTimer::EventFuncs::kwsAddFunc, _1)(PZControlContents);
		event_status = CONFIGOPEN;
	}
	else {
		boost::bind(&EventTimer::EventFuncs::kwsDelFunc, _1)(PZControlContents);
		event_status = CONFIGCLOSE;
	}
}

/* end */
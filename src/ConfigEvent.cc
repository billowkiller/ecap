#include "ConfigEvent.h"

/* define ConfigEvent */

ConfigEvent::ConfigEvent(std::string id, std::string start_time, std::string end_time):
configID(id),
event_status(CONFIGCLOSE),
GK_start_time(time_from_string(start_time)),
GK_end_time(time_from_string(end_time)){
	
}

ptime & ConfigEvent::getCurTime() {
	if(event_status == CONFIGCLOSE)
		return GK_start_time;
	else
		return GK_end_time;
}

ptime & ConfigEvent::getStartTime() {
	return GK_start_time;
}

ptime & ConfigEvent::getEndTime() {
	return GK_end_time;
}

/* end */


/* define IDConfigEvent */

IDConfigEvent(std::string id, std::string start_time, std::string end_time, std::string sid, std::string rid, std::string action):
ConfigEvent(id, start_time, end_time), PZUserID(sid), PZResourceID(rid), PZAction(action){
	//default addFuction and delFunction
}

IDConfigEvent::IDConfigEvent(std::string id, std::string start_time, std::string end_time):IDConfigEvent(id, start_time, end_time, "", "", "") {
	
}

void IDConfigEvent::setUserID(std::string id) {
	PZUserID = id;
}

void IDConfigEvent::setResourceID(std::string id) {
	PZResourceID = id;
}

void IDConfigEvent::setAction(std::string id) {
	PZAction = id;
}

void IDConfigEvent::setSOA(std::string sid, std::string rid, std::string action) {
	PZUserID = sid;
	PZResourceID = rid;
	PZAction = action;
}

void IDConfigEvent::setAddFunc() {
}

void IDConfigEvent::setDelFunc() {
}

/* end */


/* define KWConfigEvent */

KWConfigEvent(std::string id, std::string start_time, std::string end_time, std::string kw):
ConfigEvent(id, start_time, end_time), PZControlContent(kw){
	//default addFuction and delFunction
}

KWConfigEvent::KWConfigEvent(std::string id, std::string start_time, std::string end_time):IDConfigEvent(id, start_time, end_time, "") {
	
}

void KWConfigEvent::setKeyword(std::string kw) {
	PZControlContent = kw;
}
void KWConfigEvent::setAddFunc() {
}

void KWConfigEvent::setDelFunc() {
}

/* end */
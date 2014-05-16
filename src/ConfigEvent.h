#ifndef ECAP_ADAPTER_CONFIGEVENT_H
#define ECAP_ADAPTER_CONFIGEVENT_H

#include <string>
#include <boost/data_time/posix_time/posix_time.hpp>

namespace EventTimer {

class ConfigEvent {
	
public:	
	typedef enum {CONFIGOPEN, CONFIGCLOSE} EVENTSTATUS;
	std::string configID;
	EVENTSTATUS event_status;
	
protected:
	typedef boost::function<void()> func_type;
	
protected:
	
	ptime GK_start_time;
	ptime GK_end_time;

	func_type curFunction;
	func_type addFunction;
	func_type delFunction;
	
public:
	ConfigEvent(std::string id, std::string start_time, std::string end_time);
	ptime & getCurTime();
	ptime & getStartTime();
	ptime & getEndTime();
	virtual void setAddFunc() = 0;
	virtual void setDelFunc() = 0;
	
	friend bool operator<(const ConfigEvent &event1, const ConfigEvent &event2) const {
		return event1.getCurTime() < event2.getCurTime();
	};
};

class IDConfigEvent : public ConfigEvent {

private:
	std::string PZUserID;
	std::string PZResourceID;
	std::string PZAction;

public:
	IDConfigEvent(std::string id, std::string start_time, std::string end_time);
	IDConfigEvent(std::string id, std::string start_time, std::string end_time, std::string sid, std::string rid, std::string action);
	
	void setUserID(std::string id);
	void setResourceID(std::string id);
	void setAction(std::string action);
	void setSOA(std::string sid, std::string rid, std::string action);
	
	virtual void setAddFunc();
	virtual void setDelFunc();
};

class KWConfigEvent : public ConfigEvent {

private:
	std::string PZControlContent;

public:
	KWConfigEvent(std::string id, std::string start_time, std::string end_time);
	KWConfigEvent(std::string id, std::string start_time, std::string end_time, std::string kw);
	
	void setKeyword(std::string kw);
	
	virtual void setAddFunc();
	virtual void setDelFunc();
};

} //namespace EventTimer
#endif
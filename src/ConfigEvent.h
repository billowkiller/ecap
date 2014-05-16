#ifndef ECAP_ADAPTER_CONFIGEVENT_H
#define ECAP_ADAPTER_CONFIGEVENT_H

#include <string>
#include <boost/data_time/posix_time/posix_time.hpp>

namespace Event {

class ConfigEvent {
	
public:	
	typedef enum {ID, KEYWORD} EVENTTYPE;
	std::string configID;
	
protected:
	typedef boost::function<void()> func_type;
	
protected:
	
	boost::posix_time::ptime curMeasureTime;
	boost::posix_time::ptime GK_start_time;
	boost::posix_time::ptime GK_end_time;
	
	EVENTTYPE event_type;
	
	/* if event type is ID, then the array means:
	 * PZUserID, PZResourceID, PZAction.
	 * else if event type is KEYWORD, then the first
	 * string has meaning--PZControlContent
	 */
// 	std::string GK_string[3];

	func_type curFunction;
	func_type addFunction;
	func_type delFunction;
	
public:
	ConfigEvent(EVENTTYPE type, configID);
	ConfigEvent(EVENTTYPE type, configID, std::string start_time, std::string end_time);
	virtual void setAddFunc(func_type addFunc);
	virtual void setDelFunc(func_type delFunc);
	
	friend bool operator<(const ConfigEvent &event1, const ConfigEvent &event2) const;
};

class IDConfigEvent : public ConfigEvent {

private:
	std::string PZUserID;
	std::string PZResourceID;
	std::string PZAction;

public:
	IDConfigEvent(EVENTTYPE type, configID);
	IDConfigEvent(EVENTTYPE type, configID, std::string start_time, std::string end_time);
	IDConfigEvent(EVENTTYPE type, configID, std::string start_time, std::string end_time, 
					std::string sid, std::string rid, std::string action);
	
	void setUserID(std::string id);
	void setResourceID(std::string id);
	void setAction(std::string action);
	void setSOA(std::string sid, std::string rid, std::string action);
	
	virtual void setAddFunc(func_type addFunc);
	virtual void setDelFunc(func_type delFunc);
};

class KWConfigEvent : public ConfigEvent {

private:
	std::string PZControlContent;

public:
	KWConfigEvent(EVENTTYPE type, configID);
	KWConfigEvent(EVENTTYPE type, configID, std::string start_time, std::string end_time);
	KWConfigEvent(EVENTTYPE type, configID, std::string start_time, std::string end_time, std::string kw);
	
	void setKeyword(std::string kw);
	
	virtual void setAddFunc(func_type addFunc);
	virtual void setDelFunc(func_type delFunc);
};

} //namespace Event
#endif
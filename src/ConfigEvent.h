 
#ifndef ECAP_ADAPTER_CONFIGEVENT_H
#define ECAP_ADAPTER_CONFIGEVENT_H

#include <string>
#include <vector>
#include <boost/date_time/posix_time/posix_time.hpp>
#include<boost/bind.hpp>

namespace EventTimer {

class ConfigEvent {
 	
public:	

	typedef enum {CONFIGOPEN, CONFIGCLOSE} EVENTSTATUS;
	std::string configID;
	EVENTSTATUS event_status;

protected:

	const boost::posix_time::ptime GK_start_time;
	const boost::posix_time::ptime GK_end_time;

public:
	ConfigEvent(std::string id, std::string start_time, std::string end_time);
	const boost::posix_time::ptime & getCurTime() const;
	const boost::posix_time::ptime & getStartTime() const;
	const boost::posix_time::ptime & getEndTime() const;
	virtual void triggerFunc() = 0;

	bool operator<(const ConfigEvent &rhs) {
		return getCurTime() < rhs.getCurTime();
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

	virtual void triggerFunc();
};

class KWConfigEvent : public ConfigEvent {

private:
	std::vector<std::string> PZControlContents;

public:
	KWConfigEvent(std::string id, std::string start_time, std::string end_time);
	KWConfigEvent(std::string id, std::string start_time, std::string end_time, std::string kw);
	KWConfigEvent(std::string id, std::string start_time, std::string end_time, std::vector<std::string> kws);

	void addKeyword(std::string kw);
	void addKeywords(std::vector<std::string> kws);

	virtual void triggerFunc();
};

} //namespace EventTimer
#endif
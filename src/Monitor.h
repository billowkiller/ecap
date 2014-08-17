#ifndef ECAP_ADAPTER_MONITOR_H
#define ECAP_ADAPTER_MONITOR_H

#include <string>
#include <atomic>
#include <vector>
#include <boost/utility.hpp>
#include <boost/thread/thread.hpp>

class Monitor : boost::noncopyable{
	
private:
	Monitor();
	std::string vec2str(const std::vector<std::string> &vec);
	void startSending();
	
public:
	static Monitor& instance();
	static void startMonitor();
	static void addRequest();
	static void addUpstream(int size);
	static void addDownstream(int size);
	static void addLoginfo(std::string &str);
	static void addLoginfo(const char* str);

private:
	static std::atomic_int request_num;
	static std::atomic_int upstream_size;
	static std::atomic_int downstream_size;
	static std::vector<std::string> vs;  //log
	
	const int port;
	const std::string ip;
	
	static boost::mutex mutex;
};

#endif
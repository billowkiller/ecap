#ifndef ECAP_ADAPTER_STRMATCHER_H
#define ECAP_ADAPTER_STRMATCHER_H

#include <string>
#include <vector>
#include <iostream>

namespace EventTimer{

class StrMatcher {
public:
	static void kwsAddFunc(std::vector<std::string> &keyword);
	
	static void kwsDelFunc(std::vector<std::string> &keyword);
	
	static void IDAddFunc(std::string &sid, std::string &rid, std::string &action);
	
	static void IDDelFunc(std::string &sid, std::string &rid, std::string &action);
};
}

#endif
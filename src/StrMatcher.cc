#include "StrMatcher.h"

void EventTimer::StrMatcher::kwsAddFunc(std::vector<std::string> &keyword) {
		std::cout << "kwsAddFunc" << std::endl;
}

void EventTimer::StrMatcher::kwsDelFunc(std::vector<std::string> &keyword) {
	std::cout << "kwsDelFunc" << std::endl;
}

void EventTimer::StrMatcher::IDAddFunc(std::string &sid, std::string &rid, std::string &action) {
	std::cout << "IDAddFunc" << std::endl;
}

void EventTimer::StrMatcher::IDDelFunc(std::string &sid, std::string &rid, std::string &action) {
	std::cout << "IDDelFunc" << std::endl;
}
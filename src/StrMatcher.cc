#include "StrMatcher.h"
#include <cstdio>

using namespace std;

void EventTimer::StrMatcher::kwsAddFunc(std::vector<std::string> &keyword) {
	printf("kwsAddFunc\n");
}

void EventTimer::StrMatcher::kwsDelFunc(std::vector<std::string> &keyword) {
	printf("kwsDelFunc, %s\n", keyword[0].c_str());
}

void EventTimer::StrMatcher::IDAddFunc(std::string &sid, std::string &rid, std::string &action) {
	printf("IDAddFunc\n");
}

void EventTimer::StrMatcher::IDDelFunc(std::string &sid, std::string &rid, std::string &action) {
	printf("IDDelFunc\n");
}
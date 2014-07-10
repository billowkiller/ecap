#include "EventFuncs.h"
#include "SQLFactory.h"
#include <cstdio>

using namespace std;

void EventTimer::EventFuncs::kwsAddFunc(std::vector<std::string> &keyword) {
	printf("kwsAddFunc\n");
	KeywordHandler kwhandler;
	int i;
	for(i=0;i<keyword.size();i++)
	kwhandler.addKeyword(keyword[i],"","","");
}

void EventTimer::EventFuncs::kwsDelFunc(std::vector<std::string> &keyword) {
	printf("kwsDelFunc, %s\n", keyword[0].c_str());
}

void EventTimer::EventFuncs::IDAddFunc(std::string &sid, std::string &rid, std::string &action) {
	printf("IDAddFunc\n");
	RuleHandler rulehandler;
	rulehandler.addRule(sid,"",action,"","",rid,"");
}

void EventTimer::EventFuncs::IDDelFunc(std::string &sid, std::string &rid, std::string &action) {
	printf("IDDelFunc\n");
} 

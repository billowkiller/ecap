#include "EventFuncs.h"
#include "SQLFactory.h"
#include "StrMatcher.h"
#include <cstdio>

using namespace std;

void EventTimer::EventFuncs::kwsAddFunc(std::vector<std::string> &keyword) {
	printf("kwsAddFunc\n");
	KeywordHandler kwhandler;
	int i;
	for(i=0;i<keyword.size();i++)
	kwhandler.addKeyword(keyword[i],"","","");
	
	StrMatcher::instance().addMultiStr(keyword);
}

void EventTimer::EventFuncs::kwsDelFunc(std::vector<std::string> &keyword) {
	printf("kwsDelFunc, %s\n", keyword[0].c_str());
	KeywordHandler kwhandler;
	int i;
	for(i=0;i<keyword.size();i++)
	kwhandler.delKeyword(keyword[i],"","","");
	
	StrMatcher::instance().delMultiStr(keyword);
}

void EventTimer::EventFuncs::IDAddFunc(std::string &sid, std::string &rid, std::string &action) {
	printf("IDAddFunc\n");
	RuleHandler rulehandler;
	rulehandler.addRule(sid,"",action,"","",rid,"");
}

void EventTimer::EventFuncs::IDDelFunc(std::string &sid, std::string &rid, std::string &action) {
	printf("IDDelFunc\n");
	RuleHandler rulehandler;
	rulehandler.delRule(sid,"",action,"","",rid,"");
} 

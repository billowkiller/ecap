#ifndef ECAP_ADAPTER_RULEHANDLER_H
#define ECAP_ADAPTER_RULEHANDLER_H

#include <Debugger.h>
#include <string>

class RuleHandler {
public:
	void addRule(const char *pc, int size) {
		Debugger() << std::string(pc, size);
	}
};

#endif
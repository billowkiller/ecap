#ifndef ECAP_ADAPTER_STRINGUTILITY_H
#define ECAP_ADAPTER_STRINGUTILITY_H

#include <string>
#include <vector>
#include <iostream>

namespace EventTimer{
	
	void kwsAddFunc(std::vector<std::string> &keyword) {
		std::cout << "kwsAddFunc" << std::endl;
	}
	
	void kwsDelFunc(std::vector<std::string> &keyword) {
		std::cout << "kwsDelFunc" << std::endl;
	}
	
	void IDAddFunc(std::string &sid, std::string &rid, std::string &action) {
		std::cout << "IDAddFunc" << std::endl;
	}
	
	void IDDelFunc(std::string &sid, std::string &rid, std::string &action) {
		std::cout << "IDDelFunc" << std::endl;
	}
	
}

#endif
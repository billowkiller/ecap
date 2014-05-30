#ifndef ECAP_ADAPTER_RULEACCEPTER_H
#define ECAP_ADAPTER_RULEACCEPTER_H

#include "RuleHandler.h"
#include <boost/utility.hpp>
#include "Debugger.h"
#include <string>

class RuleAccepter{
private:
	RuleHandler ruleHandler;
	char port[10];
	int sfd;
	static const unsigned MAXBYTES = 1000; //We assume that receiving data is less then 1000 bytes
	static const unsigned MAXEVENTS = 64;
	const std::string response;
	
public:
	static RuleAccepter& instance();
	void startSever();
	static void startAccept();
	
private:
	RuleAccepter(int port = 12345);
	int make_socket_non_blocking(int sfd);
	int create_and_bind (char *port);
	
};

#endif
#include "Monitor.h"
#include "Debugger.h"
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

std::atomic_int Monitor::request_num(0);
std::atomic_int Monitor::upstream_size(0);
std::atomic_int Monitor::downstream_size(0);
std::vector<std::string> Monitor::vs;
boost::mutex Monitor::mutex;

Monitor::Monitor():port(10010), ip("127.0.0.1") { }

Monitor& Monitor::instance(){
	
	static Monitor _monitor;
	return _monitor;
}

void Monitor::startSending() {
	
	int sockfd,n;
	struct sockaddr_in servaddr,cliaddr;
	char recvline[1000];

	sockfd=socket(AF_INET,SOCK_STREAM,0);

	bzero(&servaddr,sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr=inet_addr(ip.c_str());
	servaddr.sin_port=htons(port);
	
	if(connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr))<0)
	   printf("connect error");
	
	while(true) {
		std::ostringstream oss;
		oss << request_num << ',' << upstream_size << ',' << vec2str(vs);
		std::string str(oss.str());
		//sprintf(sendline, "%d,%d,%s\n", request_num, upstream_size, vec2str(vs).c_str());
		sendto(sockfd, str.c_str(), str.length(), 0,
             (struct sockaddr *)&servaddr, sizeof(servaddr));
		//Debugger() << "	send complete" << str;
		n=recvfrom(sockfd,recvline,1000,0,NULL,NULL);
		//recvline[n]=0;
		request_num = 0; upstream_size = 0; downstream_size=0; vs.clear(); //regather
		sleep(1);
	}
}

void Monitor::startMonitor() {
	Debugger()<<"start monitor";
	boost::thread thrd(boost::bind(&Monitor::startSending, &Monitor::instance()));
	thrd.detach();
}

std::string Monitor::vec2str(const std::vector<std::string> &vec) {
	
	std::ostringstream oss;

	if (!vec.empty())
	{
		// Convert all but the last element to avoid a trailing " "
		std::copy(vec.begin(), vec.end()-1,
			std::ostream_iterator<std::string>(oss, "#"));
		// Now add the last element with no delimiter
		oss << vec.back();
	}
	return oss.str();
}

void Monitor::addRequest() {
	request_num++;
	//Debugger() << "addRequest";
}

void Monitor::addUpstream(int size) {
	upstream_size += size;
	//Debugger() << "addUpstream";
}

void Monitor::addDownstream(int size) {
	downstream_size += size;
	//Debugger() << "addDownstream";
}

void Monitor::addLoginfo(const char* str) {
	//Debugger() << "addLoginfo";
	mutex.lock(); 
	vs.push_back(std::string(str));
	mutex.unlock(); 
}

void Monitor::addLoginfo(std::string &str) {
	//Debugger() << "addLoginfo";
	mutex.lock(); 
	
	vs.push_back(str);
	mutex.unlock(); 
}
/*
int main() {
	Monitor::addRequest();
	Monitor::addDownstream(122);
	Monitor::addLoginfo("hello");
	Monitor::startMonitor();
}*/
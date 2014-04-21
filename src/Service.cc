#include "RespHeader.h"

std::string Adapter::Service::uri() const {
	return "ecap://billowkiller.com/response";
}

std::string Adapter::Service::tag() const {
	return PACKAGE_VERSION;
}

void Adapter::Service::describe(std::ostream &os) const {
	os << "A response adapter from " << PACKAGE_NAME << " v" << PACKAGE_VERSION;
}

void Adapter::Service::configure(const Options &) {
	// this service is not configurable
}

void Adapter::Service::reconfigure(const Options &) {
	// this service is not configurable
}

void Adapter::Service::start() {
	adapter::Service::start();
	// custom code would go here, but this service does not have one
}

void Adapter::Service::stop() {
	// custom code would go here, but this service does not have one
	adapter::Service::stop();
}

void Adapter::Service::retire() {
	// custom code would go here, but this service does not have one
	adapter::Service::stop();
}

bool Adapter::Service::wantsUrl(const char *url) const {
	return true; // minimal adapter is applied to all messages
}

adapter::Xaction *Adapter::Service::makeXaction(host::Xaction *hostx) {
	return new Adapter::Xaction(hostx);
}
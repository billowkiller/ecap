
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <iostream>

#include "RequestHandler.h"
#include "Debugger.h"
#include <libecap/common/registry.h>
#include <libecap/common/errors.h>
#include <libecap/common/header.h>
#include <libecap/common/message.h>
#include <libecap/common/log.h>
#include <libecap/common/names.h>
#include <libecap/adapter/xaction.h>
#include <libecap/adapter/service.h>
#include <libecap/host/host.h> 
#include <libecap/host/xaction.h>
#include <sqlite3.h>
#include "kw_match.h"


/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  init_sqlite
 *  Description:  
 * =====================================================================================
 */
/*
sqlite3 *db = NULL;
int init_sqlite()
{
    char* dbpath="/opt/lampp/htdocs/FBControlUI/rulecontrol/fbac.db";
   
    int rc;
    //open the database file.If the file is not exist,it will create a file.
    rc = sqlite3_open(dbpath, &db);
    if(rc)
    {
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
		sqlite3_close(db);
        return 0;
    }
	return 1;
}	

*/

using namespace libecap;

namespace Adapter { // not required, but adds clarity

class Service: public adapter::Service {
	public:
		// About
		virtual std::string uri() const; // unique across all vendors
		virtual std::string tag() const; // changes with version and config
		virtual void describe(std::ostream &os) const; // free-format info

		// Configuration
		virtual void configure(const Options &cfg);
		virtual void reconfigure(const Options &cfg);

		// Lifecycle
		virtual void start(); // expect makeXaction() calls
		virtual void stop(); // no more makeXaction() calls until start()
		virtual void retire(); // no more makeXaction() calls

		// Scope (XXX: this may be changed to look at the whole header)
		virtual bool wantsUrl(const char *url) const;

		// Work
		virtual adapter::Xaction *makeXaction(host::Xaction *hostx);

};


class Xaction: public adapter::Xaction {
	public:
		Xaction(host::Xaction *x);
		virtual ~Xaction();

		// meta-information for the host transaction
		virtual const Area option(const Name &name) const;
		virtual void visitEachOption(NamedValueVisitor &visitor) const;

		// lifecycle
		virtual void start();
		virtual void stop();

		// adapted body transmission control
		virtual void abDiscard();
		virtual void abMake();
		virtual void abMakeMore();
		virtual void abStopMaking();

		// adapted body content extraction and consumption
		virtual Area abContent(size_type offset, size_type size);
		virtual void abContentShift(size_type size);

		// virgin body state notification
		virtual void noteVbContentDone(bool atEnd);
		virtual void noteVbContentAvailable();

		// Callable API, via host::Xaction
		virtual bool callable() const;

	protected:
		host::Xaction *lastHostCall(); // clears hostx

	private:
		
		static const Name cookieName;
        static const Name contentTypeName;
        static const Name contentEncodingName;
		
		host::Xaction *hostx; // Host transaction rep
		Header::Value clientIP;
		std::string	dzBuffer;
		RequestLine *requestLine;
				
		typedef enum { opUndecided, opOn, opComplete, opNever } OperationState;
		OperationState receivingVb;
		OperationState sendingAb;
		
		RequestHandler::RequestInfo request_info;
        /* parse request header value for four-tuple */
        void fetchRequestInfo(shared_ptr<Message> &adapted);
        std::string fetchHeaderValue(shared_ptr<Message> &adapted, const Name &name);
        /* not completed */
        bool validRequestHeader(shared_ptr<Message> &adapted);
		
		void blockRequest(){ hostx->blockVirgin(); }
};

} // namespace Adapter


std::string Adapter::Service::uri() const {
	return "ecap://billowkiller.com/request";
}

std::string Adapter::Service::tag() const {
	return "0.0.0";
}

void Adapter::Service::describe(std::ostream &os) const {
	os << "A request adapter for squid v0.0.0";
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
	return true;
}

adapter::Xaction * Adapter::Service::makeXaction(host::Xaction *hostx) {
	return new Adapter::Xaction(hostx);
}

const Name Adapter::Xaction::cookieName = Name("Cookie");
const Name Adapter::Xaction::contentTypeName= Name("Content-Type");
const Name Adapter::Xaction::contentEncodingName= Name("Content-Encoding");

Adapter::Xaction::Xaction(host::Xaction *x): hostx(x),
	receivingVb(opUndecided), sendingAb(opUndecided) {
}

Adapter::Xaction::~Xaction() {
	if (host::Xaction *x = hostx) {
		hostx = 0;
		x->adaptationAborted();
	}

}

const Area Adapter::Xaction::option(const Name &) const {
	return Area(); // this transaction has no meta-information
}

void Adapter::Xaction::visitEachOption(NamedValueVisitor &) const {
	// this transaction has no meta-information to pass to the visitor
}

void Adapter::Xaction::start() {
	
	// TODO: libecap should probably supply a global LastCall() of sorts
	// to clear hostx member and then call the host transaction one last time
	Must(hostx);
	if (hostx->virgin().body()) {
		receivingVb = opOn;
		dzBuffer="";
		hostx->vbMake(); // ask host to supply virgin body
	} else {
		receivingVb = opNever;
	}

	shared_ptr<Message> adapted = hostx->virgin().clone();
	Must(adapted != 0);
	
	
	
	if (validRequestHeader(adapted))
    {
        fetchRequestInfo(adapted);
		
        Debugger()<<request_info.url;
		/*
		if(request_info.url.find("douban") != std::string::npos)
			blockRequest();
		*/
	
		
		
		if (!adapted->body()) 
		{
			
			
			RequestHandler* qh=new RequestHandler(request_info);
			
		    if(!qh->checkValidation())
		   {
			  
			blockRequest();
			Debugger()<<"blocked";
			
		   }
		    delete qh;
			sendingAb = opNever; // there is nothing to send
			lastHostCall()->useAdapted(adapted);
		} 
		else
			hostx->useAdapted(adapted);
		
    }
    else
	{
		hostx->useVirgin();
		abDiscard();
	}
}

void Adapter::Xaction::stop() {
	hostx = 0;
	// the caller will delete
}

void Adapter::Xaction::abDiscard()
{
	Must(sendingAb == opUndecided); // have not started yet
	sendingAb = opNever;
}

void Adapter::Xaction::abMake()
{
	Must(sendingAb == opUndecided); // have not yet started or decided not to send
	Must(hostx->virgin().body()); // that is our only source of ab content

	// we are or were receiving vb
	Must(receivingVb == opOn || receivingVb == opComplete);
	
	sendingAb = opOn;
	if (!dzBuffer.empty())
		hostx->noteAbContentAvailable();
}

void Adapter::Xaction::abMakeMore()
{
	Must(receivingVb == opOn); // a precondition for receiving more vb
	hostx->vbMakeMore();
}

void Adapter::Xaction::abStopMaking()
{
	sendingAb = opComplete;
	// we may still continue receiving
}


Area Adapter::Xaction::abContent(size_type offset, size_type size) {

	// required to not raise an exception on the final call with opComplete
	Must(sendingAb == opOn || sendingAb == opComplete);

	// if complete, there is nothing more to return.
	if (sendingAb == opComplete) {
		return Area::FromTempString("");
	}
	return Area::FromTempString(dzBuffer.substr(offset, size));
}

void Adapter::Xaction::abContentShift(size_type size) {
	Must(sendingAb == opOn || sendingAb == opComplete);
	dzBuffer.erase(0, size);
}


void Adapter::Xaction::noteVbContentDone(bool atEnd) {	
		
	Must(receivingVb == opOn);
	receivingVb = opComplete;
	
	if (sendingAb == opOn) {
		hostx->noteAbContentDone(atEnd);
		sendingAb = opComplete;
	}
	
}

void Adapter::Xaction::noteVbContentAvailable() {
	
	Must(receivingVb == opOn);

	const Area vb = hostx->vbContent(0, nsize); // get all vb
	std::string chunk = vb.toString(); // expensive, but simple
	hostx->vbContentShift(vb.size); 
	dzBuffer += chunk; 
	
	request_info.content=dzBuffer;
	
	
			RequestHandler* qh=new RequestHandler(request_info);
			
		    if(!qh->checkValidation())
		   {
			  
			blockRequest();
			Debugger()<<"blocked";
			
		   }
		    delete qh;
// 	RequestHandler* qh=new RequestHandler(request_info);
// 		
// 		if(!qh->checkValidation())
// 		{
// 			blockRequest();
// 			Debugger()<<"blocked";
// 			
// 		}
// 		delete qh;
	
	
	
	
	
	
	std::string a("dzBuffer: ");
	Debugger() << a << dzBuffer;
	
	if (sendingAb == opOn)
		hostx->noteAbContentAvailable();
}

void Adapter::Xaction::fetchRequestInfo(shared_ptr<Message> &adapted) {

    request_info.referrer = fetchHeaderValue(adapted, headerReferer);
    request_info.cookie = fetchHeaderValue(adapted, cookieName);
	request_info.content = "";
   
/*
     if(methodGet == requestLine->method())
         request_info.content = "";
     else if(adapted->body()){
         const Area vb = hostx->vbContent(0, nsize); // get all vb
         request_info.content = vb.toString();
     }
*/
}

std::string Adapter::Xaction::fetchHeaderValue(shared_ptr<Message> &adapted, const Name &headerName) {
    if(adapted->header().hasAny(headerName)) {
        const Header::Value value = adapted->header().value(headerName);
        if(value.size)
            return value.toString();
    }
    return "";
}

bool Adapter::Xaction::validRequestHeader(shared_ptr<Message> &adapted) {
	
	requestLine = dynamic_cast<RequestLine *>(&adapted->firstLine());
	
    request_info.url = requestLine->uri().toString();
	
	if(request_info.url.rfind("css") != std::string::npos)
        return false;
    if(request_info.url.rfind("js") != std::string::npos)
        return false;
	/*
    if(request_info.url.rfind("jpg") != std::string::npos)
        return false;
	if(request_info.url.rfind("png") != std::string::npos)
        return false;
	if(request_info.url.rfind("jpeg") != std::string::npos)
        return false;
	if(request_info.url.rfind("gif") != std::string::npos)
        return false;
	*/
    return true;
}


bool Adapter::Xaction::callable() const {
	return hostx != 0; // no point to call us if we are done
}

// this method is used to make the last call to hostx transaction
// last call may delete adapter transaction if the host no longer needs it
// TODO: replace with hostx-independent "done" method
host::Xaction *Adapter::Xaction::lastHostCall() {
	host::Xaction *x = hostx;
	Must(x);
	hostx = 0;
	return x;
}

// create the adapter and register with libecap to reach the host application
static const bool Registered = (RegisterService(new Adapter::Service), true);



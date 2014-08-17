#include "header.h"

const Name Adapter::Xaction::cookieName = Name("Cookie");
const Name Adapter::Xaction::contentTypeName= Name("Content-Type");
const Name Adapter::Xaction::contentEncodingName= Name("Content-Encoding");

Adapter::Xaction::Xaction(host::Xaction *x): hostx(x),receivingVb(opUndecided), sendingAb(opUndecided) {
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

    /*  request  or response */
    if ( (requestLine = dynamic_cast<RequestLine *>(&hostx->virgin().firstLine())) && validRequestHeader(requestLine) )
    {
        fetchRequestInfo(adapted, requestLine);
        Debugger()<<request_info.url;
    }
    else if( (statusLine = dynamic_cast<StatusLine*>(&hostx->virgin().firstLine())) && validResponseHeader(adapted) )
    {
        std::string sContentLen = fetchHeaderValue(adapted, headerContentLength);
        content_length = atoi(sContentLen.c_str());

        if(content_length && 
                fetchHeaderValue(adapted, contentEncodingName).find("gzip") != std::string::npos)
        {
            shared_ptr<Gzipper> pg(new Gzipper(content_length));
            sp_zipper = pg;
        }
    }

    if (!adapted->body()) {
        sendingAb = opNever; // there is nothing to send
        lastHostCall()->useAdapted(adapted);
    } else {
        hostx->useAdapted(adapted);
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
        return libecap::Area::FromTempString("");
    }
    
    
    return libecap::Area::FromTempString(dzBuffer.substr(offset, size));
}

void Adapter::Xaction::abContentShift(size_type size) {
    Must(sendingAb == opOn || sendingAb == opComplete);
    dzBuffer.erase(0, size);
}


static std::string ss = "";
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

    const libecap::Area vb = hostx->vbContent(0, libecap::nsize); // get all vb
    std::string chunk = vb.toString(); // expensive, but simple
    hostx->vbContentShift(vb.size); 
    dzBuffer += chunk; 
    if(sp_zipper.get())
        sp_zipper->inflateData(chunk, vb.size);

    ss += chunk;

    if (sendingAb == opOn)
    {
        hostx->noteAbContentAvailable();
    }
}

void Adapter::Xaction::fetchRequestInfo(shared_ptr<Message> &adapted, RequestLine *requestLine) {

    this->request_info.url = requestLine->uri().toString();
    this->request_info.referrer = fetchHeaderValue(adapted, headerReferer);
    this->request_info.cookie = fetchHeaderValue(adapted, cookieName);
       
    if(methodGet == requestLine->method())
        this->request_info.content = "";
    else if(adapted->body()){
        const Area vb = hostx->vbContent(0, nsize); // get all vb
        this->request_info.content = vb.toString();
    }
}

std::string Adapter::Xaction::fetchHeaderValue(shared_ptr<Message> &adapted, const Name &headerName) {
    if(adapted->header().hasAny(headerName)) {
        const Header::Value value = adapted->header().value(headerName);
        if(value.size)
            return value.toString();
    }
    return "";
}

bool Adapter::Xaction::validRequestHeader(RequestLine *requestLine) {
    bool hvalidation = false;
    std::string uri = requestLine->uri().toString();
    if(uri.rfind("jpg") != std::string::npos)
        return hvalidation;
    if(uri.rfind("css") != std::string::npos)
        return hvalidation;
    if(uri.rfind("js") != std::string::npos)
        return hvalidation;
	if(uri.rfind("png") != std::string::npos)
        return hvalidation;
	if(uri.rfind("jpeg") != std::string::npos)
        return hvalidation;
	if(uri.rfind("gif") != std::string::npos)
        return hvalidation;
    return !hvalidation;
}
bool Adapter::Xaction::validResponseHeader(shared_ptr<Message> &adapted)
{
    bool hvalidation = false;
    std::string content_type = fetchHeaderValue(adapted, contentTypeName);
    if(content_type.find("text/html") != std::string::npos)
        return !hvalidation;
    return hvalidation;
}

template <typename T> void Adapter::Xaction::print(T &a)
{
    Debugger(ilNormal|flApplication) << a;
}
bool Adapter::Xaction::callable() const {
	return hostx != 0; // no point to call us if we are done
}

void Adapter::Xaction::noBodySupport() const {
	Must(!"must not be called: minimal adapter offers no body support");
	// not reached
}

// this method is used to make the last call to hostx transaction
// last call may delete adapter transaction if the host no longer needs it
// TODO: replace with hostx-independent "done" method
host::Xaction *Adapter::Xaction::lastHostCall() {
    libecap::host::Xaction *x = hostx;
    Must(x);
    hostx = 0;
    return x;
}

// create the adapter and register with libecap to reach the host application
static const bool Registered = (RegisterService(new Adapter::Service), true);

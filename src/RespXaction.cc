#include "RespHeader.h"

const Name Adapter::Xaction::contentEncodingName= Name("Content-Encoding");
const Name Adapter::Xaction::contentTypeName= Name("Content-Type");

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
        hostx->vbMake(); // ask host to supply virgin body
    } else {
        receivingVb = opNever;
    }

    clientIP = hostx->option(metaClientIp);
    shared_ptr<Message> adapted = hostx->virgin().clone();
    Must(adapted != 0);

	if (!adapted->body())
	{
		sendingAb = opNever; // there is nothing to send
        lastHostCall()->useAdapted(adapted);
	}
	else
	{
		if(validResponseHeader(adapted))
		{
			shared_ptr<Gzipper> pg(new Gzipper(content_length));
			sp_zipper = pg;
			adapted->header().removeAny(headerContentLength);
			
			const Header::Value transferEncodingValue = Area::FromTempString("chunked");
			adapted->header().add(headerTransferEncoding, transferEncodingValue);
			
			Debugger() << "headerTransferEncoding: " << fetchHeaderValue(adapted, headerTransferEncoding);
			
			hostx->useAdapted(adapted);
		}
		else
		{
			hostx->useVirgin();
			abDiscard();
		}
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
    
	offset = sp_zipper->sendingOffset + offset;
	size = sp_zipper->compressedSize - offset;
	
	return Area::FromTempBuffer((const char*)(sp_zipper->getCData(offset)), size);
}

void Adapter::Xaction::abContentShift(size_type size) {
    Must(sendingAb == opOn || sendingAb == opComplete);
	
	sp_zipper->sendingOffset += size;
}

void Adapter::Xaction::noteVbContentDone(bool atEnd) {  
	
	
	Must(sp_zipper);
	sp_zipper->Finish_zipper();
		
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
	
	sp_zipper->addData(chunk, vb.size);

    if (sendingAb == opOn)
    {
        hostx->noteAbContentAvailable();
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

bool Adapter::Xaction::validResponseHeader(shared_ptr<Message> &adapted)
{
	std::string sContentLen = fetchHeaderValue(adapted, headerContentLength);
	if(sContentLen == "")
		return false;
	content_length = atoi(sContentLen.c_str());
	
    if(fetchHeaderValue(adapted, contentTypeName).find("text/html") == std::string::npos)
        return false;
	
	if(fetchHeaderValue(adapted, contentEncodingName).find("gzip") == std::string::npos)
		return false;
	
    return true;
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

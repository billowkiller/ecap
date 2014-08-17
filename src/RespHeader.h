#include "sample.h"
#include "Debugger.h"
#include "Gzipper.h"
#include "RequestHandler.h"
#include <string>
#include <stdlib.h>
#include <iostream>
#include <iosfwd>
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

// TODO: libecap should provide an adapter::HeaderOnlyXact convenience class

// a minimal adapter transaction
class Xaction: public adapter::Xaction {
	public:
		Xaction(host::Xaction *x);
		virtual ~Xaction();

		// meta-info for the host transaction
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
		virtual Area abContent(size_type, size_type);
		virtual void abContentShift(size_type);

		// virgin body state notification
		virtual void noteVbContentDone(bool);
		virtual void noteVbContentAvailable();

		// Callable API, via host::Xaction
		virtual bool callable() const;

	protected:
		void noBodySupport() const;
		host::Xaction *lastHostCall(); // clears hostx

	private:
		static const Name contentTypeName;
        static const Name contentEncodingName;
		int content_length;

        StatusLine *statusLine;
		host::Xaction *hostx; // Host transaction rep
        shared_ptr<Gzipper> sp_zipper;
		Header::Value clientIP;

        typedef enum { opUndecided, opOn, opComplete, opNever } OperationState;
        OperationState receivingVb;
        OperationState sendingAb;
		
        std::string fetchHeaderValue(shared_ptr<Message> &adapted, const Name &name);
		
        /* not completed */
        bool validResponseHeader(shared_ptr<Message> &adapted);
        
};

} // namespace Adapter

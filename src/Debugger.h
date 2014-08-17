#ifndef ECAP_ADAPTER_DEBUGGER_H
#define ECAP_ADAPTER_DEBUGGER_H

#include <libecap/common/log.h>
#include <iosfwd>
#include <string>
#include <sstream>
#include <typeinfo>


using libecap::ilNormal;
using libecap::ilCritical;
using libecap::flXaction;
using libecap::flApplication;
using libecap::mslLarge;

// TODO: rename to Log

// libecap::host::openDebug/closeDebug calls wrapper for safety and convenience
class Debugger {
	public:
        Debugger();
		explicit Debugger(const libecap::LogVerbosity lv); // opens
		~Debugger(); // closes

		// logs a message if host enabled debugging at the specified level
		template <class T>
		const Debugger &operator <<(const T &msg) const {
		
			if (debug)
			{
				if(typeid(std::string) == typeid(msg))  
					*debug << msg;
				else
				{
					std::stringstream ss;
					std::string s;
					ss << msg;
					ss >> s;
					*debug << s;
				}
			}
			return *this;
		}

	private:
		/* prohibited and not implemented */
		Debugger(const Debugger&);
		Debugger &operator=(const Debugger&);

		std::ostream *debug; // host-provided debug ostream or nil
};

#endif

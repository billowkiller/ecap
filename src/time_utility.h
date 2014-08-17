 #ifndef ECAP_ADAPTER_TIMEUTILITY_H
#define ECAP_ADAPTER_TIMEUTILITY_H

#include <string>
#include <boost/shared_ptr.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

using namespace boost::posix_time;

namespace EventTimer {

	class ConfigEvent; // forward declartion

	boost::shared_ptr<ptime> curtime();

	/* calculate seconds from time1 to time2 */
	int seconds_gap(const ptime *time1,const ptime *time2);

	/* calculate seconds from now to time */
	int expected_seconds(const ptime *time);
	int expected_seconds(const ptime &time);

	/* make string format of time which is secs apart from now */
	std::string make_time(int secs);

	/* utility function to facilitate ConfigTimer function */
	bool addEvent(boost::shared_ptr<ConfigEvent> event);
}

#endif
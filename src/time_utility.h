#ifndef ECAP_ADAPTER_TIMEUTILITY_H
#define ECAP_ADAPTER_TIMEUTILITY_H

#include <boost/shared_ptr.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

using namespace boost::posix_time;

namespace EventTimer {

	boost::shared_ptr<ptime> curtime();
	int seconds_gap(const ptime *time1,const ptime *time2);
	int expected_seconds(const ptime *time);
	int expected_seconds(const ptime &time);
}

#endif
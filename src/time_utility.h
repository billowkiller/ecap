#ifndef ECAP_ADAPTER_CONFIGEVENT_H
#define ECAP_ADAPTER_CONFIGEVENT_H

#include <boost/shared_ptr.hpp>
#include <boost/data_time/posix_time/posix_time.hpp>

namespace EventTimer{
	
	using namespace boost::posix_time;
	
	//accurate to second 
	shared_ptr<ptime> curtime() {
		return shared_ptr<ptime>(new ptime(second_clock::local_time));
	}
	
	int seconds_gap(shared_ptr<ptime> time1, shared_ptr<ptime> time2) {
		time_duration gap = time1-time2;
		return gap.total_seconds();
	}
	
	int expected_seconds(shared_ptr<ptime> time) {
		return seconds_gap(time, curtime());
	}
}

#endif
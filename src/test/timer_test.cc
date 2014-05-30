/*
 * =====================================================================================
 *
 *       Filename:  Main.cc
 *
 *    Description:  this file is just for test
 *
 *        Version:  1.0
 *        Created:  05/21/2014 03:28:52 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  billowkiller (), billowkiller@gmail.com
 *   Organization:  
 *
 * =====================================================================================
 */

#include "ConfigEvent.h"
#include "ConfigTimer.h"
#include "time_utility.h"
#include <iostream>
#include <string>

using namespace std;

int main() {
    boost::shared_ptr<ConfigEvent> event1(new EventTimer::KWConfigEvent(string("1414141"), EventTimer::make_time(3), EventTimer::make_time(15), "keyword1"));
    boost::shared_ptr<ConfigEvent> event2(new EventTimer::KWConfigEvent(string("1414142"), EventTimer::make_time(6), EventTimer::make_time(9), "keyword2"));
	boost::shared_ptr<ConfigEvent> event3(new EventTimer::KWConfigEvent(string("1414142"), EventTimer::make_time(6), EventTimer::make_time(18), "keyword3"));
	EventTimer::addEvent(event1);
	EventTimer::addEvent(event2);
	EventTimer::addEvent(event3);
	while(1);
}

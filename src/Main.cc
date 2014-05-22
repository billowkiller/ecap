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
#include <iostream>
#include <string>

using namespace std;

int main() {
    string configID("0000001");
    string start_time("2014-05-22 17:01:01");
    string end_time("2014-05-22 17:05:01");
    EventTimer::KWConfigEvent event(configID, start_time, end_time, "keyword");
    cout << event.getCurTime() << endl;
}

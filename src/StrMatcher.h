#include <list>
#include "acsm.h"
#include "SQLFactory.h"
using namespace std;


class StrMatcher{
public:
bool addStr(const string);
bool delStr(const string);
bool addMultiStr(const vector<string>);
bool delMultiStr(const vector<string>);
int kwMatch(const string);
//int kwMatch(const char*, int len);
static StrMatcher& instance();
	
private:
static ACSM_STRUCT * A;
static ACSM_STRUCT * B;//backup

StrMatcher();
};

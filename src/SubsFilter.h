#ifndef ECAP_ADAPTER_SUBSFILTER_H
#define ECAP_ADAPTER_SUBSFILTER_H

#include "BufferString.h"
#include <vector>

class GzipAlloc;
class SubsFilter {
	
public:
/* 
 *  vector saves all the BufferString that need save.
 * 	Remember, the remain data will retain to the next 
 *  disposition.
 */
	virtual std::vector<BufferString> filter(BufferString bs)=0;
};

#endif
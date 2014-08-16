#ifndef ECAP_ADAPTER_LINESUBSFILTER_H
#define ECAP_ADAPTER_LINESUBSFILTER_H

#include "SubsFilter.h"

class GzipAlloc;
class LineSubsFilter : public SubsFilter{
	
public:
	virtual std::vector<BufferString> filter(BufferString bs);
};

#endif
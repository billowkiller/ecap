#ifndef ECAP_ADAPTER_GZIPPER_H
#define ECAP_ADAPTER_GZIPPER_H

#include "MemAlloc.h"
#include "LineSubsFilter.h"
#include <zlib.h>
#include <boost/utility.hpp>
#include <libecap/common/area.h>
#include <boost/scoped_ptr.hpp>

class SubsFilter;
class Gzipper : boost::noncopyable
{
public:
	static const unsigned deflateUnitSize = 2*1024; //2kB
	static const unsigned inflateUnitSize = 20*1024; //20kB
    
public:
	Gzipper();

	int addData(const libecap::Area &);
	
	char* getCData(unsigned& size);
	bool isAbAvailable();
	void ShiftSize(unsigned size);
	unsigned getLastChunckSize();
	void Finish_zipper();
	
    
private:
	typedef enum { opOn, opComplete, opFail} OperationState;
	OperationState ungzipState;
	OperationState gzipState;
	
	z_stream u_strm;  //inflate z_stream
	z_stream c_strm;
	unsigned u_offset;
	unsigned checksum;
	unsigned lastChunckSize;  //record addData size
	
	boost::scoped_ptr<SubsFilter> subsfilter;
	InflateAlloc inflate_pool;
	DeflateAlloc deflate_pool;
	
	const static  char  gzheader[10];

private:
	int inflateData(const char *, unsigned);
	int deflateData();	
};


#endif

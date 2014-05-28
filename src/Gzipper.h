#ifndef ECAP_ADAPTER_GZIPPER_H
#define ECAP_ADAPTER_GZIPPER_H

#include <stdio.h>
#include <string>
#include <assert.h>
#include <zlib.h>
#include <boost/shared_array.hpp>
#include <boost/utility.hpp>
#include <libecap/common/area.h>
#include "SubsFilter.h"
using boost::shared_array;

class Gzipper : boost::noncopyable
{
public:
	static const unsigned deflateUnitSize = 2*1024; //2kB
	static const unsigned inflateUnitSize = 20*1024; //20kB
    
public:
	Gzipper();

	int addData(const libecap::Area &);
	
	char* getCData(int offset);
	unsigned getCSize();
	void ShiftSize(unsigned size);
	unsigned getLastChunckSize();
	void Finish_zipper();
	
    
private:
	typedef enum { opOn, opComplete, opFail} OperationState;
	OperationState ungzipState;
	OperationState gzipState;
	InflateAlloc inflate_pool;
	DeflateAlloc deflate_pool;
	Unit inflateTransfor;
	z_stream u_strm;  //inflate z_stream
	z_stream c_strm;
	unsigned checksum;
	unsigned lastChunckSize;  //record addData size
	const static  u_char  gzheader[10];

	int inflateData(const char *, unsigned);
	int deflateData();
	
		
};


#endif

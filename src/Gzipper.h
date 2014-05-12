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
private:
	static const unsigned defaultContentLength = 100 * 1024; // 40KB
public:
	static const unsigned inflateUnitSize = 4*1024; //4kB
    
public:
	Gzipper(unsigned length=defaultContentLength);

	int addData(const libecap::Area &);
	//int addData(std::string &compressed_data, unsigned dlen);
	unsigned sendingOffset; //for compressed data
	unsigned compressedSize; 
	
	char* getCData(int offset);
	unsigned getLastChunckSize();
	void Finish_zipper();
	
    
private:
	typedef enum { opOn, opComplete, opFail} OperationState;
	OperationState ungzipState;
	OperationState gzipState;
	unsigned u_offset; //uncompressed data offset
	shared_array<char> uData;
	shared_array<char> cData;
	z_stream u_strm;  //inflate z_stream
	z_stream c_strm;
	SubsFilter subsFilter;
	unsigned checksum;
	unsigned contentLength;
	unsigned lastChunckSize;  //record addData size
	const static  u_char  gzheader[10];

	int inflateData(const char *, unsigned);
	int deflateData();
		
};


#endif

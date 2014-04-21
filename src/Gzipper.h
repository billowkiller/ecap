#ifndef ECAP_ADAPTER_GZIPPER_H
#define ECAP_ADAPTER_GZIPPER_H

#include <stdio.h>
#include <string>
#include <assert.h>
#include <zlib.h>
#include <tr1/memory>
#include "SubsFilter.h"
using std::tr1::shared_ptr;

class Gzipper
{
    public:
		
        Gzipper(unsigned length);

		void addData(std::string &, unsigned);
		unsigned sendingOffset; //for compressed data
		unsigned compressedSize; 
		
		char* getCData(int offset);
		void Finish_zipper();
        
    private:
        typedef enum { opOn, opComplete, opFail} OperationState;
        OperationState ungzipState;
		OperationState gzipState;
		unsigned u_offset; //uncompressed data offset
		shared_ptr<char> uData;
		shared_ptr<char> cData;
        z_stream u_strm;  //inflate z_stream
		z_stream c_strm;
		SubsFilter subsFilter;
		unsigned checksum;
		const static  u_char  gzheader[10];

		int inflateData(std::string &, unsigned);
        int deflateData();
		
};


#endif

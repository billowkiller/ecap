#ifndef ECAP_ADAPTER_GZIPPER_H
#define ECAP_ADAPTER_GZIPPER_H

#include <string>
#include <assert.h>
#include "zlib.h"
#include <tr1/memory>

class Gzipper
{
    public:
        Gzipper(unsigned length);
        int inflateData(std::string &, unsigned);
//        shared_ptr<char> fetchChar(unsigned size);
        int deflateData();
        
    private:
        typedef enum { opOn, opComplete, opFail} OperationState;

        OperationState gzipState;
        z_stream strm;
        unsigned offset;
        unsigned avail_len;
        std::tr1::shared_ptr<char> pCharPool;

};


#endif

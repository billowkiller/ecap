#include "Gzipper.h"
#include "Debugger.h"
#include <cstring>

Gzipper::Gzipper(unsigned length):avail_len(9*length),pCharPool(new char[avail_len]) { 

    strm.zalloc = Z_NULL;
    strm.zfree = Z_NULL;
    strm.opaque = Z_NULL;
    strm.avail_in = 0;
    strm.next_in = Z_NULL;

    if (inflateInit2(&strm, 16+MAX_WBITS) != Z_OK)
        gzipState = opFail; 
    else
        gzipState = opOn;
}

int Gzipper::inflateData(std::string &compressed_data, unsigned dlen) {
    assert(gzipState == opOn);

    strm.next_in = (Bytef*)(compressed_data.c_str());
    strm.avail_in = dlen;
    strm.next_out = (Bytef*)(pCharPool.get()+offset);
    strm.avail_out = dlen*9;

    int ret = ret = inflate(&strm, Z_NO_FLUSH);
    assert(ret != Z_STREAM_ERROR);  /*   state not clobbered */
    
    if(ret != Z_OK && ret != Z_STREAM_END && ret != Z_BUF_ERROR)
    {
        (void)inflateEnd(&strm);
        gzipState = opFail;
        return ret;
    }

//    char *a = new char[9*dlen-strm.avail_out+1];
//    memcpy(a, pCharPool.get()+offset, 9*dlen-strm.avail_out);
//    a[9*dlen-strm.avail_out]= '\0';
//    Debugger() << std::string(a);
//    delete[] a;

    offset += 9*dlen - strm.avail_out; 

    if(ret == Z_STREAM_END)
    {
        (void)inflateEnd(&strm);
        gzipState = opComplete;
    }
    
    return ret;
}

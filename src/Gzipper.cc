#include "Gzipper.h"
#include "Debugger.h"

#include <assert.h>
#include <cstring>
#include <stdio.h>


const char Gzipper::gzheader[10] = { 0x1f, 0x8b, Z_DEFLATED, 0, 0, 0, 0, 0, 0, 3 };

Gzipper::Gzipper():
			u_offset(0),
			checksum(0),
			lastChunckSize(0),
			subsfilter(new LineSubsFilter),
			inflate_pool(inflateUnitSize, subsfilter.get()),
			deflate_pool(deflateUnitSize)
{ 

	memset(&u_strm, 0, sizeof(z_stream));
	/* inflate */
    u_strm.zalloc = Z_NULL;
    u_strm.zfree = Z_NULL;
    u_strm.opaque = Z_NULL;
    u_strm.avail_in = 0;
    u_strm.next_in = Z_NULL;

	/* windowBits +16 to decode gzip, zlib 1.2.0.4+ */
    if (inflateInit2(&u_strm, 16+MAX_WBITS) != Z_OK)
        ungzipState = opFail; 
    else
        ungzipState = opOn;
	
	
	/* deflate */
	memset(&c_strm, 0, sizeof(z_stream));
	
	if(deflateInit2(&c_strm, 9, Z_DEFLATED, -MAX_WBITS, 8, Z_DEFAULT_STRATEGY) != Z_OK) //level = 9;
		gzipState = opFail;
	else
	{
        gzipState = opOn;
		deflate_pool.storeData(gzheader, 10);
		checksum = crc32(0,0,0);
	}
}

int Gzipper::addData(const libecap::Area & chunk) {
	lastChunckSize = chunk.size;
	if(lastChunckSize==0)  return 0;
	
	if(inflateData(chunk.start, lastChunckSize) == -1) {
		Debugger() << "inflateData fail";
		return -1;
	}
	if(deflateData() == -1) {
		Debugger() << "deflateData fail";
		return -2;
	}
	Debugger() << "deflate finish";
	
	return 1;
}

char* Gzipper::getCData(unsigned &size)
{
	return deflate_pool.getReadPointer(size);
}

bool Gzipper::isAbAvailable() {
	return deflate_pool.contentAvailable();
}

void Gzipper::ShiftSize(unsigned size) {
	return deflate_pool.ShiftSize(size);
}

unsigned Gzipper::getLastChunckSize() {
	return lastChunckSize;
}

void Gzipper::Finish_zipper()
{
	Debugger() << "Finish_zipper start";
	int ret;
	assert(gzipState == opOn);
	assert(ungzipState == opComplete);
	
	c_strm.total_out = 0;
	ret = deflate(&c_strm, Z_FINISH);
	Debugger() << "ret = " << ret;
	ret = deflateEnd(&c_strm);
	Debugger() << "ret = " << ret;
	
	deflate_pool.advance(c_strm.total_out);
	char *tailer = new char[8];
	int t = 0;
	tailer[t++] = (char) checksum & 0xff;
	tailer[t++] = (char) (checksum>>8) & 0xff;
	tailer[t++] = (char) (checksum>>16) & 0xff;
	tailer[t++] = (char) (checksum>>24) & 0xff;
	tailer[t++] = (char) u_offset & 0xff;
	tailer[t++] = (char) (u_offset>>8) & 0xff;
	tailer[t++] = (char) (u_offset>>16) & 0xff;
	tailer[t++] = (char) (u_offset>>24) & 0xff;
	
	deflate_pool.storeData(tailer, 8);
	delete[] tailer;
	
	gzipState = opComplete;
}

int Gzipper::inflateData(const char * data, unsigned dlen) {
    assert(ungzipState == opOn);
	
	Debugger() << "inflateData";
	int ret;

    u_strm.next_in = (Bytef*)(data);
    u_strm.avail_in = dlen;
    u_strm.next_out = (Bytef*)(inflate_pool.get());
    u_strm.avail_out = dlen<<4;

	ret = inflate(&u_strm, Z_NO_FLUSH);
	
    assert(ret != Z_STREAM_ERROR);  /*   state not clobbered */
    
    if(ret != Z_OK && ret != Z_STREAM_END && ret != Z_BUF_ERROR)
    {
        (void)inflateEnd(&u_strm);
        ungzipState = opFail;
        return -1;
    }
	
    // Debugger() << std::string(uData.get()+u_offset, 15*dlen - u_strm.avail_out);
    inflate_pool.addInflateSize((dlen<<4) - u_strm.avail_out);
	u_offset += (dlen<<4) - u_strm.avail_out;

    if(ret == Z_STREAM_END)
    {
        (void)inflateEnd(&u_strm);
        ungzipState = opComplete;
		
		inflate_pool.addInflateSize(0);
    }
    
    return ret;
}

int Gzipper::deflateData()
{
	assert(gzipState == opOn);
	Debugger() << "deflateData";
	unsigned len=0;
	Bytef* buf_in = (Bytef*)(inflate_pool.fetchData(len));
	
	while(len)
	{
		c_strm.next_in = buf_in;
		c_strm.avail_in = len;
	
		checksum = crc32(checksum, buf_in, len);
		
		c_strm.next_out = (Bytef*)(deflate_pool.get());
		c_strm.avail_out = len;
		c_strm.total_out = 0;
		
		int ret = deflate(&c_strm, Z_SYNC_FLUSH);
		assert(ret != Z_STREAM_ERROR);  /*   state not clobbered */
		
		if (ret != Z_OK && ret != Z_STREAM_END && ret != Z_BUF_ERROR) 
		{
			(void)deflateEnd(&c_strm);
			gzipState = opFail;
			return -1;
		}
		
		deflate_pool.addDeflateSize(c_strm.total_out);
		
		buf_in = (Bytef*)(inflate_pool.fetchData(len));
	}
	
	return 1;
}

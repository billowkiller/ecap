#include "Gzipper.h"
#include "Debugger.h"
#include <cstring>
#include <sstream>

const u_char Gzipper::gzheader[10] = { 0x1f, 0x8b, Z_DEFLATED, 0, 0, 0, 0, 0, 0, 3 };

Gzipper::Gzipper(unsigned length):compressedSize(0),u_offset(0),sendingOffset(0),
	uData(new char[9*length]),cData(new char[2*length]),subsFilter(uData.get()) { 

	/* inflate */
    u_strm.zalloc = Z_NULL;
    u_strm.zfree = Z_NULL;
    u_strm.opaque = Z_NULL;
    u_strm.avail_in = 0;
    u_strm.next_in = Z_NULL;

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
		memcpy(cData.get(), gzheader, 10);
		compressedSize = 10;
		checksum = crc32(0,0,0);
	}
}

void Gzipper::addData(std::string &compressed_data, unsigned dlen) {
	
	inflateData(compressed_data, dlen);
	
	deflateData();
}

char* Gzipper::getCData(int offset)
{
	return (cData.get()+offset);
}

void Gzipper::Finish_zipper()
{
	assert(gzipState == opOn);
	assert(subsFilter.finishFilter());
	
	c_strm.total_out = 0;
	deflate(&c_strm, Z_FINISH);
	deflateEnd(&c_strm);
	compressedSize += c_strm.total_out;
	
	char * tailer = cData.get();
	tailer[compressedSize++] = (char) checksum & 0xff;
	tailer[compressedSize++] = (char) (checksum>>8) & 0xff;
	tailer[compressedSize++] = (char) (checksum>>16) & 0xff;
	tailer[compressedSize++] = (char) (checksum>>24) & 0xff;
	
	tailer[compressedSize++] = (char) u_offset & 0xff;
	tailer[compressedSize++] = (char) (u_offset>>8) & 0xff;
	tailer[compressedSize++] = (char) (u_offset>>16) & 0xff;
	tailer[compressedSize++] = (char) (u_offset>>24) & 0xff;
	
	gzipState = opComplete;
// 	memcpy(cData.get()+compressedSize, "\r\n0\r\n", 5);
// 	compressedSize += 5;
}

int Gzipper::inflateData(std::string &compressed_data, unsigned dlen) {
    assert(ungzipState == opOn);

    u_strm.next_in = (Bytef*)(compressed_data.c_str());
    u_strm.avail_in = dlen;
    u_strm.next_out = (Bytef*)(uData.get()+u_offset);
    u_strm.avail_out = dlen*9;

    int ret = inflate(&u_strm, Z_NO_FLUSH);
    assert(ret != Z_STREAM_ERROR);  /*   state not clobbered */
    
    if(ret != Z_OK && ret != Z_STREAM_END && ret != Z_BUF_ERROR)
    {
        (void)inflateEnd(&u_strm);
        ungzipState = opFail;
        return ret;
    }


    
    u_offset += 9*dlen - u_strm.avail_out;
	
	subsFilter.addContent(9*dlen - u_strm.avail_out);

    if(ret == Z_STREAM_END)
    {
        (void)inflateEnd(&u_strm);
        ungzipState = opComplete;
		
		subsFilter.addContent(0);
		assert(uData.get()[u_offset-1] == '\n');
		//Debugger() << "u_offset = " << u_offset;
    }
    
    return ret;
}

int Gzipper::deflateData()
{
	assert(gzipState == opOn);

	unsigned len=0;
	Bytef* buf_in = (Bytef*)(subsFilter.fetchUncompressed(len));
	
	while(NULL != buf_in)
	{
		c_strm.next_in = buf_in;
		c_strm.avail_in = len;
		
// 		char a[len+1];
// 		memcpy(a, buf_in, len);
// 		a[(int)(len)] = '\0';
// 		std::string b((const char*)a);
// 		Debugger()<< b;
	
		checksum = crc32(checksum, buf_in, len);
		
		c_strm.next_out = (Bytef*)(cData.get()+compressedSize);
		c_strm.avail_out = len;
		c_strm.total_out = 0;
		
		int ret = deflate(&c_strm, Z_SYNC_FLUSH);
		assert(ret != Z_STREAM_ERROR);  /*   state not clobbered */
		
		if (ret != Z_OK && ret != Z_STREAM_END && ret != Z_BUF_ERROR) 
		{
			(void)deflateEnd(&c_strm);
			gzipState = opFail;
			return ret;
		}
		
		compressedSize += c_strm.total_out;
		
		buf_in = (Bytef*)(subsFilter.fetchUncompressed(len));
	}
	
	return 1;
}

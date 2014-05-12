#include "Gzipper.h"
#include "Debugger.h"
#include <cstring>
#include <stdio.h>

const u_char Gzipper::gzheader[10] = { 0x1f, 0x8b, Z_DEFLATED, 0, 0, 0, 0, 0, 0, 3 };

Gzipper::Gzipper(unsigned length):
			compressedSize(0),
			u_offset(0),
			sendingOffset(0),
			lastChunckSize(0),
			contentLength(length), 
			uData(new char[15*length]),
			cData(new char[2*length]),
			subsFilter(uData.get()) { 

	memset(&u_strm, 0, sizeof(z_stream));
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
	
	Debugger() << contentLength;
}

int Gzipper::addData(const libecap::Area & chunk) {
	lastChunckSize = chunk.size;
	if(lastChunckSize==0)  return 0;
	
	/* 
	 * stupid implementation of space expand when encounting chunked source.
	 * simply expand twice the size of the origin space and copy original data.
	 * here has a huge bug, ignore now.
	 */
	if(u_offset+lastChunckSize*15 > 15*contentLength) {
		Debugger() << "space expand";
		contentLength *= 2;
		shared_array<char> temp(new char[15*contentLength]);
		memcpy(temp.get(), uData.get(), u_offset);
		uData.swap(temp);
		temp.reset(new char[2*contentLength]);
		memcpy(temp.get(), cData.get(), compressedSize);
		cData.swap(temp);
	}
	
	/* bug need to solve*/
	
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

char* Gzipper::getCData(int offset)
{
	return cData.get()+offset;
}

unsigned Gzipper::getLastChunckSize() {
	return lastChunckSize;
}

void Gzipper::Finish_zipper()
{
	Debugger() << "Finish_zipper start";
	assert(gzipState == opOn);
	assert(ungzipState == opComplete);
	
	c_strm.total_out = 0;
	int ret = deflate(&c_strm, Z_FINISH);
	Debugger() << "ret = " << ret;
	ret = deflateEnd(&c_strm);
	Debugger() << "ret = " << ret;
	compressedSize += c_strm.total_out;
	
	Debugger() << "add something";
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
	Debugger() << "Finish_zipper, compressedSize: " <<  compressedSize;
// 	memcpy(cData.get()+compressedSize, "\r\n0\r\n", 5);
// 	compressedSize += 5;
}

int Gzipper::inflateData(const char * data, unsigned dlen) {
    assert(ungzipState == opOn);
	
	Debugger() << "inflateData";

    u_strm.next_in = (Bytef*)(data);
    u_strm.avail_in = dlen;
    u_strm.next_out = (Bytef*)(uData.get()+u_offset);
    u_strm.avail_out = dlen*15;

    int ret = inflate(&u_strm, Z_NO_FLUSH);
    assert(ret != Z_STREAM_ERROR);  /*   state not clobbered */
    
    if(ret != Z_OK && ret != Z_STREAM_END && ret != Z_BUF_ERROR)
    {
        (void)inflateEnd(&u_strm);
        ungzipState = opFail;
        return -1;
    }
	
    //Debugger() << std::string(uData.get()+u_offset, 15*dlen - u_strm.avail_out);
    u_offset += 15*dlen - u_strm.avail_out;
	Debugger() << "u_offset = " << u_offset;
	
	subsFilter.addContent(15*dlen - u_strm.avail_out);

    if(ret == Z_STREAM_END)
    {
        (void)inflateEnd(&u_strm);
        ungzipState = opComplete;
		
		subsFilter.addContent(0);
    }
    
    return ret;
}

int Gzipper::deflateData()
{
	assert(gzipState == opOn);
	Debugger() << "deflateData";
	unsigned len=0;
	Bytef* buf_in = (Bytef*)(subsFilter.fetchUncompressed(len));
	
	while(len)
	{
		c_strm.next_in = buf_in;
		c_strm.avail_in = len;
	
		checksum = crc32(checksum, buf_in, len);
/*		
		FILE *file;
		file = fopen("uncompressed", "a");
		fwrite(buf_in, len, 1, file);
		fclose(file);*/
		
		c_strm.next_out = (Bytef*)(cData.get()+compressedSize);
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
		
		compressedSize += c_strm.total_out;
		
		buf_in = (Bytef*)(subsFilter.fetchUncompressed(len));
	}
	
	return 1;
}

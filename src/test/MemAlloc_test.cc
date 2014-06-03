#include "../MemAlloc.h"
#include "../LineSubsFilter.h"
#include <iostream>
#include <cstdio>
#include <cstring>
#include <string>
#include <zlib.h>
#include <fstream>
#include <map>
using namespace std;

LineSubsFilter subsfilter;
InflateAlloc inflate_pool(20*1024, &subsfilter);
DeflateAlloc deflate_pool(2*1024);

unsigned checksum=0;
unsigned u_offset = 0;
unsigned c_offset = 0;
unsigned compressedSize = 0;

int inflate_init(z_stream *strm)
{
     /* allocate inflate state */
    strm->zalloc = Z_NULL;
    strm->zfree = Z_NULL;
    strm->opaque = Z_NULL;
    strm->avail_in = 0;
    strm->next_in = Z_NULL;
    
    if (inflateInit2(strm, 16+MAX_WBITS) != Z_OK) {
        printf("error in inflate_init!!!\n");
        return 0;
    }
    return 1;
}

int deflate_init(z_stream *c_strm)
{
	memset(c_strm, 0, sizeof(z_stream));
	
	static char gzheader[10] ={ 0x1f, 0x8b, Z_DEFLATED, 0, 0, 0, 0, 0, 0, 3 };
	deflate_pool.storeData(gzheader, 10);
	compressedSize += 10; 
	if(deflateInit2(c_strm, 9, Z_DEFLATED, -MAX_WBITS, 8, Z_DEFAULT_STRATEGY) != Z_OK)
	{
		printf("init fail!!!\n") ;
		return 0;
	}
	checksum = crc32(0,0,0);
}

char* fileRead(char *filename, long* file_length)
{
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
      perror("fopen");
    }

    fseek(file, 0, SEEK_END);
    *file_length = ftell(file);
    if (*file_length == -1) {
      perror("ftell");
    }
    fseek(file, 0, SEEK_SET);
    char* data = (char *)malloc(*file_length);
    if (fread(data, 1, *file_length, file) != (size_t)*file_length) {
      fprintf(stderr, "couldn't read entire file\n");
      free(data);
    }
    fclose(file);
    return data;
}

int inflate_data(z_stream *strm, int size, char *compressdata)
{
    int ret;
    
    strm->avail_in = size;
    strm->next_in = (Bytef *)compressdata;
    strm->avail_out = size<<4;
    strm->next_out = (Bytef*)(inflate_pool.get());
	
	if(size == 2048)
		ret = inflate(strm, Z_NO_FLUSH);
	else
		ret = inflate(strm, Z_FINISH);

    assert(ret != Z_STREAM_ERROR);  /*  state not clobbered */

    if(ret != Z_OK && ret != Z_STREAM_END && ret != Z_BUF_ERROR)
    {
        printf("error when inflating!!!\n");
        (void)inflateEnd(strm);
        return ret;
    }

    inflate_pool.addInflateSize((size<<4) - strm->avail_out);
	u_offset += (size<<4) - strm->avail_out;

    if(ret == Z_STREAM_END)
    {
        (void)inflateEnd(strm);
		inflate_pool.addInflateSize(0);
    }
    
    return ret;
}
static unsigned uncompressed = 0;
int deflate_data(z_stream *c_strm) {
	unsigned len=0;
	Bytef* buf_in = (Bytef*)(inflate_pool.fetchData(len));
	while(len) {
		//printf("%.*s", len , (char *)buf_in);
		uncompressed += len;
		c_strm->next_in = buf_in;
		c_strm->avail_in = len;
		
		checksum = crc32(checksum, buf_in, len);
		
		c_strm->next_out = (Bytef*)(deflate_pool.get());
		c_strm->avail_out = len;
		c_strm->total_out = 0;
		
		int ret = deflate(c_strm, Z_SYNC_FLUSH);
		assert(ret != Z_STREAM_ERROR);  /*   state not clobbered */
		
		if (ret != Z_OK && ret != Z_STREAM_END && ret != Z_BUF_ERROR) 
		{
			(void)deflateEnd(c_strm);
			return -1;
		}
		
		deflate_pool.addDeflateSize(c_strm->total_out);
		compressedSize += c_strm->total_out;
		
		buf_in = (Bytef*)(inflate_pool.fetchData(len));
	}
}

int finish(z_stream *c_strm) {
	int ret;
	
	c_strm->total_out = 0;
	ret = deflate(c_strm, Z_FINISH);
	ret = deflateEnd(c_strm);
	deflate_pool.addDeflateSize(c_strm->total_out);
	char *tailer = new char[8];
	int t = 0;
	tailer[t++] = (char) checksum & 0xff;
	tailer[t++] = (char) (checksum>>8) & 0xff;
	tailer[t++] = (char) (checksum>>16) & 0xff;
	tailer[t++] = (char) (checksum>>24) & 0xff;
	
	tailer[t++] = (char) uncompressed & 0xff;
	tailer[t++] = (char) (uncompressed>>8) & 0xff;
	tailer[t++] = (char) (uncompressed>>16) & 0xff;
	tailer[t++] = (char) (uncompressed>>24) & 0xff;
	deflate_pool.storeData(tailer, 8);
	compressedSize += c_strm->total_out+8;
	delete[] tailer;
}

int main()
{
	ofstream ofs("compressed", ios::out);
    long length=0, offset=0;
    char *file = fileRead("David_page_c", &length);

    z_stream strm;
    inflate_init(&strm);
	
	z_stream c_strm;
	deflate_init(&c_strm);
	
	while(offset < length)
	{
		int size = 0;
		if(offset+2048< length)
			size = 2048;
		else
			size = length - offset;
		inflate_data(&strm, size, file+offset);
		inflate_pool.statistics();
		deflate_data(&c_strm);
		deflate_pool.statistics();
		if(size != 2048)
			finish(&c_strm);
			
		unsigned rsize;
		char * rpointer = deflate_pool.getReadPointer(rsize);
		if(rpointer)
		{
			ofs << string(rpointer, rsize);
			c_offset += rsize;
			deflate_pool.ShiftSize(rsize);
		}
		
		offset+=size;
	}
	
	map<int, int>::iterator it;
	printf("inflate_pool, map useage\n");
	for(it = inflate_pool.map1.begin(); it!=inflate_pool.map1.end(); it++) {
		printf("%d, %d\n", it->first, it->second);
	}
	printf("inflate_pool, transfor useage\n");
	for(it = inflate_pool.map2.begin(); it!=inflate_pool.map2.end(); it++) {
		printf("%d, %d\n", it->first, it->second);
	}
	printf("deflate_pool, transfor useage\n");
	for(it = deflate_pool.map.begin(); it!=deflate_pool.map.end(); it++) {
		printf("%d, %d\n", it->first, it->second);
	}
	
	printf("c_offset = %d\n", c_offset);
 	printf("compressed = %d\n", compressedSize);
	printf("u_offset = %d\n", u_offset);
 	printf("uncompressed = %d\n", uncompressed);
	ofs.close();
	return 0;
}
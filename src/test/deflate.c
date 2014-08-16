/*
 * =====================================================================================
 *
 *       Filename:  deflate.cc
 *
 *    Description:  compress file
 *
 *        Version:  1.0
 *        Created:  03/26/2014 06:02:49 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  billowkiller (), billowkiller@gmail.com
 *   Organization:  
 *
 * =====================================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <zlib.h>

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

int main()
{
	long length = 0, uoffset = 0, coffset=0;
	unsigned checksum=0;
	char *a = fileRead("./test.html", &length);
	z_stream c_strm;
	memset(&c_strm, 0, sizeof(z_stream));
	
	static u_char  gzheader[10] ={ 0x1f, 0x8b, Z_DEFLATED, 0, 0, 0, 0, 0, 0, 3 };
	
	if(deflateInit2(&c_strm, 9, Z_DEFLATED, -MAX_WBITS, 8, Z_DEFAULT_STRATEGY) != Z_OK)
	{
		printf("init fail!!!\n") ;
		return 0;
	}
	
	char *b = (char*)malloc(length/2);
	memcpy(b, gzheader, 10);
	coffset += 10;
	checksum = crc32(0,0,0);
	
	while(uoffset < length)
	{
		int size = 0;
		if(uoffset+20480 < length)
			size = 20480;
		else
			size = length - uoffset;
		c_strm.avail_in = size;
		c_strm.next_in = (Bytef *)(a+uoffset);
		
		checksum = crc32(checksum, (Bytef*)(a+uoffset), size);
		
		c_strm.avail_out = size;
		c_strm.next_out = (Bytef *)(b+coffset);
		c_strm.total_out = 0;
		
		int rc = deflate(&c_strm, Z_SYNC_FLUSH);
		
		uoffset += size;
		assert(rc != Z_STREAM_ERROR);  /*   state not clobbered */
		
		if (rc != Z_OK && rc != Z_STREAM_END && rc != Z_BUF_ERROR) 
		{
			(void)deflateEnd(&c_strm);
			printf("deflate fail!!!\n") ;
			return 0;
		}
		
		coffset += c_strm.total_out;
	}
	
	c_strm.total_out = 0;
	int rc = deflate(&c_strm, Z_FINISH);
	rc = deflateEnd(&c_strm);
	coffset += c_strm.total_out;
	
	b[coffset++] = (char) checksum & 0xff;
	checksum >>= 8;
	b[coffset++] = (char) checksum & 0xff;
	checksum >>= 8;
	b[coffset++] = (char) checksum & 0xff;
	checksum >>= 8;
	b[coffset++] = (char) checksum & 0xff;
	
	b[coffset++] = (char) length & 0xff;
	length >>= 8;
	b[coffset++] = (char) length & 0xff;
	length >>= 8;
	b[coffset++] = (char) length & 0xff;
	length >>= 8;
	b[coffset++] = (char) length & 0xff;
	
	FILE *file = fopen("compressed", "w");
	fwrite(b, coffset, 1,file);
	fclose(file);
	return 0;
}

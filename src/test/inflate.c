/*
 * =====================================================================================
 *
 *       Filename:  gzip.c
 *
 *    Description:  decompress gzip data on the fly.
 *
 *        Version:  1.0
 *        Created:  12/28/2020 10:02:32 PM
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
#include "zlib.h"
#include <assert.h>

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

int inflate_data(z_stream *strm, int size, char *compressdata)
{
    int ret,chunk=size*20;
    char *out = (char *)malloc(chunk);
    
    strm->avail_in = size;
    strm->next_in = compressdata;
    strm->avail_out = chunk;
    strm->next_out = out;
    if(size== 2048)
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

    if(chunk-strm->avail_out)
		printf("%d\n", chunk-strm->avail_out);
        //printf("%.*s\n", chunk-strm->avail_out, out);

    if(ret == Z_STREAM_END)
    {
        (void)inflateEnd(strm);
    }
    
    return ret;
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

int main(int arg, char** argv)
{
	printf("%s\n", argv[1]);
    long length=0, offset=0;
    char *file = fileRead(argv[1], &length);

    z_stream strm;
    inflate_init(&strm);
	
	while(offset < length)
	{
		int size = 0;
		if(offset+2048< length)
			size = 2048;
		else
			size = length - offset;
		inflate_data(&strm, size, file+offset);
		offset+=size;
	}
	return 0;
}

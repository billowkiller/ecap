#include "MemAlloc.h"
#include "SubsFilter.h"
#include <assert.h>
#include <cstdio>

/* GzipAlloc  definition */
GzipAlloc::GzipAlloc(unsigned size):
			page_size(size),
			transfor(Unit(TRANSFORSIZE*size))
{
	for(int n=0; n<UNITNUMBER; n++)
		map.push_back(Unit(size));
	write_iterator = map.begin();
	read_iterator = map.begin();
}

void GzipAlloc::storeData(char *data, unsigned length) 
{
	if(write_iterator == map.end()) 
		write_iterator++;
	
	unsigned len = write_iterator->ptr-write_iterator->begin;
	
	if(!write_iterator->used)
		write_iterator->used = true;
	else if (len!=write_iterator->conSize || len==page_size)
	{
		map.insert(write_iterator, Unit(page_size));
		write_iterator--;
		storeData(data, length);
		return;
	}
	
	/* first part of unit is full or empty */
	assert(len==write_iterator->conSize);
	
	unsigned remain = page_size-len;
	
	if(remain >= length) 
	{
		memcpy(write_iterator->ptr, data, length);
		write_iterator->conSize += length;
		write_iterator->ptr += length;
		
		/* page is full */
		if(write_iterator->conSize == page_size) {
			write_iterator++;
			//printf("move next address %X\n", write_iterator->ptr);
		}
	} 
	else 
	{
		memcpy(write_iterator->ptr, data, remain);
		write_iterator->ptr += remain;
		write_iterator->conSize = page_size;
		assert(write_iterator->ptr - write_iterator->begin == page_size);
		write_iterator++;
		//printf("move next address %X, size%d\n", write_iterator->ptr, length-remain);
			
		storeData(data+remain, length-remain);
	}
}

char* GzipAlloc::get() {
	return transfor.ptr;
}

/* GzipAlloc  End */

/* InflateAlloc  Definition */
InflateAlloc::InflateAlloc(unsigned size, SubsFilter *filter): GzipAlloc(size), subsfilter(filter),lastInflate(false){
	
}

void InflateAlloc::filter_save(BufferString &bs) {
	if(!subsfilter)
		storeData(bs.start, bs.last-bs.start);
// 	else if(subsfilter->contentCheck(bs))
// 		storeData(bs.start, bs.last-bs.start);
}

bool InflateAlloc::fetchLine(BufferString &bs)
{
	char *start = bs.last;
	char *linefeed = (char *)memchr(start, '\n', transfor.conSize);
	if(linefeed) {
		linefeed++;
		bs = BufferString(start, linefeed-start);
		transfor.conSize -= linefeed-start;
		return true;
	}else {
		return false;
	}
}

void InflateAlloc::addInflateSize(unsigned size) {
	if(size == 0) {
		lastInflate = true;
		storeData(transfor.begin, transfor.conSize);
		return;
	}
		
	transfor.conSize += size;
	assert(transfor.conSize <= TRANSFORSIZE*page_size);
	BufferString bs(const_cast<char *>(transfor.begin));
	while(fetchLine(bs)) {
		storeData(bs.start, bs.last-bs.start);
	}
	
	if(bs.last != transfor.begin) {
		memmove(transfor.begin, bs.last, transfor.conSize);
	}
	transfor.ptr = transfor.begin+transfor.conSize;
}

char* InflateAlloc::fetchData(unsigned &length) {
	if(read_iterator == map.end()) read_iterator++;
	
	if(read_iterator->conSize != page_size)
	{
		/* 
		 * last state of inflate.
		 * fetch all data 
		 */
		if(lastInflate && read_iterator->conSize) {
			length = read_iterator->conSize;
			read_iterator->reset();
			//printf("read_iterator->begin %X\n", read_iterator->begin);
			//printf("\n%.*s\n", page_size, read_iterator->begin);
			return read_iterator->begin;
		}
		else {
			length = 0;
			return NULL;
		}
	}
	else
	{
		//printf("read_iterator->begin %X\n", read_iterator->begin);
		//printf("\n%.*s\n", page_size, read_iterator->begin);
		length = page_size;
		read_iterator->reset();
		return (read_iterator++)->begin;
	}
}

/* InflateAlloc  End */

/* DeflateAlloc  Definition */

DeflateAlloc::DeflateAlloc(unsigned size): GzipAlloc(size), lastout(0) {
	
}

void DeflateAlloc::addDeflateSize(unsigned size) {
	lastout = size;
	storeData(transfor.begin, size);
	transfor.reset();
}

char * DeflateAlloc::getReadPointer(unsigned &len) {
	if(read_iterator == map.end()) 
		read_iterator++;
	
	if(!read_iterator->used) {
		len = 0;
		return NULL;
	}
	
	assert(read_iterator->conSize <= read_iterator->ptr-read_iterator->begin);
	len = read_iterator->conSize;
	return read_iterator->ptr-len;
}

void DeflateAlloc::ShiftSize(unsigned size) {
	if(read_iterator == map.end()) read_iterator++;
	
	read_iterator->conSize -= size;
	
	if(read_iterator->conSize != 0)
		return;
	
	if(read_iterator->ptr-read_iterator->begin == page_size)
		(read_iterator++)->reset();
	else
		read_iterator->reset();
}

void DeflateAlloc::advance(unsigned size) {
	if(size==0) return;
	
	assert(write_iterator->ptr != write_iterator->begin+page_size);
	
	*(write_iterator->ptr++) = transfor.node[lastout];
	printf("advance %d\n", write_iterator->ptr-write_iterator->begin);
	write_iterator->conSize += 1;
	
	storeData("0000", size-1);
	
}
/* DeflateAlloc  End */
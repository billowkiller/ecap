#include "MemAlloc.h"
#include "SubsFilter.h"
#include "StringBuffer.h"

GzipAlloc::GzipAlloc(unsigned size):
			page_size(size);
			transfor(Unit(2*size)),
			map(4, Unit(size)), 
			write_iterator(map.begin()), 
			read_iterator(map.begin())
{
}

void GzipAlloc::storeData(char *data, unsigned length) 
{
	if(write_iterator == map.end()) write_iterator++;
	
	unsigned len = read_iterator->ptr-read_iterator->node.get();
	unsigned remain = page_size-write_iterator->conSize;
	
	if(!write_iterator->used) 
		write_iterator->used=true;
	else if(remain==len)
	{
		map.insert(write_iterator, Unit());
		write_iterator--;
		storeData(data, length);
	}
	
	if(remain >= length) 
	{
		memcpy(write_iterator->ptr, data, length);
		write_iterator->conSize += length;
		write_iterator->ptr += length;
		
		/* page is full */
		if(write_iterator->conSize == page_size) {
			write_iterator->ptr = write_iterator->node.get();
			write_iterator++;
		}
	} 
	else 
	{
		memcpy(write_iterator->ptr, data, remain);
		write_iterator->ptr = write_iterator->node.get();
		write_iterator++;
		storeData(data+remain, length-remain);
	}
}

char* GzipAlloc::get() {
	return transfor.ptr;
}

InflateAlloc::InflateAlloc(unsigned size, SubsFilter *filter): GzipAlloc(size), subsfilter(filter),lastInflate(false){
	
}

void InflateAlloc::filter_save(StringBuffer &bs) {
	if(!subsfilter)
		storeData(bs.start, bs.end-bs.start);
	else if(subsfilter->contentCheck(bs))
		storeData(bs.start, bs.end-bs.start);
}

void InflateAlloc::fetchLine(BufferString &bs)
{
	char *linefeed = (char *)memchr(transfor.ptr, '\n', transfor.conSize);
	if(linefeed) {
		linefeed++;
		bs = BufferString(transfor.ptr, linefeed-transfor.ptr);
		transfor.conSize -= linefeed-transfor.ptr;
		transfor.ptr = linefeed;
		return true;
	}else {
		return false;
	}
}

void InflateAlloc::addInflateSize(unsigned size) {
	if(size == 0) {
		lastInflate = true;
		storeData(transfor.node.get(), transfor.conSize);
		return;
	}
		
	transfor.conSize += size;
	BufferString bs;
	while(fetchLine(bs)) {
		storeData(bs.start, bs.end-bs.start);
	}
	memmove(transfor.node.get(), transfor.ptr, transfor.conSize);
}

char* InflateAlloc::fetchData(unsigned &length) {
	if(read_iterator == map.end()) read_iterator++;
	
	if(read_iterator->conSize != page_size)
	{
		if(lastInflate) {
			length = read_iterator->conSize;
			return read_iterator->node.get();
		}
		else {
			length = 0;
			return NULL;
		}
	}
	else
	{
		length = page_size;
		read_iterator->reset();
		return (read_iterator++)->node.get();
	}
}

DeflateAlloc::DeflateAlloc(unsigned size): GzipAlloc(size) {
	
}

void DeflateAlloc::addDeflateSize(unsigned size) {
	storeData(transfor.ptr, size);
	transfor.reset();
}

char * DeflateAlloc::getReadPointer() {
	if(read_iterator == map.end()) 
		read_iterator++;
	return read_iterator->ptr;
}

unsigned DeflateAlloc::getReadSize() {
	if(read_iterator == map.end()) read_iterator++;
	return read_iterator->conSize;
}

void DeflateAlloc::ShiftSize(unsigned size) {
	if(read_iterator == map.end()) read_iterator++;
	unsigned len = read_iterator->ptr-read_iterator->node.get();
	if(size + len == page_size) {
		read_iterator->reset();
		read_iterator++;
	} else {
		read_iterator->ptr += size;
		read_iterator->conSize -= size;
		if(read_iterator->conSize == 0)
			read_iterator->reset();
	}
}
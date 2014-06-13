#include "MemAlloc.h"
#include "SubsFilter.h"
#include <vector>
#include <assert.h>
#include <cstring>
#include <cstdio>


/* GzipAlloc  definition */
GzipAlloc::GzipAlloc(unsigned size):
			page_size(size),
			transfor(Unit(TRANSFORSIZE*size))
{
}

char* GzipAlloc::get() {
	return transfor.ptr;
}

/* GzipAlloc  End */

/* InflateAlloc  Definition */
InflateAlloc::InflateAlloc(unsigned size, SubsFilter *filter): GzipAlloc(size), subsfilter(filter),lastInflate(false){
	for(int n=0; n<MINIUNITNUMBER; n++)
		map.push_back(Unit(size));
	write_iterator = map.begin();
	read_iterator = map.begin();
	
	max1=0;
	max2=0;
}

void InflateAlloc::adjustMaps() {
	if(map.size() < 3) return;
	map_pointer map_pos = write_iterator;
	if(map_pos == map.end()) map_pos++;
	
	int num_unit = MINIUNITNUMBER;
	
	while(num_unit--) {
		map_pos++;
		if(map_pos == map.end()) map_pos++;
	}
	
	while(map_pos!=read_iterator) {
		map.erase(map_pos++);
		if(map_pos == map.end()) map_pos++;
	}
	
	assert(map.size()==2);
}

void InflateAlloc::storeData(const char *data, unsigned length) 
{
	if(write_iterator == map.end()) 
		write_iterator++;
	
	unsigned len = write_iterator->ptr-write_iterator->begin;
	
// 	while(write_iterator->conSize==0 
// 		&& write_iterator+1 != map.end()
// 		&& (write_iterator+1)->conSize==0 
// 		&& write_iterator!=read_iterator 
// 		&& map.size()>4) {
// 		map.erase(write_iterator++);
// 	}
	
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

void InflateAlloc::addInflateSize(unsigned size) {
	if(size == 0) {
		lastInflate = true;
		storeData(transfor.begin, transfor.conSize);
		return;
	}
		
	transfor.conSize += size;
	assert(transfor.conSize <= TRANSFORSIZE*page_size);
	transfor.ptr += size;
	BufferString bs(const_cast<char *>(transfor.begin), transfor.conSize);
	
	std::vector<BufferString> vec = subsfilter->filter(bs);
	//printf("vec.size = %d\n", vec.size());
	if(vec.empty()) return;
	
	for(auto &buffer: vec) {
		storeData(buffer.start, buffer.last-buffer.start);
		transfor.conSize -= buffer.last-buffer.start;
		//printf("transfor.conSize = %d\n", transfor.conSize);
	}
	memmove(transfor.begin, vec.back().last, transfor.conSize);
	transfor.ptr = transfor.begin+transfor.conSize;
}

void InflateAlloc::setFilter(SubsFilter *filter) {
	subsfilter = filter;
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
			return read_iterator->begin;
		}
		else {
			assert(read_iterator == write_iterator);
			adjustMaps(); 
			length = 0;
			return NULL;
		}
	}
	else
	{
		length = page_size;
		read_iterator->reset();
			
		return (read_iterator++)->begin;
	}
}


void InflateAlloc::statistics() {
	unsigned temp = 0;
	for(auto &t: map) {
		temp += page_size;
	}
	printf("*******************\n");
	printf("InflateAlloc statistics\n");
	printf("map total number %d, size: %d\n", temp/page_size, temp);
	unsigned usedSize = 0, usedNumber = 0;
	for(auto &t: map) {
		usedSize += t.conSize;
		if(t.conSize) usedNumber++;
	}
	max1 = usedNumber>max1?usedNumber:max1;
	map1[usedNumber]++;
	printf("map used number %d/%d, size: %d\n", usedNumber, temp/page_size, usedSize);
	max2 = (transfor.conSize/page_size+1>max2)?(transfor.conSize/page_size+1):max2;
	map2[(transfor.conSize/page_size+1)]++;
	printf("used Transfor size %d/%d, number %d/%d\n", 
		   transfor.conSize, TRANSFORSIZE*page_size, 
		   transfor.conSize/page_size+1, TRANSFORSIZE
  		);
	printf("max used map = %d, max transfor = %d\n", max1, max2);
	printf("*******************\n");
}

/* InflateAlloc  End */

/* DeflateAlloc  Definition */

DeflateAlloc::DeflateAlloc(unsigned size): GzipAlloc(size) {
	max=0;
}

void DeflateAlloc::storeData(const char *data, unsigned length) {
	memcpy(transfor.ptr, data, length);
	addDeflateSize(length);
}

void DeflateAlloc::addDeflateSize(unsigned size) {
	
	transfor.conSize += size;
	assert(transfor.conSize <= TRANSFORSIZE*page_size);
	transfor.ptr += size;
}

bool DeflateAlloc::contentAvailable() {
	return transfor.conSize==0;
}

char * DeflateAlloc::getReadPointer(unsigned &len) {
	
	len = transfor.conSize;
	return transfor.begin;
}

void DeflateAlloc::ShiftSize(unsigned size) {
	
	transfor.conSize -= size;
	if(transfor.conSize == 0)
		transfor.ptr = transfor.begin;
	memmove(transfor.begin, transfor.begin+size, transfor.conSize);
	transfor.ptr = transfor.begin+transfor.conSize;
}

void DeflateAlloc::statistics() {
	printf("*******************\n");
	printf("DeflateAlloc statistics\n");
	printf("used Transfor size %d/%d, number %d/%d\n", 
		   transfor.conSize, TRANSFORSIZE*page_size, 
		   transfor.conSize/page_size+1, TRANSFORSIZE
  		);
	max = (transfor.conSize/page_size+1)>max?(transfor.conSize/page_size+1):max;
	map[(transfor.conSize/page_size+1)]++;
	printf("max transfor = %d\n", max);
	printf("*******************\n");
}

/* DeflateAlloc  End */

#include "MemAlloc.h"

MemAllocator::MemAllocator():map(8), 
			write_iterator(map.begin()), 
			read_iterator(map.begin()){
	
}

void MemAllocator::storeData(char *data, unsigned length) 
{
	if(write_iterator == map.end()) write_iterator++;
	unsigned remain = page_size-write_iterator->conSize;
	
	if(!write_iterator->used) 
		write_iterator->used=true;
	else
	{
		map.insert(write_iterator, Unit());
		write_iterator--;
	}
	
	if(remain >= length) 
	{
		memcpy(write_iterator->last, data, length);
		write_iterator->conSize += length;
		
		/* page is full */
		if(write_iterator->conSize == page_size)
			write_iterator++;
	} 
	else 
	{
		memcpy(write_iterator->last, data, remain);
		write_iterator++;
		storeData(data+remain, length-remain);
	}
}

char* fetchData(unsigned &length) {
	if(read_iterator == map.end()) read_iterator++;
	if(read_iterator->conSize != page_size)
	{
		length = 0;
		return NULL;
	}
	else
	{
		if(!hollows.empty()) {
			int hollow_length = hollows[0].first;
			for(int i=1; i<hollows.size(); i++) {
				hollow_length += hollow.second-hollow.first;
				memcpy()
			}
		}
		length = page_size;
		read_iterator->reset();
		return (read_iterator++)->node.get();
	}
}
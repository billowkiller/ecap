#ifndef ECAP_ADAPTER_MEMALLOC_H
#define ECAP_ADAPTER_MEMALLOC_H

#include <boost/shared_array.hpp>
#include <list>
#include "BufferString.h"

class SubsFilter;

class GzipAlloc
{
protected:
	struct Unit 
	{
		boost::shared_array<char> node;
		char * const begin;
		char *ptr;
		unsigned conSize;
		bool used;
		Unit(unsigned size):node(new char[size]), 
				used(false),
				conSize(0),
				begin(node.get()),
				ptr(begin){}
		void reset() {
			used=false;
			conSize = 0;
			ptr = node.get();
		}
	};
	
protected:
	const int UNITNUMBER = 4;
	const int TRANSFORSIZE = 6;
	std::list<Unit> map;//(8, new char[page_size]);
	std::list<Unit>::iterator write_iterator;
	std::list<Unit>::iterator read_iterator;
	unsigned page_size;
	Unit transfor;
	
public:
	GzipAlloc(unsigned size);
	void storeData(char *data, unsigned length);
	char* get();
};

/* programmed read */
class InflateAlloc : public GzipAlloc 
{
private:
	bool lastInflate;
	SubsFilter *subsfilter;
	void filter_save(BufferString &bs);
	bool fetchLine(BufferString &bs);
public:
	InflateAlloc(unsigned size, SubsFilter *filter);
	char* fetchData(unsigned &length);
	void addInflateSize(unsigned size);
};

/* greedy read */
class DeflateAlloc : public GzipAlloc 
{
private:
	unsigned lastout;
public:
	DeflateAlloc(unsigned size);
	void addDeflateSize(unsigned size);
	char * getReadPointer(unsigned &len);
	void ShiftSize(unsigned size);
	
	/* only for deflate end */
	void advance(unsigned size);
};

#endif
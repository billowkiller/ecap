#ifndef ECAP_ADAPTER_MEMALLOC_H
#define ECAP_ADAPTER_MEMALLOC_H

#include <boost/shared_array.hpp>
#include <list>
#include <map>
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
	const int TRANSFORSIZE = 15;
	unsigned page_size;
	Unit transfor;
	
public:
	GzipAlloc(unsigned size);
	virtual void storeData(const char *data, unsigned length) = 0;
	char* get();
	//for data statistics
	virtual void statistics() = 0;
};

/* programmed read */
class InflateAlloc : public GzipAlloc 
{
	
private:
	unsigned max1, max2;
	bool lastInflate;
	SubsFilter *subsfilter;
	const int UNITNUMBER = 2;
	std::list<Unit> map;//(8, new char[page_size]);
	std::list<Unit>::iterator write_iterator;
	std::list<Unit>::iterator read_iterator;
	
public:
	InflateAlloc(unsigned size, SubsFilter *filter);
	virtual void storeData(const char *data, unsigned length);
	char* fetchData(unsigned &length);
	void addInflateSize(unsigned size);
	void setFilter(SubsFilter *filter);
	virtual void statistics();
	std::map<int, int> map1, map2;
};

/* greedy read */
class DeflateAlloc : public GzipAlloc 
{
private: 
	unsigned max;
	
public:
	DeflateAlloc(unsigned size);
	virtual void storeData(const char *data, unsigned length);
	void addDeflateSize(unsigned size);
	char * getReadPointer(unsigned &len);
	void ShiftSize(unsigned size);
	bool contentAvailable();
	virtual void statistics();
	std::map<int, int> map;
};

#endif
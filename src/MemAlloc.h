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

class InflateAlloc : public GzipAlloc 
{
private:
	typedef std::list<Unit> map_type;
	typedef std::list<Unit>::iterator map_pointer;
	// shrink map size to 2;
	// optional
	void adjustMaps(); 
public:
	InflateAlloc(unsigned size, SubsFilter *filter);
	virtual void storeData(const char *data, unsigned length);
	char* fetchData(unsigned &length);
	void addInflateSize(unsigned size);
	void setFilter(SubsFilter *filter);
	virtual void statistics(); //for log
	std::map<int, int> map1, map2; //for log

private:
	unsigned max1, max2;
	bool lastInflate;
	SubsFilter *subsfilter;
	const int MINIUNITNUMBER = 2;
	map_type map; //read to deflate
	map_pointer write_iterator;
	map_pointer read_iterator;
	
};

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
	std::map<int, int> map; //unit number, frequency
};

#endif
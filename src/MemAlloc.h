#ifndef ECAP_ADAPTER_TIMEUTILITY_H
#define ECAP_ADAPTER_TIMEUTILITY_H

#include <boost/scoped_array.hpp>
#include <list>

class SubsFilter;
class StringBuffer;

class GzipAlloc
{
protected:
	struct Unit 
	{
		boost::scoped_array<char> node;
		char *ptr;
		unsigned conSize;
		bool used;
		Unit(int size):node(new char[size]), 
				used(false),
				conSize(0),
				ptr(node.get()){}
		void reset() {
			used=false;
			conSize = 0;
			ptr = node.get();
		}
	};
	Unit transfor;
private:
	std::list<Unit> map;//(8, new char[page_size]);
	std::list<Unit>::iterator write_iterator;
	std::list<Unit>::iterator read_iterator;
	unsigned page_size;
	
public:
	GzipAlloc(unsigned size);
	void storeData(char *data, unsigned length);
	char* get();
};

/* programmed read */
class InflateAlloc : GzipAlloc 
{
private:
	bool lastInflate;
	SubsFilter *subsfilter;
	void filter_save(StringBuffer &bs);
public:
	InflateAlloc(unsigned size, SubsFilter *filter);
	char* fetchData(unsigned &length);
	void addInflateSize(unsigned size);
};

/* greedy read */
class DeflateAlloc : GzipAlloc 
{
public:
	DeflateAlloc(unsigned size);
	void addDeflateSize(unsigned size);
	char * getReadPointer();
	unsigned getReadSize();
	void ShiftSize(unsigned size);
};

#endif
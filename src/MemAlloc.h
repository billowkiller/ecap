#ifndef ECAP_ADAPTER_TIMEUTILITY_H
#define ECAP_ADAPTER_TIMEUTILITY_H

#include <boost/shared_array.hpp>
#include <vector>
#include <list>

template<unsigned page_size>
class MemAllocator <page_size>{

	struct Unit {
		shared_array<char> node;
		char *last;
		unsigned conSize;
		bool used;
		std::vector<pair<int,int>> hollows;
		Unit():node(new char[page_size]), 
			   used(false),
			   conSize(0),
			   last(node.get()){}
		void reset() {
			used=false;
			conSize = 0;
			last = node->get();
		}
	};
private:
	list<Unit> map;//(8, new char[page_size]);
	list<Unit>::iterator write_iterator;
	list<Unit>::iterator read_iterator;
	
public:
	MemAllocator();
	char* fetchData(unsigned &length);
	void storeData(char *data, unsigned length);
};

#endif
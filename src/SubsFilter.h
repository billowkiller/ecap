#include <cstring>
#include "BufferString.h"

class SubsFilter {
	
public:
	SubsFilter(char *pchar);
	SubsFilter(char *pchar, unsigned size);

	void addContent(unsigned size);
	char * fetchUncompressed(unsigned &); // return two variable
	bool finishFilter();
	BufferString buffer;
	
	private:
		
		bool fetchLine(BufferString &bs);
		void contentCheck();
		const unsigned chunkSize = 4096;
};
#include <cstring>
#include <list>
#include <tr1/memory>
#include "BufferString.h"

class SubsFilter {
	
public:
	SubsFilter(char *pchar);
	SubsFilter(char *pchar, unsigned size);

	void addContent(unsigned size);
	char * fetchUncompressed(unsigned &); // return two variable
	bool finishFilter();
	
	
	private:
		typedef std::list<BufferString> BSList;
		BSList sendingList;
		BufferString buffer;
		void storeSendingData(BufferString bs);
		bool fetchLine(BufferString &bs);
		void contentCheck();
		const unsigned chunkSize = 4096;
		std::tr1::shared_ptr<char> cPool;
};
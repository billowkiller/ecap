#include <cstring>
#include <string.h>
#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include <list>
#include <tr1/memory>
#include "BufferString.h"
#include "FilterConf.h"

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
		void readConf(vector<FilterConf> &v_filterconf);
		void blockDelete(BufferString &bs, const int type ,const char* keyword);
		void keywordReplace(BufferString &bs, const char* s_str, const char* d_str);
		const unsigned chunkSize = 4096;
		const char* confDir = "/usr/local/squid/etc/SubsFilter.conf";
		std::tr1::shared_ptr<char> cPool;
};

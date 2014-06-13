#include "LineSubsFilter.h"
#include <cstring>

std::vector<BufferString> LineSubsFilter::filter(BufferString bs) {
	std::vector<BufferString> vec;
	
	char *linefeed = (char *)memchr(bs.start, '\n', bs.last-bs.start);
	while(linefeed) {
		linefeed++;
		vec.push_back(BufferString(bs.start, linefeed-bs.start));
		bs.start = linefeed;
		linefeed = (char *)memchr(bs.start, '\n', bs.last-bs.start);
	}
	return std::move(vec);
		
}


/*
 * =====================================================================================
 *
 *       Filename:  ContentHandler.cc
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  04/16/2014 10:30:39 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  billowkiller (), billowkiller@gmail.com
 *   Organization:  
 *
 * =====================================================================================
 */
#include "SubsFilter.h"

SubsFilter::SubsFilter(char *pchar):buffer(pchar){}

SubsFilter::SubsFilter(char *pchar, unsigned size):buffer(pchar,size){}

bool SubsFilter::fetchLine(BufferString &bs)
{
	char *linefeed = (char *)memchr(buffer.pos, '\n', buffer.last-buffer.pos);
	if(linefeed) {
		linefeed++;
		bs = BufferString(buffer.pos, linefeed-buffer.pos);
		buffer.pos = linefeed;
		return true;
	}else {
		return false;
	}
}

void SubsFilter::addContent(unsigned size)
{
	if(size == 0)
		buffer.pos = buffer.last;
	buffer.last += size;
	//contentCheck();
}
void SubsFilter::contentCheck() {
	BufferString bs;
	while(fetchLine(bs))
	{
		
	}
}

char * SubsFilter::fetchUncompressed(unsigned &size)
{
	if(buffer.start == buffer.pos)
	{
		size = 0;
		return NULL;
	}
	
	char *p = buffer.start;
	if(buffer.start + chunkSize <= buffer.pos)
	{
		size = chunkSize;
		buffer.start += chunkSize;
	}
	else
	{
		size = buffer.pos - buffer.start;
		buffer.start = buffer.pos;
	}
	return p;
}

bool SubsFilter::finishFilter()
{
	return buffer.start == buffer.last;
}
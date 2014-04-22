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

SubsFilter::SubsFilter(char *pchar):buffer(pchar),cPool(new char[chunkSize]){}

SubsFilter::SubsFilter(char *pchar, unsigned size):buffer(pchar,size),cPool(new char[chunkSize]){}

/*
 * store char position into a list.
 * if the data is continuous,
 * we add the bufferstring to the 
 * last node of the list;
 */
void SubsFilter::storeSendingData(BufferString bs)
{
	if(sendingList.size() == 0)
		sendingList.push_back(bs);
	
	BSList::reference lastBS = sendingList.back();
	if(lastBS.last == bs.start)
		lastBS.last = bs.last;
	else
		sendingList.push_back(bs);
}

/*
 * get a sentence from buffer
 */
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
	/* For if the last char in a page is LF */
	if(size == 0)
	{
		buffer.pos = buffer.last;
		// because fetchLine can't get last line 
		// if there is no LF in the end
		// we just store it.
		storeSendingData(BufferString(buffer.start, buffer.last-buffer.start));
	}
	
	buffer.last += size;
	contentCheck();
}

/* 
 * check the added data. And if there is not a complete sentence 
 * in the end of the data, we keep it and recalculate in the next turn.
 */
void SubsFilter::contentCheck() {
	BufferString bs; //fetched sentence, including last LF.
	while(fetchLine(bs))
	{
		/* yangbr */
		storeSendingData(bs);
	}
}

/*
 * copy a chunkSize or less data into cPool, 
 * and return Gzipper for compress.
 */
char * SubsFilter::fetchUncompressed(unsigned &size)
{
	if(sendingList.size() == 0)
	{
		size = 0;
		return NULL;
	}
	
	BSList::reference bs = sendingList.front();
	unsigned remain = chunkSize;
	
	while(sendingList.size()>0 && remain!=0)
	{
		unsigned copySize = (bs.start+remain<=bs.last) ? remain : bs.last-bs.start;
		
		memcpy(cPool.get()+(chunkSize-remain), bs.start, copySize);
		bs.start += copySize;
		remain -= copySize;
		
		if(bs.start == bs.last)
		{
			sendingList.pop_front();
			bs = sendingList.front();
		}
	}
	
	size = (remain==0) ? chunkSize : chunkSize-remain;
	return cPool.get();
}

bool SubsFilter::finishFilter()
{
	return buffer.start == buffer.last;
}
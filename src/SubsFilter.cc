/*
 * =====================================================================================
 *
 *       Filename:  ContentHandler.cc
 *
 *    Description:  buffer.start is the position of inflated data head; buffer.pos moves 
 * 					with the content checker, meaning that it points to the unchecked data head;
 * 					buffer.last always stays in the tail of inflated data.
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
#include "Debugger.h"

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
	{
		sendingList.push_back(bs);
		return;
	}
	
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
	/*  If the last char in a page is not LF, fetchLine can't get last line 
	 *  and we have to store it by hand.
	 */
	if(size == 0)
	{
		storeSendingData(BufferString(buffer.pos, buffer.last-buffer.pos));
		buffer.pos = buffer.last;
	}
	
	buffer.last += size;
	contentCheck();
}

/* 
 * check the added data. And if there is not a complete sentence 
 * in the end of the data, we keep it and recalculate in the next turn.
 */
bool SubsFilter::contentCheck(BufferString &bs) {
	return true;
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
	
	unsigned remain = chunkSize;
	
	while(sendingList.size()>0 && remain!=0)
	{
		BSList::reference bs = sendingList.front();
		unsigned copySize = (bs.start+remain<=bs.last) ? remain : bs.last-bs.start;
		
		memcpy(cPool.get()+(chunkSize-remain), bs.start, copySize);
		bs.start += (int)copySize;
		remain -= copySize;
		if(bs.start == bs.last)
			sendingList.pop_front();
	}
	
	size = (remain==0) ? chunkSize : chunkSize-remain;
	return cPool.get();
}

bool SubsFilter::finishFilter()
{
	if(buffer.pos != buffer.last) {
		Debugger() << buffer.pos;
		Debugger() << "u_offset: " <<  buffer.last-buffer.start;
	}
	return buffer.pos == buffer.last;
}

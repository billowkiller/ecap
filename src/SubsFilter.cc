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
void SubsFilter::contentCheck() {
Debugger()<<"=====contentCheck\n";
	BufferString bs; //fetched sentence, including last LF.
	vector<FilterConf> v_c;
	readConf(v_c);//read configuration from SubsFilter.conf
	while(fetchLine(bs))
	{
Debugger()<<"=====fetchLine\n";
		//print fetched line
                int length_ = (int)(bs.last - bs.start);
                char* str;
                str = (char*)malloc((length_+1)*sizeof(char));
                memcpy(str , bs.start , length_);
		*(str+length_)='\0';
                std::string s = std::string(str);
                Debugger()<<"print contents in loop of fetch line:\n";
                Debugger()<< s ;
                free(str);

		/* yangbr */

		//performing rules
		int i=0;
		for(;i<v_c.size();i++)
		{
			if(v_c[i].type==1)
			{
				keywordReplace(bs , v_c[i].s_str.c_str() , v_c[i].d_str.c_str());
			}
			else
			{
				blockDelete(bs , v_c[i].type , v_c[i].s_str.c_str());
			}
		}
		storeSendingData(bs);
	}
}


void SubsFilter::keywordReplace(BufferString &bs ,const char* s_str ,const char* d_str){
	if( s_str==NULL||d_str==NULL || *s_str=='\0'|| *d_str=='\0')
        {
		return;
        }
	int slen = strlen(s_str);
	int dlen = strlen(d_str);
	char* r_str;
	r_str = (char*)malloc((slen+1)*sizeof(char));
	if(slen == dlen || slen < dlen)
	{
		memcpy(r_str,d_str,slen);
		*(r_str+slen)='\0';
	}
	else
	{ //slen > dlen
		memcpy(r_str,d_str,dlen);		
		int i;
		for(i=0 ; i < slen-dlen ; i++)
		{
			memcpy(r_str+dlen+i,"  ",sizeof(char));
		}
		*(r_str+slen)='\0';
	}

	
	while(NULL!=strstr(bs.pos,s_str) && bs.last>strstr(bs.pos,s_str))
	{	
		bs.pos = strstr(bs.pos,s_str);
		memcpy(bs.pos , r_str , slen);
		bs.pos += slen;
	}
	free(r_str);
	bs.pos = bs.start;
}

void SubsFilter::blockDelete(BufferString &bs , const int type , const char* keyword){
	if( keyword==NULL || *keyword=='\0')
	{
		return;
	}
	int keyword_length = strlen(keyword);
//one comment method
if(type==3)
{
	while(NULL!=strstr(bs.pos,"\"body\":{"))
	{
		char* tlunit_start;
		char* tlunit_end;
		bs.pos = strstr(bs.pos,"\"body\":{");
		tlunit_start = bs.pos + 7;
		int licount=0;
		for( ; bs.pos<bs.last ; bs.pos++ )
		{
			if(bs.pos==strstr(bs.pos,"{"))
			{
				licount++;
			}
			if(bs.pos==strstr(bs.pos,"}"))
			{
				licount--;
				if(licount==0)
				{
					break;
				}
			}
		}
		if(licount!=0)
		{
			break;
		}
		tlunit_end = bs.pos;//points to "}" in bigpipe codes
		//found li start and end, now we check if there is a keyword in this li
		if(strstr(tlunit_start,keyword)!=NULL && strstr(tlunit_start,keyword) < tlunit_end)
		{
			//tlunit_start++;
			//tlunit_end--;
			char* tlunit_temp = tlunit_start;
			for( ; tlunit_temp<=tlunit_end ; tlunit_temp++)
			{
				memcpy(tlunit_temp,"  ",1);
			}
			memcpy(tlunit_start,"[ ",1);
			memcpy(tlunit_end,"] ",1);
		}
		bs.pos = tlunit_end;
	}
	bs.pos = bs.start;
}

//comment area method
if(type==4)
{
	while(NULL!=strstr(bs.pos,"<div class=\"uiUfi UFIContainer"))
	{
		char* tlunit_start;
		char* tlunit_end;
		bs.pos = strstr(bs.pos,"<div class=\"uiUfi UFIContainer");
		tlunit_start = bs.pos;
		int divcount=0;
		for( ; bs.pos<bs.last ; bs.pos++)
		{
			if(bs.pos==strstr(bs.pos,"<div"))
			{
				divcount++;
			}
			if(bs.pos==strstr(bs.pos,"</div>"))
			{
				divcount--;
				if(divcount==0)
				{
					break;
				}
			}
		}
		if(divcount!=0)
		{
			break;
		}
		tlunit_end = bs.pos + 5; //points to the end of "</div>" associated to <div class="uiUfi UFIContainer
		//found div start and end, now we check if there is a keyword in this div
		if(strstr(tlunit_start,keyword)!=NULL && strstr(tlunit_start,keyword) < tlunit_end)
		{
			char* tlunit_temp = tlunit_start;
			for( ; tlunit_temp<=tlunit_end ; tlunit_temp++)
			{
				memcpy(tlunit_temp,"  ",1);
			}
		}
		bs.pos = tlunit_end;
	}
	bs.pos = bs.start;
}

//unit method
if(type==5)
{
//profile page
	while(NULL!=strstr(bs.pos,"<li class=\"fbTimelineUnit fbTimelineTwoColumn clearfix"))
	{
		char* tlunit_start;
		char* tlunit_end;
		bs.pos = strstr(bs.pos,"<li class=\"fbTimelineUnit fbTimelineTwoColumn clearfix");
		tlunit_start = bs.pos;
		int licount=0;
		for( ; bs.pos<bs.last ; bs.pos++ )
		{
			if(bs.pos==strstr(bs.pos,"<li"))
			{
				licount++;
			}
			if(bs.pos==strstr(bs.pos,"</li>"))
			{
				licount--;
				if(licount==0)
				{
					break;
				}
			}
		}
		if(licount!=0)
		{
			break;
		}
		tlunit_end = bs.pos + 4;//points to the end of "</li>" associated to <li class="fbTimelineUnit FbTimelineTwoColumn clearfix
		//found li start and end, now we check if there is a keyword in this li
		if(strstr(tlunit_start,keyword)!=NULL && strstr(tlunit_start,keyword) < tlunit_end)
		{
			char* tlunit_temp = tlunit_start;
			for( ; tlunit_temp<=tlunit_end ; tlunit_temp++)
			{
				memcpy(tlunit_temp,"  ",1);
			}
		}
		bs.pos = tlunit_end;
	}
	bs.pos = bs.start;
	
//main page 
	while(NULL!=strstr(bs.pos,"<div class=\"_4-u2 mbm"))
	{
		char* tlunit_start;
		char* tlunit_end;
		bs.pos = strstr(bs.pos,"<div class=\"_4-u2 mbm");
		tlunit_start = bs.pos;
		int divcount=0;
		for( ; bs.pos<bs.last ; bs.pos++)
		{
			if(bs.pos==strstr(bs.pos,"<div"))
			{
				divcount++;
			}
			if(bs.pos==strstr(bs.pos,"</div>"))
			{
				divcount--;
				if(divcount==0)
				{
					break;
				}
			}
		}
		if(divcount!=0)
		{
			break;
		}
		tlunit_end = bs.pos + 5; //points to the end of "</div>" associated to <div class="_4-us mbm
		//found div start and end, now we check if there is a keyword in this div
		if(strstr(tlunit_start,keyword)!=NULL && strstr(tlunit_start,keyword) < tlunit_end)
		{
			char* tlunit_temp = tlunit_start;
			for( ; tlunit_temp<=tlunit_end ; tlunit_temp++)
			{
				memcpy(tlunit_temp,"  ",1);
			}
		}
		bs.pos = tlunit_end;
	}
	bs.pos = bs.start;
}

if(type==6)
{
	while(NULL!=strstr(bs.pos,"<div id=\"timeline_tab_content\">"))
	{
		char* tlunit_start;
		char* tlunit_end;
		bs.pos = strstr(bs.pos,"<div id=\"timeline_tab_content\">");
		tlunit_start = bs.pos;
		int divcount=0;
		for( ; bs.pos<bs.last ; bs.pos++)
		{
			if(bs.pos==strstr(bs.pos,"<div"))
			{
				divcount++;
			}
			if(bs.pos==strstr(bs.pos,"</div>"))
			{
				divcount--;
				if(divcount==0)
				{
					break;
				}
			}
		}
		if(divcount!=0)
		{
			break;
		}
		tlunit_end = bs.pos + 5; //points to the end of "</div>"
		//found div start and end, now we check if there is a keyword in this div
		if(strstr(tlunit_start,keyword)!=NULL && strstr(tlunit_start,keyword) < tlunit_end)
		{
			char* tlunit_temp = tlunit_start;
			for( ; tlunit_temp<=tlunit_end ; tlunit_temp++)
			{
				memcpy(tlunit_temp,"  ",1);
			}
		}
		bs.pos = tlunit_end;
	}
	bs.pos = bs.start;
}

//notes method
if(type==7)
{
	while(NULL!=strstr(bs.pos,"<div class=\"_z3h note\">"))
	{
		char* tlunit_start;
		char* tlunit_end;
		bs.pos = strstr(bs.pos,"<div class=\"_z3h note\">");
		tlunit_start = bs.pos;
		int divcount=0;
		for( ; bs.pos<bs.last ; bs.pos++)
		{
			if(bs.pos==strstr(bs.pos,"<div"))
			{
				divcount++;
			}
			if(bs.pos==strstr(bs.pos,"</div>"))
			{
				divcount--;
				if(divcount==0)
				{
					break;
				}
			}
		}
		if(divcount!=0)
		{
			tlunit_end = bs.last-2;
			//break;
		}else
		{
			tlunit_end = bs.pos + 5;//points to the end of "</div>"
		}
		//found div start and end, now we check if there is a keyword in this div
		if(strstr(tlunit_start,keyword)!=NULL && strstr(tlunit_start,keyword) < tlunit_end)
		{
			char* tlunit_temp = tlunit_start;
			for( ; tlunit_temp<=tlunit_end ; tlunit_temp++)
			{
				memcpy(tlunit_temp,"  ",1);
			}
		}
		bs.pos = tlunit_end;
	}
	bs.pos = bs.start;
}

//event method
if(type==8)
{
	while(NULL!=strstr(bs.pos,"<li class=\"_c6n\">"))
	{
		char* tlunit_start;
		char* tlunit_end;
		bs.pos = strstr(bs.pos,"<li class=\"_c6n\">");
		tlunit_start = bs.pos;
		int licount=0;
		for( ; bs.pos<bs.last ; bs.pos++ )
		{
			if(bs.pos==strstr(bs.pos,"<li"))
			{
				licount++;
			}
			if(bs.pos==strstr(bs.pos,"</li>"))
			{
				licount--;
				if(licount==0)
				{
					break;
				}
			}
		}
		if(licount!=0)
		{
			break;
		}
		tlunit_end = bs.pos + 4;//points to the end of "</li>"
		//found li start and end, now we check if there is a keyword in this li
		if(strstr(tlunit_start,keyword)!=NULL && strstr(tlunit_start,keyword) < tlunit_end)
		{
			char* tlunit_temp = tlunit_start;
			for( ; tlunit_temp<=tlunit_end ; tlunit_temp++)
			{
				memcpy(tlunit_temp,"  ",1);
			}
		}
		bs.pos = tlunit_end;
	}
	bs.pos = bs.start;
}

//group method
if(type==9)
{
	while(NULL!=strstr(bs.pos,"<li class=\"_1v6c\">"))
	{
		char* tlunit_start;
		char* tlunit_end;
		bs.pos = strstr(bs.pos,"<li class=\"_1v6c\">");
		tlunit_start = bs.pos;
		int licount=0;
		for( ; bs.pos<bs.last ; bs.pos++ )
		{
			if(bs.pos==strstr(bs.pos,"<li"))
			{
				licount++;
			}
			if(bs.pos==strstr(bs.pos,"</li>"))
			{
				licount--;
				if(licount==0)
				{
					break;
				}
			}
		}
		if(licount!=0)
		{
			break;
		}
		tlunit_end = bs.pos + 4;//points to the end of "</li>"
		//found li start and end, now we check if there is a keyword in this li
		if(strstr(tlunit_start,keyword)!=NULL && strstr(tlunit_start,keyword) < tlunit_end)
		{
			char* tlunit_temp = tlunit_start;
			for( ; tlunit_temp<=tlunit_end ; tlunit_temp++)
			{
				memcpy(tlunit_temp,"  ",1);
			}
		}
		bs.pos = tlunit_end;
	}
	bs.pos = bs.start;
}

//picture method
if(type==10)
{
	while(NULL!=strstr(bs.pos,"<div class=\"_53s fbPhotoCurationControlWrapper"))
	{
		char* tlunit_start;
		char* tlunit_end;
		bs.pos = strstr(bs.pos,"<div class=\"_53s fbPhotoCurationControlWrapper");
		tlunit_start = bs.pos;
		int divcount=0;
		for( ; bs.pos<bs.last ; bs.pos++)
		{
			if(bs.pos==strstr(bs.pos,"<div"))
			{
				divcount++;
			}
			if(bs.pos==strstr(bs.pos,"</div>"))
			{
				divcount--;
				if(divcount==0)
				{
					break;
				}
			}
		}
		if(divcount!=0)
		{
			break;
		}
		tlunit_end = bs.pos + 5; //points to the end of "</div>"
		//found div start and end, now we check if there is a keyword in this div
		if(strstr(tlunit_start,keyword)!=NULL && strstr(tlunit_start,keyword) < tlunit_end)
		{
			char* tlunit_temp = tlunit_start;
			for( ; tlunit_temp<=tlunit_end ; tlunit_temp++)
			{
				memcpy(tlunit_temp,"  ",1);
			}
		}
		bs.pos = tlunit_end;
	}
	bs.pos = bs.start;
}


//album method
if(type==11)
{
	while(NULL!=strstr(bs.pos,"<td class=\"_51m- vTop hLeft pbm prm\">"))
	{
		char* tlunit_start;
		char* tlunit_end;
		bs.pos = strstr(bs.pos,"<td class=\"_51m- vTop hLeft pbm prm\">");
		tlunit_start = bs.pos;
		int licount=0;
		for( ; bs.pos<bs.last ; bs.pos++ )
		{
			if(bs.pos==strstr(bs.pos,"<td"))
			{
				licount++;
			}
			if(bs.pos==strstr(bs.pos,"</td>"))
			{
				licount--;
				if(licount==0)
				{
					break;
				}
			}
		}
		if(licount!=0)
		{
			break;
		}
		tlunit_end = bs.pos + 4;//points to the end of "</td>" associated to <td class="_51m- vTop hLeft pbm prm">"
		//found li start and end, now we check if there is a keyword in this li
		if(strstr(tlunit_start,keyword)!=NULL && strstr(tlunit_start,keyword) < tlunit_end)
		{
			char* tlunit_temp = tlunit_start;
			for( ; tlunit_temp<=tlunit_end ; tlunit_temp++)
			{
				memcpy(tlunit_temp,"  ",1);
			}
		}
		bs.pos = tlunit_end;
	}
	bs.pos = bs.start;
}

//friend method
if(type==12)
{
	while(NULL!=strstr(bs.pos,"<div class=\"clearfix ego_unit\""))
	{
		char* tlunit_start;
		char* tlunit_end;
		bs.pos = strstr(bs.pos,"<div class=\"clearfix ego_unit\"");
		tlunit_start = bs.pos;
		int divcount=0;
		for( ; bs.pos<bs.last ; bs.pos++)
		{
			if(bs.pos==strstr(bs.pos,"<div"))
			{
				divcount++;
			}
			if(bs.pos==strstr(bs.pos,"</div>"))
			{
				divcount--;
				if(divcount==0)
				{
					break;
				}
			}
		}
		if(divcount!=0)
		{
			break;
		}
		tlunit_end = bs.pos + 5; //points to the end of "</div>"
		//found div start and end, now we check if there is a keyword in this div
		if(strstr(tlunit_start,keyword)!=NULL && strstr(tlunit_start,keyword) < tlunit_end)
		{
			char* tlunit_temp = tlunit_start;
			for( ; tlunit_temp<=tlunit_end ; tlunit_temp++)
			{
				memcpy(tlunit_temp,"  ",1);
			}
		}
		bs.pos = tlunit_end;
	}
	bs.pos = bs.start;

	while(NULL!=strstr(bs.pos,"<li class=\"_698\">"))
	{
		char* tlunit_start;
		char* tlunit_end;
		bs.pos = strstr(bs.pos,"<li class=\"_698\">");
		tlunit_start = bs.pos;
		int licount=0;
		for( ; bs.pos<bs.last ; bs.pos++ )
		{
			if(bs.pos==strstr(bs.pos,"<li"))
			{
				licount++;
			}
			if(bs.pos==strstr(bs.pos,"</li>"))
			{
				licount--;
				if(licount==0)
				{
					break;
				}
			}
		}
		if(licount!=0)
		{
			break;
		}
		tlunit_end = bs.pos + 4;//points to the end of "</li>"
		//found li start and end, now we check if there is a keyword in this li
		if(strstr(tlunit_start,keyword)!=NULL && strstr(tlunit_start,keyword) < tlunit_end)
		{
			char* tlunit_temp = tlunit_start;
			for( ; tlunit_temp<=tlunit_end ; tlunit_temp++)
			{
				memcpy(tlunit_temp,"  ",1);
			}
		}
		bs.pos = tlunit_end;
	}
	bs.pos = bs.start;
}

//default method
if(type==2)
{
	while(NULL!=strstr(bs.pos,keyword) && bs.last>strstr(bs.pos,keyword) )
	{
		bs.pos = strstr(bs.pos,keyword);
		char* b_tag_s = NULL;//start of a tag, "<", before keyword
		char* b_tag_e = NULL;//  end of a tag, ">", after keyword
		char* a_tag_s = NULL;//start of a tag, "<", before keyword
		char* a_tag_e = NULL;//  end of a tag, ">", after keyword
		char* pos_temp;
		
		pos_temp = bs.pos;
		while(pos_temp>bs.start)
		{
			if(*(pos_temp)=='<' && b_tag_s==NULL)
			{
				b_tag_s = pos_temp;
			}
			if(*(pos_temp)=='>' && b_tag_e==NULL)
			{
				b_tag_e = pos_temp;
			}
			if(b_tag_s!=NULL && b_tag_e!=NULL)
			{
				break;
			}
			pos_temp--;
		}
		
		pos_temp = bs.pos + keyword_length;	
		while(pos_temp<bs.last)
		{
			if(*(pos_temp)=='<' && a_tag_s==NULL)
			{
				a_tag_s = pos_temp;
			}
			if(*(pos_temp)=='>' && a_tag_e==NULL)
			{
				a_tag_e = pos_temp;
			}
			if(a_tag_s!=NULL && a_tag_e!=NULL)
			{
				break;
			}
			pos_temp++;
		}

		int in_tag,in_tag_b,in_tag_a;//-1= unknow , 0= not in tag , 1= in tag		
		//decide if keyword is in a tag, depend on pointers before keyword
		if(b_tag_s!=NULL && b_tag_e!=NULL)
		{
			if(b_tag_s < b_tag_e)
				in_tag_b = 0;
			else
				in_tag_b = 1;
		}
		else if(b_tag_s==NULL && b_tag_e!=NULL)
		{
			in_tag_b = 0;
		}
		else if(b_tag_s!=NULL && b_tag_e==NULL)
		{
			in_tag_b = 1;
		}
		else
		{
			in_tag_b = -1;
		}
		
		//decide if keyword is in a tag, depend on pointers after keyword
		if(a_tag_s!=NULL && a_tag_e!=NULL)
                {
                        if(a_tag_s < a_tag_e)
                                in_tag_a = 0;
                        else
                                in_tag_a = 1;
                }
                else if(a_tag_s==NULL && a_tag_e!=NULL)
                {
                        in_tag_a = 1;
                }
                else if(a_tag_s!=NULL && a_tag_e==NULL)
                {
                        in_tag_a = 0;
                }
                else
                {
                        in_tag_a = -1;
                }

		//decide a final conclusion, if keyword is in a tag
		if(in_tag_a==in_tag_b)
		{
			in_tag = in_tag_a;
		}		
		else
		{
			in_tag = -1;
		}

		//perform deleting or do nothing
		if(in_tag==1)	
		{
			/* keyword is in a tag ,do nothing*/
			bs.pos += keyword_length;
		}
		else if(in_tag==0)
		{
			char* c_start = b_tag_e + 1;
			char* c_end = a_tag_s - 1;
			int i;
			for(i=0;i<(c_end-c_start+1);i++)
			{
				memcpy(c_start+i,"  ",1);
			}
			
			bs.pos = a_tag_s;
		}
		else
		{
			/*   Can not decide if keyword is in a tag or not.
				what to do?   */
			int i;
                	for(i=0;i<keyword_length;i++)
			{
                		memcpy(bs.pos+i,"  ",1);
                	}
			bs.pos += keyword_length;
		}
	}
	bs.pos = bs.start;
}
}

void SubsFilter::readConf(vector<FilterConf> &v_filterconf){
    char buf[100];
    FILE *f = fopen(confDir,"r");
    if(f == NULL)
	{
        return;
	}
    while(fgets(buf,sizeof(buf),f)!=NULL)
    {
        if( buf == strstr(buf,"subsfilter r") )
        {
            int slen,dlen;
            char* spos;
            char* dpos;
            char* end;
            spos = buf+13;
            dpos = strchr(spos,' ')+1;
            slen = dpos - spos -1;
            end = dpos+1;
            while(*end!=' ' && *end!='\n' && *end!='\0' && *end!=EOF)
            {
                end++;
            }
            dlen = end - dpos;

            char* s_str = (char*)malloc((slen+1)*sizeof(char));
            char* d_str = (char*)malloc((dlen+1)*sizeof(char));
            memcpy(s_str,spos,slen);
            memcpy(d_str,dpos,dlen);
            *(s_str+slen) = '\0';
            *(d_str+dlen) = '\0';
            string ss = s_str;
            string ds = d_str;

            FilterConf fc(1,ss,ds);
            v_filterconf.push_back(fc);
            free(s_str);
            free(d_str);
        }
        else if( buf == strstr(buf,"subsfilter"))
        {
            int len;
            char* pos;
            char* end;
            pos = buf+13;
            end = pos+1;
            while(*end!=' ' && *end!='\n' && *end!='\0' && *end!=EOF)
            {
                end++;
            }
            len = end - pos;
            char* str = (char*)malloc((len+1)*sizeof(char));
            memcpy(str,pos,len);
            *(str+len) = '\0';
            string ss = str;
		if(*(buf+11)=='d')
		{
			FilterConf fc(2,ss);
			v_filterconf.push_back(fc);
		}
		else if(*(buf+11)=='o')
		{
			FilterConf fc(3,ss);
			v_filterconf.push_back(fc);
		}
		else if(*(buf+11)=='c')
		{
			FilterConf fc(4,ss);
			v_filterconf.push_back(fc);
		}
		else if(*(buf+11)=='u')
		{
			FilterConf fc(5,ss);
			v_filterconf.push_back(fc);
		}
		else if(*(buf+11)=='t')
		{
			FilterConf fc(6,ss);
			v_filterconf.push_back(fc);	
		}
		else if(*(buf+11)=='n')
		{
			FilterConf fc(7,ss);
			v_filterconf.push_back(fc);
		}
		else if(*(buf+11)=='e')
		{
			FilterConf fc(8,ss);
			v_filterconf.push_back(fc);
		}
		else if(*(buf+11)=='g')
		{
			FilterConf fc(9,ss);
			v_filterconf.push_back(fc);
		}
		else if(*(buf+11)=='p')
		{
			FilterConf fc(10,ss);
			v_filterconf.push_back(fc);
		}
		else if(*(buf+11)=='a')
		{
			FilterConf fc(11,ss);
			v_filterconf.push_back(fc);
		}
		else if(*(buf+11)=='f')
		{
			FilterConf fc(12,ss);
			v_filterconf.push_back(fc);
		}
            free(str);
        }
    }
    fclose(f);
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

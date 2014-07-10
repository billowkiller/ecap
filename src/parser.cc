 /*
 * =====================================================================================
 *
 *       Filename:  parser.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  12/29/2013 06:48:34 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  billowkiller (), billowkiller@gmail.com
 *   Organization:  
 *
 * =====================================================================================
 */

#include "parser.h"
#include "urlparser.h"
#include "qs_parse.h"


static URL storage;

int _page_type_(char *path)
{
	    if(strstr(path,"like"))
			 return LIKE;
		if(strstr(path, "notes"))
			 return NOTE;
		if(strstr(path, "photo")||strstr(path, "Photo"))
			 return PHOTO;
		if(strstr(path, "media_set"))
			 return MEDIA_SET;
		if(strstr(path, "friends"))
			 return FRIEND;
		if(strstr(path, "add_friend")||strstr(path,"ajax/eh.php"))
		     return ADD_FRIEND;
		if(strstr(path, "edit"))
			 return EDIT_NOTE;
		if(strstr(path, "comment"))
		     return COMMENT;
		 if(strstr(path, "updatestatus"))
			 return STATUS;
		 if(strstr(path,"media/saveunpublished"))
		     return UPLOAD_PHOTO;
	     if(strstr(path,"events"))
		 {
		     if(strstr(path,"join"))
             return JOIN_EVENT;
			 else
		     return EVENT;
		 }
	    if(strstr(path,"share"))
			return SHARE;
		if(strstr(path,"pages/fan_status"))
			return LIKE_PAGE;
		if(strstr(path,"search"))
			return SEARCH;
		if(strstr(path,"groups/membership/r2j"))
			return JOIN_GROUP;
		if(strstr(path,"groups/create_post"))
			return CREATE_GROUP;
		if(strstr(path,"mercury/send_messages"))
		    return CHAT;
	    if(strstr(path,"video/upload"))
		    return UPLOAD_VIDEO;
	    if(strstr(path,"plan")&&strstr(path,"create"))
	        return CREATE_EVENT;
		if(strstr(path,"hvideo"))
			return PLAY_VIDEO;
		return 0;
}

int parse_request_body(const char *body, struct connection_info* c_info)
{
	char *title;
	char *chat_sid;
    const char* at=body;
	int title_len;
	int detail_len;
	char* detail;
	//analysis
	if(c_info->user_id[0] == '\0')
	{
		qs_scanvalue("__user", at, c_info->user_id, sizeof(c_info->user_id));
		printf("user_id = %s\n", c_info->user_id);
	}
	switch(c_info->p_type)
	{
                case CREATE_EVENT:
			qs_scanvalue("title", at, c_info->comment, sizeof(c_info->comment));
                        title_len=strlen(c_info->comment);
                        detail_len=sizeof(c_info->comment)-title_len;
                        detail=(char*)malloc(detail_len);
			qs_scanvalue("details", at, detail, detail_len-1);
                        strcat(c_info->comment,detail);
                        free(detail);
                        break;
                case JOIN_EVENT:
			qs_scanvalue("eid", at, c_info->r_id, sizeof(c_info->r_id));
                        break;
		case COMMENT:
			qs_scanvalue("comment_text", at, c_info->comment, sizeof(c_info->comment));
			break;
		case ADD_FRIEND:
			qs_scanvalue("to_friend", at, c_info->s_id, sizeof(c_info->s_id));
			break;
		case NOTE:
			title = (char *)malloc(30);
			qs_scanvalue("title", at, title, 30);
			qs_scanvalue("note_content", at, c_info->comment, sizeof(c_info->comment));
			strcat(c_info->comment, title);
 			free(title);
			break;
		case STATUS:
			qs_scanvalue("xhpc_message_text", at, c_info->comment, sizeof(c_info->comment));
			break;
		case SHARE:
                        if(c_info->s_id[0]=='\0')
			qs_scanvalue("friendTarget", at, c_info->s_id, sizeof(c_info->s_id));
			qs_scanvalue("message_text", at, c_info->comment, sizeof(c_info->comment));
			break;
		case LIKE:
			//printf("%s,len:%d\n",at,strlen(c_info->r_id));
			qs_scanvalue("fbid", at, c_info->r_id, sizeof(c_info->r_id));	
			printf("%s,len:%d\n",at,strlen(c_info->r_id));
			if(c_info->r_id[0]=='\0')
			{
			qs_scanvalue("ft_ent_identifier", at, c_info->r_id, sizeof(c_info->r_id));
			
			}
			break;
        case LIKE_PAGE:
			qs_scanvalue("fbpage_id", at, c_info->s_id, sizeof(c_info->s_id));
			break;
        case JOIN_GROUP:
			printf("join a group\n");
			qs_scanvalue("group_id", at, c_info->r_id, sizeof(c_info->r_id));
			break;
        case CREATE_GROUP:
			qs_scanvalue("name", at, c_info->comment, sizeof(c_info->comment));
			break;
        case CHAT:
			printf("ok\n");
			chat_sid=(char*)malloc(25);
			qs_scanvalue("message_batch[0][specific_to_list][0]", at, chat_sid, 25);
			printf("still ok557chat_sid:%s\n",chat_sid);
			if(strlen(chat_sid)>5)
			memcpy(c_info->s_id, chat_sid+5,strlen(chat_sid)-5);
			printf("still ok589\n");
            c_info->s_id[strlen(chat_sid)-5]='\0';
			qs_scanvalue("message_batch[0][body]", at, c_info->comment, sizeof(c_info->comment));
			printf("still ok\n");
                        free(chat_sid);
			break;
	}
	
	//added 
	if((c_info->p_type==WELCOME||c_info->p_type==NEWSFEED)&&c_info->s_id[0] =='\0')
					{
						int len =strlen(c_info->user_id);
						memcpy(c_info->s_id,c_info->user_id , len);
						c_info->s_id[len]= '\0';
					}
					
					
					
	return 0;
}

int parse_cookie(const char *cookie, struct connection_info* c_info)
{
	if(c_info->user_id[0]=='\0')
 	{
		char* temp=const_cast<char*>(cookie);
 		char *start = strstr(temp, "c_user");
		if(start)
		{
			int len = strchr(start+7, ';')-start-7;
			memcpy(c_info->user_id, start+7, len);
			c_info->user_id[len] = '\0';
	    }
					
	}
	return 0;
}

int parse_url(const char *url, struct connection_info* c_info)
{
	//printf("............url = %s.........\n", url);
	char *pos, *path;
	int len=0;
	
	
	/*
	 * //===start== add by yangbr, 2014.1.10
//bbc news parse
	memset(bbc_news_id,0,10);
	if(strstr(url,"/news/")==url)
	{
		printf("parsing bbc news\n");
		char *bbcpos; 
		bbcpos = url;
		while(*bbcpos<'0'||*bbcpos>'9')  //find the start of news id
		{
			bbcpos++;
		}
		memcpy(bbc_news_id,bbcpos,8);
		bbc_news_id[8]='\0';
		printf("bbc news id:%s\n",bbc_news_id);
	}
//bbc pic parse
	bbc_p_info.flag=0;
	if( strstr( url,"/media/images/")==url)// && strstr(url,"jpg")==(url+23) )
	{
		printf("parsing bbc pic\n");
		char *bbcpos;
		bbcpos = url;
		bbc_p_info.flag=1;

		bbcpos = strchr(bbcpos,'.');
		while(*bbcpos != '/')
		{
			bbcpos--;
		}
		bbcpos += 2;
		memcpy(bbc_p_info.nid,bbcpos,5);
		memcpy(bbc_p_info.id,bbcpos,8);
		bbc_p_info.nid[5]='\0';
		bbc_p_info.id[8]='\0';
		
		printf("bbc picture nid is: %s\n",bbc_p_info.nid);
		printf("bbc picture id  is: %s\n",bbc_p_info.id);
	}
//===end==== add by yangbr, 2014.1.10
	*/
	//parse url
	parseURL(url, &storage);
	storage.path.start += 1;  //remove '/'
	if(storage.path.start == storage.path.end)
		return 0;
	
	path = readURLField(url, storage.path);
	//avoid referer url check
	if(c_info->p_type == 0)
		c_info->p_type = _page_type_(path);

	//modify me, stupid implemention of s_id
	int path_len;
	path_len=strlen(path);
	int i=-1;
	int r_id_len=0;
	char* temp;
	if(c_info->p_type==EVENT)
	{
		if(path[path_len-1]>='/'&&path[path_len-2]>='0'&&path[path_len-2]<='9')
		{
			i=path_len-3;
			r_id_len=1;
		}
	}
	else if(c_info->p_type==NOTE)
	{
		if(path[path_len-1]>='0'&&path[path_len-1]<='9')
		{
			i=path_len-2;
			r_id_len=1;
		}
	}
	while(path[i]>='0'&&path[i]<='9'&&i>=0)
	{
		i--;
		r_id_len++;
	}
	if(r_id_len>10&&r_id_len<52)
	{	
		temp=path+(i+1);
		strncpy(c_info->r_id,temp,r_id_len);
		c_info->r_id[r_id_len]='\0';
	}
        if(c_info->p_type==PLAY_VIDEO&&*(path+strlen(path)-4)=='.')
        {
             char* endV=path+strlen(path)-5;
             char* startV;
             if(*endV=='n')
             {
                  endV--;
                  if(*endV=='_')
                  {
                      do{
                        endV--;
                       }
                      while(*endV!='_');
                     startV=endV;
                      do{
                        startV--;
                       }
                      while(*startV!='_');
                     startV++;
                      int lenV=endV-startV;
                      strncpy(c_info->r_id,startV,lenV);
                       printf("%s\n",c_info->r_id);
                  }
             }
            
        }



	if(!strncmp(path+(strlen(path)-6),"_n.jpg",6))
	{
		printf("matched jpg\n");
		int resource_id_length;
		char* resource_id;
		char* position;
		position = path + strlen(path) - 7;
		while(*position!='_'&& *position!='/'){
			position--;
		}
		char* ptemp=position;
		position--;
		while(*position!='_'&& *position!='/'){
			 position--;
		}
		if(*position == '_'){
			position++;
			resource_id = position;
			resource_id_length = ptemp - position;
			printf("length=%d\n",resource_id_length);
			memcpy(c_info->r_id,resource_id,resource_id_length);
			c_info->r_id[resource_id_length]='\0';
			printf("%s\n",c_info->r_id);
		}
		
		FREE(path);
	}
	else if((pos = strchr(path, '/')) == strrchr(path, '/'))
	{
		if(!pos)
		{
			strcpy(c_info->s_id, path);
			FREE(path);
		}
		else
		{
			memcpy(c_info->s_id,path, pos-path);
			c_info->s_id[pos-path] = '\0';
			FREE(path);
		}
		if(strlen(c_info->s_id) > 4 && !strcmp(c_info->s_id+ strlen(c_info->s_id) - 4, ".php"))
		{
			bzero(c_info->s_id, sizeof(c_info->s_id));
			c_info->s_id[0] = '\0';
		}
	}else
		FREE(path);
//	regex_match("(\\w+\\.)+\\w+", path, &pos, &len);   //regex [\w+\.]+, not endwith php
//	if(len != 0 && !strstr(path, "php"))  //need modify
//	{
//		printf("pos = %s, len=%d\n", pos, len);
//	    strcpy(c_info->s_id, path);
//		printf("subject name:%s\n", c_info->s_id);
//	}

	if(storage.query.end != storage.query.start)
	{
		char * query = readURLField(url, storage.query);
		if(c_info->p_type == 0)
		{
			if(strstr(query, "sk=nf"))
				c_info->p_type = NEWSFEED;
			if(strstr(query, "sk=welcome"))
				c_info->p_type = WELCOME;
		}
		qs_scanvalue("__user", query, c_info->user_id, sizeof(c_info->user_id));
		qs_scanvalue("q", query, c_info->comment, sizeof(c_info->comment));
        char media_temp[100];
               char* photo_data;
	   
		switch(c_info->p_type)
		{
                        case SHARE:
                                if(c_info->s_id[0]=='\0')
				qs_scanvalue("profile_id", query, c_info->s_id, sizeof(c_info->s_id));
                                if(c_info->r_id[0]=='\0')
				qs_scanvalue("p[0]", query, c_info->r_id, sizeof(c_info->r_id));
                                if(c_info->r_id[0]!='\0'&&!strcmp(c_info->r_id,c_info->s_id))
                                {
                                memset(c_info->r_id,0,sizeof(c_info->r_id));
				qs_scanvalue("p[1]", query,c_info->r_id, sizeof(c_info->r_id));
                                }
                                break; 
			case MEDIA_SET:
			    {
				qs_scanvalue("set", query, media_temp, sizeof(media_temp));
			    char* media_start=strchr(media_temp,'.');
				media_start++;
				char* media_end=strchr(media_start,'.');
				int media_len=media_end-media_start;
			    memcpy(c_info->r_id,media_start,media_len);
				c_info->r_id[media_len]='\0';
				}
				break;
			case PHOTO:
			{
				photo_data=(char*)malloc(300);
				photo_data[0]='\0';
                                printf("OK\n");
				qs_scanvalue("data", query, photo_data, 300);
				char* photo_rid=strstr(photo_data,"fbid");
				char* photo_set=strstr(photo_data,"set");

				if(photo_data[0]!='\0'&&photo_rid&&photo_set)
				{
			        printf("data:%s\n",photo_data);	
				printf("OK\n");
                             
				photo_rid+=5;
				if(*photo_rid==':')
                                {
				printf("OK\n");
                                    photo_rid+=2;
				printf("OK First\n");
                                    char* photo_rid_end= strchr(photo_rid,'"');
				    int photo_rid_len=photo_rid_end-photo_rid;
				    printf("rid_len is :%d\n",photo_rid_len);
                                    if(photo_rid_len>0)
                                    {                         
                                    memcpy(c_info->r_id,photo_rid,photo_rid_len);
				    c_info->r_id[photo_rid_len]='\0';
                                    }
                                }
                                char* photo_sid=strchr(photo_set,'.');
				photo_sid++;
				if(*photo_sid>='0'&&*photo_sid<='9')
                                {
                               
                                    char* photo_sid_end= strchr(photo_sid,'.');
				    int photo_sid_len=photo_sid_end-photo_sid;
                                    if(photo_sid_len>0)
                                    {                         
                                    memcpy(c_info->s_id,photo_sid,photo_sid_len);
				    c_info->r_id[photo_sid_len]='\0';
                                    }
                               }
                               }
                                free(photo_data);
                               if(c_info->r_id[0]=='\0')
				qs_scanvalue("fbid", query, c_info->r_id, sizeof(c_info->r_id));
              }                 
				break;
			case UPLOAD_PHOTO:
			    qs_scanvalue("target_id",query,c_info->s_id,sizeof(c_info->s_id));
		       break;
			case SEARCH:
			    qs_scanvalue("value",query,c_info->comment,sizeof(c_info->comment));
                        break;
                        case PLAY_VIDEO:
                            if(c_info->r_id[0]=='\0')       
			    qs_scanvalue("video_id",query,c_info->r_id,sizeof(c_info->r_id));
                       break;
		}
		FREE(query);
	}
	return 0;
}


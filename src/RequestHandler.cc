#include <string>
#include "RequestHandler.h"
#include <stdlib.h>
#include <string.h>
#include "parser.h"
#include <iostream>
#include "SQLFactory.h"
#include "StrMatcher.h"
#include <ctime>


void RequestHandler::_init_c_info()
{
	bzero(&c_info, sizeof(c_info));
	c_info.user_id[0] = '\0';
	c_info.s_id[0] = '\0';
	c_info.r_id[0] = '\0';	
	c_info.comment[0] = '\0'; 
	c_info.p_type = 0;
	
}
bool RequestHandler::checkValidation()
{
	 
     _init_c_info();
	 	 
	
     parse_url(request_info.url.c_str(),&c_info);
	
	 
	 parse_cookie(request_info.cookie.c_str(),&c_info);
	 
	 
     parse_request_body(request_info.content.c_str(),&c_info);
	
	 //
	 
	 
			if(StrMatcher::instance().kwMatch(c_info.comment))
			{
		//		post_H.head_length -= http_len;
				
//printf("keywords match\n");
				return false;
			}
			
	 
	 //
			
			
			
			
			RuleHandler rulehandler;
           char info_p_type[64];
		   sprintf(info_p_type,"%d",c_info.p_type);
			if(rulehandler.chkRule(c_info.user_id,"",info_p_type,"","",c_info.r_id,"")||rulehandler.chkRule(c_info.user_id,"",info_p_type,"","",c_info.s_id,""))
			{			
				//write log first
				LogTable lt;
				lt.UserId=c_info.user_id;
				lt.Action=info_p_type;
				lt.ResourceId=c_info.s_id;
				
				char time_str[30];
	            time_t now_time;
                now_time = time(NULL);
	            struct tm *p;
	            p=localtime(&now_time);
	            sprintf(time_str,"%d.%d.%d,%d:%d:%d", (1900+p->tm_year),(1+p->tm_mon),p->tm_mday, p->tm_hour, p->tm_min, p->tm_sec);
	            lt.LogTime=time_str;
	            Log mylog;
				mylog.addLog(lt);
				
				
				return false;
			}
            
	return true;
}



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
		   LogTable lt;
		   int ruleFlag=0;
		  
           if(c_info.r_id[0]=='\0')
		   {
			   strcpy(c_info.r_id,"NotFound");
		   }
		   if(c_info.s_id[0]=='\0')
		   {
			   strcpy(c_info.s_id,"NotFound");
		   }
		   if(c_info.user_id[0]=='\0')
		   {
			   strcpy(c_info.user_id,"NotFound");
		   }



           if(rulehandler.chkRule(c_info.user_id,"",info_p_type,"","",c_info.r_id,""))
			{
				lt.ResourceId=c_info.r_id;
				ruleFlag=1;
			}
			else if(rulehandler.chkRule(c_info.user_id,"",info_p_type,"","",c_info.s_id,""))
			{			
				//write log first
				lt.ResourceId=c_info.s_id;
				ruleFlag=1;
					
			}
			if(ruleFlag)
			{
				lt.UserId=c_info.user_id;
				lt.Action=info_p_type;
				char time_str[30];
	            time_t now_time;
                now_time = time(NULL);
	            struct tm *p;
	            p=localtime(&now_time);
	            sprintf(time_str,"%d.%d.%d,%d:%d:%d", (1900+p->tm_year),(1+p->tm_mon),p->tm_mday, p->tm_hour, p->tm_min, p->tm_sec);
	            lt.LogTime=time_str;
				Pz_idTable idtable;
				Pz_id myPz_id;
				
				idtable=myPz_id.getPz_idByTuple(lt.UserId,lt.ResourceId,lt.Action);
				
				lt.PZConfigId=idtable.ConfigId;
				lt.PZOperator=idtable.PZOperator;
				lt.PZOperateTime=idtable.PZOperateTime;
				lt.PZControlTimeFrom=idtable.PZControlTimeFrom;
				lt.PZControlTimeTo=idtable.PZControlTimeTo;
				lt.PZControlType=idtable.PZControlType;
				lt.SNSType=idtable.PZSNSType;
				
	            Log mylog;
				
				mylog.addLog(lt);
				return false;
			}
            
	return true;
}



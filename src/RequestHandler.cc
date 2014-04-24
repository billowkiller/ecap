#include <string>
#include "RequestHandler.h"
#include <stdlib.h>
#include <string.h>
#include "parser.h"
#include "kw_match.h"
#include "sqlite.h"
#include <iostream>


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
	 //ignore referrer
     parse_request_body(request_info.content.c_str(),&c_info);
	 std::cout<<"type:"<<c_info.p_type<<"u:"<<c_info.user_id<<"s:"<<c_info.s_id<<"r:"<<c_info.r_id<<"c:"<<c_info.comment<<std::endl;
	 
	 if(c_info.url)
			{
//printf("url match\n");
				return false;
			}
			if(kw_match(c_info.comment))
			{
		//		post_H.head_length -= http_len;
				
//printf("keywords match\n");
				return false;
			}
			int n = find_db(c_info.user_id, c_info.s_id, c_info.p_type, c_info.r_id);
			if(n)
			{
				
				return false;
			}


	return true;
}



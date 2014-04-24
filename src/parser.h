#ifndef PARSER_INCLUDED
#define PARSER_INCLUDED

#define FREE(X) if((X)) {free((X)); (X)=NULL;}
#define PR(X, Y) printf(#X " = " Y "\n", X ) //i=1; PR(i, "%d")
#define PS(X) printf(#X " = %s\n", X ) //PR(str)
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define MIN(a, b) ((a) < (b) ? (a) : (b))


#ifndef WEBTYPE
#define WEBTYPE
	#define FRIEND 1
	#define STATUS 2
	#define NOTE 3
	#define COMMENT 4
	#define PHOTO 5
	#define MEDIA_SET 6
	#define ADD_FRIEND 7
	#define EDIT_NOTE 8
    #define UPLOAD_PHOTO 9
    #define EVENT 10
    #define NEWSFEED 11
    #define WELCOME 12
    #define SHARE 13
    #define LIKE 14
    #define LIKE_PAGE 15
    #define SEARCH 16
    #define JOIN_GROUP 17
    #define CREATE_GROUP 18
    #define CHAT 19
    #define UPLOAD_VIDEO 20
    #define JOIN_EVENT 21
    #define CREATE_EVENT 22
    #define PLAY_VIDEO 23
#endif
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "kw_match.h"

struct connection_info{
        	char user_id[20];
        	char s_id[20]; //visitor view subject's page
        	int p_type; /* page type */
        	char r_id[52]; /* resource id */
        	char comment[100]; /* post content */
        	int url;
};

int parse_cookie(const char *cookie,struct connection_info* c_info);
int parse_url(const char *url,struct connection_info* c_info);
int parse_request_body(const char *body,struct connection_info* c_info);


#endif



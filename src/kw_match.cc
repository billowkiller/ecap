/*
 * =====================================================================================
 *
 *       Filename:  kw_match.c
 *
 *    Description:  check whether string contains keywords
 *
 *        Version:  1.0
 *        Created:  08/04/2013 08:46:19 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  outao
 *   Organization:  
 *
 * =====================================================================================
 */


#include "kw_match.h"
#include "ac.h"
#define MAXWORD 30
#define MAXURL 500

/*

void add_url_list(char * kw)
{
	g_url_list = g_list_append(g_url_list, kw);
}
void add_kw_list(char * kw)
{
	g_kw_list = g_list_append(g_kw_list, kw);
}

void read_kw_file()
{
	FILE *file = fopen("/home/billowkiller/ecap/src/keywords", "r");
	while(TRUE)
	{
		char *word = (char *)malloc(MAXWORD);
		int n = fscanf(file, "%s", word);
		if(n > 0)
			add_kw_list(word);
		else
			break;
	}
	fclose(file);
}

void read_url_file()
{
	FILE *file = fopen("/opt/lampp/htdocs/FBControlUI/rulecontrol/facebookRuleControl/url.txt", "r");
	while(TRUE)
	{
		char *word = (char *)malloc(500);
		int n = fscanf(file, "%s", word);
		if(n > 0)
			add_url_list(word);
		else
			break;
	}
	fclose(file);
}



char* _fileRead(char *filename)
{
	long file_length;
	FILE* file = fopen(filename, "r");
	if (file == NULL) {
	  perror("fopen");
	}

	fseek(file, 0, SEEK_END);
	file_length = ftell(file);
	if (file_length == -1) {
	  perror("ftell");
	}
	fseek(file, 0, SEEK_SET);
	char* data = (char*)malloc(file_length);
	if (fread(data, 1, file_length, file) != (size_t)file_length) {
	  fprintf(stderr, "couldn't read entire file\n");
	  free(data);
	}
	return data;
}
*/
void free_string(void* data)
{
	free((char*)data);
}

int url_match(char *str)
{
	GList *g_url_list = NULL;
	FILE *file = fopen("/opt/lampp/htdocs/FBControlUI/rulecontrol/facebookRuleControl/url.txt", "r");
	while(TRUE)
	{
		char *word = (char *)malloc(500);
		int n = fscanf(file, "%s", word);
		if(n > 0)
			g_url_list=g_list_append(g_url_list, word);
		else
			break;
	}
	fclose(file);
	
	
	
	
	if(str[0]=='\0')
		return 0;
	//printf("str = %s\n", str);
	GList *iterator = NULL;
	for (iterator = g_url_list; iterator; iterator = iterator->next)
		 if(strstr(str, (char*)iterator->data))
		 {
			 printf("url = %s\n", (char *)iterator->data);
			 g_list_free_full(g_url_list,free_string);
			 return 1;
		 }
		 
    g_list_free_full(g_url_list,free_string);
	return 0;
}
int kw_match(char *str)
{
	int rt=0;
	GList *g_kw_list = NULL;
	FILE *file = fopen("/home/billowkiller/ecap/src/keywords", "r");
	while(TRUE)
	{
		char *word = (char *)malloc(MAXWORD);
		int n = fscanf(file, "%s", word);
		if(n > 0)
			g_kw_list = g_list_append(g_kw_list, word);
		else
			break;
	}
	fclose(file);
	
	
	if(str[0]=='\0')
	{
		g_list_free_full(g_kw_list,free_string);
		return 0;
	}
		
	printf("str = %s\n", str);
	initial_goto(g_kw_list);
    buildFail();
	if(ac(str))
		rt=1;
	/*
	GList *iterator = NULL;
	for (iterator = g_kw_list; iterator; iterator = iterator->next)
		 if(strstr(str, (char*)iterator->data))
		 {
			 printf("data = %s\n", (char *)iterator->data);
			 g_list_free_full(g_kw_list,free_string);
			 return 1;
		 }
	*/
	g_list_free_full(g_kw_list,free_string);	 
	return rt;
}

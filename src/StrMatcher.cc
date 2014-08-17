#include "StrMatcher.h"
#include "RuleAccepter.h"

ACSM_STRUCT * StrMatcher::A=NULL;
ACSM_STRUCT * StrMatcher::B=NULL;

StrMatcher::StrMatcher()
{
	A=acsmNew();
	B=acsmNew();
	KeywordHandler kh;
	addMultiStr(kh.getAllKeywords());
	RuleAccepter::startAccept();
}
bool StrMatcher::addStr(const string s)
{
	int len=s.length();
	char *word=(char*)malloc(len+1);
	memset(word,0,len+1);
	s.copy(word,len,0);
	acsmAddPattern (B, (unsigned char*)word, strlen(word));
	free(word);
	A->acsmPatterns=B->acsmPatterns;
	acsmCompile(B);
	
	ACSM_STRUCT * C=A;
	A=B;
	B=C;
	return 1;
}

bool StrMatcher::delStr(const string s)
{
	int len=s.length();
    char *word=(char*)malloc(len+1);
	memset(word,0,len+1);
	s.copy(word,len,0);
	acsmDelPattern(B,word,strlen(word));
	free(word);
	A->acsmPatterns=B->acsmPatterns;
	acsmCompile(B);
	
	ACSM_STRUCT * C=A;
	A=B;
	B=C;
	return 1;
}
bool StrMatcher::addMultiStr(const vector<string> v)
{
	int i;
	for(i=0;i<v.size();i++)
	{
		int len=v[i].length();
	    char *word=(char*)malloc(len+1);
	    memset(word,0,len+1);
	    v[i].copy(word,len,0);
	    acsmAddPattern (B, (unsigned char*)word, strlen(word));
	    free(word);
	}
	
	A->acsmPatterns=B->acsmPatterns;
	acsmCompile(B);
	
	ACSM_STRUCT * C=A;
	A=B;
	B=C;
	
	return 1;
}
bool StrMatcher::delMultiStr(const vector<string> v)
{
	int i;
	for(i=0;i<v.size();i++)
	{
		int len=v[i].length();
	    char *word=(char*)malloc(len+1);
	    memset(word,0,len+1);
	    v[i].copy(word,len,0);
	    acsmDelPattern (B, word, strlen(word));
	    free(word);
	}
	A->acsmPatterns=B->acsmPatterns;
	acsmCompile(B);
	
	ACSM_STRUCT * C=A;
	A=B;
	B=C;
	return 1;
}
StrMatcher& StrMatcher::instance()
{
	static StrMatcher strmatcher;
	return strmatcher;
}

int  StrMatcher::kwMatch(const string s)
{
	int len=s.length();
	char *str=(char*)malloc(len+1);
	memset(str,0,len+1);
	s.copy(str,len,0);
	int found=acsmSearch (A, (unsigned char*)str, strlen (str));
	free(str);
	return found;
	
}





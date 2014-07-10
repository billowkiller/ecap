#include "SQLFactory.h"
#include "StrMatcher.h"
KeywordHandler::KeywordHandler()
{
	
}
vector<string> KeywordHandler::getAllKeywords()
{
	string sql="select keyword from keywordtable";
	int nrow=0,ncolumn;
	char *zErrMsg;
    char **azResult;
	sqlite3_get_table(DBHandler::instance()->db, sql.c_str(), &azResult, &nrow, &ncolumn, &zErrMsg);
	vector<string> v;
	string keyword;
	if(ncolumn==1)
	{
		int i;
		for(i=1;i<=nrow;i++)
		{
		keyword=azResult[i];
		v.push_back(keyword);
		}
	}
	return v;
	
}
bool KeywordHandler::addKeyword(string Keyword, string StreamType, string ControlTimeFrom, string ControlTimeTo)
{
	vector<pair<string,string> > v;
	if(!Keyword.empty())
		v.push_back(make_pair("Keyword",Keyword));
	if(!StreamType.empty())
		v.push_back(make_pair("StreamType",StreamType));
	if(!ControlTimeFrom.empty())
		v.push_back(make_pair("ControlTimeFrom",ControlTimeFrom));
	if(!ControlTimeTo.empty())
		v.push_back(make_pair("ControlTimeTo",ControlTimeTo));
	
	string sql=sqlFactory.makeAddSQL(v);
	
	StrMatcher::instance().addStr(Keyword);
	
	return DBHandler::instance()->writeRecord(sql);
	
	
}

bool KeywordHandler::addKeyword(string rule, int len)
{
	//rule like this:K < Keyword ControlTimeFrom controlTimeTo
	string keyword,streamtype,controltimefrom,controltimeto;
	int i=4;
    int j;
    j=rule.find(' ',i);
    keyword.assign(rule,i,j-i);
	
	i=j+1;
    j=rule.find(' ',i);
    streamtype.assign(rule,i,j-i);
	
	i=j+1;
    j=rule.find(' ',i);
    controltimefrom.assign(rule,i,j-i);
	
	i=j+1;
    j=len+1;
    controltimeto.assign(rule,i,j-i);
	
	addKeyword(keyword,streamtype,controltimefrom,controltimeto);
	return 1;

}
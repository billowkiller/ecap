#ifndef SQLFACTORY_INCLUDED
#define SQLFACTORY_INCLUDED
#include <iostream>
#include <vector>
#include <map>
#include <string>
#include <sqlite3.h>

using namespace std;


class SQLFactory{
public:
	SQLFactory(string chk,string add,string del);
	
	//static map<string,vector<string>> tableSchemes;//do not needed

	string makeAddSQL(vector<pair<string,string> >&);
	//string makeAddSQL(vector<string>&);
	//string makeModSQL(vector<pair<string,string>>&);
	//string makeModSQL(vector<string>&);
	string makeDelSQL(vector<pair<string,string> >&);
	//string makeDelSQL(vector<string>&);
	string makeChkSQL(vector<pair<string,string> >&);
	//string makeChkSQL(vector<string>&);
	string makeRuleChkSQL(vector<pair<string,string> >&);
  
private:
	const string chkSQLHeader;
	const string addSQLHeader;
    const string delSQLHeader;//they are not static
	
	string makeConditionSQL(vector<pair<string,string> >&);
    string makeRuleConditionSQL(vector<pair<string,string> >&);
};

class DBHandler{
public:
	//SQLFactory sqlFactory;
	//vector<string> readRecord(string);
	bool writeRecord(string);
	bool checkRecord(string);
	bool execSQL(string);
	static DBHandler* instance(); 
	sqlite3 *db;
private:
	
	static DBHandler* m_instance;
	
	DBHandler();
	~DBHandler();
};

class RuleHandler{
public:
	RuleHandler();
	bool addRule(string Subject,string Sname,string Action,string ControTimeFrom,string ControlTimeTo,string ContentId,string ContentType);
	bool addRule(string,int);
	//bool modifyRule(const string);
	bool delRule(string Id);//param id
	bool chkRule(string Subject,string Sname,string Action,string ControTimeFrom,string ControlTimeTo,string ContentId,string ContentType);
	
private:
	//bool addTimer(ptime,function,string[],opreatorType);
	//void dataTransform();
	//static DBHandler* dbHandler;
	SQLFactory sqlFactory=SQLFactory("select * from idblacklist","insert into idblacklist","delete from idblacklist");
};

class LogTable{
public:
	string PZConfigId;
	string UserId;
	string Username;
	string ResourceId;
	string ResourceType;
	string LogTime;
	string Keyword;
	string Action;
	string SNSType;
	string ClientIP;
	string ServerIP;
	string PZOperator;
	string PZOperateTime;
	string PZControlTimeFrom;
	string PZControlTimeTo;
	string PZControlType;
};
class Log{
public:
	Log();
	bool addLog(LogTable logtable);
private:
	SQLFactory sqlFactory=SQLFactory("select * from LogTable","insert into LogTable","delete from LogTable");
};

class KeywordHandler{
public:
	KeywordHandler();
	bool addKeyword(string Keyword, string StreamType, string ControlTimeFrom, string ControlTimeTo);
	bool addKeyword(string,int);
	vector<string> getAllKeywords();
private:
	SQLFactory sqlFactory=SQLFactory("select * from keywordtable","insert into keywordtable","delete from keywordtable");
};
class Pz_kwTable{
public:
	string PZConfigId;
	string PZControlContent;
	string PZOperator;
	string PZOperateTime;
	string PZControlTimeFrom;
	string PZControlTimeTo;
	string PZControlType;
	string PZStreamType;	
};
class Pz_idTable{
public:
	string ConfigId;
	string PZUserID;
	string PZResourceID;
	string PZAction;
	string PZOperator;
	string PZOperateTime;
	string PZControlTimeFrom;
	string PZControlTimeTo;
	string PZControlType;
	string PZSNSType;
};
class Pz_kw{
public:
	bool addPz_kw(Pz_kwTable&);
	bool addPz_kw(string,int);
	bool delPz_kw(Pz_kwTable&);
    Pz_kwTable getPz_kwByKw(string);
	
private:
	//SQLFactory sqlFactory=SQLFactory("select * from pzxftable_keyword","insert into pzxftable_keyword","delete from pzxftable_keyword");
	SQLFactory sqlFactory=SQLFactory("select PZConfigId,PZControlContent,PZOperator,PZOperateTime,PZControlTimeFrom,PZControlTimeTo,PZControlType,PZStreamType from pzxftable_keyword","insert into pzxftable_keyword","delete from pzxftable_keyword");
	
};
class Pz_id{
public:
	bool addPz_id(Pz_idTable&);
	bool addPz_id(string,int);
	bool delPz_id(Pz_idTable&);
	
private:
	SQLFactory sqlFactory=SQLFactory("select * from pzxftable_id","insert into pzxftable_id","delete from pzxftable_id");
	
};


#endif

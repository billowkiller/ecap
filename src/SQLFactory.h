#ifndef SQLFACTORY_INCLUDED
#define SQLFACTORY_INCLUDED

#include <iostream>
#include <vector>
#include <string>
#include <sqlite3.h>

class SQLFactory{
	
public:
	SQLFactory(std::string chk,std::string add,std::string del);
	
	//static map<std::string,std::vector<std::string>> tableSchemes;//do not needed

	std::string makeAddSQL(std::vector<std::pair<std::string,std::string> >&);
	//std::string makeAddSQL(std::vector<std::string>&);
	//std::string makeModSQL(std::vector<std::pair<std::string,std::string>>&);
	//std::string makeModSQL(std::vector<std::string>&);
	std::string makeDelSQL(std::vector<std::pair<std::string,std::string> >&);
	//std::string makeDelSQL(std::vector<std::string>&);
	std::string makeChkSQL(std::vector<std::pair<std::string,std::string> >&);
	//std::string makeChkSQL(std::vector<std::string>&);
	std::string makeRuleChkSQL(std::vector<std::pair<std::string,std::string> >&);
  
private:
	const std::string chkSQLHeader;
	const std::string addSQLHeader;
    const std::string delSQLHeader;//they are not static
	
	std::string makeConditionSQL(std::vector<std::pair<std::string,std::string> >&);
    std::string makeRuleConditionSQL(std::vector<std::pair<std::string,std::string> >&);
};

class DBHandler{
	
public:
	//SQLFactory sqlFactory;
	//std::vector<std::string> readRecord(std::string);
	bool writeRecord(std::string);
	bool checkRecord(std::string);
	bool execSQL(std::string);
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
	bool addRule(std::string Subject, std::string Sname, std::string Action, std::string ControTimeFrom, std::string ControlTimeTo, std::string ContentId, std::string ContentType);
	bool addRule(std::string,int);
	//bool modifyRule(const std::string);
	bool delRule(std::string Subject, std::string Sname, std::string Action, std::string ControTimeFrom, std::string ControlTimeTo, std::string ContentId, std::string ContentType);
	bool delRule(std::string Id);//param id
	bool chkRule(std::string Subject,std::string Sname,std::string Action,std::string ControTimeFrom,std::string ControlTimeTo,std::string ContentId,std::string ContentType);
	
private:
	//bool addTimer(ptime,function,std::string[],opreatorType);
	//void dataTransform();
	//static DBHandler* dbHandler;
	SQLFactory sqlFactory=SQLFactory("select * from idblacklist","insert into idblacklist","delete from idblacklist");
};

class LogTable{
	
public:
	std::string PZConfigId;
	std::string UserId;
	std::string Username;
	std::string ResourceId;
	std::string ResourceType;
	std::string LogTime;
	std::string Keyword;
	std::string Action;
	std::string SNSType;
	std::string ClientIP;
	std::string ServerIP;
	std::string PZOperator;
	std::string PZOperateTime;
	std::string PZControlTimeFrom;
	std::string PZControlTimeTo;
	std::string PZControlType;
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
	bool addKeyword(std::string Keyword, std::string StreamType, std::string ControlTimeFrom, std::string ControlTimeTo);
	bool addKeyword(std::string,int);
	std::vector<std::string> getAllKeywords();
	bool delKeyword(std::string Keyword, std::string StreamType, std::string ControlTimeFrom, std::string ControlTimeTo);
private:
	SQLFactory sqlFactory=SQLFactory("select * from keywordtable","insert into keywordtable","delete from keywordtable");
};
class Pz_kwTable{
	
public:
	std::string PZConfigId;
	std::string PZControlContent;
	std::string PZOperator;
	std::string PZOperateTime;
	std::string PZControlTimeFrom;
	std::string PZControlTimeTo;
	std::string PZControlType;
	std::string PZStreamType;	
};
class Pz_idTable{
	
public:
	std::string ConfigId;
	std::string PZUserID;
	std::string PZResourceID;
	std::string PZAction;
	std::string PZOperator;
	std::string PZOperateTime;
	std::string PZControlTimeFrom;
	std::string PZControlTimeTo;
	std::string PZControlType;
	std::string PZSNSType;
};
class Pz_kw{
	
public:
	bool addPz_kw(Pz_kwTable&);
	bool addPz_kw(std::string,int);
	bool delPz_kw(Pz_kwTable&);
    Pz_kwTable getPz_kwByKw(std::string);
	
private:
	//SQLFactory sqlFactory=SQLFactory("select * from pzxftable_keyword","insert into pzxftable_keyword","delete from pzxftable_keyword");
	SQLFactory sqlFactory=SQLFactory("select PZConfigId,PZControlContent,PZOperator,PZOperateTime,PZControlTimeFrom,PZControlTimeTo,PZControlType,PZStreamType from pzxftable_keyword","insert into pzxftable_keyword","delete from pzxftable_keyword");
	
};
class Pz_id{
	
public:
	bool addPz_id(Pz_idTable&);
	bool addPz_id(std::string,int);
	bool delPz_id(Pz_idTable&);
	Pz_idTable getPz_idByTuple(std::string&, std::string&, std::string&);
	
private:
	SQLFactory sqlFactory=SQLFactory("select PZUserID,PZResourceID,PZAction,PZOperator,PZOperateTime,PZControlTimeFrom,PZControlTimeTo,PZControlType,PZSNSType,ConfigId from pzxftable_id","insert into pzxftable_id","delete from pzxftable_id");
	
};


#endif

#include "SQLFactory.h"

Log::Log()
{
	//sqlFactory=SQLFactory("select * from Log","insert into Log","delete from Log");
}
bool Log::addLog(LogTable logtable)
{
	vector<pair<string,string> > v;
	    if(!logtable.PZConfigId.empty())
		v.push_back(make_pair("PZConfigId",logtable.PZConfigId));
		if(!logtable.UserId.empty())
		v.push_back(make_pair("UserId",logtable.UserId));
		if(!logtable.Username.empty())
		v.push_back(make_pair("Username",logtable.Username));
		if(!logtable.ResourceId.empty())
	    v.push_back(make_pair("ResourceId",logtable.ResourceId));
		if(!logtable.ResourceType.empty())
	    v.push_back(make_pair("ResourceType",logtable.ResourceType));
		if(!logtable.LogTime.empty())
		v.push_back(make_pair("LogTime",logtable.LogTime));
		if(!logtable.Keyword.empty())
		v.push_back(make_pair("Keyword",logtable.Keyword));
		if(!logtable.Action.empty())
	    v.push_back(make_pair("Action",logtable.Action));
		if(!logtable.SNSType.empty())
		v.push_back(make_pair("SNSType",logtable.SNSType));
		if(!logtable.ClientIP.empty())
		v.push_back(make_pair("ClientIP",logtable.ClientIP));
		if(!logtable.ServerIP.empty())
		v.push_back(make_pair("ServerIP",logtable.ServerIP));
		if(!logtable.PZOperator.empty())
	    v.push_back(make_pair("PZOperator",logtable.PZOperator));
		if(!logtable.PZOperateTime.empty())
		v.push_back(make_pair("PZOperateTime",logtable.PZOperateTime));
		if(!logtable.PZControlTimeFrom.empty())
		v.push_back(make_pair("PZControlTimeFrom",logtable.PZControlTimeFrom));
		if(!logtable.PZControlTimeTo.empty())
		v.push_back(make_pair("PZControlTimeTo",logtable.PZControlTimeTo));
		if(!logtable.PZControlType.empty())
	    v.push_back(make_pair("PZControlType",logtable.PZControlType));
	
	string sql=sqlFactory.makeAddSQL(v);
	
	DBHandler::instance()->writeRecord(sql);
}
#include "SQLFactory.h"

RuleHandler::RuleHandler()
{
	//sqlFactory=SQLFactory("select * from idblacklist","insert into idblacklist","delete from idblacklist");
}



bool RuleHandler::addRule(string Subject,string Sname,string Action,string ControlTimeFrom,string ControlTimeTo,string ContentId,string ContentType)
{
	vector<pair<string,string> > v;
	if(!Subject.empty())
		v.push_back(make_pair("Subject",Subject));
	if(!Sname.empty())
		v.push_back(make_pair("Sname",Sname));
	if(!Action.empty())
		v.push_back(make_pair("Action",Action));
	if(!ControlTimeFrom.empty())
		v.push_back(make_pair("ControlTimeFrom",ControlTimeFrom));
	if(!ControlTimeTo.empty())
		v.push_back(make_pair("ControlTimeTo",ControlTimeTo));
	if(!ContentId.empty())
		v.push_back(make_pair("ContentId",ContentId));
	if(!ContentType.empty())
		v.push_back(make_pair("ContentType",ContentType));
	
	
	string sql=sqlFactory.makeAddSQL(v);
	
	return DBHandler::instance()->writeRecord(sql);
}

bool RuleHandler::addRule(string rule,int len)
{
	//rule like this: R < Subject Action ControlTimeFrom ControlTimeTo ContentId
	string subject,action,controltimefrom,controltimeto,contentid;
	int i=4;
    int j;
    j=rule.find(' ',i);
    subject.assign(rule,i,j-i);
    
    i=j+1;
    j=rule.find(' ',i);
    action.assign(rule,i,j-i);
   
    i=j+1;
    j=rule.find(' ',i);
    controltimefrom.assign(rule,i,j-i);
   
    i=j+1;
    j=rule.find(' ',i);
    controltimeto.assign(rule,i,j-i);
    
    i=j+1;
    j=len+1;
    contentid.assign(rule,i,j-i);
	
	addRule(subject,"",action,controltimefrom,controltimeto,contentid,"");
	return 1;
}

bool RuleHandler::chkRule(string Subject,string Sname,string Action,string ControlTimeFrom,string ControlTimeTo,string ContentId,string ContentType)
{
	vector<pair<string,string> > v;
	if(!Subject.empty())
		v.push_back(make_pair("Subject",Subject));
	if(!Sname.empty())
		v.push_back(make_pair("Sname",Sname));
	if(!Action.empty())
		v.push_back(make_pair("Action",Action));
	if(!ControlTimeFrom.empty())
		v.push_back(make_pair("ControlTimeFrom",ControlTimeFrom));
	if(!ControlTimeTo.empty())
		v.push_back(make_pair("ControlTimeTo",ControlTimeTo));
	if(!ContentId.empty())
		v.push_back(make_pair("ContentId",ContentId));
	if(!ContentType.empty())
		v.push_back(make_pair("ContentType",ContentType));
	
	
	string sql=sqlFactory.makeRuleChkSQL(v);
	
	return DBHandler::instance()->checkRecord(sql);
}

bool RuleHandler::delRule(string Id)
{
	vector<pair<string,string> > v;
	if(!Id.empty())
		v.push_back(make_pair("Id",Id));
	
	string sql=sqlFactory.makeDelSQL(v);
	return DBHandler::instance()->execSQL(sql);
}



#include "SQLFactory.h"
#include "ConfigEvent.h"
#include "ConfigTimer.h"
#include "time_utility.h"
#include <string>

bool Pz_id::addPz_id(Pz_idTable& idtable)
{
	vector<pair<string,string> > v;
	    if(!idtable.ConfigId.empty())
		v.push_back(make_pair("ConfigId",idtable.ConfigId));
		if(!idtable.PZUserID.empty())
		v.push_back(make_pair("PZUserID",idtable.PZUserID));
		
		if(!idtable.PZResourceID.empty())
	    v.push_back(make_pair("PZResourceID",idtable.PZResourceID));
		
		if(!idtable.PZAction.empty())
	    v.push_back(make_pair("PZAction",idtable.PZAction));
		
		if(!idtable.PZOperator.empty())
	    v.push_back(make_pair("PZOperator",idtable.PZOperator));
		if(!idtable.PZOperateTime.empty())
		v.push_back(make_pair("PZOperateTime",idtable.PZOperateTime));
		if(!idtable.PZControlTimeFrom.empty())
		v.push_back(make_pair("PZControlTimeFrom",idtable.PZControlTimeFrom));
		if(!idtable.PZControlTimeTo.empty())
		v.push_back(make_pair("PZControlTimeTo",idtable.PZControlTimeTo));
		if(!idtable.PZControlType.empty())
	    v.push_back(make_pair("PZControlType",idtable.PZControlType));
		if(!idtable.PZSNSType.empty())
	    v.push_back(make_pair("PZSNSType",idtable.PZSNSType));
	
	string sql=sqlFactory.makeAddSQL(v);
	
	return DBHandler::instance()->writeRecord(sql);
}
bool Pz_id::delPz_id(Pz_idTable& idtable)
{
	vector<pair<string,string> > v;
	    if(!idtable.ConfigId.empty())
		v.push_back(make_pair("ConfigId",idtable.ConfigId));
		if(!idtable.PZUserID.empty())
		v.push_back(make_pair("PZUserID",idtable.PZUserID));
		
		if(!idtable.PZResourceID.empty())
	    v.push_back(make_pair("PZResourceID",idtable.PZResourceID));
		
		if(!idtable.PZAction.empty())
	    v.push_back(make_pair("PZAction",idtable.PZAction));
		
		if(!idtable.PZOperator.empty())
	    v.push_back(make_pair("PZOperator",idtable.PZOperator));
		if(!idtable.PZOperateTime.empty())
		v.push_back(make_pair("PZOperateTime",idtable.PZOperateTime));
		if(!idtable.PZControlTimeFrom.empty())
		v.push_back(make_pair("PZControlTimeFrom",idtable.PZControlTimeFrom));
		if(!idtable.PZControlTimeTo.empty())
		v.push_back(make_pair("PZControlTimeTo",idtable.PZControlTimeTo));
		if(!idtable.PZControlType.empty())
	    v.push_back(make_pair("PZControlType",idtable.PZControlType));
		if(!idtable.PZSNSType.empty())
	    v.push_back(make_pair("PZSNSType",idtable.PZSNSType));
	
	string sql=sqlFactory.makeDelSQL(v);
	
	return DBHandler::instance()->execSQL(sql);
}
bool Pz_id::addPz_id(string rule, int len)
{
	//rule like this:PR < ConfigId PZUserID PZResourceID PZAction PZOperator PZOperateTime PZControlTimeFrom PZControlTimeTo PZControlType PZSNSType
	int k;
	Pz_idTable idtable;
	int i=5;
    int j;
    j=rule.find(' ',i);
    idtable.ConfigId.assign(rule,i,j-i);
	
	i=j+1;
    j=rule.find(' ',i);
    idtable.PZUserID.assign(rule,i,j-i);
	
	i=j+1;
    j=rule.find(' ',i);
    idtable.PZResourceID.assign(rule,i,j-i);
	
	i=j+1;
    j=rule.find(' ',i);
    idtable.PZAction.assign(rule,i,j-i);
	
	i=j+1;
    j=rule.find(' ',i);
    idtable.PZOperator.assign(rule,i,j-i);
	
	i=j+1;
    j=rule.find(' ',i);
    idtable.PZOperateTime.assign(rule,i,j-i);
	
	/*
	i=j+1;
    j=rule.find(' ',i);
    idtable.PZControlTimeFrom.assign(rule,i,j-i);
	
	i=j+1;
    j=rule.find(' ',i);
    idtable.PZControlTimeTo.assign(rule,i,j-i);
	*/
	i=j+1;
    j=rule.find(' ',i);
	k=j+1;
    j=rule.find(' ',k);
    idtable.PZControlTimeFrom.assign(rule,i,j-i);
	
	i=j+1;
    j=rule.find(' ',i);
	k=j+1;
    j=rule.find(' ',k);
    idtable.PZControlTimeTo.assign(rule,i,j-i);
	
	i=j+1;
    j=rule.find(' ',i);
    idtable.PZControlType.assign(rule,i,j-i);
	
	i=j+1;
    j=len+1;
    idtable.PZSNSType.assign(rule,i,j-i);
	
	addPz_id(idtable);
	boost::shared_ptr<ConfigEvent> event4(new EventTimer::IDConfigEvent(idtable.ConfigId, idtable.PZControlTimeFrom, idtable.PZControlTimeTo, idtable.PZUserID, idtable.PZResourceID, idtable.PZAction));
	EventTimer::addEvent(event4);
	
	return 1;

}

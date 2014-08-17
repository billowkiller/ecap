#include "SQLFactory.h"
#include "ConfigEvent.h"
#include "ConfigTimer.h"
#include "time_utility.h"
#include <string>
//#include <iostream>
using namespace std;
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
Pz_idTable Pz_id::getPz_idByTuple(string& PZUserID, string& PZResourceID, string& PZAction)
{
	vector<pair<string,string> > v;
	if(!PZUserID.empty())
		v.push_back(make_pair("PZUserID",PZUserID));
	if(!PZUserID.empty())
		v.push_back(make_pair("PZResourceID",PZResourceID));
	if(!PZUserID.empty())
		v.push_back(make_pair("PZAction",PZAction));
	
	string sql=sqlFactory.makeChkSQL(v);
	int nrow=0,ncolumn;
	char *zErrMsg;
    char **azResult;
	std::cout<<sql<<std::endl;
	sqlite3_get_table(DBHandler::instance()->db, sql.c_str(), &azResult, &nrow, &ncolumn, &zErrMsg);
	//std::cout<<ncolumn<<" "<<nrow<<std::endl;
	Pz_idTable idtable;
	//if(ncolumn==10&&nrow==1)
	if(nrow>0)
	{
		int i;
		i=ncolumn;
		idtable.PZUserID=azResult[i++];
		idtable.PZResourceID=azResult[i++];
		idtable.PZAction=azResult[i++];
		idtable.PZOperator=azResult[i++];
		idtable.PZOperateTime=azResult[i++];
		idtable.PZControlTimeFrom=azResult[i++];
		idtable.PZControlTimeTo=azResult[i++];
		idtable.PZControlType=azResult[i++];
		idtable.PZSNSType=azResult[i++];
		idtable.ConfigId=azResult[i++];
	}
	return idtable;
}

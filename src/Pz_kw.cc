#include "SQLFactory.h"
#include "ConfigEvent.h"
#include "ConfigTimer.h"
#include "time_utility.h"
#include <string>

bool Pz_kw::addPz_kw(Pz_kwTable& kwtable)
{
	vector<pair<string,string> > v;
	    if(!kwtable.PZConfigId.empty())
		v.push_back(make_pair("PZConfigId",kwtable.PZConfigId));
	
		if(!kwtable.PZControlContent.empty())
	    v.push_back(make_pair("PZControlContent",kwtable.PZControlContent));
		
		if(!kwtable.PZOperator.empty())
	    v.push_back(make_pair("PZOperator",kwtable.PZOperator));
		if(!kwtable.PZOperateTime.empty())
		v.push_back(make_pair("PZOperateTime",kwtable.PZOperateTime));
		if(!kwtable.PZControlTimeFrom.empty())
		v.push_back(make_pair("PZControlTimeFrom",kwtable.PZControlTimeFrom));
		if(!kwtable.PZControlTimeTo.empty())
		v.push_back(make_pair("PZControlTimeTo",kwtable.PZControlTimeTo));
		if(!kwtable.PZControlType.empty())
	    v.push_back(make_pair("PZControlType",kwtable.PZControlType));
		if(!kwtable.PZStreamType.empty())
		v.push_back(make_pair("PZStreamType",kwtable.PZStreamType));
		
	
	string sql=sqlFactory.makeAddSQL(v);
	
	DBHandler::instance()->writeRecord(sql);
}
bool Pz_kw::delPz_kw(Pz_kwTable& kwtable)
{
	vector<pair<string,string> > v;
	    if(!kwtable.PZConfigId.empty())
		v.push_back(make_pair("PZConfigId",kwtable.PZConfigId));
	
		if(!kwtable.PZControlContent.empty())
	    v.push_back(make_pair("PZControlContent",kwtable.PZControlContent));
		
		if(!kwtable.PZOperator.empty())
	    v.push_back(make_pair("PZOperator",kwtable.PZOperator));
		if(!kwtable.PZOperateTime.empty())
		v.push_back(make_pair("PZOperateTime",kwtable.PZOperateTime));
		if(!kwtable.PZControlTimeFrom.empty())
		v.push_back(make_pair("PZControlTimeFrom",kwtable.PZControlTimeFrom));
		if(!kwtable.PZControlTimeTo.empty())
		v.push_back(make_pair("PZControlTimeTo",kwtable.PZControlTimeTo));
		if(!kwtable.PZControlType.empty())
	    v.push_back(make_pair("PZControlType",kwtable.PZControlType));
		if(!kwtable.PZStreamType.empty())
		v.push_back(make_pair("PZStreamType",kwtable.PZStreamType));
		
	
	string sql=sqlFactory.makeDelSQL(v);
	
	DBHandler::instance()->execSQL(sql);
}

 Pz_kwTable Pz_kw::getPz_kwByKw(string kw)
{
	 string sql="select PZConfigId,PZControlContent,PZOperator,PZOperateTime,PZControlTimeFrom,PZControlTimeTo,PZControlType,PZStreamType from pzxftable_keyword where PZControlContent = '"+kw+"'";
	int nrow=0,ncolumn;
	char *zErrMsg;
    char **azResult;
	sqlite3_get_table(DBHandler::instance()->db, sql.c_str(), &azResult, &nrow, &ncolumn, &zErrMsg);
	Pz_kwTable kwtable;
	//if(ncolumn==8&&nrow=1)
	{
		int i;
		i=ncolumn;
		kwtable.PZConfigId=azResult[i++];
		kwtable.PZControlContent=azResult[i++];
		kwtable.PZOperator=azResult[i++];
		kwtable.PZOperateTime=azResult[i++];
		kwtable.PZControlTimeFrom=azResult[i++];
		kwtable.PZControlTimeTo=azResult[i++];
		kwtable.PZControlType=azResult[i++];
		kwtable.PZStreamType=azResult[i++];
			
	}
	return kwtable;
}

bool Pz_kw::addPz_kw(string rule, int len)
{
	//rule like this:PK < PZConfigId PZControlContent PZOperator PZOperatorTime PZOperatorTimeFrom PZOperatorTimeTo PZControlType PZStreamType
	int k;
	Pz_kwTable kwtable;
	int i=5;
    int j;
    j=rule.find(' ',i);
    kwtable.PZConfigId.assign(rule,i,j-i);
	
	i=j+1;
    j=rule.find(' ',i);
    kwtable.PZControlContent.assign(rule,i,j-i);
	
	i=j+1;
    j=rule.find(' ',i);
    kwtable.PZOperator.assign(rule,i,j-i);
	
	i=j+1;
    j=rule.find(' ',i);
    kwtable.PZOperateTime.assign(rule,i,j-i);
	
	//fime format:2014-Jul-09 13:30:31 
	/*
	i=j+1;
    j=rule.find(' ',i);
    kwtable.PZControlTimeFrom.assign(rule,i,j-i);
	
	i=j+1;
    j=rule.find(' ',i);
    kwtable.PZControlTimeTo.assign(rule,i,j-i);
	*/
	i=j+1;
    j=rule.find(' ',i);
	k=j+1;
    j=rule.find(' ',k);
    kwtable.PZControlTimeFrom.assign(rule,i,j-i);
	
	i=j+1;
    j=rule.find(' ',i);
	k=j+1;
    j=rule.find(' ',k);
    kwtable.PZControlTimeTo.assign(rule,i,j-i);
	
	i=j+1;
    j=rule.find(' ',i);
    kwtable.PZControlType.assign(rule,i,j-i);
	
	i=j+1;
    j=len+1;
    kwtable.PZStreamType.assign(rule,i,j-i);
	
	addPz_kw(kwtable);
	std::cout<<"addkw is ok"<<std::endl;
	boost::shared_ptr<ConfigEvent> event1(new EventTimer::KWConfigEvent(kwtable.PZConfigId, kwtable.PZControlTimeFrom, kwtable.PZControlTimeTo, kwtable.PZControlContent));
	EventTimer::addEvent(event1);
	std::cout<<"settimer is ok"<<std::endl;
	return 1;

}

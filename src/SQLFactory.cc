#include "SQLFactory.h"

/*const string chkSQLHeader;
	const string addSQLHeader;
    const string delSQLHeader;//they are not static
    
    string makeAddSQL(vector<pair<string,string>>&);
	string makeAddSQL(vector<string>&);
	string makeModSQL(vector<pair<string,string>>&);
	string makeModSQL(vector<string>&);
	string makeDelSQL(vector<pair<string,string>>&);
	string makeDelSQL(vector<string>&);
	string makeChkSQL(vector<pair<string,string>>&);
	string makeChkSQL(vector<string>&);
	
	string makeConditionSQL(vector<pair<string,string>>&);
    */
SQLFactory::SQLFactory(string chk,string add,string del):chkSQLHeader(chk),addSQLHeader(add),delSQLHeader(del)
{
	
}

string SQLFactory::makeConditionSQL(vector<pair<string,string> >& v)
{
	string conditionSQL="";
	int i=0;
	if(v.size()>0)
	{
		conditionSQL=conditionSQL+" where ";
		for(i=0;i<v.size()-1;i++)
		{
			conditionSQL=conditionSQL+v[i].first+"='"+v[i].second+"' and ";
		}
		conditionSQL=conditionSQL+v[i].first+"='"+v[i].second+"'";
		
	}
	return conditionSQL;
}
//fuction makeRuleConditionSQL allow wildcard * as joker
string SQLFactory::makeRuleConditionSQL(vector<pair<string,string> >& v)
{
	string conditionSQL="";
	int i=0;
	if(v.size()>0)
	{
		conditionSQL=conditionSQL+" where ";
		for(i=0;i<v.size()-1;i++)
		{
			conditionSQL=conditionSQL+"("+v[i].first+"='"+v[i].second+"' or "+v[i].first+"='*') and ";
		}
		conditionSQL=conditionSQL+"("+v[i].first+"='"+v[i].second+"' or "+v[i].first+"='*')";
		
	}
	//std::cout<<conditionSQL<<std::endl;
	return conditionSQL;
}


string SQLFactory::makeAddSQL(vector<pair<string,string> >& v)
{
	string addSQL;
	string value="values(";
	int i=0;
	if(v.size()>0)
	{
	    addSQL=addSQLHeader+"(";
		for(i=0;i<v.size()-1;i++)
		{
			addSQL=addSQL+v[i].first+",";
			value=value+"'"+v[i].second+"',";
		}
		addSQL=addSQL+v[i].first+") ";
		value=value+"'"+v[i].second+"')";
		addSQL=addSQL+value;
	}
	
	return addSQL;
	
}

string SQLFactory::makeDelSQL(vector<pair<string,string> >& v)
{
	string delSQL;
	delSQL=delSQLHeader+makeConditionSQL(v);
	return delSQL;
}

string SQLFactory::makeChkSQL(vector<pair<string,string> >& v)
{
	string chkSQL;
	chkSQL=chkSQLHeader+makeConditionSQL(v);
	return chkSQL;
}

string SQLFactory::makeRuleChkSQL(vector<pair<string,string> >& v)
{
	string chkSQL;
	chkSQL=chkSQLHeader+makeRuleConditionSQL(v);
	return chkSQL;
}

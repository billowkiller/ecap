#include "SQLFactory.h"
#include <stdio.h>

DBHandler* DBHandler::m_instance=NULL;

DBHandler::DBHandler()
{
	string dbpath="/home/billowkiller/ecap/src/shutter.db";
	sqlite3_open(dbpath.c_str(), &db);
}

DBHandler::~DBHandler()
{
	sqlite3_close(db);
}

bool DBHandler::execSQL(string sql)
{
	std::cout<<"sql:"<<sql<<endl;
	 char *zErrMsg;
    int r=sqlite3_exec(db, sql.c_str(), 0, 0, &zErrMsg);//return 0 if ok
	if(r)
	{
		printf("%s\n",zErrMsg);
	}
    return (r==0);
}

DBHandler* DBHandler::instance()
{
	if(m_instance==NULL)
	{
		m_instance=new DBHandler();
	}
	return m_instance;
}

bool DBHandler::writeRecord(string sql)
{
	
	return execSQL(sql);
}
bool DBHandler::checkRecord(string sql)
{
	int nrow=0,ncolumn;
	char *zErrMsg;
    char **azResult;
	//std::cout<<"sql:"<<sql<<endl;
	sqlite3_get_table(db, sql.c_str(), &azResult, &nrow, &ncolumn, &zErrMsg);
	return (nrow>0);
}

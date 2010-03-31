//The MIT License
//
//Copyright (c) 2006-2010 Michael Simpson
//
//Permission is hereby granted, free of charge, to any person obtaining a copy
//of this software and associated documentation files (the "Software"), to deal
//in the Software without restriction, including without limitation the rights
//to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
//copies of the Software, and to permit persons to whom the Software is
//furnished to do so, subject to the following conditions:
//
//The above copyright notice and this permission notice shall be included in
//all copies or substantial portions of the Software.
//
//THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
//AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
//OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
//THE SOFTWARE.

#include "<%productName%>database.h"
#include <QSqlQuery>
#include <QSqlError>

// This File was auto generated on <%date%> by QTSqlGen.  
// Do not modify!!!!!
// Changes will be overwritten.

#include <QCoreApplication.h>

static quint32 gConnectionCount(1); 

static char* gConnectionStrings[] = 
{
	"Driver={SQL Server Native Client 10.0};Server=<%server%>;Database=<%database%>;Uid=<%user%>;Pwd=<%pass%>;",
	"Driver={SQL Native Client};Server=<%server%>;Database=<%database%>; Uid=<%user%>;Pwd=<%pass%>;",
	"Driver={SQL Server};Server=<%server%>;Database=<%database%>;Uid=<%user%>;Pwd=<%pass%>;",
	"Driver={Microsoft Access Driver (*.mdb)};Dbq=<%database%>;Uid=<%user%>;Pwd=<%pass%>;",
	"Driver={MySQL ODBC 5.1 Driver};Server=<%server%>;Port=<%port%>;Database=<%database%>;User=<%user%>; Password=<%pass%>;Option=3;",
	"Driver=Driver={Oracle ODBC};DBQ=<%server%>;Uid=<%user%>;Pwd=<%pass%>;",
	"Driver={PostgreSQL UNICODE};Server=<%server%>;Port=<%port%>;Database=<%database%>;Uid=<%user%>;Pwd=<%pass%>;"
};

<%productName%>Database::<%productName%>Database() :
<%initializers%>
{
}

<%productName%>Database::~<%productName%>Database()
{
	Close();

<%destructors%>

	QSqlDatabase::removeDatabase(_connectionName);
}

void <%productName%>Database::SetODBCDriver(ODBCDrivers odbcDriver)
{
	switch (odbcDriver)
	{
	case eSqlServer2000:
		_driverString = gConnectionStrings[0];
		break;

	case eSqlServer2005:
		_driverString = gConnectionStrings[1];
		break;

	case eSqlServer2008:
		_driverString = gConnectionStrings[2];
		break;

	case eAccess:
		_driverString = gConnectionStrings[3];
		break;

	case eMySql:
		_driverString = gConnectionStrings[4];
		_port = "3306";
		break;

	case eOracle:
		_driverString = gConnectionStrings[5];
		break;

	case ePostgres:
		_driverString = gConnectionStrings[6];
		_port = "5432";  // default port
		break;
	}

	_connectionString = "";
}

void <%productName%>Database::SetHost
(
	const QString& host
)
{
	_host = host;
	_connectionString = "";
}

void <%productName%>Database::SetPort
(
	int portNum
)
{
	_port = QString::number(portNum);
}

void <%productName%>Database::SetDatabase
(
	const QString& database
)
{
	_databaseName = database;
}

void <%productName%>Database::SetUserName
(
	const QString& userName
)
{
	_userName = userName;
}

void <%productName%>Database::SetPassword
(
	const QString& password
)
{
	_password = password;
}

void <%productName%>Database::CreateConnection
(
	const QString& connectionName
)
{
	_connectionName = connectionName + QString::number(gConnectionCount++);
	_database = QSqlDatabase::addDatabase("QODBC", _connectionName);

	if (_connectionString.length() == 0)
		BuildConnectionString();

	_database.setDatabaseName(_connectionString);
}

void <%productName%>Database::BuildConnectionString()
{
	_driverString.replace("<%server%>", _host.toAscii());
	_driverString.replace("<%port%>", _port.toAscii());
	_driverString.replace("<%database%>", _databaseName.toAscii());
	_driverString.replace("<%user%>", _userName.toAscii());
	_driverString.replace("<%pass%>", _password.toAscii());

	_connectionString = _driverString;
}

bool <%productName%>Database::Open()
{
	bool result(false);

	if ((result = _database.isOpen()) == false)
	{
		result = _database.open();
	}

	return result;
}

void <%productName%>Database::Close()
{
	if (IsOpen())
		_database.close();
}

bool <%productName%>Database::IsOpen()
{
	bool result(false);

	if (_database.isValid())
		result = _database.isOpen();

	return result;
}

<%tableRoutines%>
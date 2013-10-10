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

#include "<%productName%>Database.h"
#include <QSqlQuery>
#include <QSqlError>

// This File was auto generated on <%date%> by <%applicationName%>.  
// Do not modify!!!!!
// Changes will be overwritten.

#include <QCoreApplication>
<%namespaceStart%>
static quint32 gConnectionCount(1); 

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

void <%productName%>Database::CreateConnection
(
	const QString& connectionName
)
{
	_connectionName = connectionName + QString::number(gConnectionCount++);
	_database = QSqlDatabase::addDatabase("QSQLITE", _connectionName);
}

bool <%productName%>Database::Open()
{
	bool result(false);

	if ((result = _database.isOpen()) == false)
	{
		if (_filePath.size())
		{
			_database.setDatabaseName(_filePath);
			
			result = _database.open();
		}
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
<%namespaceEnd%>
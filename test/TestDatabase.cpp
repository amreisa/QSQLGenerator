#include "Testdatabase.h"
#include <QSqlQuery>
#include <QSqlError>

// This File was auto generated on Sun Apr 20 2008 by QTSqliteGen.  
// Do not modify!!!!!
// Changes will be overwritten.

#include <QCoreApplication.h>

static quint32 gConnectionCount(1); 

TestDatabase::TestDatabase() :
	_employeesTable(NULL),
	_addressesTable(NULL)
{
}

TestDatabase::~TestDatabase()
{
	if (_employeesTable != NULL)
	{
		delete _employeesTable;
		_employeesTable = NULL;
	}
	if (_addressesTable != NULL)
	{
		delete _addressesTable;
		_addressesTable = NULL;
	}

	if (_database.isValid())
	{
		Close();

		QSqlDatabase::removeDatabase(_connectionName);
	}
}

void TestDatabase::CreateConnection
(
	const QString& connectionName
)
{
	_connectionName = connectionName + QString::number(gConnectionCount++);
	_database = QSqlDatabase::addDatabase("QSQLITE", _connectionName);
}

bool TestDatabase::Open
(
	const QString& filePath
)
{
	bool result(false);

	_filePath = filePath;
	if (_filePath.size())
	{
		_database.setDatabaseName(_filePath);
		
		result = _database.open();
	}

	return result;
}

void TestDatabase::Close()
{
	if (_database.isOpen())
		_database.close();
}

bool TestDatabase::IsOpen()
{
	bool result(false);

	if (_database.isValid())
		result = _database.isOpen();

	return result;
}

EmployeesTable* TestDatabase::GetEmployeesTable()
{
	if (_employeesTable == NULL)
		_employeesTable = new EmployeesTable(this);
		
	return _employeesTable;
}
AddressesTable* TestDatabase::GetAddressesTable()
{
	if (_addressesTable == NULL)
		_addressesTable = new AddressesTable(this);
		
	return _addressesTable;
}

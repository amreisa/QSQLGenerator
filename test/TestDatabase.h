#ifndef TESTDATABASE_H
#define TESTDATABASE_H

// This File was auto generated on Sun Apr 20 2008 by QTSqliteGen.  
// Do not modify!!!!!
// Changes will be overwritten.

#include <QSqlDatabase>

#include "TestExport.h"
#include "EmployeesTable.h"
#include "AddressesTable.h"

// singleton class encapsulating Qt database
class DLL_TEST TestDatabase
{
public:
	TestDatabase();
	virtual ~TestDatabase();

	void CreateConnection(const QString& connectionName);
	QSqlDatabase Database(void)
		{ return _database; }

	bool Open(const QString& filePath);
	void Close(void);

	QString FilePath(void) 
		{ return _filePath; }
	bool IsOpen(void);

	// Tables
	EmployeesTable* GetEmployeesTable(void);
	AddressesTable* GetAddressesTable(void);

	// Views

protected:
	EmployeesTable* 	_employeesTable;
	AddressesTable* 	_addressesTable;

	QSqlDatabase				_database;
	QString						_connectionName;
	QString						_filePath;
};

#endif

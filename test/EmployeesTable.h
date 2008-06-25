#ifndef EMPLOYEESTABLE_H
#define EMPLOYEESTABLE_H

// This File was auto generated on Sun Apr 20 2008 by QTSqliteGen.  
// Do not modify!!!!!
// Changes will be overwritten.

#include <QMutex>
#include "EmployeesRecord.h"

class TestDatabase;

class DLL_TEST EmployeesTable
{
friend class TestDatabase;

public:
	bool Create(void);

	bool Add(EmployeesRecord& ent);
	bool Delete(quint32 key);

	bool Get(QList<EmployeesRecord>& recordList, EmployeesRecord& record, QStringList sortList = QStringList());
	bool Get(quint32 key, EmployeesRecord& record);
	quint32 GetKey(EmployeesRecord& record, const QString& propName, const QString& propValue);

	bool Update(quint32 key, const QHash<QString, QVariant>& updates);
	bool Update(EmployeesRecord& record);

private:
	EmployeesTable(TestDatabase* database);
	virtual ~EmployeesTable(void);

	TestDatabase* _database;
	static QMutex				_mutexEmployees;

	static QString				_createStatement;
};

#endif

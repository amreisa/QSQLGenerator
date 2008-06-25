#ifndef ADDRESSESTABLE_H
#define ADDRESSESTABLE_H

// This File was auto generated on Sun Apr 20 2008 by QTSqliteGen.  
// Do not modify!!!!!
// Changes will be overwritten.

#include <QMutex>
#include "AddressesRecord.h"

class TestDatabase;

class DLL_TEST AddressesTable
{
friend class TestDatabase;

public:
	bool Create(void);

	bool Add(AddressesRecord& ent);
	bool Delete(quint32 key);

	bool Get(QList<AddressesRecord>& recordList, AddressesRecord& record, QStringList sortList = QStringList());
	bool Get(quint32 key, AddressesRecord& record);
	quint32 GetKey(AddressesRecord& record, const QString& propName, const QString& propValue);

	bool Update(quint32 key, const QHash<QString, QVariant>& updates);
	bool Update(AddressesRecord& record);

private:
	AddressesTable(TestDatabase* database);
	virtual ~AddressesTable(void);

	TestDatabase* _database;
	static QMutex				_mutexAddresses;

	static QString				_createStatement;
};

#endif

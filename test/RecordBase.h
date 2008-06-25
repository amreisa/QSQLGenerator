#ifndef TESTRECORDBASE_H
#define TESTRECORDBASE_H

// This File was auto generated on Sun Apr 20 2008 by QTSqliteGen.  
// Do not modify!!!!!
// Changes will be overwritten.

#include <QSqlQuery>
#include <QDate>
#include <QDateTime>
#include <QStringList>
#include <QVariant>
#include <QHash>
#include <QTime>
#include <QUuid>
#include <QVector>

#include "TestExport.h"

class QDomDocument;
class Database;

typedef QHash<QString, QVariant> UpdateHash;
typedef QHashIterator<QString, QVariant> UpdateHashIter;

enum ColumnType
{
	eUnknown,
	eString,
	eInteger,
	eDateTime,
	eDate,
	eTime,
	eQUuid,
	eReal
};

struct ColumnMetaData
{
	ColumnMetaData() 
	{
		_type = eUnknown;
		_changed = false; 
	}

	ColumnMetaData(const ColumnMetaData& copyMe)
	{
		_field = copyMe._field;
		_value = copyMe._value;
		_type = copyMe._type;
		_changed = copyMe._changed;
	}

	QString						_field;
	QVariant					_value;
	ColumnType					_type;
	bool						_changed;
};

typedef QVector<ColumnMetaData> RecordMetaData;
typedef QVector<ColumnMetaData>::iterator ColumnMetaIterator;

class DLL_TEST RecordBase
{
public:
	RecordBase(const QString& table, quint32 fieldCount);
    RecordBase (const RecordBase& copyMe);
    RecordBase& operator=(const RecordBase& copyMe);

	virtual ~RecordBase(void);

	virtual quint32 Count(void);
	virtual QString Table(void);
	virtual QString Field(quint32 index);

	bool Changed(quint32 index);
	bool Changed(const QString& name);

	QVariant Value(quint32 index);
	QVariant Value(const QString& name);

	void SetValue(quint32 index, const QVariant& value);
	void SetValue(const QString& name, const QVariant& val);

	virtual quint32 Key(void);
	virtual bool IsEmpty(void);

	virtual	QString EntityName(void);
	virtual	void EntityName(const QString& value);
	virtual	QString ToXml(bool preferAttrib	= false, bool skipEmpty = false, bool upperCase = false);
	virtual	QDomDocument& GetDom(bool preferAttrib = false, bool skipEmpty = false, bool upperCase = false);

	// encode/decode Datatypes
	QString EncodeDateTime(const QDateTime& dateTime);
	QDateTime DecodeDateTime(const QString& isoDateTime);

	QString EncodeDate(const QDate& dateTime);
	QDate DecodeDate(const QString& isoDate);

	QString EncodeTime(const QTime& dateTime);
	QTime DecodeTime(const QString& isoTime);

	QString EncodeGuid(const QUuid& uid);
	QUuid DecodeGuid(const QString& uuidString);
	
	ColumnType FieldDataType(quint32 index);
	ColumnType FieldDataType(const QString& fieldName);
	void SetFieldDataType(quint32 index, const QString& fieldName, ColumnType type);

	QDateTime Now(void);
	void Clear(void);
	void ClearUpdates(void);

	UpdateHash GetUpdateHash(void);

protected:
	QString						_entityName;
	QString						_table;

	RecordMetaData				_recordData;
	QDomDocument*				_xmlDoc;

private:
	RecordBase();
};

#endif


#ifndef <%PRODUCTNAME%>RECORDBASE_H
#define <%PRODUCTNAME%>RECORDBASE_H

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

// This File was auto generated on <%date%> by <%applicationName%>.  
// Do not modify!!!!!
// Changes will be overwritten.

#include <QSqlQuery>
#include <QByteArray>
#include <QDate>
#include <QDateTime>
#include <QStringList>
#include <QVariant>
#include <QHash>
#include <QTime>
#include <QUuid>
#include <QVector>

#include "<%productInclude%>"

class QDomDocument;
<%namespaceStart%>
class Database;

typedef QHash<QString, QVariant> UpdateHash;
typedef QHashIterator<QString, QVariant> UpdateHashIter;

enum ColumnType
{
	eUnknown,
	eString,
	eBytes,
	eInteger,
	eDateTime,
	eDate,
	eTime,
	eQUuid,
	eReal,
	eBool
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

class <%dllExport%> RecordBase
{
public:
	RecordBase(const QString& table, quint32 fieldCount);
    RecordBase (const RecordBase& copyMe);
    RecordBase& operator=(const RecordBase& assignme);

	void AssignMe(const RecordBase* assignMe);

	virtual ~RecordBase(void);

	virtual quint32 Count(void);
	virtual QString Table(void);
	virtual QString Field(quint32 index);

	bool Changed(quint32 index);
	void Changed(quint32 index, bool changed);
	bool Changed(const QString& name);
	void Changed(const QString& name, bool changed);

	QVariant FieldValue(quint32 index);
	QVariant FieldValue(const QString& name);

	void SetValue(quint32 index, const QVariant& value);
	void SetValue(const QString& name, const QVariant& val);

	virtual quint32 Key(void);
	virtual bool IsEmpty(void);

	virtual	QString EntityName(void);

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
<%namespaceEnd%>
#endif


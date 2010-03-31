#include "RecordBase.h"
#include <QtDebug>
#include <QDomDocument>
#include <QDomElement>

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

// This File was auto generated on <%date%> by QTSqlGen.  
// Do not modify!!!!!
// Changes will be overwritten.

const static QString kTagID   = "id";
const static QString kTagAttr = "attribute";
const static QString kTagName = "name";
const static QString kTagValue  = "value";

RecordBase::RecordBase() :
	_xmlDoc(NULL)
{
}

RecordBase::RecordBase
(
	const QString& table, 
	quint32 fieldCount
) :
	_table(table),
	_xmlDoc(NULL)
{
	_recordData.resize(fieldCount);
	
	for (quint32 i(0); i < fieldCount; i++) 
	{
		if (i == 0)
		{
			_recordData[i]._type = eInteger;
			_recordData[i]._value = QVariant((quint32) 0);
		}
		else
		{
			_recordData[i]._type = eString;
			_recordData[i]._value = QVariant("");
		}

		_recordData[i]._changed = false;
	}
}

// copy const
RecordBase::RecordBase 
(
	const RecordBase& copyMe
) :
	_table(copyMe._table),
	_xmlDoc(NULL)
{
	_recordData.resize(copyMe._recordData.size());

	for (quint32 i(0); i < Count(); i++) 
	{
		_recordData[i]._field = copyMe._recordData[i]._field;
		_recordData[i]._type = copyMe._recordData[i]._type;
		_recordData[i]._value = copyMe._recordData[i]._value;
		_recordData[i]._changed = copyMe._recordData[i]._changed;
	}
}

RecordBase& RecordBase::operator=
(
	const RecordBase& copyMe
) 
{
	_table = copyMe._table;
	_recordData.resize(copyMe._recordData.size());
		
	if (_xmlDoc != NULL)
	{
		delete _xmlDoc;
		_xmlDoc = NULL;
	}

	for (quint32 i(0); i < Count(); i++) 
	{
		_recordData[i]._field = copyMe._recordData[i]._field;
		_recordData[i]._type = copyMe._recordData[i]._type;
		_recordData[i]._value = copyMe._recordData[i]._value;
		_recordData[i]._changed = copyMe._recordData[i]._changed;
	}

	return *this;
}

void RecordBase::AssignMe
(
	const RecordBase* assignMe
)
{
	*this = *assignMe;
}

RecordBase::~RecordBase()
{
	if (_xmlDoc)
	{
		delete _xmlDoc;
		_xmlDoc = NULL;
	}
}

QString RecordBase::Table()
{ 
	return _table;
}

QString RecordBase::Field
(
	quint32 index
) 
{
	return _recordData[index]._field;
}

quint32 RecordBase::Count() 
{
	return _recordData.size();
}

// key MUST be first
quint32 RecordBase::Key() 
{
	return FieldValue(0).toInt();
}

bool RecordBase::Changed
(
	quint32 index
)
{
	return _recordData[index]._changed;
}

void RecordBase::Changed
(
	quint32 index, 
	bool changed
)
{
	_recordData[index]._changed = changed;
}

bool RecordBase::Changed
(
	const QString& name
)
{	
	for (quint32 i(1); i < Count(); i++) 
	{
		if (name.toLower() == Field(i).toLower())
			return Changed(i);
	}

	return false;
}

void RecordBase::Changed
(
	const QString& name, 
	bool changed
)
{	
	for (quint32 i(1); i < Count(); i++) 
	{
		if (name.toLower() == Field(i).toLower())
		{
			Changed(i, changed);

			return;
		}
	}
}

QVariant RecordBase::FieldValue
(
	quint32 index
) 
{
	return _recordData[index]._value;
}

QVariant RecordBase::FieldValue
(
	const QString& name
) 
{
	for (quint32 i(1); i < Count(); i++) 
	{
		if (name.toLower() == Field(i).toLower())
			return FieldValue(i);
	}

	return QVariant();
}

void RecordBase::SetValue
(
	quint32 index, 
	const QVariant& val
) 
{
	if (_recordData[index]._value != val)
	{
		_recordData[index]._value = val;
		_recordData[index]._changed = true;
	}
}

void RecordBase::SetValue
(
	const QString& name, 
	const QVariant& value
) 
{
	for (quint32 i(1); i < Count(); i++) 
	{
		if (name.toLower() == Field(i).toLower()) 
			 return SetValue(i, value);
	}
}

bool RecordBase::IsEmpty() 
{
	bool empty(true);

	for (quint32 i(0); i < Count(); i++) 
	{
		QString val = FieldValue(i).toString().trimmed();

		if (val.length() > 0 && val != "0") 
		{
			empty = false;
			break;
		}
	}

	return empty;
}

///
/// Property: entityName 
///	Used for xmlGeneration
///
/// getter
QString RecordBase::EntityName() 
{
	if (_entityName.size() == 0) 
		_entityName = Table() + "RecordBase";
	
	return _entityName;
}

UpdateHash RecordBase::GetUpdateHash()
{
	UpdateHash newHash;

	for (quint32 i(1); i < Count(); i++)
	{
		if (Changed(i))
			newHash.insert(Field(i), FieldValue(i));
	}

	return newHash;
}

void RecordBase::Clear() 
{
	for (quint32 i(0); i < Count(); i++) 
		_recordData[i]._value.clear();
}

void RecordBase::ClearUpdates()
{
	for (quint32 i(0); i < Count(); i++)
		_recordData[i]._changed = false;
}

QDateTime RecordBase::Now() 
{
	return QDateTime::currentDateTime();
}

QString RecordBase::EncodeDateTime
(
	const QDateTime& dateTime
)
{
	return dateTime.toString();
}

QDateTime RecordBase::DecodeDateTime
(
	const QString& dateTimeString
)
{
	QDateTime fromString = QDateTime::fromString(dateTimeString);

	return fromString;
}

QString RecordBase::EncodeDate
(
	const QDate& date
)
{
	return date.toString();
}

QDate RecordBase::DecodeDate
(
	const QString& dateString
)
{
	QDate fromString = QDate::fromString(dateString);

	return fromString;
}

QString RecordBase::EncodeTime
(
	const QTime& time
)
{
	return time.toString();
}

QTime RecordBase::DecodeTime
(
	const QString& timeString
)
{
	QTime fromString = QTime::fromString(timeString);

	return fromString;
}

QString RecordBase::EncodeGuid
(
	const QUuid& uid
) 
{
	if (uid.isNull() == false)
	{
	    QString guidStr = uid.toString();
		guidStr.remove(QChar('{')).remove(QChar('}'));

		return guidStr;
	}

	return QString();
}

QUuid RecordBase::DecodeGuid
(
	const QString& decodeMe
) 
{
	QString temp(decodeMe);

	if (temp[0] != QChar('{'))
		temp = QChar('{') + temp;

	if (temp[temp.size() - 1] != QChar('}'))
		temp = temp + QChar('}');

	QUuid fromString(temp);

	return fromString;	
}

ColumnType RecordBase::FieldDataType
(
	quint32 index
) 
{
	if (index >= (quint32) _recordData.size())
		return eUnknown;

	return _recordData[index]._type;
}

ColumnType RecordBase::FieldDataType
(
	const QString& fieldName
) 
{	
	for (quint32 i(1); i < Count(); i++) 
	{
		if (fieldName.toLower() == Field(i).toLower()) 
			 return FieldDataType(i);
	}

	return eUnknown;
}

void RecordBase::SetFieldDataType
(
	quint32 index, 
	const QString& fieldName, 
	ColumnType type
)
{
	_recordData[index]._field = fieldName;
	_recordData[index]._type = type;
	
	switch (type)
	{
	case eInteger:
		_recordData[index]._value = 0;
		break;

	case eDateTime:
		_recordData[index]._value = QDateTime::currentDateTime();
		break;

	case eDate:
		_recordData[index]._value = QDate::currentDate();
		break;

	case eTime:
		_recordData[index]._value = QTime::currentTime();
		break;

	case eQUuid:
		_recordData[index]._value = EncodeGuid(QUuid());
		break;

	case eReal:
		_recordData[index]._value = QVariant((double) 0.0);
		break;

	case eBool:
		_recordData[index]._value = QVariant(false);
		break;

	case eString:
	case eUnknown:
	default:
		_recordData[index]._value = QVariant("");
		break;
	}
}

/**
	Convert entity to xml dom document
   	\param preferAttrib - if true (default) generate attributes, else elements
	... <attr name="atrname" val="atrVal"/> ... vs <atrname>atrVal<atrname/>
	\param skipEmpty	- if true (default) skip empty values
	\param upperCase	- if true - uppercase all tags (default is false)
	\return Dom document reference
*/
QDomDocument& RecordBase::GetDom
(
	bool preferAttrib,
	bool skipEmpty,
	bool upperCase
)
{
	QString idTag(kTagID);
	QString attrTag(kTagAttr);
	QString nameTag(kTagName);
	QString valTag(kTagValue);
	QString entName(EntityName());

	if (upperCase) 
	{
		entName = entName.toUpper();
		idTag   = idTag.toUpper();
		attrTag = attrTag.toUpper();
		nameTag = nameTag.toUpper();
		valTag  = valTag.toUpper();
	}

	if (_xmlDoc)
		delete _xmlDoc;

	QDomElement root;

	_xmlDoc = new QDomDocument;
	
	root = _xmlDoc->createElement(entName);
    _xmlDoc->appendChild(root);
	// append id attrib to root

	root.setAttribute(idTag, Key());
	// enumerate fields
	for (quint32 i(1); i < Count(); i++) 
	{
		QString name = Field(i);
		QString v = FieldValue(i).toString().trimmed();
		
		if (v.length() > 0 || (v.length() == 0 && !skipEmpty)) 
		{
			if (upperCase) 
				name = name.toUpper();

			if (preferAttrib) 
			{
				QDomElement elem = _xmlDoc->createElement(attrTag);
				elem.setAttribute(nameTag, name);
				elem.setAttribute(valTag , v);
				root.appendChild (elem);
			}
			else // use elements
			{
				
				QDomElement elem = _xmlDoc->createElement(name);
				QDomText txt  = _xmlDoc->createTextNode(v);
				elem.appendChild(txt);
				root.appendChild(elem);
			}
		}
	}

	return *_xmlDoc;
}


/**
	Convert entity content to xml string
   	\param preferAttrib - if true (default) generate attributes, else elements
	... <attr name="atrname" val="atrVal"/> ... vs <atrname>atrVal<atrname/>
	\param skipEmpty	- if true (default) skip empty values
	\param upperCase	- if true - uppercase all tags (default is false)
	\return Dom document reference
*/
QString RecordBase::ToXml
(
	bool preferAttrib,
	bool skipEmpty,
	bool upperCase
)
{
	return GetDom(preferAttrib, skipEmpty, upperCase).toString();
}


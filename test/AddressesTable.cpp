#include "AddressesTable.h"
#include <QtDebug>
#include <QDomDocument>
#include <QSqlError>

#include "TestDatabase.h"

// This File was auto generated on Sun Apr 20 2008 by QTSqliteGen.  
// Do not modify!!!!!
// Changes will be overwritten.

QMutex AddressesTable::_mutexAddresses;

AddressesTable::AddressesTable
(
	TestDatabase* database
): 
	_database(database)
{
}

AddressesTable::~AddressesTable()
{
	_database = NULL;
}

QString AddressesTable::_createStatement("CREATE TABLE Addresses ( _id INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL, foo TEXT NULL, bar INTEGER DEFAULT '0' NULL, snafu INTEGER NULL, gooid INTEGER NULL, realItem REAL NULL, memoItem TEXT NULL, varCharItem VARCHAR(50) NULL, numericItem NUMERIC NULL, intItem INTEGER NULL, dateItem DateTime NULL, timeItem TIME NULL, floatItem FLOAT NULL, doubleItem REAL NULL )");

bool AddressesTable::Create(void)
{
	if (_database->Database().isValid())
	{
		QSqlQuery createQuery(_database->Database());

		return createQuery.exec(AddressesTable::_createStatement);
	}

	return false;
}

bool AddressesTable::Add
(
	AddressesRecord& record
) 
{
	bool result(false);	
	
	if (_database->Database().isValid())
	{
		QSqlQuery insert(_database->Database());
		QString insertStatement = AddressesRecord::GetInsertStatement();

		insert.prepare(insertStatement);

		for (quint32 i = 1; i < record.Count(); i++) // assumes _id is at position zero
		{
			QString bindName(":" + record.Field(i));

			insert.bindValue(bindName, record.Value(i));
		}

		QMutexLocker lock(&AddressesTable::_mutexAddresses);

		result = insert.exec();

		if (result) 
		{
			if (insert.numRowsAffected() == 1)
			{
				record.SetValue(0, insert.lastInsertId());
			} 
			else 
			{
				qDebug() << "Insert didn't affect exactly one row: " << insert.executedQuery();
				result = false; // exec lied
			}
		} 
		else 
		{
			QSqlError sqlError = insert.lastError();
			qDebug() << "Insert error: " << insert.executedQuery();
			qDebug() << "Insert error: " << sqlError.text();
		}
	}

	return result;
}

bool AddressesTable::Get
(
	quint32 key,
	AddressesRecord& record
)
{
	bool result(false);

	QSqlQuery query(_database->Database());
	QString sql(QString("select * from %1 where %2 = %3").arg(record.Table()).arg(record.Field(0)).arg(key));

	QMutexLocker lock(&AddressesTable::_mutexAddresses);

	if (query.exec(sql)) 
	{	
		// we assume that order of columns is the same as order of entity fields - guaranteed by Dal code generator
		if (query.next()) 
		{
			// copy fields
			for (quint32 i(0); i < record.Count(); i++) 
				record.SetValue(i, query.value(i));
			
			result = true;
		} 
		else 
		{
			qDebug() << "Can't find key " << key;
		}
	} 
	else 
	{
		//qDebug("ERROR");
	}

	return result;
}

bool AddressesTable::Get
(	
	QList<AddressesRecord>& list,
	AddressesRecord& record,
	QStringList sortFields
)
{
	bool result(false);

	QString filter;

	list.clear();

	QString sql = "select * from " + record.Table();

	if (!record.IsEmpty()) 
	{
		for (quint32 i(1); i < record.Count(); i++) // _id has to be in position 0
		{
			QString value = record.Value(i).toString();

			if (value.length() > 0) 
			{
				if (filter.length() > 0) 
					filter = filter.append(" and ");

				// number type?
				switch (record.FieldDataType(i))
				{
				case eInteger: 
				case eReal: 
					filter = filter.append(record.Field(i) + " = " + value + " ");
					break;

				default: 
					value = value.replace('\'', "''");
					filter = filter.append(record.Field(i) + " = '" + value + "' ");
					break;
				}
			}
		}
	} 
	else 
	{
		//qDebug("empty example - returning all");
	}

	if (filter.length() > 0)
		sql.append(" where " + filter);

	if (sortFields.size() > 0)
	{
		bool first(true);
		sql.append(" order by ");

		QStringList::iterator sortString = sortFields.begin();

		while (sortString != sortFields.end())
		{
			if (first)
				first = false;
			else
				sql.append(", ");

			sql.append(*sortString);

			sortString++;
		}
	}

	QMutexLocker lock(&AddressesTable::_mutexAddresses);

	QSqlQuery query(_database->Database());

	if (query.exec(sql)) 
	{	
		// we assume the order of columns is the same as order of entity fields - guaranteed by Dal code generator
		while (query.next()) 
		{
			AddressesRecord retrieved(record);
			
			for (quint32 i(0); i < retrieved.Count(); i++) 
				retrieved.SetValue(i, query.value(i));
			
			list.push_back(retrieved);
		}

		result = true;
	} 
	else 
	{
		//qDebug("ERROR");
	}

	return result;
}

quint32	AddressesTable::GetKey
(
	AddressesRecord& ent, 
	const QString& colName, 
	const QString& colValue
) 
{
	quint32 id = -1;
	QString sql;

	sql = QString("select %1 from %2 where %3 = '%4'").arg(ent.Field(0)).arg(ent.Table()).arg(colName)
		.arg(QString(colValue).replace('\'', "''"));

	QSqlQuery query(_database->Database());
	QMutexLocker lock(&AddressesTable::_mutexAddresses);

	if (query.exec(sql) && query.next()) 
		id = query.value(0).toInt();

	return id;
}

bool AddressesTable::Delete
(
	quint32 key
)
{
	AddressesRecord ent;
	bool result(false);

	QString sql(QString("delete from %1 where %2 = %3").arg(ent.Table()).arg(ent.Field(0)).arg(key));

	QSqlQuery deleteQuery(_database->Database());
	QMutexLocker lock(&AddressesTable::_mutexAddresses);

	result = deleteQuery.exec(sql);

	if (result)
		result = deleteQuery.numRowsAffected() == 1;

	return result;

}

bool AddressesTable::Update
(
	quint32 key, 
	const QHash<QString, QVariant>& updates
) 
{
	AddressesRecord ent;

	bool result = false;
	QString sql;
	QString upd;

	// build update part
    UpdateHashIter hashItem(updates);

    while (hashItem.hasNext()) 
	{
        hashItem.next();
		if (upd.length() > 0) 
			upd.append(", ");

		upd.append(hashItem.key());
		upd.append("=");

		QVariant val = hashItem.value();
		QString v = val.toString();

		if (val.type() == QVariant::String)
				v = "'" + v.replace("'", "''") + "'";

		upd.append(v);
    }

	sql = QString("update %1 set %2 where %3 = %4").arg(ent.Table()).arg(upd).arg(ent.Field(0)).arg(key);

	QSqlQuery updateQuery(_database->Database());

	QMutexLocker lock(&AddressesTable::_mutexAddresses);

	result = updateQuery.exec(sql);
	if (result) 
		result = updateQuery.numRowsAffected() == 1;
	
	return result;
}


bool AddressesTable::Update
(
	AddressesRecord& ent
)
{
	return Update(ent.Key(), ent.GetUpdateHash());
}


/**
	return entity list as DomDocument
	\param	ent sample enitity - if empty will eliminate filtering and return all
	\param  docName xml document root element
	\param  sort - column to sort by
	\return dom document object reference (use toString() to get xml string)


QDomDocument& AddressesTable::GetDom
(
	AddressesRecord& ent,
	QString docName,
	QString	sort,
	bool preferAttrib,
	bool skipEmpty,
	bool upperCase
)
{
	if (_dom)
		delete _dom;

	_dom = new QDomDocument;
	if (_dom)
	{
		AddressesRecordList list;
		AddressesRecordListIter iter;

		QDomElement root = _dom->createElement(docName);
		_dom->appendChild(root);


		Get(list, ent, sort);

		iter = list.begin();
		while (iter != list.end()) 
		{
			AddressesRecord ent = (*iter);
			QDomDocument xent = ent.GetDom(preferAttrib, skipEmpty, upperCase);
			root.appendChild(xent.firstChild());
			++iter;
		}
	}

	return *_dom;
}
*/
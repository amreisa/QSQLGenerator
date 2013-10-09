#include "<%table%>Table.h"
#include <QtDebug>
#include <QDomDocument>
#include <QSqlError>

#include "<%productName%>Database.h"

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
<%namespaceStart%>
QMutex <%table%>Table::_mutex;
QSqlQuery* <%table%>Table::_insertQuery(NULL);

<%table%>Table::<%table%>Table
(
	<%productName%>Database* database
): 
	_database(database)
{
	if (<%table%>Table::_insertQuery == NULL)
	{
		<%table%>Table::_insertQuery = new QSqlQuery(_database->Database());
		QString insertStatement = <%table%>Record::GetInsertStatement();

		<%table%>Table::_insertQuery->prepare(insertStatement);
	}
}

<%table%>Table::~<%table%>Table()
{
	_database = NULL;
}

QString <%table%>Table::_createStatement("<%createStmt%>");

bool <%table%>Table::Create(void)
{
	if (_database->Database().isValid())
	{
		QSqlQuery createQuery(_database->Database());

		return createQuery.exec(<%table%>Table::_createStatement);
	}

	return false;
}

bool <%table%>Table::Add
(
	<%table%>Record& record
) 
{
	bool result(false);	

	Q_ASSERT(<%table%>Table::_insertQuery != NULL);
	
	if (_database->Database().isValid())
	{
		for (quint32 i = 1; i < record.Count(); i++) // assumes _id is at position zero
		{
			QString bindName(":" + record.Field(i));

			<%table%>Table::_insertQuery->bindValue(bindName, record.FieldValue(i));
		}

		QMutexLocker lock(&<%table%>Table::_mutex);

		result = <%table%>Table::_insertQuery->exec();

		if (result) 
		{
			if (<%table%>Table::_insertQuery->numRowsAffected() == 1)
			{
				record.SetValue(0, <%table%>Table::_insertQuery->lastInsertId());
			} 
			else 
			{
				qDebug() << "Insert didn't affect exactly one row: " << _insertQuery->executedQuery();
				result = false; // exec lied
			}
		} 
		else 
		{
			QSqlError sqlError = <%table%>Table::_insertQuery->lastError();
			qDebug() << "Insert error: " << <%table%>Table::_insertQuery->executedQuery();
			qDebug() << "Insert error: " << sqlError.text();
		}
	}

	return result;
}

bool <%table%>Table::AddOrUpdate
(
	<%table%>Record& ent
)
{
	if (ent.Key() == 0)
		return Add(ent);
	
	return Update(ent);
}

bool <%table%>Table::ClearTable()
{
	bool result(false);

	QSqlQuery query(_database->Database());
	QString sql(QString("delete from \"<%table%>\""));

	QMutexLocker lock(&<%table%>Table::_mutex);

	if (query.exec(sql)) 	
		result = true;

	return result;
}

bool <%table%>Table::Get
(
	quint32 key,
	<%table%>Record& record
)
{
	bool result(false);

	QSqlQuery query(_database->Database());
	QString sql(QString("select * from \"<%table%>\" where %1 = %2").arg(record.Field(0)).arg(key));

	QMutexLocker lock(&<%table%>Table::_mutex);

	if (query.exec(sql)) 
	{	
		// we assume that order of columns is the same as order of entity fields - guaranteed by Dal code generator
		if (query.next()) 
		{
			// copy fields
			for (quint32 i(0); i < record.Count(); i++) 
			{
				record.SetValue(i, query.value(i));
				record.Changed(i, false);
			}
			
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

bool <%table%>Table::Get
(	
	QList<<%table%>Record>& list,
	<%table%>SelectionCriteria& constraints
)
{
	bool result(false);

	list.clear();

	QString sql = "select * from \"<%table%>\"";

	if (constraints.GetFilterStatement().length() > 0)
		sql.append(" where " + constraints.GetFilterStatement());

	if (constraints.GetOrderStatement().size() > 0)
		sql.append(" order by " + constraints.GetOrderStatement());

	QMutexLocker lock(&<%table%>Table::_mutex);

	QSqlQuery query(_database->Database());

	if (query.exec(sql)) 
	{	
		// we assume the order of columns is the same as order of entity fields - guaranteed by Dal code generator
		while (query.next()) 
		{
			<%table%>Record retrieved;
			
			for (quint32 i(0); i < retrieved.Count(); i++) 
			{
				retrieved.SetValue(i, query.value(i));
				retrieved.Changed(i, false);
			}
			
			list.push_back(retrieved);

			result = true;
		}
	} 
	else 
	{
		//qDebug("ERROR");
	}

	return result;
}

quint32	<%table%>Table::GetKey
(
	<%table%>Record& ent, 
	const QString& colName, 
	const QString& colValue
) 
{
	quint32 id = -1;
	QString sql;

	sql = QString("select \"%1\" from \"<%table%>\" where %2 = '%3'").arg(ent.Field(0)).arg(colName)
		.arg(QString(colValue).replace('\'', "''"));

	QSqlQuery query(_database->Database());
	QMutexLocker lock(&<%table%>Table::_mutex);

	if (query.exec(sql) && query.next()) 
		id = query.value(0).toInt();

	return id;
}

bool <%table%>Table::Delete
(
	quint32 key
)
{
	<%table%>Record ent;
	bool result(false);

	QString sql(QString("delete from \"<%table%>\" where %1 = %2").arg(ent.Field(0)).arg(key));

	QSqlQuery deleteQuery(_database->Database());
	QMutexLocker lock(&<%table%>Table::_mutex);

	result = deleteQuery.exec(sql);

	if (result)
		result = deleteQuery.numRowsAffected() == 1;

	return result;

}

bool <%table%>Table::Update
(
	quint32 key, 
	const QHash<QString, QVariant>& updates
) 
{
	<%table%>Record ent;

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

	sql = QString("update \"<%table%>\" set \"%1\" where \"%2\" = %3").arg(upd).arg(ent.Field(0)).arg(key);

	QSqlQuery updateQuery(_database->Database());

	QMutexLocker lock(&<%table%>Table::_mutex);

	result = updateQuery.exec(sql);
	if (result) 
		result = updateQuery.numRowsAffected() == 1;
	
	return result;
}


bool <%table%>Table::Update
(
	<%table%>Record& ent
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


QDomDocument& <%table%>Table::GetDom
(
	<%table%>Record& ent,
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
		<%table%>RecordList list;
		<%table%>RecordListIter iter;

		QDomElement root = _dom->createElement(docName);
		_dom->appendChild(root);


		Get(list, ent, sort);

		iter = list.begin();
		while (iter != list.end()) 
		{
			<%table%>Record ent = (*iter);
			QDomDocument xent = ent.GetDom(preferAttrib, skipEmpty, upperCase);
			root.appendChild(xent.firstChild());
			++iter;
		}
	}

	return *_dom;
}
*/
<%namespaceEnd%>
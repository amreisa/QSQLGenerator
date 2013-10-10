#ifndef <%TABLE%>TABLE_H
#define <%TABLE%>TABLE_H

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

#include <QMutex>
#include "<%table%>Record.h"
#include "<%table%>SelectionCriteria.h"

#include "<%productInclude%>"

class QSqlQuery;
<%namespaceStart%>
class <%productName%>Database;

class <%dllExport%> <%table%>Table
{
friend class <%productName%>Database;

public:
	bool Create(void);

	bool Add(<%table%>Record& ent);
	bool AddOrUpdate(<%table%>Record& ent);
	bool ClearTable(void);
	bool Delete(quint32 key);

	bool Get(QList<<%table%>Record>& recordList, <%table%>SelectionCriteria& constraints = <%table%>SelectionCriteria());
	bool Get(QList<QVariant>& columnList, const QString& columnName, <%table%>SelectionCriteria& constraints = <%table%>SelectionCriteria());
	bool Get(quint32 key, <%table%>Record& record);
	quint32 GetKey(<%table%>Record& record, const QString& propName, const QString& propValue);

	bool Update(quint32 key, const QHash<QString, QVariant>& updates);
	bool Update(<%table%>Record& record);

private:
	<%table%>Table(<%productName%>Database* database);
	virtual ~<%table%>Table(void);

	<%productName%>Database*	_database;
	static QMutex				_mutex;
	static QSqlQuery*			_insertQuery;
	static QString				_createStatement;
};
<%namespaceEnd%>
#endif

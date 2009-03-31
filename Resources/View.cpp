#include "<%record%>View.h"
#include <QtDebug>
#include <QDomDocument>
#include "<%productName%>Database.h"

//The MIT License
//
//Copyright (c) 2008 Michael Simpson
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

<%record%>View::<%record%>View() : 
	_db(<%productName%>Database::GetInstance()),
	_dom(0)
{
	_db->open();
}

<%record%>View::~<%record%>View()
{
	if (_dom)
		delete _dom;
}

/**
	get entity by example
	\return value list of entities
*/
bool <%record%>View::Get
(	
	QList<<%record%>Record>& list,
	<%record%>Record& ent,
	QString sort
)
{
	bool result(false);
	QSqlQuery query(*_db->GetDb());
	QString filter;

	list.clear();

	QMutexLocker lock(&_mutex);

	QString sql = "select * from " + ent.table();

	//qDebug() << "<%productName%>Adapter::get " << ent.table();

	if (!ent.isEmpty()) 
	{
		for (quint32 i(0); i < ent.count(); i++) 
		{
			// must use encoded values in filter
			QString val = ent.EncValue(i).toString().trimmed();
			//qDebug(ent.field(i) + ": " + val);
			if (val.length() > 0) 
			{
				if (filter.length() > 0) 
					filter = filter.append(" and ");

				// number type?
				if (ent.PropDataType(i) == 'N') 
				{
					filter = filter.append(ent.Field(i) + " = " + val + " ");
				} 
				else 
				{
					// make sure to escape ' char
					val = val.replace('\'', "''");
					filter = filter.append(ent.Field(i) + " = '" + val + "' ");
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

	if (sort.length() > 0)
		sql.append(" order by " + sort);

	if (query.exec(sql)) 
	{	
		// we assume the order of columns is the same as order of entity fields - guaranteed by Dal code generator
		while (query.next()) 
		{
			<%record%>Record en(ent);
			// copy fields
			for (quint32 i(0); i < en.Count(); i++) 
				en.SetValue(i, query.value(i));
			
			list.push_back(en);
		}

		result = true;
	} 
	else 
	{
		//qDebug("ERROR");
	}

	return result;
}

/**
	return entity list as DomDocument
	\param	ent sample enitity - if empty will eliminate filtering and return all
	\param  docName xml document root element
	\param  sort - column to sort by
	\return dom document object reference (use toString() to get xml string)
*/

QDomDocument& <%record%>View::getDom
(
	<%record%>Record& ent,
	QString docName,
	QString	sort,
	bool preferAttrib,
	bool skipEmpty,
	bool upperCase
)
{
	<%record%>RecordList list;
	<%record%>RecordListIter iter;

	if (_dom)
		delete _dom;

	_dom = new QDomDocument;

	QDomElement root = _dom->createElement(docName);
    _dom->appendChild(root);
	get(list, ent, sort);
	iter = list.begin();
	while (iter != list.end()) 
	{
		<%record%>Record e = (*iter);
		QDomDocument xent = e.getDom(preferAttrib, skipEmpty, upperCase);
		root.appendChild(xent.firstChild());
		++iter;
	}

	return *_dom;
}


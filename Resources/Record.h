#ifndef <%TABLE%>RECORD_H
#define <%TABLE%>RECORD_H

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

// This File was auto generated on <%date%> by QTSqliteGen.  
// Do not modify!!!!!
// Changes will be overwritten.

#include <QList>
#include <QSqlQuery>

#include "RecordBase.h"
#include "<%productInclude%>"

class <%dllExport%> <%table%>Record : 
	public RecordBase
{
public:
	<%table%>Record(void);
	<%table%>Record(const <%table%>Record& copyMe);
	virtual ~<%table%>Record(void);

	<%table%>Record& operator=(const <%table%>Record& assignMe);

	void FieldList(QStringList& fields);

	enum Fields 
	{
<%fieldEnum%>
	};

<%accessors%>
	QString GetCreateStatement(void);
	static QString GetInsertStatement(void)
		{ return <%table%>Record::_insertStatement; }

private:
	static QString				_insertStatement;
};

typedef QList<<%table%>Record> <%table%>RecordList;
typedef QList<<%table%>Record>::iterator <%table%>RecordListIter;

#endif

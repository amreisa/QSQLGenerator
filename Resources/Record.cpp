#include "<%table%>Record.h"
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

// This File was auto generated on <%date%> by <%applicationName%>.  
// Do not modify!!!!!
// Changes will be overwritten.

<%namespaceStart%>

QString <%table%>Record::_insertStatement
(
	"Insert into \"<%table%>\""
	"(<%fields%>) "
	"values (<%binds%>)"
);

<%table%>Record::<%table%>Record(): 
	RecordBase("<%table%>", <%fieldCount%>)
{
<%propertyDataTypes%>
}

<%table%>Record::~<%table%>Record()
{
}

<%table%>Record::<%table%>Record
(
	const <%table%>Record& copyMe
):
	RecordBase(copyMe)
{
}

<%table%>Record& <%table%>Record::operator=
(
	const <%table%>Record& assignMe
)
{
	RecordBase::AssignMe(&assignMe);

	return *this;
}

void <%table%>Record::FieldList
(
	QStringList& stringList
)
{
	quint32 i(0);

	stringList.clear();

	while (i < Count())
	{
		stringList.push_back(Field(i));
		i++;
	}
}

<%accessors%>

<%namespaceEnd%>

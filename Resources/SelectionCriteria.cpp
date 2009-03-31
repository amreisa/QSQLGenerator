#include "<%table%>SelectionCriteria.h"
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

<%table%>SelectionCriteria::<%table%>SelectionCriteria() :
	_filterConjunction(false),
	_levels(0)
{
}

<%table%>SelectionCriteria::~<%table%>SelectionCriteria()
{
}

<%table%>SelectionCriteria::<%table%>SelectionCriteria
(
	const <%table%>SelectionCriteria& copyMe
)
{
	_levels = copyMe._filterConjunction;
	_filterConjunction = copyMe._filterConjunction;
	_filterStatement = copyMe._filterStatement;
	_orderStatement = copyMe._orderStatement;
}
				// number type?
				/*switch (record.FieldDataType(i))
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
				*/

void <%table%>SelectionCriteria::AppendFilterCondition
(
	const QString& filterCondition
)
{
	if (_filterConjunction == false)
		And();

	_filterStatement += filterCondition;

	_filterConjunction = false;
}

void <%table%>SelectionCriteria::AppendSortCriteria
(
	const QString& sortCriteria,
	bool ascending
)
{
	if (_orderStatement.indexOf(sortCriteria) == -1)
	{
		if (_orderStatement.size())
			_orderStatement.append(", "); 

		_orderStatement += sortCriteria;

		if (ascending == false)
			_orderStatement += " desc";
	}
}

void <%table%>SelectionCriteria::And()
{
	if (_filterConjunction == false && _filterStatement.size() > 0)
	{
		_filterStatement += " and "; 
		_filterConjunction = true;
	}
}

void <%table%>SelectionCriteria::Or()
{	
	if (_filterConjunction == false && _filterStatement.size() > 0)
	{
		_filterStatement += " or "; 
		_filterConjunction = true;
	}
}

void <%table%>SelectionCriteria::BeginPrecedence()
{	
	_filterStatement += "(";
	_levels++;
}

void <%table%>SelectionCriteria::EndPrecedence()
{
	if (_levels > 0)
	{
		_filterStatement += ")";
		_levels--;
	}
}


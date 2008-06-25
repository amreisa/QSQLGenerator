#include "EmployeesRecord.h"
#include "TestDatabase.h"

// This File was auto generated on Sun Apr 20 2008 by QTSqliteGen.  
// Do not modify!!!!!
// Changes will be overwritten.

QString EmployeesRecord::_insertStatement
(
	"Insert into Employees"
	"(Name, EmployeeNumber) "
	"values (:Name, :EmployeeNumber)"
);

EmployeesRecord::EmployeesRecord(): 
	RecordBase("Employees", 3)
{
	SetFieldDataType(f_Id, "_id", eInteger);
	SetFieldDataType(f_Name, "Name", eString);
	SetFieldDataType(f_EmployeeNumber, "EmployeeNumber", eInteger);

}

EmployeesRecord::~EmployeesRecord()
{
}

EmployeesRecord::EmployeesRecord(const EmployeesRecord& copyMe):
	RecordBase(copyMe)
{
}

void EmployeesRecord::FieldList
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

void EmployeesRecord::setId
(
	quint32 id
)
{
	_recordData[f_Id]._value = id;
	_recordData[f_Id]._changed = true;
}

quint32 EmployeesRecord::Id()
{
	return _recordData[f_Id]._value.toInt();
}
void EmployeesRecord::setName
(
	const QString& name
)
{
	_recordData[f_Name]._value = name;
	_recordData[f_Name]._changed = true;
}

QString EmployeesRecord::Name()
{
	return _recordData[f_Name]._value.toString();
}
void EmployeesRecord::setEmployeeNumber
(
	quint32 employeeNumber
)
{
	_recordData[f_EmployeeNumber]._value = employeeNumber;
	_recordData[f_EmployeeNumber]._changed = true;
}

quint32 EmployeesRecord::EmployeeNumber()
{
	return _recordData[f_EmployeeNumber]._value.toInt();
}


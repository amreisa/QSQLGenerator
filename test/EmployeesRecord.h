#ifndef EMPLOYEESRECORD_H
#define EMPLOYEESRECORD_H

// This File was auto generated on Sun Apr 20 2008 by QTSqliteGen.  
// Do not modify!!!!!
// Changes will be overwritten.

#include <QList>
#include <QSqlQuery>

#include "RecordBase.h"
#include "TestExport.h"

class DLL_TEST EmployeesRecord : 
	public RecordBase
{
public:
	EmployeesRecord(void);
	EmployeesRecord(const EmployeesRecord& copyMe);
	virtual ~EmployeesRecord(void);

	void FieldList(QStringList& fields);

	enum Fields 
	{
		f_Id,
		f_Name,
		f_EmployeeNumber
	};

	void setId(quint32 id);
	quint32 Id(void);

	void setName(const QString& name);
	QString Name(void);

	void setEmployeeNumber(quint32 employeeNumber);
	quint32 EmployeeNumber(void);

	QString GetCreateStatement(void);
	static QString GetInsertStatement(void)
		{ return EmployeesRecord::_insertStatement; }

private:
	static QString				_insertStatement;
};

typedef QList<EmployeesRecord> EmployeesRecordList;
typedef QList<EmployeesRecord>::iterator EmployeesRecordListIter;

#endif

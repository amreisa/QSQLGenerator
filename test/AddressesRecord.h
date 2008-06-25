#ifndef ADDRESSESRECORD_H
#define ADDRESSESRECORD_H

// This File was auto generated on Sun Apr 20 2008 by QTSqliteGen.  
// Do not modify!!!!!
// Changes will be overwritten.

#include <QList>
#include <QSqlQuery>

#include "RecordBase.h"
#include "TestExport.h"

class DLL_TEST AddressesRecord : 
	public RecordBase
{
public:
	AddressesRecord(void);
	AddressesRecord(const AddressesRecord& copyMe);
	virtual ~AddressesRecord(void);

	void FieldList(QStringList& fields);

	enum Fields 
	{
		f_Id,
		f_Foo,
		f_Bar,
		f_Snafu,
		f_Gooid,
		f_RealItem,
		f_MemoItem,
		f_VarCharItem,
		f_NumericItem,
		f_IntItem,
		f_DateItem,
		f_TimeItem,
		f_FloatItem,
		f_DoubleItem
	};

	void setId(quint32 id);
	quint32 Id(void);

	void setFoo(const QString& foo);
	QString Foo(void);

	void setBar(quint32 bar);
	quint32 Bar(void);

	void setSnafu(quint32 snafu);
	quint32 Snafu(void);

	void setGooid(quint32 gooid);
	quint32 Gooid(void);

	void setRealItem(qreal realItem);
	qreal RealItem(void);

	void setMemoItem(const QString& memoItem);
	QString MemoItem(void);

	void setVarCharItem(const QString& varCharItem);
	QString VarCharItem(void);

	void setNumericItem(quint32 numericItem);
	quint32 NumericItem(void);

	void setIntItem(quint32 intItem);
	quint32 IntItem(void);

	void setDateItem(const QDateTime& dateItem);
	QDateTime DateItem(void);

	void setTimeItem(const QTime& timeItem);
	QTime TimeItem(void);

	void setFloatItem(qreal floatItem);
	qreal FloatItem(void);

	void setDoubleItem(qreal doubleItem);
	qreal DoubleItem(void);

	QString GetCreateStatement(void);
	static QString GetInsertStatement(void)
		{ return AddressesRecord::_insertStatement; }

private:
	static QString				_insertStatement;
};

typedef QList<AddressesRecord> AddressesRecordList;
typedef QList<AddressesRecord>::iterator AddressesRecordListIter;

#endif

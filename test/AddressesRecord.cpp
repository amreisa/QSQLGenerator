#include "AddressesRecord.h"
#include "TestDatabase.h"

// This File was auto generated on Sun Apr 20 2008 by QTSqliteGen.  
// Do not modify!!!!!
// Changes will be overwritten.

QString AddressesRecord::_insertStatement
(
	"Insert into Addresses"
	"(foo, bar, snafu, gooid, realItem, memoItem, varCharItem, numericItem, intItem, dateItem, timeItem, floatItem, doubleItem) "
	"values (:foo, :bar, :snafu, :gooid, :realItem, :memoItem, :varCharItem, :numericItem, :intItem, :dateItem, :timeItem, :floatItem, :doubleItem)"
);

AddressesRecord::AddressesRecord(): 
	RecordBase("Addresses", 14)
{
	SetFieldDataType(f_Id, "_id", eInteger);
	SetFieldDataType(f_Foo, "foo", eString);
	SetFieldDataType(f_Bar, "bar", eInteger);
	SetFieldDataType(f_Snafu, "snafu", eInteger);
	SetFieldDataType(f_Gooid, "gooid", eInteger);
	SetFieldDataType(f_RealItem, "realItem", eReal);
	SetFieldDataType(f_MemoItem, "memoItem", eString);
	SetFieldDataType(f_VarCharItem, "varCharItem", eString);
	SetFieldDataType(f_NumericItem, "numericItem", eInteger);
	SetFieldDataType(f_IntItem, "intItem", eInteger);
	SetFieldDataType(f_DateItem, "dateItem", eDateTime);
	SetFieldDataType(f_TimeItem, "timeItem", eTime);
	SetFieldDataType(f_FloatItem, "floatItem", eReal);
	SetFieldDataType(f_DoubleItem, "doubleItem", eReal);

}

AddressesRecord::~AddressesRecord()
{
}

AddressesRecord::AddressesRecord(const AddressesRecord& copyMe):
	RecordBase(copyMe)
{
}

void AddressesRecord::FieldList
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

void AddressesRecord::setId
(
	quint32 id
)
{
	_recordData[f_Id]._value = id;
	_recordData[f_Id]._changed = true;
}

quint32 AddressesRecord::Id()
{
	return _recordData[f_Id]._value.toInt();
}
void AddressesRecord::setFoo
(
	const QString& foo
)
{
	_recordData[f_Foo]._value = foo;
	_recordData[f_Foo]._changed = true;
}

QString AddressesRecord::Foo()
{
	return _recordData[f_Foo]._value.toString();
}
void AddressesRecord::setBar
(
	quint32 bar
)
{
	_recordData[f_Bar]._value = bar;
	_recordData[f_Bar]._changed = true;
}

quint32 AddressesRecord::Bar()
{
	return _recordData[f_Bar]._value.toInt();
}
void AddressesRecord::setSnafu
(
	quint32 snafu
)
{
	_recordData[f_Snafu]._value = snafu;
	_recordData[f_Snafu]._changed = true;
}

quint32 AddressesRecord::Snafu()
{
	return _recordData[f_Snafu]._value.toInt();
}
void AddressesRecord::setGooid
(
	quint32 gooid
)
{
	_recordData[f_Gooid]._value = gooid;
	_recordData[f_Gooid]._changed = true;
}

quint32 AddressesRecord::Gooid()
{
	return _recordData[f_Gooid]._value.toInt();
}
void AddressesRecord::setRealItem
(
	qreal realItem
)
{
	_recordData[f_RealItem]._value = realItem;
	_recordData[f_RealItem]._changed = true;
}

qreal AddressesRecord::RealItem()
{
	return _recordData[f_RealItem]._value.toDouble();
}
void AddressesRecord::setMemoItem
(
	const QString& memoItem
)
{
	_recordData[f_MemoItem]._value = memoItem;
	_recordData[f_MemoItem]._changed = true;
}

QString AddressesRecord::MemoItem()
{
	return _recordData[f_MemoItem]._value.toString();
}
void AddressesRecord::setVarCharItem
(
	const QString& varCharItem
)
{
	_recordData[f_VarCharItem]._value = varCharItem;
	_recordData[f_VarCharItem]._changed = true;
}

QString AddressesRecord::VarCharItem()
{
	return _recordData[f_VarCharItem]._value.toString();
}
void AddressesRecord::setNumericItem
(
	quint32 numericItem
)
{
	_recordData[f_NumericItem]._value = numericItem;
	_recordData[f_NumericItem]._changed = true;
}

quint32 AddressesRecord::NumericItem()
{
	return _recordData[f_NumericItem]._value.toInt();
}
void AddressesRecord::setIntItem
(
	quint32 intItem
)
{
	_recordData[f_IntItem]._value = intItem;
	_recordData[f_IntItem]._changed = true;
}

quint32 AddressesRecord::IntItem()
{
	return _recordData[f_IntItem]._value.toInt();
}
void AddressesRecord::setDateItem
(
	const QDateTime& dateItem
)
{
	_recordData[f_DateItem]._value = EncodeDateTime(dateItem);
	_recordData[f_DateItem]._changed = true;
}

QDateTime AddressesRecord::DateItem()
{
	return DecodeDateTime(_recordData[f_DateItem]._value.toString());
}
void AddressesRecord::setTimeItem
(
	const QTime& timeItem
)
{
	_recordData[f_TimeItem]._value = EncodeTime(timeItem);
	_recordData[f_TimeItem]._changed = true;
}

QTime AddressesRecord::TimeItem()
{
	return DecodeTime(_recordData[f_TimeItem]._value.toString());
}
void AddressesRecord::setFloatItem
(
	qreal floatItem
)
{
	_recordData[f_FloatItem]._value = floatItem;
	_recordData[f_FloatItem]._changed = true;
}

qreal AddressesRecord::FloatItem()
{
	return _recordData[f_FloatItem]._value.toDouble();
}
void AddressesRecord::setDoubleItem
(
	qreal doubleItem
)
{
	_recordData[f_DoubleItem]._value = doubleItem;
	_recordData[f_DoubleItem]._changed = true;
}

qreal AddressesRecord::DoubleItem()
{
	return _recordData[f_DoubleItem]._value.toDouble();
}


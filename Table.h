#ifndef TABLE_H
#define TABLE_H

#include <QList>
#include <QString>

struct Column
{
	Column()
	{
		_type = eUnknown;
	}

	Column(const Column& copyMe)
	{
		_name = copyMe._name;
		_type = copyMe._type;
	}

	QString						_name;
	enum Type
	{
		eUnknown,
		eText,
		eInt,
		eUInt,
		eReal,
		eDate,
		eTime,
		eDateTime,
		eGuid,
		eBoolean
	}							_type;
};

typedef QList<Column> Columns;
typedef QList<Column>::const_iterator ColumnIter;

struct Table
{
	Table()
	{
	}

	Table(const Table& copyMe)
	{
		_type = copyMe._type;
		_name = copyMe._name;
		_createStatement = copyMe._createStatement;

		_columns.clear();

		ColumnIter iter = (ColumnIter) copyMe._columns.begin();
		while (iter != copyMe._columns.end())
		{
			_columns.push_back((*iter));
			iter++;
		}
	}

	enum Type
	{
		eTable,
		eView,
		eIndex
	}							_type;
	QString						_name;
	QString						_createStatement;
	QList<Column>				_columns;
};

typedef QList<Table> Tables;
typedef QList<Table>::iterator TableIter;


#endif

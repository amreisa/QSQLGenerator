#ifndef TABLE_H
#define TABLE_H

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

#include <QList>
#include <QString>

struct Column
{
    Column()
    {
        _type = eUnknown;
    }

    Column( const Column& copyMe )
    {
        _name = copyMe._name;
        _type = copyMe._type;
    }

    QString                     _name;
    enum Type
    {
        eUnknown,
        eText,
        eBytes,
        eInt,
        eUInt,
        eReal,
        eDate,
        eTime,
        eDateTime,
        eGuid,
        eBoolean
    }                           _type;
};

typedef QList<Column> Columns;
typedef QList<Column>::const_iterator ColumnIter;

struct Table
{
    Table()
    {
    }

    Table( const Table& copyMe )
    {
        _type = copyMe._type;
        _name = copyMe._name;
        _createStatement = copyMe._createStatement;

        _columns.clear();

        ColumnIter iter = ( ColumnIter )copyMe._columns.begin();
        while ( iter != copyMe._columns.end() )
        {
            _columns.push_back( ( *iter ) );
            iter++;
        }
    }

    enum Type
    {
        eTable,
        eView,
        eIndex
    }                           _type;
    QString                     _name;
    QString                     _createStatement;
    QList<Column>               _columns;
};

typedef QList<Table> Tables;
typedef QList<Table>::iterator TableIter;


#endif

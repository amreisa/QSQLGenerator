#ifndef DALGEN_H
#define DALGEN_H

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

#include <QDate>
#include <QDialog>
#include <QtSql>
#include "ui_QTSqliteGen.h"
#include "Table.h"

class QTSqliteGen : 
	public QDialog, 
	public Ui::QTSqliteGenClass
{
    Q_OBJECT

public:
    QTSqliteGen(QWidget *parent = NULL, Qt::WFlags flags = 0);
    ~QTSqliteGen();

private slots:
	void on__aboutButton_clicked(void);
	void on__staticRadio_toggled(bool);
	void on__dynamicRadio_toggled(bool);
    void on__locatePath_clicked(void);
	void on__locateTarget_clicked(void);
    void on__genDal_clicked(void);
	void on__replaceProject_stateChanged(int state);

private:
	void SetProductName(const QString& databaseName);
	void GenCode(Tables & tables, const QString templateFile);
	void GenRecordHeader(Table& table);
	void GenRecordSource(Table& table);
	void GenSelectionHeader(Table& table);
	void GenSelectionSource(Table& table);
	QString GenerateAccessor(const QString& name, const QString& type);
	QString GenerateAccessorSource(const QString& tableName, const QString& name, const Column::Type type);
	QString GenerateSelector(const QString& tableName, const QString& name, const Column::Type type);
	QString GenerateFieldType(const QString& columnName, const QString& name, Column::Type type);
	QString GenerateTableRoutine(const Table& table);
	void LoadColumns(void);
	void LoadTables(void);
	void AppendOutput(const QString& output);
	void WriteDatabaseFiles(void);
	void WriteStaticFiles(void);
	void WriteHeaderFile(void);
	void WriteExportHeaderFile(void);
	void WriteProject(void);

	void AddHeaderFile(const QString& headerFileName);
	void AddSourceFile(const QString& sourceFileName);

	void LoadViews(void);
    void LoadViewColumns(void);
	
	void StandardReplacements(QByteArray& replaceMe);

	QSqlDatabase				_db;
	Tables						_tables;
	Tables                      _views;
	QStringList					_indexes;
	QString						_productName;
	QString						_dllExport;
	QString						_dllExportDefine;
	QString						_productInclude;
	QString						_projectGUID;
	QString						_headerFiles;
	QString						_headers;
	QString						_sources;
	QDate						_now;
};

#endif // DALGEN_H

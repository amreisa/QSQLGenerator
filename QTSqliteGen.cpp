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

#include "QTSqliteGen.h"
#include "AboutDlg.h"
#include <QDir>
#include <QFile>
#include <QFileDialog>
#include <QMap>
#include <QUuid>
#include <QSettings>
#include <QSqlError>

void InitializeMap(void);
QMap<QString, Column::Type> gDataMap;

QTSqliteGen::QTSqliteGen
(
	QWidget* parent, 
	Qt::WFlags flags
) : 
	QDialog(parent, flags),
	_projectGUID(QUuid::createUuid().toString().toUpper()),
	_now(QDate::currentDate())
{
	InitializeMap();

	setupUi(this);

	QSettings settings;

	QString lastPath = settings.value("lastDBPath").toString();
	if (lastPath.size())
	{
		if (QFile::exists(lastPath))
		{
			_dbPath->setText(lastPath);

			SetProductName(lastPath);

			settings.beginGroup("TargetPaths");
			QString regPath = _dbPath->text();
				
			regPath.replace("/", ".");

			QString lastTarget = settings.value(regPath).toString();
			settings.endGroup();

			if (lastTarget.size())
				_targetPath->setText(lastTarget);

		}
		else
			AppendOutput("Saved database source not found.");
	}

	_replaceProject->setChecked(settings.value("WriteProject").toBool());
	_dynamicRadio->setChecked(settings.value("Dynamic").toBool());
	_staticRadio->setChecked(settings.value("Dynamic").toBool() == false);

	QStringList paths = QCoreApplication::libraryPaths();

	AppendOutput("Paths:");
	for (int i = 0; i < paths.size(); i++) 
		AppendOutput("   " + paths.at(i));

	_db = QSqlDatabase::addDatabase("QSQLITE");
	if (_db.isValid() == false)
	{
		QSqlError se;
	
		se = _db.lastError();

		AppendOutput(se.text());
	}
}

QTSqliteGen::~QTSqliteGen()
{

}

void QTSqliteGen::on__replaceProject_stateChanged
(
	int state
)
{
	bool newState = false;

	switch (state)
	{
	case Qt::Unchecked:
	case Qt::PartiallyChecked:
		newState = false;
		break;

	case Qt::Checked:
		newState = true;
		break;
	}

	QSettings settings;

	settings.setValue("WriteProject", newState);
}

void QTSqliteGen::SetProductName
(
	const QString& databaseName
)
{
	quint32 pos = databaseName.lastIndexOf("/");
	if (pos != -1)
	{
		_productName = databaseName.mid(pos + 1);
		_productName = _productName.mid(0, _productName.indexOf("."));
	}
	else
	{
		_productName.clear();
	}

	_productName[0] = QChar(_productName[0]).toUpper();
	_productInclude = _productName + "Export.h";
	_dllExport = "DLL_" + _productName.toUpper();
	_dllExportDefine = _productName.toUpper() + "_DLL";
}

 void QTSqliteGen::on__locatePath_clicked()
 {
    QString s = QFileDialog::getOpenFileName(this,
		"Select a SQLite File", QDir::homePath(), "SQLite (*.dat *.db *.sdb *s3db)");
	if (s.size() > 0)
	{
		_dbPath->setText(s);	

		QSettings settings;

        settings.setValue("lastDBPath", s);

		settings.beginGroup("TargetPaths");
		
		QString regPath = _dbPath->text();
				
		regPath.replace("/", ".");
		QString lastTarget = settings.value(regPath).toString();
		settings.endGroup();

		if (lastTarget.size())
			_targetPath->setText(lastTarget);

		SetProductName(s);
	}
 }

void QTSqliteGen::on__locateTarget_clicked()
{    
	QString s = QFileDialog::getExistingDirectory(this, "Select the target directory for the source files.", 
		QDir::homePath());

	if (s.size() > 0)
	{
		QString regPath = _dbPath->text();

		regPath.replace("/", ".");

		_targetPath->setText(s);	

		QSettings settings;

		settings.beginGroup("TargetPaths");
        settings.setValue(regPath, s);
		settings.endGroup();
	}
}

void QTSqliteGen::on__genDal_clicked()
{
	if (_dbPath->text().size() > 0)
	{
		_db.setDatabaseName(_dbPath->text());
		if (_db.open())
		{
			_output->clear();
			_headerFiles.clear();
			_sources.clear();
			_headers.clear();

			LoadTables();
			LoadColumns();
            GenCode(_tables, "Table");

			LoadViews();
			LoadViewColumns();
            GenCode(_views, "View");
			
			WriteDatabaseFiles();
			WriteStaticFiles();
			WriteHeaderFile();
			WriteExportHeaderFile();

			if (_replaceProject->isChecked())
				WriteProject();
		}
		else
		{
			QSqlError se = _db.lastError();

			QStringList drivers = QSqlDatabase::drivers();

			AppendOutput("Open Database Error");
			AppendOutput(se.text());

			AppendOutput("Drivers");
			for (int i = 0; i < drivers.size(); ++i)
				AppendOutput("   " + drivers.at(i));
		}
	}
}

void QTSqliteGen::LoadTables()
{
	QSqlQuery query(_db);
	quint32 typePosition;
	quint32 namePosition;
	quint32 sqlPosition;

	if (query.exec("select * from sqlite_master")) 
	{    
		QSqlRecord rec = query.record();

		typePosition = rec.indexOf("type");
		namePosition = rec.indexOf("name");
		sqlPosition = rec.indexOf("sql");

		_tables.clear();
		_indexes.clear();

		while (query.next()) 
		{
			QString type = query.value(typePosition).toString();
			if (type == "table" | type == "index")
			{

				QString name = query.value(namePosition).toString();
				QString sql = query.value(sqlPosition).toString();

				if (sql.size() &&  name != "sqlite_sequence")
				{
					Table table;

					table._type = (type == "table") ? table.eTable : table.eIndex;
					table._name = name;
					table._createStatement = sql;

					_tables.push_back(table);
				}
			}
		}

		AppendOutput("Tables:");
		TableIter iter = _tables.begin();
		while (iter != _tables.end())
		{
			QString type = ((*iter)._type == (*iter).eTable) ? "   Table: " : "   Index: ";
			AppendOutput(type +  (*iter)._name);
			iter++;
		}
	}
	else
	{		
		QSqlError se = query.lastError();

		AppendOutput("Exec Failed");
		AppendOutput(se.text());
	}
}

void QTSqliteGen::LoadColumns()
{		
	TableIter iter = _tables.begin();
	while (iter != _tables.end())
	{
		QString tableName = (*iter)._name;
		if ((*iter)._type == (*iter).eTable)
		{
			QStringList columns;
			QString createQuery = (*iter)._createStatement;
			QString removeStr("Create table " + (*iter)._name + " (");
			createQuery = createQuery.simplified();
			createQuery.replace("[", "");
			createQuery.replace("]", "");
			createQuery.replace(")", "");
			createQuery.replace("\"", "");
			createQuery.replace(removeStr, "", Qt::CaseInsensitive);
			columns = createQuery.split(",");
			QStringList::iterator columnText = columns.begin();
			while (columnText != columns.end())
			{
				QStringList properties = (*columnText).split(" ", QString::SkipEmptyParts);		
				Column column;

				column._name = properties.at(0).simplified();
				QString columnType = properties.at(1).toLower();

				if (columnType.contains("varchar") == true)
					columnType = "text";

				QMap<QString, Column::Type>::iterator typeMapping = gDataMap.find(columnType.toLower());
				if (typeMapping != gDataMap.end())
					column._type = typeMapping.value();
				else
					column._type = column.eUnknown;

				if (column._type == column.eUnknown)
					AppendOutput("Unknown column type in Table:" + (*iter)._name + " Column:" + column._name + " " + columnType);
				else
					(*iter)._columns.push_back(column);
				
				columnText++;
			}
		}
		iter++;
	}
}
void QTSqliteGen::LoadViews()
{
	quint32 namePosition;
	quint32 sqlPosition;

	QSqlQuery query(_db);
	if (query.exec("select * from sqlite_master where type = 'view'")) 
	{    
		QSqlRecord rec = query.record();

		namePosition = rec.indexOf("name");
		sqlPosition = rec.indexOf("sql");

		_views.clear();

		AppendOutput("Views:");
		while (query.next()) 
		{
			QString name = query.value(namePosition).toString();
			QString sql = query.value(sqlPosition).toString();

			if ( sql.size() )
			{
				Table aView;

			    aView._type = aView.eView;
				aView._name = name;
				aView._createStatement = sql;

				_views.push_back(aView);

			    AppendOutput("   View: " +  aView._name);
			}
		}
	}
	else
	{		
		QSqlError se = query.lastError();

		AppendOutput("Exec Failed");
		AppendOutput(se.text());
	}
}


void QTSqliteGen::LoadViewColumns()
{		
	TableIter iter = _views.begin();
	while (iter != _views.end())
	{
		QString tableName = (*iter)._name;
	    if ((*iter)._type == (*iter).eView)    // safety check
		{
			QString createQuery = (*iter)._createStatement;
			createQuery = createQuery.simplified();
			createQuery.replace("[", "").replace("]", "").replace("\"", "");
			QString removeStr("Create view " + tableName + " as select");
			createQuery.replace(removeStr, "", Qt::CaseInsensitive);
			quint32 fromInd = createQuery.indexOf ( "From", 0, Qt::CaseInsensitive);
            createQuery = createQuery.left( fromInd );  // remove 'from' clause and everything after it

			QStringList columns = createQuery.split(",");
			QStringList::iterator columnText = columns.begin();
			while (columnText != columns.end())
			{
				QStringList properties = (*columnText).split("as", QString::SkipEmptyParts);		
				Column column;	
                column._type = column.eText;  //since create view sql does not contain type information
				column._name = properties.at(1).simplified();
				(*iter)._columns.push_back(column);

				columnText++;
			}
		}
		iter++;
	}
}

void QTSqliteGen::GenCode
(
	Tables& tables, 
	QString fileName
)
{		
	QFile templateFile;

	templateFile.setFileName(":/templates/Resources/" + fileName + ".h");
	
	if (templateFile.open(QIODevice::ReadOnly))
	{
		QByteArray headerTmpl;

		headerTmpl = templateFile.readAll();

		TableIter iter = tables.begin();
		while (iter != tables.end())
		{
			QByteArray header(headerTmpl);
			QString tableNameDefine = (*iter)._name.toUpper();

			StandardReplacements(header);

			header.replace("<%table%>", (*iter)._name.toAscii());
			header.replace("<%TABLE%>", tableNameDefine.toAscii());

			QFile headerFile;
			QString headerFilePath = _targetPath->text() + "/" + (*iter)._name + fileName + ".h";

			headerFile.setFileName(headerFilePath);
			if (headerFile.open(QIODevice::WriteOnly))
			{
				AddHeaderFile(QString((*iter)._name + fileName + ".h"));
				headerFile.write(header);

				AppendOutput(headerFilePath + " written");
				headerFile.close();
			}

			templateFile.close();
			
			GenRecordHeader(*iter);
			GenSelectionHeader(*iter);

			iter++;
		}

		templateFile.close();
	}
	else
	{
		QFile::FileError fileError = templateFile.error();
		
		AppendOutput("Error opening: " + templateFile.fileName() + ":" + QString::number(fileError));
	}

    templateFile.setFileName(":/templates/Resources/" + fileName + ".cpp");

	if (templateFile.open(QIODevice::ReadOnly))
	{		
		QByteArray sourceTmpl;

		sourceTmpl = templateFile.readAll();

		TableIter iter = tables.begin();
		while (iter != tables.end())
		{
			QByteArray sourceText(sourceTmpl);
			QString tableNameDefine = (*iter)._name.toUpper();

			StandardReplacements(sourceText);

			sourceText.replace("<%table%>", (*iter)._name.toAscii());
			sourceText.replace("<%TABLE%>", tableNameDefine.toAscii());

			QString createStatement((*iter)._createStatement);
			createStatement.replace("\"", ""); 
			createStatement.replace("\n", " ");
			createStatement.replace("\r", " ");
			createStatement.replace("  ", " ");
			sourceText.replace("<%createStmt%>", createStatement.toAscii());

			QFile cppFile;
			QString cppFilePath = _targetPath->text() + "/" + (*iter)._name + fileName + ".cpp";

			cppFile.setFileName(cppFilePath);
			if (cppFile.open(QIODevice::WriteOnly))
			{
				AddSourceFile((*iter)._name + fileName + ".cpp");
				cppFile.write(sourceText);

				AppendOutput(cppFilePath + " written");
				cppFile.close();
			}
			
			GenRecordSource(*iter);
			GenSelectionSource(*iter);

			iter++;
		}
		
		templateFile.close();
	}
	else
	{
		QFile::FileError fileError = templateFile.error();
		
		AppendOutput("Error opening: " + templateFile.fileName() + ":" + QString::number(fileError));
	}
}

QString QTSqliteGen::GenerateFieldType
(
	const QString& columnName,
	const QString& name, 
	Column::Type type
)
{	
	QString result;
	QString uName = name.mid(0, 1).toUpper() + name.mid(1);

	QFile templateFile;

	templateFile.setFileName(":/templates/Resources/fieldType");
	if (templateFile.open(QIODevice::ReadOnly))
	{	
		QByteArray fieldTypeTemplate;
		QByteArray fieldType;

		switch (type)
		{
		case Column::eText:
			fieldType = "eString";
			break;

		case Column::eInt:
			fieldType = "eInteger";
			break;

		case Column::eUInt:
			fieldType = "eInteger";
			break;

		case Column::eReal:
			fieldType = "eReal";
			break;

		case Column::eDate:
			fieldType = "eDate";
			break;

		case Column::eTime:
			fieldType = "eTime";
			break;

		case Column::eDateTime:
			fieldType = "eDateTime";
			break;

		case Column::eGuid:
			fieldType = "eQUuid";
			break;

		case Column::eBoolean:
			fieldType = "eBool";
			break;
		}

		fieldTypeTemplate = templateFile.readAll();

		fieldTypeTemplate.replace("<%fieldType%>", fieldType);
		fieldTypeTemplate.replace("<%uName%>", uName.toAscii());
		fieldTypeTemplate.replace("<%name%>", columnName.toAscii());

		StandardReplacements(fieldTypeTemplate);

		result = QString(fieldTypeTemplate);
		result += "\n";
	}

	return result;
}

QString QTSqliteGen::GenerateSelector
(
	const QString& tableName, 
	const QString& name, 
	const Column::Type type
)
{
	QString lName(name);
	lName.replace("_", "");
	lName[0] = QChar(lName[0]).toLower();

	QString uName(name);
	uName.replace("_", "");
	uName[0] = QChar(uName[0]).toUpper();

	QByteArray accessor;

	QFile templateFile;

	switch (type)
	{
	case Column::eText:
		templateFile.setFileName(":/templates/Resources/SelectorString");
		break;

	case Column::eDateTime:
		templateFile.setFileName(":/templates/Resources/SelectorDateTime");
		break;		
				
	case Column::eDate:	
		templateFile.setFileName(":/templates/Resources/SelectorDate");
		break;		

	case Column::eTime:
		templateFile.setFileName(":/templates/Resources/SelectorTime");
		break;		

	case Column::eReal:
		templateFile.setFileName(":/templates/Resources/SelectorReal");
		break;

	case Column::eInt:
		templateFile.setFileName(":/templates/Resources/SelectorInt");
		break;

	case Column::eGuid:
		templateFile.setFileName(":/templates/Resources/SelectorGUID");
		break;

	case Column::eBoolean:
		templateFile.setFileName(":/templates/Resources/SelectorBoolean");
		break;
	}

	if (templateFile.open(QIODevice::ReadOnly))
	{
		accessor = templateFile.readAll();

		accessor.replace(QByteArray("<%uName%>"), uName.toAscii());
		accessor.replace(QByteArray("<%tableName%>"), tableName.toAscii());
		accessor.replace(QByteArray("<%lName%>"), lName.toAscii());
		accessor.replace(QByteArray("<%name%>"), name.toAscii());

		StandardReplacements(accessor);
	}

	return QString(accessor);
}

void QTSqliteGen::GenRecordHeader
(
	Table& table
)
{
	QFile templateFile;

	templateFile.setFileName(":/templates/Resources/Record.h");
	if (templateFile.open(QIODevice::ReadOnly))
	{
		QByteArray header;
		QString fieldEnums;
		QString accessors;

		QString TABLENAME = table._name.toUpper();
		QString tableName = table._name;

		ColumnIter column = table._columns.begin();
		while (column != table._columns.end())
		{
			QString type;
			QString name = (*column)._name;

			name.replace("_", "");

			QString lName = name.mid(0, 1).toLower() + name.mid(1);
			QString uName = name.mid(0, 1).toUpper() + name.mid(1);

			if (column != table._columns.begin())
			{
				fieldEnums.append(",\n");
				accessors.append("\n");
			}

			fieldEnums.append("\t\tf_" + uName);

			switch ((*column)._type)
			{
			case Column::eText:
				accessors += GenerateAccessor(name, QString("const QString&"));
				break;

			case Column::eDateTime:
				accessors += GenerateAccessor(name, QString("const QDateTime&"));
				break;

			case Column::eDate:
				accessors += GenerateAccessor(name, QString("const QDate&"));
				break;

			case Column::eTime:
				accessors += GenerateAccessor(name, QString("const QTime&"));
				break;

			case Column::eInt:
				accessors += GenerateAccessor(name, QString("quint32"));
				break;

            case Column::eGuid:
				accessors += GenerateAccessor(name, QString("const QUuid&"));
				break;

            case Column::eReal:
				accessors += GenerateAccessor(name, QString("qreal"));
				break;

			case Column::eBoolean:
				accessors += GenerateAccessor(name, QString("bool"));
				break;

			default:
				AppendOutput("Unknown Type Error Table:" + table._name + " Column:" + (*column)._name);
				break;
			}

			column++;
		}

		header = templateFile.readAll();

		StandardReplacements(header);

		header.replace("<%table%>", tableName.toAscii());
		header.replace("<%TABLE%>", TABLENAME.toAscii());
		header.replace("<%fieldEnum%>", fieldEnums.toAscii());
		header.replace("<%accessors%>", accessors.toAscii());
		
		QString headerFilePath = _targetPath->text() + "/" + table._name + "Record.h";
		QFile headerFile;

		headerFile.setFileName(headerFilePath);
		if (headerFile.open(QIODevice::WriteOnly))
		{
			AddHeaderFile(QString(table._name + "Record.h"));

			headerFile.write(header);
		
			AppendOutput(headerFilePath + " written");
			headerFile.close();
		}
		else
		{
			AppendOutput("Error opening: " + headerFilePath);
		}
		
		templateFile.close();
	}
	else
	{
		QFile::FileError fileError = templateFile.error();
		
		AppendOutput("Error opening: " + templateFile.fileName() + ":" + QString::number(fileError));
	}
}

void QTSqliteGen::GenRecordSource
(
	Table& table
)
{
	QFile templateFile;

	templateFile.setFileName(":/templates/Resources/Record.cpp");
	if (templateFile.open(QIODevice::ReadOnly))
	{
		QByteArray cpp;
		QString fields;
		QString binds;
		QString accessors;
		QString propertyDataTypes;

		QString tableName = table._name;

		ColumnIter column = table._columns.begin();
		while (column != table._columns.end())
		{
			QString type;
			QString name = (*column)._name;

			name.replace("_", "");

			QString lName = name.mid(0, 1).toLower() + name.mid(1);
			QString uName = name.mid(0, 1).toUpper() + name.mid(1);

			if (column != table._columns.begin())
			{
				if (fields.size())
				{
					fields.append(", ");
					binds.append(", ");
				}
			}

			if ((*column)._name != "_id")
			{
				fields.append((*column)._name);
				binds.append(":" + (*column)._name);
			}

			propertyDataTypes.append(GenerateFieldType((*column)._name, lName, (*column)._type));
			accessors.append(GenerateAccessorSource(tableName, lName, (*column)._type));

			column++;
		}

		cpp = templateFile.readAll();

		StandardReplacements(cpp);

		cpp.replace("<%table%>", tableName.toAscii());
		cpp.replace("<%fields%>", fields.toAscii());
		cpp.replace("<%binds%>", binds.toAscii());
		cpp.replace("<%accessors%>", accessors.toAscii());
		cpp.replace("<%fieldCount%>", QByteArray::number(table._columns.size()));

		cpp.replace("<%propertyDataTypes%>", propertyDataTypes.toAscii());
		
		QString cppFilePath = _targetPath->text() + "/" + table._name + "Record.cpp";
		QFile cppFile;

		cppFile.setFileName(cppFilePath);
		if (cppFile.open(QIODevice::WriteOnly))
		{
			AddSourceFile(table._name + "Record.cpp");
			cppFile.write(cpp);

			AppendOutput(cppFilePath + " written");
			cppFile.close();
		}
		else
		{
			AppendOutput("Error opening: " + cppFilePath);
		}

		templateFile.close();
	}
	else
	{
		QFile::FileError fileError = templateFile.error();
		
		AppendOutput("Error opening: " + templateFile.fileName() + ":" + QString::number(fileError));
	}
}

// todo
void QTSqliteGen::GenSelectionHeader
(
	Table& table
)
{
	QFile templateFile;

	templateFile.setFileName(":/templates/Resources/SelectionCriteria.h");
	if (templateFile.open(QIODevice::ReadOnly))
	{
		QByteArray header;
		QString accessors;

		QString TABLENAME = table._name.toUpper();
		QString tableName = table._name;

		ColumnIter column = table._columns.begin();
		while (column != table._columns.end())
		{
			accessors.append(GenerateSelector(tableName, (*column)._name, (*column)._type));

			column++;
		}

		header = templateFile.readAll();

		StandardReplacements(header);

		header.replace("<%table%>", tableName.toAscii());
		header.replace("<%TABLE%>", TABLENAME.toAscii());
		header.replace("<%accessors%>", accessors.toAscii());
		
		QString headerFilePath = _targetPath->text() + "/" + table._name + "SelectionCriteria.h";
		QFile headerFile;

		headerFile.setFileName(headerFilePath);
		if (headerFile.open(QIODevice::WriteOnly))
		{
			AddHeaderFile(QString(table._name + "SelectionCriteria.h"));

			headerFile.write(header);
		
			AppendOutput(headerFilePath + " written");
			headerFile.close();
		}
		else
		{
			AppendOutput("Error opening: " + headerFilePath);
		}
		
		templateFile.close();
	}
	else
	{
		QFile::FileError fileError = templateFile.error();
		
		AppendOutput("Error opening: " + templateFile.fileName() + ":" + QString::number(fileError));
	}
}

void QTSqliteGen::GenSelectionSource
(
	Table& table
)
{
	QFile templateFile;

	templateFile.setFileName(":/templates/Resources/SelectionCriteria.cpp");
	if (templateFile.open(QIODevice::ReadOnly))
	{
		QByteArray cpp;
		QString tableName = table._name;

		cpp = templateFile.readAll();

		StandardReplacements(cpp);

		cpp.replace("<%table%>", tableName.toAscii());

		QString cppFilePath = _targetPath->text() + "/" + table._name + "SelectionCriteria.cpp";
		QFile cppFile;

		cppFile.setFileName(cppFilePath);
		if (cppFile.open(QIODevice::WriteOnly))
		{
			AddSourceFile(table._name + "SelectionCriteria.cpp");
			cppFile.write(cpp);

			AppendOutput(cppFilePath + " written");
			cppFile.close();
		}
		else
		{
			AppendOutput("Error opening: " + cppFilePath);
		}

		templateFile.close();
	}
	else
	{
		QFile::FileError fileError = templateFile.error();
		
		AppendOutput("Error opening: " + templateFile.fileName() + ":" + QString::number(fileError));
	}
}

QString QTSqliteGen::GenerateAccessor
(
	const QString& name, 
	const QString& type
)
{
	QString lName(name);
	lName.replace("_", "");
	lName[0] = QChar(lName[0]).toLower();

	QString uName(name);
	uName.replace("_", "");
	uName[0] = QChar(uName[0]).toUpper();

	QString inType(type);
	QString outType(type);

	QByteArray accessor;

	QFile templateFile;

	templateFile.setFileName(":/templates/Resources/Accessor");
	if (templateFile.open(QIODevice::ReadOnly))
	{
		accessor = templateFile.readAll();

		outType.replace(QString("&"), QString(""));
		outType.replace(QString("const "), QString(""));

		accessor.replace(QByteArray("<%uName%>"), uName.toAscii());
		accessor.replace(QByteArray("<%inType%>"), inType.toAscii());
		accessor.replace(QByteArray("<%name%>"), lName.toAscii());
		accessor.replace(QByteArray("<%outType%>"), outType.toAscii());

		StandardReplacements(accessor);
	}

	return QString(accessor);
}

QString QTSqliteGen::GenerateAccessorSource
(
	const QString& tableName, 
	const QString& name, 
	const Column::Type type
)
{	
	QString lName(name);
	lName.replace("_", "");
	lName[0] = QChar(lName[0]).toLower();

	QString uName(name);
	uName.replace("_", "");
	uName[0] = QChar(uName[0]).toUpper();

	QByteArray accessor;

	QFile templateFile;


	switch (type)
	{
	case Column::eText:
		templateFile.setFileName(":/templates/Resources/AccessorSourceString");
		break;

	case Column::eDateTime:
		templateFile.setFileName(":/templates/Resources/AccessorSourceDateTime");
		break;		
				
	case Column::eDate:	
		templateFile.setFileName(":/templates/Resources/AccessorSourceDate");
		break;		

	case Column::eTime:
		templateFile.setFileName(":/templates/Resources/AccessorSourceTime");
		break;		

	case Column::eReal:
		templateFile.setFileName(":/templates/Resources/AccessorSourceReal");
		break;

	case Column::eInt:
		templateFile.setFileName(":/templates/Resources/AccessorSourceInt");
		break;

	case Column::eGuid:
		templateFile.setFileName(":/templates/Resources/AccessorSourceGUID");
		break;

	case Column::eBoolean:
		templateFile.setFileName(":/templates/Resources/AccessorSourceBoolean");
		break;
	}

	if (templateFile.open(QIODevice::ReadOnly))
	{
		accessor = templateFile.readAll();

		accessor.replace(QByteArray("<%uName%>"), uName.toAscii());
		accessor.replace(QByteArray("<%tableName%>"), tableName.toAscii());
		accessor.replace(QByteArray("<%name%>"), lName.toAscii());

		StandardReplacements(accessor);
	}

	return QString(accessor);
}

QString QTSqliteGen::GenerateTableRoutine
(
	const Table& table
)
{
	QByteArray tableRoutine;
	QString type(table._type == Table::eTable ? "Table" : "View");
	QString tableName(table._name + type);
	QString variableName(table._name);

	variableName[0] = QChar(variableName[0]).toLower();
	variableName += type;

	QFile templateFile;

	templateFile.setFileName(":/templates/Resources/TableRoutine");
	if (templateFile.open(QIODevice::ReadOnly))
	{
		tableRoutine = templateFile.readAll();

		StandardReplacements(tableRoutine);
		
		tableRoutine.replace("<%tableName%>", tableName.toAscii());
		tableRoutine.replace("<%variableName%>", variableName.toAscii());

	}

	return tableRoutine;
}

void QTSqliteGen::WriteStaticFiles()
{
	QStringList staticFiles;
	QStringList::iterator staticFile;

	staticFiles << ":/templates/Resources/RecordBase.h" 
		<< ":/templates/Resources/RecordBase.cpp";

	if (_replaceProject->isChecked())
		staticFiles	<< ":/templates/Resources/CreateVCProj.bat";

	staticFile = staticFiles.begin();
	while (staticFile != staticFiles.end())
	{	
		QFile templateFile;

		templateFile.setFileName(*staticFile);
		if (templateFile.open(QIODevice::ReadOnly))
		{
			QByteArray templateStr = templateFile.readAll();

			QStringList parts = (*staticFile).split("/", QString::SkipEmptyParts);

			QString srcFilePath = _targetPath->text() + "/" + parts.last();
			QFile srcFile;

			srcFile.setFileName(srcFilePath);
			if (srcFile.open(QIODevice::WriteOnly))
			{
				if (parts.last().indexOf(".h") > -1)
					AddHeaderFile(parts.last());
				else if (parts.last().indexOf(".cpp") > -1)
					AddSourceFile(parts.last());

				StandardReplacements(templateStr);

				srcFile.write(templateStr);

				AppendOutput(srcFilePath + " written");
				srcFile.close();
			}
			else
			{
				AppendOutput("Error opening: " + srcFilePath);
			}

			templateFile.close();
		}
		else
		{
			QFile::FileError fileError = templateFile.error();
			
			AppendOutput("Error opening: " + templateFile.fileName() + ":" + QString::number(fileError));
		}
		staticFile++;
	}
}

void QTSqliteGen::WriteHeaderFile()
{
	QFile templateFile;
	QFile srcFile;

	templateFile.setFileName(":/templates/Resources/Header");

	if (templateFile.open(QIODevice::ReadOnly))
	{
		QByteArray templateStr = templateFile.readAll();

		QString srcFilePath = _targetPath->text() + "/" + _productName + "DB.h";

		srcFile.setFileName(srcFilePath);
		if (srcFile.open(QIODevice::WriteOnly))
		{
			StandardReplacements(templateStr);

			srcFile.write(templateStr);

			AppendOutput(srcFilePath + " written");
			srcFile.close();
		}
		else
		{
			AppendOutput("Error opening: " + srcFilePath);
		}
	}
	else
	{
		QFile::FileError fileError = templateFile.error();
		
		AppendOutput("Error opening: " + templateFile.fileName() + ":" + QString::number(fileError));
	}
}

void QTSqliteGen::WriteExportHeaderFile()
{
	QFile templateFile;
	QFile srcFile;

	templateFile.setFileName(":/templates/Resources/Header2");

	if (templateFile.open(QIODevice::ReadOnly))
	{
		QByteArray templateStr = templateFile.readAll();

		QString srcFilePath = _targetPath->text() + "/" + _productInclude;

		srcFile.setFileName(srcFilePath);
		if (srcFile.open(QIODevice::WriteOnly))
		{
			StandardReplacements(templateStr);

			srcFile.write(templateStr);

			AppendOutput(srcFilePath + " written");
			srcFile.close();

			AddHeaderFile(_productInclude);
		}
		else
		{
			AppendOutput("Error opening: " + srcFilePath);
		}
	}
	else
	{
		QFile::FileError fileError = templateFile.error();
		
		AppendOutput("Error opening: " + templateFile.fileName() + ":" + QString::number(fileError));
	}
}

void QTSqliteGen::WriteProject()
{
	QFile templateFile;
	QFile srcFile;

	templateFile.setFileName(":/templates/Resources/pro");
	if (templateFile.open(QIODevice::ReadOnly))
	{
		QByteArray templateStr = templateFile.readAll();

		QString srcFilePath = _targetPath->text() + "/" + _productName + "DB.pro";

		srcFile.setFileName(srcFilePath);
		if (srcFile.open(QIODevice::WriteOnly))
		{
			StandardReplacements(templateStr);

			templateStr.replace(QByteArray("<%linkage%>"), 
				_dynamicRadio->isChecked() ? "dll" : "staticlib" );

			templateStr.replace(QByteArray("<%linkagedef%>"), 
				_dynamicRadio->isChecked() ? "BASE_DLL" : "BASE_STATIC" );

			srcFile.write(templateStr);

			AppendOutput(srcFilePath + " written");
			srcFile.close();
		}
		else
		{
			AppendOutput("Error opening: " + srcFilePath);
		}
	}
	else
	{
		QFile::FileError fileError = templateFile.error();
		
		AppendOutput("Error opening: " + templateFile.fileName() + ":" + QString::number(fileError));
	}
}

void QTSqliteGen::AddHeaderFile
(
	const QString& headerFileName
)
{	
	if (_headerFiles.size())
		_headerFiles.append("\r\n");
	_headerFiles.append("#include \"" + headerFileName + "\"");

	if (_headers.size())
		_headers.append(" \\\r\n");
	_headers.append("\t" + headerFileName);

}

void QTSqliteGen::AddSourceFile
(
	const QString& sourceFileName
)
{
	if (_sources.size())
		_sources.append(" \\\r\n");

	_sources.append("\t" + sourceFileName );
}

void QTSqliteGen::AppendOutput
(
	const QString& output
)
{
	_output->append(output);
}

void QTSqliteGen::StandardReplacements
(
	QByteArray& replaceMe
)
{
	QString PRODUCTNAME = _productName.toUpper();

	replaceMe.replace("<%PRODUCTNAME%>", PRODUCTNAME.toAscii());
	replaceMe.replace("<%productInclude%>", _productInclude.toAscii());
	replaceMe.replace("<%productName%>", _productName.toAscii());
	replaceMe.replace("<%projectGUID%>", _projectGUID.toAscii());
	replaceMe.replace("<%dllExport%>", _dllExport.toAscii());
	replaceMe.replace("<%dllExportDefine%>", _dllExportDefine.toAscii());
	replaceMe.replace("<%includeFiles%>", _headerFiles.toAscii());
	replaceMe.replace("<%sources%>", _sources.toAscii());
	replaceMe.replace("<%headers%>", _headers.toAscii());
	replaceMe.replace("<%headerFiles%>", _headerFiles.toAscii());
	replaceMe.replace("<%date%>", _now.toString().toAscii());
}


void QTSqliteGen::on__aboutButton_clicked()
{
	AboutDlg aboutDlg(this);

	aboutDlg.exec();
}

void QTSqliteGen::on__dynamicRadio_toggled
(
	bool state
)
{
	QSettings settings;

	settings.setValue("Dynamic", state);
}

void QTSqliteGen::on__staticRadio_toggled
(
	bool state
)
{
	QSettings settings;

	settings.setValue("Dynamic", state == false);
}

void QTSqliteGen::WriteDatabaseFiles()
{
	QStringList staticFiles;
	QStringList::iterator staticFile;

	QFile templateFile;

	templateFile.setFileName(":/templates/Resources/Database.h");
	if (templateFile.open(QIODevice::ReadOnly))
	{
		QByteArray templateStr = templateFile.readAll();

		QString srcFilePath = _targetPath->text() + "/" + _productName + "Database.h";
		QFile srcFile;

		srcFile.setFileName(srcFilePath);
		if (srcFile.open(QIODevice::WriteOnly))
		{
			AddHeaderFile(_productName + "Database.h");

			QString includes;
			QString tables;
			QString views;
			QString tablePtrs;
	
			TableIter table = _tables.begin();
			while (table != _tables.end())
			{
				QString variableName((*table)._name);

				variableName[0] = QChar(variableName[0]).toLower();

				includes += "#include \"" + (*table)._name + "Table.h\"\n";

				tables += "\t" + (*table)._name + "Table* Get" + (*table)._name + "Table(void);\n";
				
				if (tablePtrs.size())
					tablePtrs += "\n";

				tablePtrs += "\t" + (*table)._name + "Table* \t_" + variableName + "Table;";

				table++;
			}	

			TableIter view = _views.begin();
			while (view != _views.end())
			{
				QString variableName((*view)._name);

				variableName[0] = QChar(variableName[0]).toLower();

				includes += "#include \"" + (*view)._name + "View.h\"\n";
				views += "\t" + (*view)._name + "View* Get" + (*view)._name + "View(void);\n";	
				
				if (tablePtrs.size())
					tablePtrs += "\n";
				tablePtrs += "\t_" + (*view)._name + "View* \t_" + variableName + "View;";

				view++;
			}

			templateStr.replace("<%tables%>", tables.toAscii());
			templateStr.replace("<%views%>", views.toAscii());
			templateStr.replace("<%tablePtrs%>", tablePtrs.toAscii());
			templateStr.replace("<%includes%>", includes.toAscii());
			StandardReplacements(templateStr);
			
			srcFile.write(templateStr);

			AppendOutput(srcFilePath + " written");
			srcFile.close();
		}
		else
		{
			AppendOutput("Error opening: " + srcFilePath);
		}

		templateFile.close();
	}
	else
	{
		QFile::FileError fileError = templateFile.error();
		
		AppendOutput("Error opening: " + templateFile.fileName() + ":" + QString::number(fileError));
	}

	templateFile.setFileName(":/templates/Resources/Database.cpp");
	if (templateFile.open(QIODevice::ReadOnly))
	{
		QByteArray templateStr = templateFile.readAll();

		QString srcFilePath = _targetPath->text() + "/" + _productName + "Database.cpp";
		QFile srcFile;

		srcFile.setFileName(srcFilePath);
		if (srcFile.open(QIODevice::WriteOnly))
		{
			AddSourceFile(_productName + "Database.cpp");

			QString statics;
			QString destructors;
			QString tableRoutines;
	
			TableIter table = _tables.begin();
			while (table != _tables.end())
			{
				QString variableName((*table)._name);

				variableName[0] = QChar(variableName[0]).toLower();

				if (statics.size())
					statics += ",\n";
				statics += "\t_" + variableName + "Table(NULL)";
				
				tableRoutines += GenerateTableRoutine(*table);

				destructors += "\tif (_" + variableName + "Table != NULL)\n\t{\n";
				destructors += "\t\tdelete _" + variableName + "Table;\n";
				destructors += "\t\t_" + variableName + "Table = NULL;\n";
				destructors += "\t}\n";
				table++;
			}	

			TableIter view = _views.begin();
			while (view != _views.end())
			{
				QString variableName((*view)._name);

				variableName[0] = QChar(variableName[0]).toLower();

				if (statics.size())
					statics += ",\n";

				statics += "\t_" + variableName + "View(NULL)";

				tableRoutines += GenerateTableRoutine(*view);

				destructors += "\tif (_" + variableName + "View != NULL)\n\t{\n";
				destructors += "\t\tdelete _" + variableName + "View;\n";
				destructors += "\t\t_" + variableName + "View = NULL;\n";
				destructors += "\t}\n";

				view++;
			}

			StandardReplacements(templateStr);

			templateStr.replace("<%destructors%>", destructors.toAscii());
			templateStr.replace("<%initializers%>", statics.toAscii());
			templateStr.replace("<%tableRoutines%>", tableRoutines.toAscii());
			
			srcFile.write(templateStr);

			AppendOutput(srcFilePath + " written");
			srcFile.close();
		}
		else
		{
			AppendOutput("Error opening: " + srcFilePath);
		}

		templateFile.close();
	}
	else
	{
		QFile::FileError fileError = templateFile.error();
		
		AppendOutput("Error opening: " + templateFile.fileName() + ":" + QString::number(fileError));
	}
}

void InitializeMap()
{	
	gDataMap["text"] = Column::eText; 
	gDataMap["memo"] = Column::eText;
	gDataMap["varchar"] = Column::eText; 
	gDataMap["quint32"] = Column::eUInt;
	gDataMap["numeric"] = Column::eInt;
	gDataMap["integer"] = Column::eInt;
	gDataMap["int"] = Column::eInt;
	gDataMap["datetime"] = Column::eDateTime;
	gDataMap["date"] = Column::eDate;
	gDataMap["time"] = Column::eTime;
	gDataMap["guid"] = Column::eGuid;
	gDataMap["float"] = Column::eReal;
	gDataMap["double"] = Column::eReal;
	gDataMap["real"] = Column::eReal;
	gDataMap["bool"] = Column::eBoolean;
	gDataMap["boolean"] = Column::eBoolean;
};
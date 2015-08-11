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

#include "QTSqlGen.h"
#include "AboutDlg.h"
#include "SqlProjects.h"

#include <QtAlgorithms>
#include <QDir>
#include <QFile>
#include <QFileDialog>
#include <QMap>
#include <QMessageBox>
#include <QUuid>
#include <QSettings>
#include <QSqlError>
#include <QVariant>

void InitializeMap( void );
QMap<QString, Column::Type> gDataMap;

QTSqlGen::QTSqlGen( QWidget* parent, Qt::WindowFlags flags ) :
    QDialog( parent, flags ),
    _projectGUID( QUuid::createUuid().toString().toUpper() ),
    _now( QDate::currentDate() )
{
    InitializeMap();
    setupUi( this );
    PopulateDrivers();
    LoadProjects();
    QStringList paths = QCoreApplication::libraryPaths();
    AppendOutput( tr( "Paths:" ) );
    for ( int i = 0; i < paths.size(); i++ )
        AppendOutput( "   " + paths.at( i ) );
}

QTSqlGen::~QTSqlGen()
{
    SaveProjects();
}

void QTSqlGen::SetProductName( const QString& databaseName )
{
    quint32 pos = databaseName.lastIndexOf( "/" );
    if ( pos != -1 )
    {
        _productName = databaseName.mid( pos + 1 );
        _productName = _productName.mid( 0, _productName.indexOf( "." ) );
    }
    else
    {
        _productName = databaseName;
    }

    _productName[0] = QChar( _productName[0] ).toUpper();
    _productInclude = _productName + "Export.h";
    _dllExport = "DLL_" + _productName.toUpper();
    _dllExportDefine = _productName.toUpper() + "_DLL";
}

void QTSqlGen::on__locatePath_clicked()
{
    SqlProject* currentProject = GetProject();

    if ( currentProject != NULL )
    {
        QString s = QFileDialog::getOpenFileName( this, tr( "Select a SQLite File" ), QDir::homePath(), "SQLite (*.dat *.db *.sdb *s3db)" );
        if ( s.size() > 0 )
        {
            _dbPath->setText( s );

            currentProject->_databasePath = s;
        }
    }
}

void QTSqlGen::on__locateTarget_clicked()
{
    SqlProject* currentProject = GetProject();

    if ( currentProject != NULL )
    {
        QString s = QFileDialog::getExistingDirectory( this, tr( "Select the target directory for the source files." ), QDir::homePath() );

        if ( s.size() > 0 )
        {
            _targetPath->setText( s );
            currentProject->_targetPath = s;
        }
    }
}

void QTSqlGen::on__genDal_clicked()
{
    SqlProject* currentProject = GetProject();

    if ( currentProject == NULL )
        return;

    if ( currentProject->_projectName.size() == 0 || currentProject->_projectName == "<Untitled>" )
    {
        AppendOutput( tr( "Supply a Product Name" ) );
        return;
    }

    SetProductName( currentProject->_projectName );

    if ( _targetPath->text().size() == 0 )
        return;

    switch ( currentProject->_sourceType )
    {
    case eODBC:
        _db = QSqlDatabase::addDatabase( "QODBC" );
        break;

    case eSqlite:
        if ( _dbPath->text().size() == 0 )
            return ;

        _db = QSqlDatabase::addDatabase( "QSQLITE" );
        break;

    default:
        AppendOutput( tr( "Invalid Source" ) );
        return;
    }

    if ( _db.isValid() == false )
    {
        QSqlError se;

        se = _db.lastError();

        AppendOutput( se.text() );

        return;
    }

    switch ( currentProject->_sourceType )
    {
    case eODBC:
        _db.setDatabaseName( _connectionString->text() );
        break;

    case eSqlite:
        _db.setDatabaseName( _dbPath->text() );
        break;

    }

    if ( _db.open() )
    {
        _output->clear();
        _headerFiles.clear();
        _sources.clear();
        _headers.clear();

        switch ( currentProject->_sourceType )
        {
        case eODBC:
            LoadODBCTables();
            LoadODBCColumns();
            break;

        case eSqlite:
            LoadSqliteTables();
            LoadSqliteColumns();
            break;
        }

        GenCode( _tables, "Table" );

        switch ( currentProject->_sourceType )
        {
        case eODBC:
            /*LoadODBCViews();
            LoadODBCViewColumns();*/
            break;

        case eSqlite:
            LoadSqliteViews();
            LoadSqliteViewColumns();
            break;
        }

        GenCode( _views, "View" );

        WriteDatabaseFiles();
        WriteStaticFiles();
        WriteHeaderFile();
        WriteExportHeaderFile();

        if ( _replaceProject->isChecked() )
            WriteProject();
    }
    else
    {
        QSqlError se = _db.lastError();

        QStringList drivers = QSqlDatabase::drivers();

        AppendOutput( tr( "Open Database Error" ) );
        AppendOutput( se.text() );

        AppendOutput( tr( "Drivers" ) );
        for ( int i = 0; i < drivers.size(); ++i )
            AppendOutput( "   " + drivers.at( i ) );
    }
}

void QTSqlGen::LoadSqliteTables()
{
    QSqlQuery query( _db );
    quint32 typePosition;
    quint32 namePosition;
    quint32 sqlPosition;

    if ( query.exec( "select * from sqlite_master" ) )
    {
        QSqlRecord rec = query.record();

        typePosition = rec.indexOf( "type" );
        namePosition = rec.indexOf( "name" );
        sqlPosition = rec.indexOf( "sql" );

        _tables.clear();
        _indexes.clear();

        while ( query.next() )
        {
            QString type = query.value( typePosition ).toString();
            if ( type == "table" | type == "index" )
            {

                QString name = query.value( namePosition ).toString();
                QString sql = query.value( sqlPosition ).toString();

                if ( sql.size() &&  name != "sqlite_sequence" )
                {
                    Table table;

                    table._type = ( type == "table" ) ? table.eTable : table.eIndex;
                    table._name = name;
                    table._createStatement = sql;

                    _tables.push_back( table );
                }
            }
        }

        AppendOutput( "Tables:" );
        TableIter iter = _tables.begin();
        while ( iter != _tables.end() )
        {
            QString type = ( ( *iter )._type == ( *iter ).eTable ) ? "   Table: " : "   Index: ";
            AppendOutput( type +  ( *iter )._name );
            iter++;
        }
    }
    else
    {
        QSqlError se = query.lastError();

        AppendOutput( "Exec Failed" );
        AppendOutput( se.text() );
    }
}

void QTSqlGen::LoadSqliteColumns()
{
    TableIter iter = _tables.begin();
    while ( iter != _tables.end() )
    {
        QString tableName = ( *iter )._name;
        if ( ( *iter )._type == ( *iter ).eTable )
        {
            QStringList columns;
            QString createQuery = ( *iter )._createStatement;
            QString removeStr( "Create table " + ( *iter )._name + " (" );
            createQuery = createQuery.simplified();
            createQuery.replace( "[", "" );
            createQuery.replace( "]", "" );
            createQuery.replace( ")", "" );
            createQuery.replace( "\"", "" );
            createQuery.replace( removeStr, "", Qt::CaseInsensitive );
            columns = createQuery.split( "," );
            QStringList::iterator columnText = columns.begin();
            while ( columnText != columns.end() )
            {
                QStringList properties = ( *columnText ).split( " ", QString::SkipEmptyParts );
                Column column;

                column._name = properties.at( 0 ).simplified();
                QString columnType = properties.at( 1 ).toLower();

                if ( columnType.contains( "varchar" ) == true )
                    columnType = "text";

                QMap<QString, Column::Type>::iterator typeMapping = gDataMap.find( columnType.toLower() );
                if ( typeMapping != gDataMap.end() )
                    column._type = typeMapping.value();
                else
                    column._type = column.eUnknown;

                if ( column._type == column.eUnknown )
                    AppendOutput( "Unknown column type in Table:" + ( *iter )._name + " Column:" + column._name + " " + columnType );
                else
                    ( *iter )._columns.push_back( column );

                columnText++;
            }
        }
        iter++;
    }
}
void QTSqlGen::LoadSqliteViews()
{
    quint32 namePosition;
    quint32 sqlPosition;

    QSqlQuery query( _db );
    if ( query.exec( "select * from sqlite_master where type = 'view'" ) )
    {
        QSqlRecord rec = query.record();

        namePosition = rec.indexOf( "name" );
        sqlPosition = rec.indexOf( "sql" );

        _views.clear();

        AppendOutput( "Views:" );
        while ( query.next() )
        {
            QString name = query.value( namePosition ).toString();
            QString sql = query.value( sqlPosition ).toString();

            if ( sql.size() )
            {
                Table aView;

                aView._type = aView.eView;
                aView._name = name;
                aView._createStatement = sql;

                _views.push_back( aView );

                AppendOutput( "   View: " +  aView._name );
            }
        }
    }
    else
    {
        QSqlError se = query.lastError();

        AppendOutput( "Exec Failed" );
        AppendOutput( se.text() );
    }
}


void QTSqlGen::LoadSqliteViewColumns()
{
    TableIter iter = _views.begin();
    while ( iter != _views.end() )
    {
        QString tableName = ( *iter )._name;
        if ( ( *iter )._type == ( *iter ).eView ) // safety check
        {
            QString createQuery = ( *iter )._createStatement;
            createQuery = createQuery.simplified();
            createQuery.replace( "[", "" ).replace( "]", "" ).replace( "\"", "" );
            QString removeStr( "Create view " + tableName + " as select" );
            createQuery.replace( removeStr, "", Qt::CaseInsensitive );
            quint32 fromInd = createQuery.indexOf ( "From", 0, Qt::CaseInsensitive );
            createQuery = createQuery.left( fromInd );  // remove 'from' clause and everything after it

            QStringList columns = createQuery.split( "," );
            QStringList::iterator columnText = columns.begin();
            while ( columnText != columns.end() )
            {
                QStringList properties = ( *columnText ).split( "as", QString::SkipEmptyParts );
                Column column;
                column._type = column.eText;  //since create view sql does not contain type information
                column._name = properties.at( 1 ).simplified();
                ( *iter )._columns.push_back( column );

                columnText++;
            }
        }
        iter++;
    }
}

void QTSqlGen::LoadODBCColumns()
{
    QString tableQuery;

    SqlProject* currentProject = GetProject();
    if ( currentProject != NULL )
    {
        switch ( currentProject->_odbcDriver )
        {
        case eSqlServer2000:  //sql server 2008
        case eSqlServer2005:  //sql server 2005
        case eSqlServer2008:     //sql server 2000
        case eAccess:
            tableQuery = "SELECT TOP 1 * FROM <%T%>";
            break;

        case eMySql:
            tableQuery = "SELECT * FROM <%T%> LIMIT 1";
            break;

        case ePostgres:
            tableQuery = "SELECT * FROM \"<%T%>\" LIMIT 1";
            break;

        case eOracle:
            tableQuery = "SELECT * from \"<%T%>\" WHERE ROWNUM <= 1";
            break;
        }

        TableIter iter = _tables.begin();
        while ( iter != _tables.end() )
        {
            QString selectStatement = tableQuery;
            selectStatement.replace( "<%T%>", ( *iter )._name );

            QSqlQuery query( _db );

            if ( query.exec( selectStatement ) )
            {
                QSqlRecord rec = query.record();

                for ( int i = 0; i < rec.count(); i++ )
                {
                    Column column;
                    QSqlField sqlField = rec.field( i );

                    column._name = sqlField.name();

                    switch ( sqlField.type() )
                    {
                    case QVariant::Int:
                        column._type = Column::eInt;
                        break;
                    case QVariant::String:
                        column._type = Column::eText;
                        break;
                    case QVariant::UInt:
                        column._type = Column::eUInt;
                        break;
                    case QVariant::DateTime:
                        column._type = Column::eDateTime;
                        break;
                    case QVariant::Date:
                        column._type = Column::eDate;
                        break;
                    case QVariant::Time:
                        column._type = Column::eTime;
                        break;
                    case QVariant::Double:
                        column._type = Column::eReal;
                        break;
                    case QVariant::Bool:
                        column._type = Column::eBoolean;
                        break;
                    case QVariant::ByteArray:
                        column._type = Column::eBytes;
                        break;
                    default:
                        column._type = Column::eUnknown;

                        QString message = "Unhandled Type:";
                        message += QString( QVariant::typeToName( sqlField.type() ) );
                        message += " In Column:" + column._name;
                        message += " In Table:" + ( *iter )._name;

                        AppendOutput( message );
                        break;
                    }

                    ( *iter )._columns.push_back( column );
                }
            }
            else
            {
                QSqlError se = query.lastError();

                AppendOutput( "Exec Failed" );
                AppendOutput( se.text() );
            }

            iter++;
        }
    }
}

void QTSqlGen::LoadODBCTables()
{
    QStringList tables;
    SqlProject* sqlProject = GetProject();

    _tables.clear();
    _indexes.clear();

    if ( sqlProject != NULL )
    {
        switch ( sqlProject->_odbcDriver )
        {
        case eOracle: // QT returns too much, use an explicit query
            tables = LoadOracleTables();
            break;

        default: // just use QTs default
            tables = _db.tables( QSql::Tables );
            break;
        }

        QStringList::const_iterator tableIter = tables.begin();
        while ( tableIter != tables.constEnd() )
        {
            Table table;

            table._type = table.eTable;
            table._name = *tableIter;

            _tables.push_back( table );

            tableIter++;
        }
    }

    AppendOutput( "Tables:" );
    TableIter iter = _tables.begin();
    while ( iter != _tables.end() )
    {
        QString type = ( ( *iter )._type == ( *iter ).eTable ) ? "   Table: " : "   Index: ";
        AppendOutput( type +  ( *iter )._name );
        iter++;
    }
}

QStringList QTSqlGen::LoadOracleTables()
{
    QStringList result;

    if ( _db.isOpen() )
    {
        QSqlQuery query( "select * from user_objects where OBJECT_TYPE = 'TABLE'", _db );
        int fieldNo = query.record().indexOf( "OBJECT_NAME" );
        while ( query.next() )
        {
            QString tableName = query.value( fieldNo ).toString();
            result.push_back( tableName );
        }
    }

    return result;
}

void QTSqlGen::LoadODBCViews()
{
    _views.clear();

    QStringList views;

    SqlProject* sqlProject = GetProject();

    if ( sqlProject != NULL )
    {
        switch ( sqlProject->_odbcDriver )
        {
        case eOracle: // QT returns too much, use an explicit query
            views = LoadOracleViews();
            break;

        default: // just use QTs default
            views = _db.tables( QSql::Views );
            break;
        }

        QStringList::const_iterator viewIter = views.begin();
        while ( viewIter != views.constEnd() )
        {
            Table aView;

            aView._type = Table::eView;
            aView._name = *viewIter;

            _views.push_back( aView );

            viewIter++;
        }
    }

    AppendOutput( "Views:" );
    TableIter iter = _views.begin();
    while ( iter != _views.end() )
    {
        AppendOutput( "   " +  ( *iter )._name );
        iter++;
    }
}

QStringList QTSqlGen::LoadOracleViews()
{
    QStringList result;

    if ( _db.isOpen() )
    {
        QSqlQuery query( "select * from user_objects where OBJECT_TYPE = 'VIEW'", _db );
        int fieldNo = query.record().indexOf( "OBJECT_NAME" );
        while ( query.next() )
        {
            QString tableName = query.value( fieldNo ).toString();
            result.push_back( tableName );
        }
    }

    return result;
}

void QTSqlGen::LoadODBCViewColumns()
{
}

void QTSqlGen::GenCode
(
        Tables& tables,
        QString fileName
)
{
    QFile templateFile;

    templateFile.setFileName( ":/templates/Resources/" + fileName + ".h" );

    if ( templateFile.open( QIODevice::ReadOnly ) )
    {
        QByteArray headerTmpl;

        headerTmpl = templateFile.readAll();

        TableIter iter = tables.begin();
        while ( iter != tables.end() )
        {
            QByteArray header( headerTmpl );
            QString tableNameDefine = ( *iter )._name.toUpper();

            StandardReplacements( header );

            header.replace( "<%table%>", ( *iter )._name.toLatin1() );
            header.replace( "<%TABLE%>", tableNameDefine.toLatin1() );

            QFile headerFile;
            QString headerFilePath = _targetPath->text() + "/" + ( *iter )._name + fileName + ".h";

            headerFile.setFileName( headerFilePath );
            if ( headerFile.open( QIODevice::WriteOnly ) )
            {
                AddHeaderFile( QString( ( *iter )._name + fileName + ".h" ) );
                headerFile.write( header );

                AppendOutput( headerFilePath + " written" );
                headerFile.close();
            }

            templateFile.close();

            GenRecordHeader( *iter );
            GenSelectionHeader( *iter );

            iter++;
        }

        templateFile.close();
    }
    else
    {
        QFile::FileError fileError = templateFile.error();

        AppendOutput( "Error opening: " + templateFile.fileName() + ":" + QString::number( fileError ) );
    }

    templateFile.setFileName( ":/templates/Resources/" + fileName + ".cpp" );

    if ( templateFile.open( QIODevice::ReadOnly ) )
    {
        QByteArray sourceTmpl;

        sourceTmpl = templateFile.readAll();

        TableIter iter = tables.begin();
        while ( iter != tables.end() )
        {
            QByteArray sourceText( sourceTmpl );
            QString tableNameDefine = ( *iter )._name.toUpper();

            StandardReplacements( sourceText );

            sourceText.replace( "<%table%>", ( *iter )._name.toLatin1() );
            sourceText.replace( "<%TABLE%>", tableNameDefine.toLatin1() );

            QString createStatement( ( *iter )._createStatement );
            createStatement.replace( "\"", "" );
            createStatement.replace( "\n", " " );
            createStatement.replace( "\r", " " );
            createStatement.replace( "  ", " " );
            sourceText.replace( "<%createStmt%>", createStatement.toLatin1() );

            QFile cppFile;
            QString cppFilePath = _targetPath->text() + "/" + ( *iter )._name + fileName + ".cpp";

            cppFile.setFileName( cppFilePath );
            if ( cppFile.open( QIODevice::WriteOnly ) )
            {
                AddSourceFile( ( *iter )._name + fileName + ".cpp" );
                cppFile.write( sourceText );

                AppendOutput( cppFilePath + " written" );
                cppFile.close();
            }

            GenRecordSource( *iter );
            GenSelectionSource( *iter );

            iter++;
        }

        templateFile.close();
    }
    else
    {
        QFile::FileError fileError = templateFile.error();

        AppendOutput( "Error opening: " + templateFile.fileName() + ":" + QString::number( fileError ) );
    }
}

QString QTSqlGen::GenerateFieldType
(
        const QString& columnName,
        const QString& name,
        Column::Type type
)
{
    QString result;
    QString uName = name.mid( 0, 1 ).toUpper() + name.mid( 1 );

    QFile templateFile;

    templateFile.setFileName( ":/templates/Resources/fieldType" );
    if ( templateFile.open( QIODevice::ReadOnly ) )
    {
        QByteArray fieldTypeTemplate;
        QByteArray fieldType;

        switch ( type )
        {
        case Column::eText:
            fieldType = "eString";
            break;

        case Column::eBytes:
            fieldType = "eBytes";
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

        fieldTypeTemplate.replace( "<%fieldType%>", fieldType );
        fieldTypeTemplate.replace( "<%uName%>", uName.toLatin1() );
        fieldTypeTemplate.replace( "<%name%>", columnName.toLatin1() );

        StandardReplacements( fieldTypeTemplate );

        result = QString( fieldTypeTemplate );
        result += "\n";
    }

    return result;
}

QString QTSqlGen::GenerateSelector
(
        const QString& name,
        const Column::Type type
)
{
    QString lName( name );
    lName.replace( "_", "" );
    lName[0] = QChar( lName[0] ).toLower();

    QString uName( name );
    uName.replace( "_", "" );
    uName[0] = QChar( uName[0] ).toUpper();

    QByteArray accessor;

    QFile templateFile;

    switch ( type )
    {
    case Column::eText:
        templateFile.setFileName( ":/templates/Resources/SelectorString" );
        break;

    case Column::eBytes:
        templateFile.setFileName( ":/templates/Resources/SelectorBytes" );
        break;

    case Column::eDateTime:
        templateFile.setFileName( ":/templates/Resources/SelectorDateTime" );
        break;

    case Column::eDate:
        templateFile.setFileName( ":/templates/Resources/SelectorDate" );
        break;

    case Column::eTime:
        templateFile.setFileName( ":/templates/Resources/SelectorTime" );
        break;

    case Column::eReal:
        templateFile.setFileName( ":/templates/Resources/SelectorReal" );
        break;

    case Column::eInt:
        templateFile.setFileName( ":/templates/Resources/SelectorInt" );
        break;

    case Column::eGuid:
        templateFile.setFileName( ":/templates/Resources/SelectorGUID" );
        break;

    case Column::eBoolean:
        templateFile.setFileName( ":/templates/Resources/SelectorBoolean" );
        break;
    }

    if ( templateFile.open( QIODevice::ReadOnly ) )
    {
        accessor = templateFile.readAll();

        accessor.replace( QByteArray( "<%uName%>" ), uName.toLatin1() );
        accessor.replace( QByteArray( "<%lName%>" ), lName.toLatin1() );
        accessor.replace( QByteArray( "<%name%>" ), name.toLatin1() );

        StandardReplacements( accessor );
    }

    return QString( accessor );
}

void QTSqlGen::GenRecordHeader
(
        Table& table
)
{
    QFile templateFile;

    templateFile.setFileName( ":/templates/Resources/Record.h" );
    if ( templateFile.open( QIODevice::ReadOnly ) )
    {
        QByteArray header;
        QString fieldEnums;
        QString accessors;

        QString TABLENAME = table._name.toUpper();
        QString tableName = table._name;

        ColumnIter column = table._columns.begin();
        while ( column != table._columns.end() )
        {
            QString type;
            QString name = ( *column )._name;

            name.replace( "_", "" );

            QString lName = name.mid( 0, 1 ).toLower() + name.mid( 1 );
            QString uName = name.mid( 0, 1 ).toUpper() + name.mid( 1 );

            if ( column != table._columns.begin() )
            {
                fieldEnums.append( ",\n" );
                accessors.append( "\n" );
            }

            fieldEnums.append( "\t\tf_" + uName );

            switch ( ( *column )._type )
            {
            case Column::eText:
                accessors += GenerateAccessor( name, QString( "const QString&" ) );
                break;

            case Column::eBytes:
                accessors += GenerateAccessor( name, QString( "const QByteArray&" ) );
                break;

            case Column::eDateTime:
                accessors += GenerateAccessor( name, QString( "const QDateTime&" ) );
                break;

            case Column::eDate:
                accessors += GenerateAccessor( name, QString( "const QDate&" ) );
                break;

            case Column::eTime:
                accessors += GenerateAccessor( name, QString( "const QTime&" ) );
                break;

            case Column::eInt:
                accessors += GenerateAccessor( name, QString( "quint32" ) );
                break;

            case Column::eGuid:
                accessors += GenerateAccessor( name, QString( "const QUuid&" ) );
                break;

            case Column::eReal:
                accessors += GenerateAccessor( name, QString( "qreal" ) );
                break;

            case Column::eBoolean:
                accessors += GenerateAccessor( name, QString( "bool" ) );
                break;

            default:
                AppendOutput( "Unknown Type Error Table:" + table._name + " Column:" + ( *column )._name );
                break;
            }

            column++;
        }

        header = templateFile.readAll();

        StandardReplacements( header );

        header.replace( "<%table%>", tableName.toLatin1() );
        header.replace( "<%TABLE%>", TABLENAME.toLatin1() );
        header.replace( "<%fieldEnum%>", fieldEnums.toLatin1() );
        header.replace( "<%accessors%>", accessors.toLatin1() );

        QString headerFilePath = _targetPath->text() + "/" + table._name + "Record.h";
        QFile headerFile;

        headerFile.setFileName( headerFilePath );
        if ( headerFile.open( QIODevice::WriteOnly ) )
        {
            AddHeaderFile( QString( table._name + "Record.h" ) );

            headerFile.write( header );

            AppendOutput( headerFilePath + " written" );
            headerFile.close();
        }
        else
        {
            AppendOutput( "Error opening: " + headerFilePath );
        }

        templateFile.close();
    }
    else
    {
        QFile::FileError fileError = templateFile.error();

        AppendOutput( "Error opening: " + templateFile.fileName() + ":" + QString::number( fileError ) );
    }
}

void QTSqlGen::GenRecordSource
(
        Table& table
)
{
    QFile templateFile;

    templateFile.setFileName( ":/templates/Resources/Record.cpp" );
    if ( templateFile.open( QIODevice::ReadOnly ) )
    {
        QByteArray cpp;
        QString fields;
        QString binds;
        QString accessors;
        QString propertyDataTypes;

        QString tableName = table._name;

        ColumnIter column = table._columns.begin();
        while ( column != table._columns.end() )
        {
            QString type;
            QString name = ( *column )._name;

            name.replace( "_", "" );

            QString lName = name.mid( 0, 1 ).toLower() + name.mid( 1 );
            QString uName = name.mid( 0, 1 ).toUpper() + name.mid( 1 );

            if ( column != table._columns.begin() )
            {
                if ( fields.size() )
                {
                    fields.append( ", " );
                    binds.append( ", " );
                }
            }

            if ( ( *column )._name != "_id" && ( *column )._name != "ID" )
            {
                fields.append( ( *column )._name );
                binds.append( ":" + ( *column )._name );
            }

            propertyDataTypes.append( GenerateFieldType( ( *column )._name, lName, ( *column )._type ) );
            accessors.append( GenerateAccessorSource( tableName, lName, ( *column )._type ) );

            column++;
        }

        cpp = templateFile.readAll();

        StandardReplacements( cpp );

        cpp.replace( "<%table%>", tableName.toLatin1() );
        cpp.replace( "<%fields%>", fields.toLatin1() );
        cpp.replace( "<%binds%>", binds.toLatin1() );
        cpp.replace( "<%accessors%>", accessors.toLatin1() );
        cpp.replace( "<%fieldCount%>", QByteArray::number( table._columns.size() ) );

        cpp.replace( "<%propertyDataTypes%>", propertyDataTypes.toLatin1() );

        QString cppFilePath = _targetPath->text() + "/" + table._name + "Record.cpp";
        QFile cppFile;

        cppFile.setFileName( cppFilePath );
        if ( cppFile.open( QIODevice::WriteOnly ) )
        {
            AddSourceFile( table._name + "Record.cpp" );
            cppFile.write( cpp );

            AppendOutput( cppFilePath + " written" );
            cppFile.close();
        }
        else
        {
            AppendOutput( "Error opening: " + cppFilePath );
        }

        templateFile.close();
    }
    else
    {
        QFile::FileError fileError = templateFile.error();

        AppendOutput( "Error opening: " + templateFile.fileName() + ":" + QString::number( fileError ) );
    }
}

// todo
void QTSqlGen::GenSelectionHeader
(
        Table& table
)
{
    QFile templateFile;

    templateFile.setFileName( ":/templates/Resources/SelectionCriteria.h" );
    if ( templateFile.open( QIODevice::ReadOnly ) )
    {
        QByteArray header;
        QString accessors;

        QString TABLENAME = table._name.toUpper();
        QString tableName = table._name;

        ColumnIter column = table._columns.begin();
        while ( column != table._columns.end() )
        {
            accessors.append( GenerateSelector( ( *column )._name, ( *column )._type ) );

            column++;
        }

        header = templateFile.readAll();

        StandardReplacements( header );

        header.replace( "<%table%>", tableName.toLatin1() );
        header.replace( "<%TABLE%>", TABLENAME.toLatin1() );
        header.replace( "<%accessors%>", accessors.toLatin1() );

        QString headerFilePath = _targetPath->text() + "/" + table._name + "SelectionCriteria.h";
        QFile headerFile;

        headerFile.setFileName( headerFilePath );
        if ( headerFile.open( QIODevice::WriteOnly ) )
        {
            AddHeaderFile( QString( table._name + "SelectionCriteria.h" ) );

            headerFile.write( header );

            AppendOutput( headerFilePath + " written" );
            headerFile.close();
        }
        else
        {
            AppendOutput( "Error opening: " + headerFilePath );
        }

        templateFile.close();
    }
    else
    {
        QFile::FileError fileError = templateFile.error();

        AppendOutput( "Error opening: " + templateFile.fileName() + ":" + QString::number( fileError ) );
    }
}

void QTSqlGen::GenSelectionSource
(
        Table& table
)
{
    QFile templateFile;

    templateFile.setFileName( ":/templates/Resources/SelectionCriteria.cpp" );
    if ( templateFile.open( QIODevice::ReadOnly ) )
    {
        QByteArray cpp;
        QString tableName = table._name;

        cpp = templateFile.readAll();

        StandardReplacements( cpp );

        cpp.replace( "<%table%>", tableName.toLatin1() );

        QString cppFilePath = _targetPath->text() + "/" + table._name + "SelectionCriteria.cpp";
        QFile cppFile;

        cppFile.setFileName( cppFilePath );
        if ( cppFile.open( QIODevice::WriteOnly ) )
        {
            AddSourceFile( table._name + "SelectionCriteria.cpp" );
            cppFile.write( cpp );

            AppendOutput( cppFilePath + " written" );
            cppFile.close();
        }
        else
        {
            AppendOutput( "Error opening: " + cppFilePath );
        }

        templateFile.close();
    }
    else
    {
        QFile::FileError fileError = templateFile.error();

        AppendOutput( "Error opening: " + templateFile.fileName() + ":" + QString::number( fileError ) );
    }
}

QString QTSqlGen::GenerateAccessor
(
        const QString& name,
        const QString& type
)
{
    QString lName( name );
    lName.replace( "_", "" );
    lName[0] = QChar( lName[0] ).toLower();

    QString uName( name );
    uName.replace( "_", "" );
    uName[0] = QChar( uName[0] ).toUpper();

    QString inType( type );
    QString outType( type );

    QByteArray accessor;

    QFile templateFile;

    templateFile.setFileName( ":/templates/Resources/Accessor" );
    if ( templateFile.open( QIODevice::ReadOnly ) )
    {
        accessor = templateFile.readAll();

        outType.replace( QString( "&" ), QString( "" ) );
        outType.replace( QString( "const " ), QString( "" ) );

        accessor.replace( QByteArray( "<%uName%>" ), uName.toLatin1() );
        accessor.replace( QByteArray( "<%inType%>" ), inType.toLatin1() );
        accessor.replace( QByteArray( "<%name%>" ), lName.toLatin1() );
        accessor.replace( QByteArray( "<%outType%>" ), outType.toLatin1() );

        StandardReplacements( accessor );
    }

    return QString( accessor );
}

QString QTSqlGen::GenerateAccessorSource
(
        const QString& tableName,
        const QString& name,
        const Column::Type type
)
{
    QString lName( name );
    lName.replace( "_", "" );
    lName[0] = QChar( lName[0] ).toLower();

    QString uName( name );
    uName.replace( "_", "" );
    uName[0] = QChar( uName[0] ).toUpper();

    QByteArray accessor;

    QFile templateFile;


    switch ( type )
    {
    case Column::eText:
        templateFile.setFileName( ":/templates/Resources/AccessorSourceString" );
        break;

    case Column::eBytes:
        templateFile.setFileName( ":/templates/Resources/AccessorSourceBytes" );
        break;

    case Column::eDateTime:
        templateFile.setFileName( ":/templates/Resources/AccessorSourceDateTime" );
        break;

    case Column::eDate:
        templateFile.setFileName( ":/templates/Resources/AccessorSourceDate" );
        break;

    case Column::eTime:
        templateFile.setFileName( ":/templates/Resources/AccessorSourceTime" );
        break;

    case Column::eReal:
        templateFile.setFileName( ":/templates/Resources/AccessorSourceReal" );
        break;

    case Column::eInt:
        templateFile.setFileName( ":/templates/Resources/AccessorSourceInt" );
        break;

    case Column::eGuid:
        templateFile.setFileName( ":/templates/Resources/AccessorSourceGUID" );
        break;

    case Column::eBoolean:
        templateFile.setFileName( ":/templates/Resources/AccessorSourceBoolean" );
        break;
    }

    if ( templateFile.open( QIODevice::ReadOnly ) )
    {
        accessor = templateFile.readAll();

        accessor.replace( QByteArray( "<%uName%>" ), uName.toLatin1() );
        accessor.replace( QByteArray( "<%tableName%>" ), tableName.toLatin1() );
        accessor.replace( QByteArray( "<%name%>" ), lName.toLatin1() );

        StandardReplacements( accessor );
    }

    return QString( accessor );
}

QString QTSqlGen::GenerateTableRoutine
(
        const Table& table
)
{
    QByteArray tableRoutine;
    QString type( table._type == Table::eTable ? "Table" : "View" );
    QString tableName( table._name + type );
    QString variableName( table._name );

    variableName[0] = QChar( variableName[0] ).toLower();
    variableName += type;

    QFile templateFile;

    templateFile.setFileName( ":/templates/Resources/TableRoutine" );
    if ( templateFile.open( QIODevice::ReadOnly ) )
    {
        tableRoutine = templateFile.readAll();

        StandardReplacements( tableRoutine );

        tableRoutine.replace( "<%tableName%>", tableName.toLatin1() );
        tableRoutine.replace( "<%variableName%>", variableName.toLatin1() );

    }

    return tableRoutine;
}

void QTSqlGen::WriteStaticFiles()
{
    SqlProject* currentProject = GetProject();

    if ( currentProject != NULL )
    {

        QStringList staticFiles;
        QStringList::iterator staticFile;

        staticFiles << ":/templates/Resources/RecordBase.h"
                << ":/templates/Resources/RecordBase.cpp";

        if ( currentProject->_writeProject == true )
            staticFiles << ":/templates/Resources/CreateVCProj.bat";

        staticFile = staticFiles.begin();
        while ( staticFile != staticFiles.end() )
        {
            QFile templateFile;

            templateFile.setFileName( *staticFile );
            if ( templateFile.open( QIODevice::ReadOnly ) )
            {
                QByteArray templateStr = templateFile.readAll();

                QStringList parts = ( *staticFile ).split( "/", QString::SkipEmptyParts );

                QString srcFilePath = _targetPath->text() + "/" + parts.last();
                QFile srcFile;

                srcFile.setFileName( srcFilePath );
                if ( srcFile.open( QIODevice::WriteOnly ) )
                {
                    if ( parts.last().indexOf( ".h" ) > -1 )
                        AddHeaderFile( parts.last() );
                    else if ( parts.last().indexOf( ".cpp" ) > -1 )
                        AddSourceFile( parts.last() );

                    StandardReplacements( templateStr );

                    srcFile.write( templateStr );

                    AppendOutput( srcFilePath + " written" );
                    srcFile.close();
                }
                else
                {
                    AppendOutput( "Error opening: " + srcFilePath );
                }

                templateFile.close();
            }
            else
            {
                QFile::FileError fileError = templateFile.error();

                AppendOutput( "Error opening: " + templateFile.fileName() + ":" + QString::number( fileError ) );
            }
            staticFile++;
        }
    }
}

void QTSqlGen::WriteHeaderFile()
{
    QFile templateFile;
    QFile srcFile;

    templateFile.setFileName( ":/templates/Resources/Header" );

    if ( templateFile.open( QIODevice::ReadOnly ) )
    {
        QByteArray templateStr = templateFile.readAll();

        QString srcFilePath = _targetPath->text() + "/" + _productName + "DB.h";

        srcFile.setFileName( srcFilePath );
        if ( srcFile.open( QIODevice::WriteOnly ) )
        {
            StandardReplacements( templateStr );

            srcFile.write( templateStr );

            AppendOutput( srcFilePath + " written" );
            srcFile.close();
        }
        else
        {
            AppendOutput( "Error opening: " + srcFilePath );
        }
    }
    else
    {
        QFile::FileError fileError = templateFile.error();

        AppendOutput( "Error opening: " + templateFile.fileName() + ":" + QString::number( fileError ) );
    }
}

void QTSqlGen::WriteExportHeaderFile()
{
    SqlProject* currentProject = GetProject();

    if ( currentProject != NULL )
    {
        QFile templateFile;
        QFile srcFile;

        templateFile.setFileName( ":/templates/Resources/Header2" );

        if ( templateFile.open( QIODevice::ReadOnly ) )
        {
            QByteArray templateStr = templateFile.readAll();

            QString srcFilePath = _targetPath->text() + "/" + _productInclude;

            srcFile.setFileName( srcFilePath );
            if ( srcFile.open( QIODevice::WriteOnly ) )
            {
                StandardReplacements( templateStr );

                srcFile.write( templateStr );

                AppendOutput( srcFilePath + " written" );
                srcFile.close();

                AddHeaderFile( _productInclude );
            }
            else
            {
                AppendOutput( "Error opening: " + srcFilePath );
            }
        }
        else
        {
            QFile::FileError fileError = templateFile.error();

            AppendOutput( "Error opening: " + templateFile.fileName() + ":" + QString::number( fileError ) );
        }
    }
}

void QTSqlGen::WriteProject()
{
    QFile templateFile;
    QFile srcFile;

    SqlProject* currentProject = GetProject();

    if ( currentProject != NULL )
    {
        templateFile.setFileName( ":/templates/Resources/pro" );
        if ( templateFile.open( QIODevice::ReadOnly ) )
        {
            QByteArray templateStr = templateFile.readAll();

            QString srcFilePath = _targetPath->text() + "/" + _productName + "DB.pro";

            srcFile.setFileName( srcFilePath );
            if ( srcFile.open( QIODevice::WriteOnly ) )
            {
                StandardReplacements( templateStr );

                templateStr.replace( QByteArray( "<%linkage%>" ),
                        currentProject->_dynamicLibrary ? "dll" : "staticlib" );

                templateStr.replace( QByteArray( "<%linkagedef%>" ),
                        currentProject->_dynamicLibrary ? QByteArray( "BASE_DLL " ) + _dllExportDefine.toLatin1() : "BASE_STATIC" );

                srcFile.write( templateStr );

                AppendOutput( srcFilePath + " written" );
                srcFile.close();
            }
            else
            {
                AppendOutput( "Error opening: " + srcFilePath );
            }
        }
        else
        {
            QFile::FileError fileError = templateFile.error();

            AppendOutput( "Error opening: " + templateFile.fileName() + ":" + QString::number( fileError ) );
        }
    }
}

void QTSqlGen::AddHeaderFile
(
        const QString& headerFileName
)
{
    if ( _headerFiles.size() )
        _headerFiles.append( "\r\n" );
    _headerFiles.append( "#include \"" + headerFileName + "\"" );

    if ( _headers.size() )
        _headers.append( " \\\r\n" );
    _headers.append( "\t" + headerFileName );

}

void QTSqlGen::AddSourceFile( const QString& sourceFileName )
{
    if ( _sources.size() )
        _sources.append( " \\\r\n" );

    _sources.append( "\t" + sourceFileName );
}

void QTSqlGen::AppendOutput( const QString& output )
{
    _output->append( output );
}

void QTSqlGen::StandardReplacements( QByteArray& replaceMe )
{
    QString PRODUCTNAME = _productName.toUpper();

    replaceMe.replace( "<%PRODUCTNAME%>", PRODUCTNAME.toLatin1() );
    replaceMe.replace( "<%productInclude%>", _productInclude.toLatin1() );
    replaceMe.replace( "<%productName%>", _productName.toLatin1() );
    replaceMe.replace( "<%projectGUID%>", _projectGUID.toLatin1() );
    replaceMe.replace( "<%dllExport%>", _dllExport.toLatin1() );
    replaceMe.replace( "<%dllExportDefine%>", _dllExportDefine.toLatin1() );
    replaceMe.replace( "<%includeFiles%>", _headerFiles.toLatin1() );
    replaceMe.replace( "<%sources%>", _sources.toLatin1() );
    replaceMe.replace( "<%headers%>", _headers.toLatin1() );
    replaceMe.replace( "<%headerFiles%>", _headerFiles.toLatin1() );
    replaceMe.replace( "<%date%>", _now.toString().toLatin1() );
    replaceMe.replace( "<%applicationName%>", QCoreApplication::applicationName().toLatin1() );

    QString productNamespace = _namespaceEdit->text();

    if ( !productNamespace.isEmpty() )
    {
        QString namespaceStart = "\nnamespace ";
        namespaceStart += productNamespace;
        namespaceStart += "\n{\n";

        replaceMe.replace( "<%namespaceStart%>", namespaceStart.toLatin1() );
        replaceMe.replace( "<%namespaceEnd%>", "\n}\n" );
    }
    else
    {
        // Replace anyways, as productNamespace is an empty string
        replaceMe.replace( "<%namespaceStart%>", productNamespace.toLatin1() );
        replaceMe.replace( "<%namespaceEnd%>", productNamespace.toLatin1() );
    }
}


void QTSqlGen::on__aboutButton_clicked()
{
    AboutDlg aboutDlg( this );

    aboutDlg.exec();
}

void QTSqlGen::on__dynamicRadio_toggled
(
        bool state
)
{
    SqlProject* currentProject = GetProject();

    if ( currentProject != NULL )
    {
        currentProject->_dynamicLibrary = state;
    }
}

void QTSqlGen::on__staticRadio_toggled
(
        bool state
)
{
    SqlProject* currentProject = GetProject();

    if ( currentProject != NULL )
    {
        currentProject->_dynamicLibrary = state == false;
    }
}

void QTSqlGen::WriteDatabaseFiles()
{
    QStringList staticFiles;
    QStringList::iterator staticFile;

    QFile templateFile;
    SqlProject* currentProject = GetProject();

    if ( currentProject != NULL )
    {
        switch ( currentProject->_sourceType )
        {
        case eODBC:
            templateFile.setFileName( ":/templates/Resources/ODBCDatabase.h" );
            break;

        case eSqlite:
            templateFile.setFileName( ":/templates/Resources/SqliteDatabase.h" );
            break;

        default:
            return;
        }

        if ( templateFile.open( QIODevice::ReadOnly ) )
        {
            QByteArray templateStr = templateFile.readAll();

            QString srcFilePath = _targetPath->text() + "/" + _productName + "Database.h";
            QFile srcFile;

            srcFile.setFileName( srcFilePath );
            if ( srcFile.open( QIODevice::WriteOnly ) )
            {
                AddHeaderFile( _productName + "Database.h" );

                QString includes;
                QString tables;
                QString views;
                QString tablePtrs;

                TableIter table = _tables.begin();
                while ( table != _tables.end() )
                {
                    QString variableName( ( *table )._name );

                    variableName[0] = QChar( variableName[0] ).toLower();

                    includes += "#include \"" + ( *table )._name + "Table.h\"\n";

                    tables += "\t" + ( *table )._name + "Table* Get" + ( *table )._name + "Table(void);\n";

                    if ( tablePtrs.size() )
                        tablePtrs += "\n";

                    tablePtrs += "\t" + ( *table )._name + "Table* \t_" + variableName + "Table;";

                    table++;
                }

                TableIter view = _views.begin();
                while ( view != _views.end() )
                {
                    QString variableName( ( *view )._name );

                    variableName[0] = QChar( variableName[0] ).toLower();

                    includes += "#include \"" + ( *view )._name + "View.h\"\n";
                    views += "\t" + ( *view )._name + "View* Get" + ( *view )._name + "View(void);\n";

                    if ( tablePtrs.size() )
                        tablePtrs += "\n";
                    tablePtrs += "\t_" + ( *view )._name + "View* \t_" + variableName + "View;";

                    view++;
                }

                templateStr.replace( "<%tables%>", tables.toLatin1() );
                templateStr.replace( "<%views%>", views.toLatin1() );
                templateStr.replace( "<%tablePtrs%>", tablePtrs.toLatin1() );
                templateStr.replace( "<%includes%>", includes.toLatin1() );
                StandardReplacements( templateStr );

                srcFile.write( templateStr );

                AppendOutput( srcFilePath + " written" );
                srcFile.close();
            }
            else
            {
                AppendOutput( "Error opening: " + srcFilePath );
            }

            templateFile.close();
        }
        else
        {
            QFile::FileError fileError = templateFile.error();

            AppendOutput( "Error opening: " + templateFile.fileName() + ":" + QString::number( fileError ) );
        }

        switch ( currentProject->_sourceType )
        {
        case eODBC:
            templateFile.setFileName( ":/templates/Resources/ODBCDatabase.cpp" );
            break;

        case eSqlite:
            templateFile.setFileName( ":/templates/Resources/SqliteDatabase.cpp" );
            break;

        default:
            return;
        }

        if ( templateFile.open( QIODevice::ReadOnly ) )
        {
            QByteArray templateStr = templateFile.readAll();

            QString srcFilePath = _targetPath->text() + "/" + _productName + "Database.cpp";
            QFile srcFile;

            srcFile.setFileName( srcFilePath );
            if ( srcFile.open( QIODevice::WriteOnly ) )
            {
                AddSourceFile( _productName + "Database.cpp" );

                QString statics;
                QString destructors;
                QString tableRoutines;

                TableIter table = _tables.begin();
                while ( table != _tables.end() )
                {
                    QString variableName( ( *table )._name );

                    variableName[0] = QChar( variableName[0] ).toLower();

                    if ( statics.size() )
                        statics += ",\n";
                    statics += "\t_" + variableName + "Table(NULL)";

                    tableRoutines += GenerateTableRoutine( *table );

                    destructors += "\tif (_" + variableName + "Table != NULL)\n\t{\n";
                    destructors += "\t\tdelete _" + variableName + "Table;\n";
                    destructors += "\t\t_" + variableName + "Table = NULL;\n";
                    destructors += "\t}\n";
                    table++;
                }

                TableIter view = _views.begin();
                while ( view != _views.end() )
                {
                    QString variableName( ( *view )._name );

                    variableName[0] = QChar( variableName[0] ).toLower();

                    if ( statics.size() )
                        statics += ",\n";

                    statics += "\t_" + variableName + "View(NULL)";

                    tableRoutines += GenerateTableRoutine( *view );

                    destructors += "\tif (_" + variableName + "View != NULL)\n\t{\n";
                    destructors += "\t\tdelete _" + variableName + "View;\n";
                    destructors += "\t\t_" + variableName + "View = NULL;\n";
                    destructors += "\t}\n";

                    view++;
                }

                StandardReplacements( templateStr );

                templateStr.replace( "<%destructors%>", destructors.toLatin1() );
                templateStr.replace( "<%initializers%>", statics.toLatin1() );
                templateStr.replace( "<%tableRoutines%>", tableRoutines.toLatin1() );

                srcFile.write( templateStr );

                AppendOutput( srcFilePath + tr( " written" ) );
                srcFile.close();
            }
            else
            {
                AppendOutput( tr( "Error opening: " ) + srcFilePath );
            }

            templateFile.close();
        }
        else
        {
            QFile::FileError fileError = templateFile.error();

            AppendOutput( tr( "Error opening: " ) + templateFile.fileName() + ":" + QString::number( fileError ) );
        }

        if ( currentProject->_sourceType == eODBC )
        {
            templateFile.setFileName( ":/templates/Resources/ODBCDrivers.h" );

            if ( templateFile.open( QIODevice::ReadOnly ) )
            {
                QByteArray templateStr = templateFile.readAll();

                QString srcFilePath = _targetPath->text() + "/" + "ODBCDrivers.h";
                QFile odbcHeaderFile;

                odbcHeaderFile.setFileName( srcFilePath );
                if ( odbcHeaderFile.open( QIODevice::WriteOnly ) )
                {
                    AddHeaderFile( "ODBCDrivers.h" );

                    odbcHeaderFile.write( templateStr );

                    AppendOutput( tr( "ODBCDrivers.h written" ) );
                    odbcHeaderFile.close();
                }
                else
                {
                    AppendOutput( tr( "Error opening: " ) + srcFilePath );
                }

                templateFile.close();
            }
            else
            {
                QFile::FileError fileError = templateFile.error();

                AppendOutput( tr( "Error opening: " ) + templateFile.fileName() + ":" + QString::number( fileError ) );
            }
        }
    }
}

void QTSqlGen::PopulateDrivers()
{
    int i = 0;

    _schemaSource->clear();

    while ( DriverNames[i] != NULL )
    {
        _schemaSource->addItem( QString( DriverNames[i] ) );
        i++;
    }
}

const QString kProjects( "Projects" );
const QString kProjectName( "ProjectName" );
const QString kDatabasePath( "DatabasePath" );
const QString kTargetPath( "TargetPath" );
const QString kWriteProject( "WriteProject" );
const QString kDynamic( "Dynamic" );
const QString kSourceType( "SourceType" );
const QString kODBCDriver( "ODBCDriver" );
const QString kConnectionString( "ConnectionString" );

void QTSqlGen::LoadProjects()
{
    QSettings settings;

    _projectNames->blockSignals( true );
    _projectNames->clear();

    settings.beginGroup( kProjects );

    QStringList projects = settings.childGroups();
    if ( projects.size() > 0 )
    {
        QList<QString>::iterator project = projects.begin();
        while ( project != projects.end() )
        {
            settings.beginGroup( *project );

            SqlProject* sqlProject = new SqlProject;

            sqlProject->_projectName = settings.value( kProjectName ).toString();
            sqlProject->_targetPath = settings.value( kTargetPath ).toString();
            sqlProject->_sourceType = ( DatabaseSourceType ) settings.value( kSourceType ).toInt();
            sqlProject->_connectString = settings.value( kConnectionString ).toString();
            sqlProject->_dynamicLibrary = settings.value( kDynamic ).toBool();
            sqlProject->_writeProject = settings.value( kWriteProject ).toBool();
            sqlProject->_databasePath = settings.value( kDatabasePath ).toString();

            int driverIndex = settings.value( kODBCDriver, QVariant( 0 ) ).toInt();
            sqlProject->_odbcDriver = ( ODBCDrivers ) driverIndex;

            AddProject( sqlProject, _projectNames->count() == 0 );

            settings.endGroup();

            project++;
        }
    }

    settings.endGroup();

    if ( _projectNames->count() == 0 )
        on__newProject_clicked();

    _projectNames->blockSignals( false );
}

void QTSqlGen::SaveProjects()
{
    QSettings settings;
    QString groupKey;

    settings.beginGroup( kProjects );

    settings.remove( "" );

    for ( int index = 0; index < _projectNames->count(); index++ )
    {
        SqlProject* project = GetProject( index );
        if ( project != NULL )
        {
            groupKey = QString( "Project%1" ).arg( index );

            QString projectName = _projectNames->itemText( index );

            settings.beginGroup( groupKey );

            settings.setValue( kProjectName, projectName );
            settings.setValue( kTargetPath, project->_targetPath );
            settings.setValue( kSourceType, QVariant( ( int )( project->_sourceType ) ) );
            settings.setValue( kDynamic, project->_dynamicLibrary );
            settings.setValue( kWriteProject, project->_writeProject );
            settings.setValue( kDatabasePath, project->_databasePath );
            settings.setValue( kODBCDriver, QVariant( ( int ) ( project->_odbcDriver ) ) );
            settings.setValue( kConnectionString, QVariant( project->_connectString ) );

            settings.endGroup();
        }
    }

    settings.endGroup();
}

void QTSqlGen::SetCurrentProject( int index )
{
    if ( index != -1 )
    {
        SqlProject* currentProject = GetProject();

        switch ( currentProject->_sourceType )
        {
        case eODBC:
            _databaseType->setCurrentIndex( 0 );
            _databaseStack->setCurrentIndex( 0 );
            break;

        case eSqlite:
            _databaseType->setCurrentIndex( 1 );
            _databaseStack->setCurrentIndex( 1 );
            break;
        }

        _targetPath->setText( currentProject->_targetPath );
        _dbPath->setText( currentProject->_databasePath );
        _connectionString->setText( currentProject->_connectString );
        _replaceProject->setChecked( currentProject->_writeProject );
        _dynamicRadio->setChecked( currentProject->_dynamicLibrary );
        _staticRadio->setChecked( currentProject->_dynamicLibrary == false );
        _schemaSource->setCurrentIndex( ( int ) ( currentProject->_odbcDriver ) );
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

#include "ProjectNameDialog.h"

void QTSqlGen::on__newProject_clicked()
{
    ProjectNameDialog pnd( this );

    if ( pnd.exec() == QDialog::Accepted && pnd._projectName->text().size() > 0 )
    {
        SqlProject* sqlProject = new SqlProject;

        sqlProject->_projectName = pnd._projectName->text();

        AddProject( sqlProject, true );
    }
}

void QTSqlGen::on__deleteProject_clicked()
{
    if ( QMessageBox::warning( this, tr( "Delete the Project?" ), tr( "Are you sure?" ), QMessageBox::Yes | QMessageBox::No, QMessageBox::No ) == QMessageBox::Yes )
    {
        int index = _projectNames->currentIndex();
        SqlProject* sqlProject = GetProject( index );

        _projectNames->removeItem( index );

        if( sqlProject != NULL )
            delete sqlProject;

        SetCurrentProject( _projectNames->currentIndex() );
    }
}

void QTSqlGen::on__projectNames_currentIndexChanged( int index )
{
    if ( index != -1 )
        SetCurrentProject( index );
}

void QTSqlGen::on__connectionString_textChanged( const QString& newText )
{
    SqlProject* currentProject = GetProject();

    if ( currentProject != NULL )
    {
        currentProject->_connectString = newText;
    }
}

void QTSqlGen::on__schemaSource_currentIndexChanged( int index )
{
    SqlProject* currentProject = GetProject();

    if( currentProject != NULL )
    {
        currentProject->_odbcDriver = ( ODBCDrivers )index;
    }
}

void QTSqlGen::on__databaseType_currentIndexChanged( int index )
{
    SqlProject* currentProject = GetProject();

    if ( currentProject != NULL )
    {
        int source = _databaseType->currentIndex();
        switch ( source )
        {
        case 0:
            currentProject->_sourceType = eODBC;
            _databaseStack->setCurrentIndex( 0 );
            break;

        case 1:
            currentProject->_sourceType = eSqlite;
            _databaseStack->setCurrentIndex( 1 );
            break;
        }
    }
}

void QTSqlGen::on__replaceProject_stateChanged( int state )
{
    bool newState = false;

    switch ( state )
    {
    case Qt::Unchecked:
    case Qt::PartiallyChecked:
        newState = false;
        break;

    case Qt::Checked:
        newState = true;
        break;
    }

    SqlProject* currentProject = GetProject();

    if ( currentProject != NULL )
        currentProject->_writeProject = newState;
}

void QTSqlGen::AddProject( SqlProject* sqlProject, bool setAsCurrentProject )
{
    if ( sqlProject != NULL )
    {
        QVariant variant;

#if QT_POINTER_SIZE == 4
        variant.setValue( ( uint )sqlProject );
#endif

#if QT_POINTER_SIZE == 8
        variant.setValue( ( qlonglong )sqlProject );
#endif

        _projectNames->addItem( sqlProject->_projectName, variant );

        if ( setAsCurrentProject )
        {
            SetCurrentProject( _projectNames->count() - 1 );
            _projectNames->setCurrentIndex( _projectNames->count() - 1 );
        }
    }
}

SqlProject* QTSqlGen::GetProject( int index )
{
    SqlProject* currentProject( NULL );
    if ( index == -1 )
        index = _projectNames->currentIndex();

    if( index != -1 )
#if QT_POINTER_SIZE == 4
        currentProject = ( SqlProject* )_projectNames->itemData( index ).toUInt();
#endif

#if QT_POINTER_SIZE == 8
    currentProject = ( SqlProject* )_projectNames->itemData( index ).toULongLong();
#endif

    return currentProject;
}

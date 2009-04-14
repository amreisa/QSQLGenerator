#ifndef SQLPROJECTS_H
#define SQLPROJECTS_H

#include <QList>
#include <QString>
#include <QStringList>

enum DatabaseSourceType
{
	eODBC,
	eSqlite,
	eUnknown
};

#include ".\\Resources\\ODBCDrivers.h"

class SqlProject;

typedef QList<SqlProject*> SqlProjects;
typedef QList<SqlProject*>::iterator SqlProjectIter;
typedef QList<SqlProject*>::const_iterator SqlProjectConstIter;

class SqlProject
{
public:
	SqlProject();
	SqlProject(const SqlProject& copyMe);
	~SqlProject();

	QString						_projectName;
	QString						_targetPath;
	QString						_databasePath;
	bool						_writeProject;
	bool						_dynamicLibrary;
	DatabaseSourceType			_sourceType;
	ODBCDrivers					_odbcDriver;
	bool						_dirty;

	static SqlProject* FindProject(const QString& name, SqlProjects& projects);
	static QStringList GetProjectNames(SqlProjects& projects);
};

#endif


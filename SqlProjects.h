#ifndef SQLPROJECTS_H
#define SQLPROJECTS_H

#include <QList>
#include <QString>

enum DatabaseSourceType
{
	eODBC,
	eSqlite,
	eUnknown
};

#include ".\\Resources\\ODBCDrivers.h"

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
};

typedef QList<SqlProject*> SqlProjects;
typedef QList<SqlProject*>::iterator SqlProjectIter;
typedef QList<SqlProject*>::const_iterator SqlProjectConstIter;

#endif


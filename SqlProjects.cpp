#include "SqlProjects.h"

SqlProject::SqlProject() :
	_dynamicLibrary(true),
	_sourceType(eUnknown),
	_dirty(false)
{
}

SqlProject::SqlProject
(
	const SqlProject& copyMe
)
{
	_projectName = copyMe._projectName;
	_targetPath = copyMe._targetPath;
	_databasePath = copyMe._databasePath;
	_writeProject = copyMe._writeProject;
	_dynamicLibrary = copyMe._dynamicLibrary;
	_sourceType = copyMe._sourceType;
	_odbcDriver = copyMe._odbcDriver;
	_dirty = copyMe._dirty;
}

SqlProject::~SqlProject()
{
}

SqlProject* SqlProject::FindProject
(
	const QString& projectName,
	SqlProjects& projects
)
{
	SqlProject* result(NULL);

	SqlProjectIter projectIter = projects.begin();
	while (projectIter != projects.end())
	{
		if ((*projectIter)->_projectName == projectName)
		{
			result = *projectIter;
			break;
		}

		projectIter++;
	}

	return result;
}

QStringList GetProjectNames
(
	SqlProjects& projects
)
{
	QStringList results;

	SqlProjectIter projectIter = projects.begin();
	while (projectIter != projects.end())
	{
		results.push_back((*projectIter)->_projectName);

		projectIter++;
	}

	return results;
}

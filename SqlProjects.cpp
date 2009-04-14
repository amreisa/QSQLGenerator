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
	_dirty = copyMe._dirty;
}

SqlProject::~SqlProject()
{
}


#ifndef SQLPROJECTS_H
#define SQLPROJECTS_H

#include <QMap>
#include <QString>

class SqlProject
{
public:
	SqlProject();
	~SqlProject();
};

typedef QMap<QString, SqlProject*> SqlProjects;
typedef QMap<QString, SqlProject*>::iterator SqlProjectIter;

#endif


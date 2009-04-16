#ifndef ODBCDRIVERS_H
#define ODBCDRIVERS_H

enum ODBCDrivers
{
	eSqlServer2000 = 0,
	eSqlServer2005 = 1,
	eSqlServer2008 = 2,
	eAccess = 3,
	eMySql = 4,
	eOracle = 5,
	ePostgres = 6
};

static char* DriverNames[] = 
{
	"Sql Server 2000",
	"Sql Server 2005",
	"Sql Server 2008",
	"Microsoft Access",
	"MySql",
	"Oracle",
	"Postgres",
	NULL
};

#endif
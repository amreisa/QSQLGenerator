#ifndef TESTEXPORT_H
#define TESTEXPORT_H

// This File was auto generated on Sun Apr 20 2008 by QTSqliteGen.  
// Do not modify!!!!!
// Changes will be overwritten.

#include <QtGlobal>

#ifdef BASE_DLL
	#ifdef Q_WS_WIN
		#ifdef TEST_DLL
			#define DLL_TEST Q_DECL_EXPORT
		#else
			#define DLL_TEST Q_DECL_IMPORT
		#endif
	#else
		#define DLL_TEST
	#endif
#else
	#define DLL_TEST
#endif

#endif
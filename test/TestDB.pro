# This File was auto generated on Sun Apr 6 2008 by QTSqliteGen.  
# Do not modify!!!!!
# Changes will be overwritten.
TEMPLATE = lib
CONFIG -= app
CONFIG *= qt debug_and_release
CONFIG *= staticlib
DEFINES += BASE_STATIC
QT *= xml sql
TARGET = TestDB
QMAKE_INCDIR_QT = $(QTDIR)\include
QMAKE_LIBDIR_QT = $(QTDIR)\libs
  
CONFIG(debug, debug|release) {
  DESTDIR = debug
} else {
  DESTDIR = release
}
 
DEPENDPATH += .
INCLUDEPATH += .

# Input
HEADERS += 	EmployeesTable.h \
	EmployeesRecord.h \
	AddressesTable.h \
	AddressesRecord.h \
	TestDatabase.h \
	RecordBase.h \
	TestExport.h

SOURCES += 	EmployeesTable.cpp \
	EmployeesRecord.cpp \
	AddressesTable.cpp \
	AddressesRecord.cpp \
	TestDatabase.cpp \
	RecordBase.cpp

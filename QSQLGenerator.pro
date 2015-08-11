TEMPLATE = app
TARGET = QSQLGenerator
DEPENDPATH += .
INCLUDEPATH += .

# Input
HEADERS += AboutDlg.h \
           ProjectNameDialog.h \
           QTSqlGen.h \
           resource.h \
           SqlProjects.h \
           Table.h \

FORMS += AboutDialog.ui ProjectNameDialog.ui QTSqlGen.ui

SOURCES += AboutDlg.cpp \
           main.cpp \
           ProjectNameDialog.cpp \
           QTSqlGen.cpp \
           SqlProjects.cpp \
           
RESOURCES += QTSqlGen.qrc

QT += sql
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

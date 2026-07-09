QT += testlib
QT -= gui
CONFIG += qt console warn_on depend_includepath testcase
TEMPLATE = app

SOURCES += tst_linestest.cpp \
           ../Lines/gamefield.cpp

HEADERS += ../Lines/gamefield.h

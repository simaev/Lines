QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Lines
TEMPLATE = app

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    gamefield.cpp \
    ballwidget.cpp

HEADERS += \
    mainwindow.h \
    gamefield.h \
    ballwidget.h

FORMS += \
    mainwindow.ui

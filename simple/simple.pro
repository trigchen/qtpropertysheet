
QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = QtPropertySheetsSimpleDemo
TEMPLATE = app
DESTDIR = $$PWD/../bin

include(simple.pri)

LIBS += -L$$DESTDIR -lQtPropertySheet
DEPENDPATH += $$PWD/../src

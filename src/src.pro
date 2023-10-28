
QT += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = QtPropertySheet
TEMPLATE = lib
DESTDIR = $$PWD/../bin

# build shared library
DEFINES += QTPROPERTYSHEET_SHARED

# build static library
# DEFINES += QTPROPERTYSHEET_STATIC

include(QtPropertySheet.pri)

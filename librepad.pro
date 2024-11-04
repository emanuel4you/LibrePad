TEMPLATE = app
TARGET = librepad

QT += widgets
QT += printsupport

SOURCES += \
    main.cpp \
    librepad.cpp \
    texteditor.cpp

HEADERS += \
    librepad.h \
    texteditor.h


FORMS += librepad.ui

RESOURCES += \
    librepad.qrc

# install
target.path = /usr/local/bin
INSTALLS += target


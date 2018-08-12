#-------------------------------------------------
#
# Project created by QtCreator 2018-08-03T19:59:45
#
#-------------------------------------------------

QT       -= gui

TARGET = ssnetwork
TEMPLATE = lib

DEFINES += SSNETWORK_LIBRARY

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    sscnetwork.cpp \
    ssccycledetector.cpp \
    sscneuron.cpp \
    ../helpers/sscvariantmap.cpp \
    ../som/som.cpp

HEADERS += \
    sscnetwork.hpp \
    sscnetwork_global.h \
    ssccycledetector.hpp \
    sscneuron.hpp \
    ../helpers/sscvariantmap.hpp \
    ../som/som.hpp

unix {
    target.path = /usr/lib
    INSTALLS += target
}

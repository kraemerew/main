QT += core
QT += gui
QT += widgets

CONFIG += c++11

TARGET = test
CONFIG += console
CONFIG -= app_bundle

TEMPLATE = app

SOURCES += main.cpp \
    cam/cam.cpp \
    nnhelpers/sscactivation.cpp \
    nnhelpers/ssctrainableparameter.cpp \
    nnhelpers/ssccycledetector.cpp \
    #nn/sscnetwork.cpp \
    #nn/sscneuron.cpp \
    highwaynn/sschighwaynetwork.cpp \
    highwaynn/sschighwayneuron.cpp \
    highwaynn/sschighwaygate.cpp \
    image/matrix.cpp \
    image/image.cpp \
    cam/camcapability.cpp \
    cam/frameintervaldescriptor.cpp \
    cam/camwidget.cpp \
    filter/filter.cpp \
    filter/selector.cpp \
    cam/framereader.cpp \
    image/moments.cpp \
    nn/sscgate.cpp

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# derecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

HEADERS += \
    cam/cam.hpp \
    nnhelpers/ssccycledetector.hpp \
    nnhelpers/sscvm.hpp \
    nnhelpers/sscactivation.hpp \
    nnhelpers/ssctrainableparameter.hpp \
    #nn/sscnetwork.hpp \
    #nn/sscnetwork_global.h \
    #nn/sscneuron.hpp \
    highwaynn/sschighwaynetwork.hpp \
    highwaynn/sschighwayneuron.hpp \
    highwaynn/sschighwaygate.hpp \
    image/matrix.hpp \
    image/image.hpp \
    cam/camcapability.hpp \
    cam/frameintervaldescriptor.hpp \
    cam/camwidget.hpp \
    filter/filter.hpp \
    filter/selector.hpp \
    cam/framereader.hpp \
    image/moments.hpp \
    nn/ssineuron.hpp \
    nn/sscgate.hpp

LIBS += -lv4l1

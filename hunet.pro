QT += core
QT += gui
QT += widgets

CONFIG += c++11

TARGET = hunet
CONFIG += console
CONFIG -= app_bundle

INCLUDEPATH += hunet nnhelpers blas highwaynn highwaynn/neurons
TEMPLATE = app

LIBS += -lcblas -lopencv_core -lopencv_imgcodecs -lopencv_imgproc

SOURCES += hunet/main.cpp \
    nnhelpers/sscactivation.cpp \
    nnhelpers/ssctrainableparameter.cpp \
    nnhelpers/ssccycledetector.cpp \
    highwaynn/neurons/gated.cpp \
    highwaynn/neurons/carry.cpp \
    highwaynn/neurons/pool.cpp \
    highwaynn/neurons/neuron.cpp \
    highwaynn/neurons/conv.cpp \
    blas/blasvector.cpp \
    nnhelpers/sscnetworkbase.cpp \
    nnhelpers/ssnjsonhandler.cpp \
    highwaynn/gate.cpp \
    highwaynn/network.cpp \
    highwaynn/convunit.cpp \
    highwaynn/kernel.cpp \
    highwaynn/convhelpers.cpp \
    image/convimageconverter.cpp \
    highwaynn/imageprovider.cpp \
    hunet/imagedropper.cpp \
    hunet/cannycontainer.cpp \
    hunet/contour.cpp \
    hunet/hunetmainwindow.cpp \
    hunet/hunetcannydisplay.cpp \
    hunet/hunetcontourlist.cpp

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# derecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

HEADERS += \
    nnhelpers/ssccycledetector.hpp \
    nnhelpers/sscvm.hpp \
    nnhelpers/sscactivation.hpp \
    nnhelpers/ssctrainableparameter.hpp  \
    highwaynn/neurons/gated.hpp \
    highwaynn/neurons/input.hpp \
    highwaynn/neurons/bias.hpp \
    highwaynn/neurons/carry.hpp \
    highwaynn/neurons/pool.hpp \
    highwaynn/neurons/conv.hpp \
    highwaynn/neurons/neuron.hpp \
    blas/blasvector.hpp \
    nnhelpers/sscnetworkbase.hpp \
    nnhelpers/ssnjsonhandler.hpp \
    highwaynn/gate.hpp \
    highwaynn/network.hpp \
    highwaynn/convunit.hpp \
    highwaynn/kernel.hpp \
    highwaynn/convpatternprovider.hpp \
    highwaynn/convhelpers.hpp \
    image/convimageconverter.hpp \
    highwaynn/imageprovider.hpp \
    hunet/imagedropper.hpp \
    hunet/cannycontainer.hpp \
    hunet/contour.hpp \
    hunet/hunetmainwindow.hpp \
    hunet/hunetcannydisplay.hpp \
    hunet/hunetcontourlist.hpp


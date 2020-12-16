QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    binarymap.cpp \
    binarymapwindow.cpp \
    main.cpp \
    mainwindow.cpp \
    resultswindow.cpp \
    solverwindow.cpp \
    userinterface.cpp

HEADERS += \
    binarymap.h \
    binarymapwindow.h \
    mainwindow.h \
    resultswindow.h \
    solverwindow.h \
    userinterface.h

FORMS += \
    binarymapwindow.ui \
    mainwindow.ui \
    resultswindow.ui \
    solverwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

INCLUDEPATH += C:\opencv\opencv\build\include

LIBS += C:\opencv\opencv\release\bin\libopencv_core430.dll
LIBS += C:\opencv\opencv\release\bin\libopencv_highgui430.dll
LIBS += C:\opencv\opencv\release\bin\libopencv_imgcodecs430.dll
LIBS += C:\opencv\opencv\release\bin\libopencv_imgproc430.dll
LIBS += C:\opencv\opencv\release\bin\libopencv_features2d430.dll
LIBS += C:\opencv\opencv\release\bin\libopencv_calib3d430.dll

SOURCES += \
    binarymap.cpp \
    binarymapwindow.cpp \
    cell.cpp \
    main.cpp \
    mainwindow.cpp \
    results.cpp \
    resultswindow.cpp \
    solver.cpp \
    solverwindow.cpp \
    userinterface.cpp

HEADERS += \
    binarymap.h \
    binarymapwindow.h \
    cell.h \
    mainwindow.h \
    results.h \
    resultswindow.h \
    solver.h \
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

QT       += core gui printsupport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++20
LIBS += -lfftw3
LIBS += -lrtlsdr

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    asyncreader.cpp \
    main.cpp \
    mainwindow.cpp \
    qcustomplot.cpp \
    valueinput.cpp

HEADERS += \
    asyncreader.h \
    mainwindow.h \
    qcustomplot.h \
    valueinput.h

FORMS += \
    mainwindow.ui \
    valueinput.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

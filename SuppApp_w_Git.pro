QT       += core gui
QT       += network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    Networks/adtraction.cpp \
    api_Classes/adtractionapi.cpp \
    apimanager.cpp \
    datamanager.cpp \
    encryptionhelper.cpp \
    main.cpp \
    mainwindow.cpp \
    networkmanager.cpp \
    p_supportpage.cpp \
    setapikey.cpp

HEADERS += \
    APIData.h \
    Networks/adtraction.h \
    api_Classes/adtractionapi.h \
    advertiserdata.h \
    apimanager.h \
    BaseClasses/basenetwork.h \
    datamanager.h \
    encryptionhelper.h \
    mainwindow.h \
    networkmanager.h \
    p_supportpage.h \
    setapikey.h

FORMS += \
    mainwindow.ui \
    p_supportpage.ui \
    setapikey.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

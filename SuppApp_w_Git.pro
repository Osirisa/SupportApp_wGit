QT       += core gui
QT       += network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

TARGET = SupportHelper

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    networkchannels.cpp \
    Networks/adtraction.cpp \
    api_Classes/adtractionapi.cpp \
    apimanager.cpp \
    DataManager/datamanager.cpp \
    encryptionhelper.cpp \
    main.cpp \
    mainwindow.cpp \
    networkmanager.cpp \
    p_supportpageAdtraction.cpp \
    setapikey.cpp \
    suppeventbus.cpp

HEADERS += \
    APIData.h \
    networkchannels.h \
    Networks/adtraction.h \
    api_Classes/adtractionapi.h \
    advertiserdata.h \
    apimanager.h \
    BaseClasses/basenetwork.h \
    DataManager/datamanager.h \
    encryptionhelper.h \
    mainwindow.h \
    networkmanager.h \
    p_supportpageAdtraction.h \
    setapikey.h \
    suppeventbus.h

FORMS += \
    networkchannels.ui \
    mainwindow.ui \
    p_supportpageAdtraction.ui \
    setapikey.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    ressources.qrc

RC_FILE = appIcon/AppIcon.rc

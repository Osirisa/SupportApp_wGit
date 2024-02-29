QT       += core gui
QT       += network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

TARGET = SupportHelper

# Pfad zu den OpenSSL-Include-Dateien und den Lib-Dateien
INCLUDEPATH += "C:/Program Files/OpenSSL-Win64/include"

# Für Debug-Konfiguration
debug:LIBS += -L"C:/Program Files/OpenSSL-Win64/lib/VC/x64/MDd" -llibcrypto -llibssl

# Für Release-Konfiguration
release:LIBS += -L"C:/Program Files/OpenSSL-Win64/lib/VC/x64/MD" -llibcrypto -llibssl

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    DataManager/adtractionsuppdatamanager.cpp \
    DataManager/basesuppdatamanager.cpp \
    DataManager/standardfiles.cpp \
    networkchannels.cpp \
    Networks/adtraction.cpp \
    api_Classes/adtractionapi.cpp \
    apimanager.cpp \
    DataManager/datamanager.cpp \
    encryptionhelper.cpp \
    main.cpp \
    mainwindow.cpp \
    networkmanager.cpp \
    setapikey.cpp \
    suppeventbus.cpp \
    supportClasses/adtractionsuppcase.cpp \
    supportClasses/supportcaseBase.cpp \
    supportPages/p_supportpageAdtraction.cpp

HEADERS += \
    APIData.h \
    DataManager/adtractionsuppdatamanager.h \
    DataManager/basesuppdatamanager.h \
    DataManager/standardfiles.h \
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
    setapikey.h \
    suppeventbus.h \
    supportClasses/adtractionsuppcase.h \
    supportClasses/supportcaseBase.h \
    supportPages/p_supportpageAdtraction.h

FORMS += \
    networkchannels.ui \
    mainwindow.ui \
    setapikey.ui \
    supportPages/p_supportpageAdtraction.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    ressources.qrc

RC_FILE = appIcon/AppIcon.rc

#ifndef APIMANAGER_H
#define APIMANAGER_H

#include <QObject>
#include <QString>
#include <QStringList>
#include <QHash>
#include <QTimer>

#include "DataManager/datamanager.h"
#include "networkmanager.h"
#include "Networks/adtraction.h"
#include "encryptionhelper.h"

class APIManager: public QObject
{
    Q_OBJECT

public:
    APIManager(DataManager* dataManager,  EncryptionHelper* encryptionHelper);

    void refreshApiManager();

    //"Network" Classes
    Adtraction* adtraction;

private:

    //Manager Classes
    NetworkManager* networkManager;
    DataManager* dataManager;



    //
    EncryptionHelper* encryptionHelper;

    void initApiNetwork();
};

#endif // APIMANAGER_H

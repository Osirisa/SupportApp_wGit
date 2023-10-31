#ifndef APIMANAGER_H
#define APIMANAGER_H

#include <QObject>
#include <QString>
#include <QStringList>
#include <QHash>

#include "datamanager.h"
#include "networkmanager.h"
#include "Networks/adtraction.h"

class APIManager: public QObject
{
    Q_OBJECT

public:
    APIManager(DataManager* dataManager);

    //"Network" Classes
    Adtraction* adtraction;
private:

    //Manager Classes
    NetworkManager* networkManager;
    DataManager* dataManager;



    //TBD: other "Network" Classes
};

#endif // APIMANAGER_H

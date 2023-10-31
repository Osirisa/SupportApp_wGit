#ifndef APIMANAGER_H
#define APIMANAGER_H

#include <QObject>
#include "datamanager.h"

class APIManager: public QObject
{
    Q_OBJECT

public:
    APIManager(DataManager* dataManager,QObject *parent = nullptr);

    void addAPI();
private:
    DataManager* dataManager;
};

#endif // APIMANAGER_H

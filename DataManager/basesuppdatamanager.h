#ifndef BASESUPPDATAMANAGER_H
#define BASESUPPDATAMANAGER_H

#include <QObject>
#include <QJsonDocument>
#include "DataManager/datamanager.h"

class BaseSuppDataManager : public QObject
{
    Q_OBJECT
public:
    BaseSuppDataManager(DataManager* dataManager);
    virtual ~BaseSuppDataManager();

    QJsonDocument getNetworkChannelsDoc();

protected:

    QString pathToSessionJson;

    DataManager dataManager;
};

#endif // BASESUPPDATAMANAGER_H

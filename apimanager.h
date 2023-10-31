#ifndef APIMANAGER_H
#define APIMANAGER_H

#include <QObject>
#include <QString>
#include <QStringList>
#include <QHash>

#include "datamanager.h"

class APIManager: public QObject
{
    Q_OBJECT

public:
    struct NetworkInfo {
        QString apiKey;
        QList<int> channelIDs;
    };

    struct Admin {
        APIManager* apiManager;

        Admin(APIManager* manager) : apiManager(manager) {}

        void addAPIKey(const QString& apiName, const QString& key);
        void addAPIChannelID(const QString& apiName, int channelID);
    };

    struct GetAdvertisers {
        APIManager* apiManager;

        GetAdvertisers(APIManager* manager) : apiManager(manager) {}

        QStringList all();
        QStringList fromChannel(const QString& apiName, int channelID);
        QStringList fromNetwork(const QString& apiName);
    };

    struct RefreshAdvertisers {
        APIManager* apiManager;

        RefreshAdvertisers(APIManager* manager) : apiManager(manager) {}

        void all();
        void fromChannel(const QString& apiName, int channelID);
        void fromNetwork(const QString& apiName);
    };

    GetAdvertisers getAdvertisers;
    RefreshAdvertisers refreshAdvertisers;
    Admin admin;

    APIManager();
private:
    QHash<QString, NetworkInfo> networks;
};

#endif // APIMANAGER_H

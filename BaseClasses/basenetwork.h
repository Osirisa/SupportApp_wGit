#ifndef BASENETWORK_H
#define BASENETWORK_H

#include <QObject>
#include <QString>
#include <QList>
#include <QStringList>
#include "DataManager/datamanager.h"
#include "networkmanager.h"

class BaseNetwork : public QObject {
    Q_OBJECT  // Required for all QObject derived classes

public:
    explicit BaseNetwork(DataManager* dataManager, NetworkManager* networkManager,const QString &apiToken ,QObject *parent = nullptr)
        : QObject(parent), dataManager(dataManager), networkManager(networkManager), apiToken(apiToken) {}

    class UpdateAdvertisersBase : public QObject {

    public:
        explicit UpdateAdvertisersBase(BaseNetwork* parent) : QObject(parent), parentNetwork(parent) {}
        virtual void byChannel(int channelID, const QString &channelRegion) {}
        virtual void allChannels() {}

    protected:
        BaseNetwork* parentNetwork;
    };

    class GetAdvertisersBase : public QObject {

    public:
        explicit GetAdvertisersBase(BaseNetwork* parent) : QObject(parent), parentNetwork(parent) {}
        virtual QStringList fromChannel(int channelID) { return QStringList(); }
        virtual QStringList fromNetwork() { return QStringList(); }

    protected:
        BaseNetwork* parentNetwork;
    };

    class Admin : public QObject {

    public:
        explicit Admin(BaseNetwork* parent) : QObject(parent), parentNetwork(parent) {}
        void addAPIKey(const QString& key);
        void addAPIChannelID(int channelID);

    protected:
        BaseNetwork* parentNetwork;
    };

protected:
    struct NetworkInfo {
        QString apiKey;
        QList<int> channelIDs;
    };
    NetworkInfo info;

    DataManager* dataManager;
    NetworkManager* networkManager;
    QString apiToken;

private:
};

#endif // BASENETWORK_H

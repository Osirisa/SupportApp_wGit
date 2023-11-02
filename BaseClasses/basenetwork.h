#ifndef BASENETWORK_H
#define BASENETWORK_H

#include <QString>
#include <QList>
#include <QStringList>

class BaseNetwork {
public:
    BaseNetwork()
        : UpdateAdverts(this), GetAdverts(this), Admin(this) {}

    class UpdateAdvertisers {
    public:
        UpdateAdvertisers(BaseNetwork* parent) : parentNetwork(parent) {}
        virtual void byChannel(int channelID) {}
        virtual void allChannels() {}

    private:
        BaseNetwork* parentNetwork;
    } UpdateAdverts;

    class GetAdvertisers {
    public:
        GetAdvertisers(BaseNetwork* parent) : parentNetwork(parent) {}
        virtual QStringList fromChannel(int channelID) { return QStringList(); }
        virtual QStringList fromNetwork() { return QStringList(); }

    private:
        BaseNetwork* parentNetwork;
    } GetAdverts;

    class Admin {
    public:
        Admin(BaseNetwork* parent) : parentNetwork(parent) {}
        void addAPIKey(const QString& key) { parentNetwork->info.apiKey = key; }
        void addAPIChannelID(int channelID) { parentNetwork->info.channelIDs.append(channelID); }

    private:
        BaseNetwork* parentNetwork;
    } Admin;

protected:
    struct NetworkInfo {
        QString apiKey;
        QList<int> channelIDs;
    };
    NetworkInfo info;
};

#endif // BASENETWORK_H

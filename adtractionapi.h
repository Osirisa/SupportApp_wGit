#ifndef ADTRACTIONAPI_H
#define ADTRACTIONAPI_H

#include <QObject>
#include <QHash>
#include "advertiserdata.h"
#include "networkmanager.h"
#include "datamanager.h"
#include "apimanager.h"

class AdtractionAPI : public QObject
{
    Q_OBJECT
public:
    explicit AdtractionAPI(NetworkManager* networkManager, DataManager* dataManager, APIManager* apiManager,const QString& apiToken, QObject *parent = nullptr);

    const QHash<QString, AdvertiserData>& getAdvertisers() const;
    const AdvertiserData& getAdvertiserData(const QString& shopName) const;

    void updateCurrencies();
    void updateAdvertisers(int channelId);
    void loadAdvertisersData();

signals:
    void requestFinished(QNetworkReply*);
    void advertisersUpdated(const QHash<QString, AdvertiserData> &advertisers);

private slots:
    void onCurrenciesRequestFinished(QNetworkReply* reply);
    void onAdvertisersRequestFinished(QNetworkReply* reply);

private:
    NetworkManager* networkManager;
    DataManager* dataManager;
    APIManager* apiManager;
    QString apiToken;

    QHash<QString, AdvertiserData> m_advertisers;
};

#endif // ADTRACTIONAPI_H

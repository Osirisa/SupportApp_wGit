#ifndef ADTRACTIONAPI_H
#define ADTRACTIONAPI_H

#include <QObject>
#include <QHash>
#include "networkmanager.h"
#include "DataManager/datamanager.h"

class AdtractionAPI : public QObject
{
    Q_OBJECT
public:
    explicit AdtractionAPI(NetworkManager* networkManager, DataManager* dataManager,const QString& apiToken, QObject *parent = nullptr);
    ~AdtractionAPI();
    static QEvent::Type SupportMessageEventType;

    void updateCurrencies();
    void updateAdvertisers(int channelId, const QString &marketRegion);
    void updateRegions();
    void deleteAdvertiser();
    void listApprovedChannels();

    void loadAdvertisersData();
    void sendSuppData(int programId, int channelId,QString orderId,int commissionId,double expecetedCom,QString transactionDate, double orderVal, QString currency, QString userId);



signals:
    void requestFinished(QNetworkReply*);

private slots:
    void onCurrenciesRequestFinished(QNetworkReply* reply);
    void onAdvertisersRequestFinished(QNetworkReply* reply, int channelId);
    void onSuppDataRequestFinished(QNetworkReply* reply, QString userId, QString orderId);
    void onRegionsRequestFinished(QNetworkReply* reply);
    void onListApprovedChannelsRequestFinished(QNetworkReply *reply);

private:
   // QJsonObject jsonObject; // Declare jsonObject as a member variable in your class

    NetworkManager* networkManager;
    DataManager* dataManager;
    QString apiToken;
};

#endif // ADTRACTIONAPI_H

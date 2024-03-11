#ifndef ADTRACTIONSUPPDATAMANAGER_H
#define ADTRACTIONSUPPDATAMANAGER_H

#include <QObject>
#include "DataManager/basesuppdatamanager.h"
#include "supportClasses/adtractionsuppcase.h"

struct Commission {
    QString id;
    QString name;
    double value;
    QString type; // e.g., "%" or currency
};

struct Shop {
    QString programName;
    int programId;
    QString channelID;
    QList<Commission> commissions;
};

struct AdtractionNetworkChannelData
{
    QString channelID;
    QString channelName;
    QString channelRegion;
};

class AdtractionSuppDataManager : public BaseSuppDataManager
{
public:
    AdtractionSuppDataManager(DataManager *dataManager);
    ~AdtractionSuppDataManager();

    bool saveObjectToSessionFile(const AdtractionSuppCase& adtractionObject);
    bool saveObjectToSessionFile(const QList<AdtractionSuppCase>& adtractionObjects);

    void deleteOrder(const QString& userId, const QString& orderId);

    AdtractionSuppCase loadObjectFromSessionFile(const QString& userId, const QString& orderId);
    QList<AdtractionSuppCase> loadAllObjectsFromSessionFile();
    QList<AdtractionNetworkChannelData> getAdtractionNetworkChannels();

    QList<Shop> getShopsByChannelID(const QString& channelID);
    QList<Commission> getCommissionsByShop(const QString& shopName, const QString& channelID = QString());
    QStringList getSortedCurrencies();
};

#endif // ADTRACTIONSUPPDATAMANAGER_H

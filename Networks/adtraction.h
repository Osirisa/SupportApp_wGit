#ifndef ADTRACTION_H
#define ADTRACTION_H

#include "BaseClasses/BaseNetwork.h"
#include "api_Classes/adtractionapi.h"


class Adtraction : public BaseNetwork {
    Q_OBJECT

public:
    explicit Adtraction(DataManager* dataManager, NetworkManager* networkManager, const QString &apiToken, QObject *parent = nullptr);
    ~Adtraction();


    class UpdateAdvs : public UpdateAdvertisersBase {

    public:
        explicit UpdateAdvs(Adtraction* parent);

        void currencies();
        void deleteAdvertisers();
        void byChannel(int channelID) override;

    protected:
        Adtraction* parentAdtraction;
    };

    class GetAdvs : public GetAdvertisersBase {

    public:
        explicit GetAdvs(Adtraction* parent);

        QStringList fromChannel(int channelID) override;
        QStringList fromNetwork() override;

    protected:
        Adtraction* parentAdtraction;
    };

    UpdateAdvs updater;
    GetAdvs getter;

    void sendSuppData(int programId, int channelId,QString orderId,int commissionId,double expecetedCom,QString transactionDate, double orderVal, QString currency, QString userId);
    void refreshAPI();

private:

    AdtractionAPI* adtractionAPI;
    void initApi();
};

#endif // ADTRACTION_H

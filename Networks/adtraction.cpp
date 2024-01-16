#include "Networks/adtraction.h"


Adtraction::Adtraction(DataManager* dataManager, NetworkManager* networkManager, const QString &apiToken ,QObject *parent)
    : BaseNetwork(dataManager, networkManager, apiToken, parent), updater(this), getter(this){

    initApi();
}

Adtraction::~Adtraction(){

    delete adtractionAPI;
}

void Adtraction::sendSuppData(int programId, int channelId,QString orderId,int commissionId,double expecetedCom,QString transactionDate, double orderVal, QString currency, QString userId)
{
    adtractionAPI->sendSuppData(programId,channelId,orderId,commissionId,expecetedCom,transactionDate,orderVal,currency,userId);
}

void Adtraction::refreshAPI()
{
    if(adtractionAPI){
        delete adtractionAPI;
    }
    initApi();
}

void Adtraction::initApi()
{
    adtractionAPI = new AdtractionAPI(networkManager,dataManager,apiToken,this);
}

Adtraction::UpdateAdvs::UpdateAdvs(Adtraction* parent)
    : UpdateAdvertisersBase(parent), parentAdtraction(parent) {}

void Adtraction::UpdateAdvs::currencies()
{
    if(parentAdtraction->adtractionAPI){
        parentAdtraction->adtractionAPI->updateCurrencies();
    }
}

void Adtraction::UpdateAdvs::byChannel(int channelID) {
    parentAdtraction->adtractionAPI->updateAdvertisers(channelID);
}

// void Adtraction::UpdateAdvs::allChannels() {
//     // Implementation for updating all channels
// }

Adtraction::GetAdvs::GetAdvs(Adtraction* parent)
    : GetAdvertisersBase(parent), parentAdtraction(parent) {}

QStringList Adtraction::GetAdvs::fromChannel(int channelID) {

    //TBD:
    QStringList advertisers;
    return advertisers;
}

QStringList Adtraction::GetAdvs::fromNetwork() {

    //TBD:
    QStringList advertisers;
    return advertisers;
}

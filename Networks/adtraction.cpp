#include "Networks/adtraction.h"


Adtraction::Adtraction(DataManager* dataManager, NetworkManager* networkManager, const QString &apiToken ,QObject *parent)
    : BaseNetwork(dataManager, networkManager, apiToken, parent), updater(this), getter(this){

    qDebug()<<"Test1";
    initApi();
}

Adtraction::~Adtraction(){

    delete adtractionAPI;
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

void Adtraction::UpdateAdvs::byChannel(int channelID) {
    // Implementation for updating advertisers by channel
}

void Adtraction::UpdateAdvs::allChannels() {
    // Implementation for updating all channels
    qDebug()<<"Test2";
    if(parentAdtraction->adtractionAPI){
        qDebug()<<"Test3";
        parentAdtraction->adtractionAPI->updateAdvertisers(1592293656);
    }
}

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

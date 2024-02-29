#include "basesuppdatamanager.h"

BaseSuppDataManager::BaseSuppDataManager(DataManager* dataManager) : dataManager(dataManager){}

QJsonDocument BaseSuppDataManager::getNetworkChannelsDoc()
{
    return dataManager.json->load("NetworkChannels");
}

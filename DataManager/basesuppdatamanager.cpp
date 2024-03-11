#include "DataManager/basesuppdatamanager.h"

BaseSuppDataManager::BaseSuppDataManager(DataManager* dataManager) : dataManager(dataManager){}

QJsonDocument BaseSuppDataManager::getNetworkChannelsDoc()
{
    return dataManager->json->load("NetworkChannels");
}
BaseSuppDataManager::~BaseSuppDataManager() {
    // destructor implementation, can be empty if no cleanup is needed
}

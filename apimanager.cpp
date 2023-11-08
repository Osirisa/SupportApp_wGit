#include "APIManager.h"

APIManager::APIManager(DataManager* dataManager, EncryptionHelper* encryptionHelper) :
    dataManager(dataManager), encryptionHelper(encryptionHelper)
{

    QTimer::singleShot(300, this, &APIManager::initApiNetwork);
}

void APIManager::initApiNetwork()
{
    QString adtractionApiKey = encryptionHelper->encryptDecrypt(dataManager->txt->load("adtractionKey"));
    qDebug() << QString("%1").arg(adtractionApiKey);
    networkManager = new NetworkManager(this);

    adtraction = new Adtraction(dataManager,networkManager,adtractionApiKey,this);
}

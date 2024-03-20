#include "APIManager.h"

APIManager::APIManager(DataManager* dataManager, EncryptionHelper* encryptionHelper) :
    dataManager(dataManager), encryptionHelper(encryptionHelper)
{

    QTimer::singleShot(300, this, &APIManager::initApiNetwork);
}

void APIManager::refreshApiManager()
{
    if(networkManager){
        delete networkManager;
    }
    if(adtraction){
        delete adtraction;
    }

    initApiNetwork();
}

void APIManager::initApiNetwork()
{
    QByteArray encryptedData = dataManager->txt->load("AdtractionKey").toUtf8();

    // Assuming the data is stored in Base64 encoding
    QByteArray decodedData = QByteArray::fromBase64(encryptedData);

    // Decrypt the API key
    QString decryptedKey = encryptionHelper->decrypt(decodedData, "hello");

    //qDebug() << QString("%1").arg(decryptedKey);
    networkManager = new NetworkManager(this);

    adtraction = new Adtraction(dataManager,networkManager,decryptedKey,this);
}

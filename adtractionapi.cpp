#include "adtractionapi.h"
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>

AdtractionAPI::AdtractionAPI(NetworkManager* networkManager, DataManager* dataManager, const QString& apiToken, QObject *parent)
    : QObject(parent), networkManager(networkManager), dataManager(dataManager), apiToken(apiToken)
{
    loadAdvertisersData();  // Load data from file on object creation
}

void AdtractionAPI::updateCurrencies()
{
    QNetworkReply* reply = networkManager->sendGetRequest("https://api.adtraction.com/v2/partner/currencies/", apiToken);
    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        onCurrenciesRequestFinished(reply);
    });
}

void AdtractionAPI::onCurrenciesRequestFinished(QNetworkReply* reply)
{
    if(reply->error() == QNetworkReply::NoError) {
        QByteArray responseData = reply->readAll();
        dataManager->saveToFile("currencies.txt", responseData);
    } else {
        qWarning() << "Network request failed:" << reply->errorString();
    }
    reply->deleteLater();
}

void AdtractionAPI::updateAdvertisers(int channelId)
{
    QString endpoint = QString("https://api.adtraction.com/v3/partner/programs/?token=%1").arg(apiToken);

    // Create a QJsonObject with all the necessary parameters
    QJsonObject json;
    json.insert("channelId", channelId);
    json.insert("approvalStatus", 1);
    json.insert("market", "CH");  // replace "CH" with the actual market value
    json.insert("status", 0);  // replace 0 with the actual status value

    QNetworkReply* reply = networkManager->sendPostRequest(endpoint, apiToken, json);
    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        onAdvertisersRequestFinished(reply);
    });
}

void AdtractionAPI::onAdvertisersRequestFinished(QNetworkReply* reply)
{
    int statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    qDebug() << "HTTP status code:" << statusCode;

    if(reply->error() == QNetworkReply::NoError) {
        QByteArray responseData = reply->readAll();
        QJsonDocument jsonDoc = QJsonDocument::fromJson(responseData);
        QJsonArray advertisersArray = jsonDoc.array();

        for(const QJsonValue& value : advertisersArray) {
            QJsonObject advertiserObj = value.toObject();
            AdvertiserData adData;
            adData.programId = advertiserObj["programId"].toInt();
            adData.market = advertiserObj["market"].toString();
            adData.currency = advertiserObj["currency"].toString();
            adData.programName = advertiserObj["programName"].toString();

            // Extracting commission data
            QJsonArray commissionsArray = advertiserObj["commissions"].toArray();
            for(const QJsonValue& commissionValue : commissionsArray) {
                QJsonObject commissionObj = commissionValue.toObject();
                Commission commission;
                commission.id = QString::number(commissionObj["id"].toInt());  // Adjusted line
                commission.type = commissionObj["type"].toString();
                commission.name = commissionObj["name"].toString();
                commission.value = commissionObj["value"].toDouble();
                commission.transactionType = commissionObj["transactionType"].toInt();
                adData.commissions.append(commission);
            }

            m_advertisers.insert(adData.programName, adData);  // Updating m_advertisers hash
        }

        dataManager->saveJsonToFile("advertisers.json", jsonDoc);
    } else {
        qWarning() << "Network request failed:" << reply->errorString();
    }
    reply->deleteLater();
    emit advertisersUpdated(m_advertisers);
}

void AdtractionAPI::loadAdvertisersData()
{
    QJsonDocument jsonDoc = dataManager->loadJsonFromFile("advertisers.json");
    if (!jsonDoc.isNull()) {
        QJsonObject jsonObject = jsonDoc.object();
        m_advertisers.clear();  // Clear any existing data
        for(const QString& key : jsonObject.keys()) {
            QJsonObject advertiserObject = jsonObject[key].toObject();
            AdvertiserData adData;
            adData.programId = advertiserObject["programId"].toInt();
            adData.market = advertiserObject["market"].toString();
            adData.currency = advertiserObject["currency"].toString();
            adData.programName = key;
            m_advertisers.insert(key, adData);
        }
        emit advertisersUpdated(m_advertisers);
    }
}

const QHash<QString, AdvertiserData>& AdtractionAPI::getAdvertisers() const {
    return m_advertisers;
}

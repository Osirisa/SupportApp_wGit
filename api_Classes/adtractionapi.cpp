#include "api_Classes/adtractionapi.h"
#include "qjsonobject.h"
#include "suppeventbus.h"
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QPointer>
#include <QEvent>

//Takes in the NetworkManager, the DataManager, the APIToken and the Parent to ensure being loaded the whole time
AdtractionAPI::AdtractionAPI(NetworkManager* networkManager, DataManager* dataManager, const QString& apiToken, QObject *parent)
    : QObject(parent), networkManager(networkManager), dataManager(dataManager), apiToken(apiToken)
{}

AdtractionAPI::~AdtractionAPI()
{
}

QEvent::Type AdtractionAPI::SupportMessageEventType = static_cast<QEvent::Type>(QEvent::registerEventType());


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
        dataManager->txt->save("AdtractionCurrencies", responseData);
    } else {
        qWarning() << "Network request failed:" << reply->errorString();
    }
    reply->deleteLater();

    SuppEventBus::instance()->publish("currenciesUpdated");
}


//Sends a Post-Request to the server with the needed information to get all the accepted shops in the selected market.
void AdtractionAPI::updateAdvertisers(int channelId, const QString &marketRegion)
{
    //takes the endpoint and adds the apiToken
    QString endpoint = QString("https://api.adtraction.com/v3/partner/programs/?token=%1").arg(apiToken);
    // Create a QJsonObject with all the necessary parameters
    QJsonObject json;
    json.insert("channelId", channelId);        // current channelId of rewardo. ch/de/at, standard.at...
    json.insert("approvalStatus", 1);           // 0 rejected, 1 approved, 2 pending review
    json.insert("market", marketRegion);                // market: CH AT DE
    json.insert("status", 0);                   // 0 Live, 3 closing

    //gets the reply from the server and saves it into a QNetworkReply object
    QNetworkReply* reply = networkManager->sendPostRequest(endpoint, apiToken, json);
    QPointer<QNetworkReply> safeReply = reply;

    //If reply received a signal gets emited to the slot onAdvertisersRequestFinished
    connect(reply, &QNetworkReply::finished, this, [this, safeReply, channelId]() {
        qDebug() << "Finished signal emitted";
        if (safeReply) {
            qDebug() << "Reply is safe to use";
            onAdvertisersRequestFinished(safeReply, channelId);
        } else {
            qDebug() << "Reply was deleted before slot execution";
        }
    });
}

//gets called if the the signal from the reply gets emited. Takes in the reply
void AdtractionAPI::onAdvertisersRequestFinished(QNetworkReply* reply, int channelID)
{
    //DEBUG -> TBD: set up a visual error feedback
    int statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    qDebug() << "HTTP status code:" << statusCode;

    //checks if no Error was sendet back
    if(reply->error() == QNetworkReply::NoError) {

        //gets the reply and copies it onto a QBytearray
        QByteArray responseData = reply->readAll();

        //saves it into a JSONDOC
        QJsonDocument originalJsonDoc = QJsonDocument::fromJson(responseData);
        QJsonArray originalAdvertisersArray = originalJsonDoc.array();

        // Create a new JSON array for storing the modified data
        QJsonArray newAdvertisersArray;

        //fills up a AdvertiserData Structure with the needed Data
        for(const QJsonValue& value : originalAdvertisersArray) {
            QJsonObject advertiserObj = value.toObject();

            // Create a new JSON object for this advertiser
            QJsonObject newAdvertiserObj;
            newAdvertiserObj["programId"] = advertiserObj["programId"].toInt();
            newAdvertiserObj["market"] = advertiserObj["market"].toString();
            newAdvertiserObj["currency"] =advertiserObj["currency"].toString();
            newAdvertiserObj["programName"] = advertiserObj["programName"].toString();


            //fills up a AdvertiserData Structure with the needed Data -> specific the comissionID
            QJsonArray commissionsArray = advertiserObj["commissions"].toArray();
            QJsonArray newCommissionsArray;

            for(const QJsonValue& commissionValue : commissionsArray) {
                QJsonObject commissionObj = commissionValue.toObject();

                // Add the commission to the new commissions array
                QJsonObject newCommissionObj;
                newCommissionObj["id"] = QString::number(commissionObj["id"].toInt());
                newCommissionObj["type"] = commissionObj["type"].toString();
                newCommissionObj["name"] = commissionObj["name"].toString();
                newCommissionObj["value"] = commissionObj["value"].toDouble();
                newCommissionObj["transactionType"] = commissionObj["transactionType"].toInt();
                newCommissionsArray.append(newCommissionObj);
            }

            newAdvertiserObj["commissions"] = newCommissionsArray;
            newAdvertisersArray.append(newAdvertiserObj);
        }

        // Create a new JSON document from the modified array and save it
        QJsonDocument newJsonDoc(newAdvertisersArray);
        dataManager->json->save("Adtraction"+QString::number(channelID), newJsonDoc);

    } else {
        qWarning() << "Network request failed:" << reply->errorString();
    }
    reply->deleteLater();
    SuppEventBus::instance()->publish("shopsUpdated");
}


void AdtractionAPI::sendSuppData(int programId, int channelId,QString orderId,int commissionId,double expecetedCom,QString transactionDate, double orderVal, QString currency, QString userId)
{
    QString endpoint = QString("https://api.adtraction.com/v2/partner/claims/?token=%1").arg(apiToken);

    // Create a QJsonObject with all the necessary parameters
    QJsonObject json;
    json.insert("programId", programId);
    json.insert("channelId", channelId);
    json.insert("orderId", orderId);
    json.insert("commissionId", commissionId);
    json.insert("expectedCommission", expecetedCom);
    json.insert("transactionDate", transactionDate);
    json.insert("orderValue", orderVal);
    json.insert("currency", currency);
//    json.insert("epi", "banner top right");
    json.insert("epi",userId);

    //gets the reply from the server and saves it into a QNetworkReply object
    QNetworkReply* reply = networkManager->sendPutRequest(endpoint, apiToken, json);

    connect(reply, &QNetworkReply::finished, this, [this, reply, userId,orderId]() {
        onSuppDataRequestFinished(reply, userId, orderId);
    });
}

void AdtractionAPI::onSuppDataRequestFinished(QNetworkReply *reply,QString epi, QString orderId) {

    QJsonDocument suppData = dataManager->json->load("NetworkSuppAnswers");
    QJsonObject jObj;

    int statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    qDebug() << "HTTP status code:" << statusCode;

    if (!suppData.isNull()) {
        // If the JSON document is not null, use its object
        jObj = suppData.object();
    }

    if(jObj.contains(epi)){
        QJsonObject userObj = jObj[epi].toObject();
        QJsonArray ordersArray = userObj["orders"].toArray();

        for(int i = 0; i<ordersArray.size(); ++i){
            QJsonObject orderObj = ordersArray[i].toObject();

            if(orderObj["orderID"].toString() == orderId){

                orderObj["networkStatusCode"] = QString::number(reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt());
                orderObj["networkReply"] = QString::fromUtf8(reply->readAll());

                ordersArray[i] = orderObj;

                break;
            }
        }

        userObj["orders"] = ordersArray;

        jObj[epi] = userObj;

        QJsonDocument suppdoc(jObj);

        dataManager->json->save("NetworkSuppAnswers",suppdoc);
    }

    SuppEventBus::instance()->publish("networkResponse",QString::number(reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt()),epi,orderId);
}

void AdtractionAPI::updateRegions()
{
    QNetworkReply* reply = networkManager->sendGetRequest("https://api.adtraction.com/v2/partner/markets/", apiToken);
    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        onRegionsRequestFinished(reply);
    });
}

void AdtractionAPI::onRegionsRequestFinished(QNetworkReply *reply)
{
    if(reply->error() == QNetworkReply::NoError) {

        //gets the reply and copies it onto a QBytearray
        QByteArray responseData = reply->readAll();

        //saves it into a JSONDOC
        QJsonDocument replyJsonDoc = QJsonDocument::fromJson(responseData);
        dataManager->json->save("AdtractionRegions",replyJsonDoc);
        SuppEventBus::instance()->publish("adtractionRegionsUpdated");
    } else {
        qWarning() << "Network request failed:" << reply->errorString();
    }
    reply->deleteLater();
}


//TBD: implement so that you only have to write the ID into the channelIDs tab -> retreive name and markets
void AdtractionAPI::listApprovedChannels() {
    QString endpoint = "https://api.adtraction.com/v2/partner/channels";
    QNetworkReply* reply = networkManager->sendGetRequest(endpoint, apiToken);
    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        onListApprovedChannelsRequestFinished(reply);
    });
}

void AdtractionAPI::onListApprovedChannelsRequestFinished(QNetworkReply* reply) {
    if (reply->error() == QNetworkReply::NoError) {
        QByteArray responseData = reply->readAll();
        QJsonDocument jsonDoc = QJsonDocument::fromJson(responseData);
        QJsonArray channelsArray = jsonDoc.array();

        // Process each channel object
        for (const QJsonValue &value : channelsArray) {
            QJsonObject channelObj = value.toObject();
            qDebug() << "Channel ID:" << channelObj["channelId"].toInt();
            qDebug() << "Channel Name:" << channelObj["channelName"].toString();
            // Add more processing as needed
        }

        // Signal or further process the list of channels as needed
        SuppEventBus::instance()->publish("channelsListUpdated");
    } else {
        qWarning() << "Network request for listing approved channels failed:" << reply->errorString();
    }
    reply->deleteLater();
}

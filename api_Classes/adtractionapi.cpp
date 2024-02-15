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
        dataManager->txt->save("currenciesAdtraction", responseData);
    } else {
        qWarning() << "Network request failed:" << reply->errorString();
    }
    reply->deleteLater();
}


//Sends a Post-Request to the server with the needed information to get all the accepted shops in the selected market.
void AdtractionAPI::updateAdvertisers(int channelId)
{
    //takes the endpoint and adds the apiToken
    QString endpoint = QString("https://api.adtraction.com/v3/partner/programs/?token=%1").arg(apiToken);
    // Create a QJsonObject with all the necessary parameters
    QJsonObject json;
    json.insert("channelId", channelId);        // current channelId of rewardo. ch/de/at, standard.at...
    json.insert("approvalStatus", 1);           // 0 rejected, 1 approved, 2 pending review
    json.insert("market", "CH");                // market: CH AT DE
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
            AdvertiserData adData;
            adData.programId = advertiserObj["programId"].toInt();
            adData.market = advertiserObj["market"].toString();
            adData.currency = advertiserObj["currency"].toString();
            adData.programName = advertiserObj["programName"].toString();

            // Create a new JSON object for this advertiser
            QJsonObject newAdvertiserObj;
            newAdvertiserObj["programId"] = adData.programId;
            newAdvertiserObj["market"] = adData.market;
            newAdvertiserObj["currency"] = adData.currency;
            newAdvertiserObj["programName"] = adData.programName;

            //fills up a AdvertiserData Structure with the needed Data -> specific the comissionID
            QJsonArray commissionsArray = advertiserObj["commissions"].toArray();
            QJsonArray newCommissionsArray;
            for(const QJsonValue& commissionValue : commissionsArray) {
                QJsonObject commissionObj = commissionValue.toObject();
                Commission commission;
                commission.id = QString::number(commissionObj["id"].toInt());  // Adjusted line
                commission.type = commissionObj["type"].toString();
                commission.name = commissionObj["name"].toString();
                commission.value = commissionObj["value"].toDouble();
                commission.transactionType = commissionObj["transactionType"].toInt();
                adData.commissions.append(commission);

                // Add the commission to the new commissions array
                QJsonObject newCommissionObj;
                newCommissionObj["id"] = commission.id;
                newCommissionObj["type"] = commission.type;
                newCommissionObj["name"] = commission.name;
                newCommissionObj["value"] = commission.value;
                newCommissionObj["transactionType"] = commission.transactionType;
                newCommissionsArray.append(newCommissionObj);
            }

            newAdvertiserObj["commissions"] = newCommissionsArray;
            newAdvertisersArray.append(newAdvertiserObj);

            m_advertisers.insert(adData.programName, adData);  // Updating m_advertisers hash
        }

        // Create a new JSON document from the modified array and save it
        QJsonDocument newJsonDoc(newAdvertisersArray);
        dataManager->json->save(QString::number(channelID), newJsonDoc);
        emit advertisersUpdated(m_advertisers);
    } else {
        qWarning() << "Network request failed:" << reply->errorString();
    }
    reply->deleteLater();
}


void AdtractionAPI::deleteAdvertiser()
{
    QJsonDocument deleteDocument;
    dataManager->json->save("advertisersAdtraction", deleteDocument);
}

void AdtractionAPI::loadAdvertisersData()
{
    QJsonDocument jsonDoc = dataManager->json->load("advertisersAdtraction");
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

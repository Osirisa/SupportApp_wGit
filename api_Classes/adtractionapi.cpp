#include "api_Classes/adtractionapi.h"
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>

//Takes in the NetworkManager, the DataManager, the APIToken and the Parent to ensure being loaded the whole time
AdtractionAPI::AdtractionAPI(NetworkManager* networkManager, DataManager* dataManager, const QString& apiToken, QObject *parent)
    : QObject(parent), networkManager(networkManager), dataManager(dataManager), apiToken(apiToken)
{
    dataManager->registerFile("currenciesAdtraction","/dataAdtraction/currencies.txt");
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
        dataManager->txt->save("currencies", responseData);
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

    //If reply received a signal gets emited to the slot onAdvertisersRequestFinished
    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        onAdvertisersRequestFinished(reply);
    });
}

//gets called if the the signal from the reply gets emited. Takes in the reply
void AdtractionAPI::onAdvertisersRequestFinished(QNetworkReply* reply)
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
        dataManager->saveJsonToFile("advertisers.json", newJsonDoc);
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

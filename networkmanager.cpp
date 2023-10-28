#include "NetworkManager.h"
#include "qjsondocument.h"

NetworkManager::NetworkManager(QObject *parent)
    : QObject(parent), networkManager(new QNetworkAccessManager(this))
{
}

QNetworkReply* NetworkManager::sendGetRequest(const QString& endpoint, const QString& apiToken)
{
    QUrl url(endpoint);
    QNetworkRequest request(url);

    request.setRawHeader("X-Token", apiToken.toUtf8());
    request.setRawHeader("accept", "application/json");

    return networkManager->get(request);
}

QNetworkReply* NetworkManager::sendPostRequest(const QString& endpoint, const QString& apiToken, const QJsonObject& data)
{
    QUrl url(endpoint);
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json;charset=UTF-8");
    request.setRawHeader("X-Token", apiToken.toUtf8());  // Set the API token in the request header

    // Convert the QJsonObject to a QByteArray
    QJsonDocument jsonDoc(data);
    QByteArray jsonData = jsonDoc.toJson();

    // Send the request with the JSON object as the body
    return networkManager->post(request, jsonData);
}

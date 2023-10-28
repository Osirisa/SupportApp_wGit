#ifndef NETWORKMANAGER_H
#define NETWORKMANAGER_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>

class NetworkManager : public QObject
{
    Q_OBJECT
public:
    explicit NetworkManager(QObject *parent = nullptr);

    QNetworkReply* sendGetRequest(const QString& endpoint, const QString& apiToken);
    QNetworkReply* sendPostRequest(const QString& endpoint, const QString& apiToken, const QJsonObject& data);

signals:
    void requestFinished(QNetworkReply*);

private:
    QNetworkAccessManager* networkManager;
};

#endif

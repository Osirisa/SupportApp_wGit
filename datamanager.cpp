#include "datamanager.h"

DataManager::DataManager(QObject *parent) : QObject(parent)
{

}

void DataManager::saveToFile(const QString &filename, const QByteArray &data)
{
    QFile file(filename);
    if (file.open(QIODevice::WriteOnly)) {
        file.write(data);
        file.close();
    } else {
        qWarning() << "Unable to open file" << filename << "for writing:" << file.errorString();
    }
}

QByteArray DataManager::loadFromFile(const QString &filename)
{
    QFile file(filename);
    if (file.open(QIODevice::ReadOnly)) {
        QByteArray data = file.readAll();
        file.close();
        return data;
    } else {
        qWarning() << "Unable to open file" << filename << "for reading:" << file.errorString();
        return QByteArray();
    }
}

void DataManager::saveJsonToFile(const QString &filename, const QJsonDocument &jsonDoc)
{
    saveToFile(filename, jsonDoc.toJson());
}

QJsonDocument DataManager::loadJsonFromFile(const QString &filename)
{
    QByteArray data = loadFromFile(filename);
    return QJsonDocument::fromJson(data);
}

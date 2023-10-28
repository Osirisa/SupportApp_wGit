#ifndef DATAMANAGER_H
#define DATAMANAGER_H

#include <QObject>
#include <QJsonDocument>
#include <QFile>
#include <QTextStream>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDebug>

class DataManager : public QObject
{
    Q_OBJECT
public:
    explicit DataManager(QObject *parent = nullptr);

    void saveToFile(const QString &filename, const QByteArray &data);
    QByteArray loadFromFile(const QString &filename);

    void saveJsonToFile(const QString &filename, const QJsonDocument &jsonDoc);
    QJsonDocument loadJsonFromFile(const QString &filename);

};

#endif

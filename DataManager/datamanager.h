#ifndef DATAMANAGER_H
#define DATAMANAGER_H

#include <QObject>
#include <QJsonDocument>
#include <QFile>
#include <QFileInfo>
#include <QDir>
#include <QCoreApplication>
#include <QTextStream>
#include <QStringList>
#include <QMap>
#include <QStandardPaths>

/*--------------
 * Name: DataManager
 * Usage: To manage the storage and retrieval of different data formats (JSON, CSV, TXT).
 *-----------------*/
class DataManager : public QObject {
    Q_OBJECT

public:
    explicit DataManager(QObject *parent = nullptr);

    /*--------------
     * Name: JsonManager
     * Usage: Nested class to handle JSON operations.
     *-----------------*/
    class JsonManager {
    public:
        explicit JsonManager(DataManager *parent);
        void save(const QString &key, const QJsonDocument &jsonDoc);
        void append(const QString &key, const QJsonDocument &jsonDoc);
        QJsonDocument load(const QString &key);

    private:
        DataManager *parent;
    };

    /*--------------
     * Name: CsvManager
     * Usage: Nested class to handle CSV operations.
     *-----------------*/
    class CsvManager {
    public:
        explicit CsvManager(DataManager *parent);
        void save(const QString &key, const QList<QStringList> &data);
        QList<QStringList> load(const QString &key);

    private:
        DataManager *parent;
    };

    /*--------------
     * Name: TxtManager
     * Usage: Nested class to handle plain text operations.
     *-----------------*/
    class TxtManager {
    public:
        explicit TxtManager(DataManager *parent);
        void save(const QString &key, const QString &text);
        QString load(const QString &key);

    private:
        DataManager *parent;
    };

    // Registers a file with a unique key for later access.
    void registerFile(const QString &key, const QString &filename);
    bool hasKey(const QString &key) const;
    // Pointers to the nested class instances for handling JSON, CSV, and TXT.
    JsonManager *json;
    CsvManager *csv;
    TxtManager *txt;

private:
    QString standardAppDataPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);

    QMap<QString, QString> fileMap; // Map to associate keys with file names.

    // Converts a list of string lists to a CSV byte array.
    QByteArray convertListToCsv(const QList<QStringList> &list);
    // Converts a CSV byte array to a list of string lists.
    QList<QStringList> convertCsvToList(const QByteArray &csvData);

    // Saves data to a file.
    bool saveToFile(const QString &filename, const QByteArray &data);

    //Saves data to a file but wont override anything
    bool appendToFile(const QString &key, const QByteArray &data);


    // Loads data from a file.
    QByteArray loadFromFile(const QString &filename);
};

#endif // DATAMANAGER_H

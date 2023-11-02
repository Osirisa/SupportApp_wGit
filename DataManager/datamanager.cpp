#include "DataManager.h"

/*--------------
 * Name: DataManager
 * Usage: Constructor for the DataManager class.
 * Parameters: QObject *parent - parent object in the QObject hierarchy.
 * Return: N/A
 *-----------------*/
DataManager::DataManager(QObject *parent) : QObject(parent) {
    json = new JsonManager(this);
    csv = new CsvManager(this);
    txt = new TxtManager(this);
}

/*--------------
 * Name: registerFile
 * Usage: Registers a file with a unique key for later access.
 * Parameters:
 * - const QString &key: The unique key to identify the file.
 * - const QString &filename: The filename or path to be registered.
 * Return: void
 *-----------------*/
void DataManager::registerFile(const QString &key, const QString &filename) {
    fileMap[key] = filename;
}

/*--------------
 * Name: saveToFile
 * Usage: Saves data to a file associated with a key.
 * Parameters:
 * - const QString &filename: The filename or path where data will be saved.
 * - const QByteArray &data: The data to be saved in the file.
 * Return: void
 *-----------------*/
void DataManager::saveToFile(const QString &filename, const QByteArray &data) {
    QFile file(filename);
    if (file.open(QIODevice::WriteOnly)) {
        file.write(data);
        file.close();
    }
}

/*--------------
 * Name: loadFromFile
 * Usage: Loads data from a file associated with a key.
 * Parameters:
 * - const QString &filename: The filename or path from where data will be loaded.
 * Return: QByteArray: The data loaded from the file.
 *-----------------*/
QByteArray DataManager::loadFromFile(const QString &filename) {
    QFile file(filename);
    QByteArray data;
    if (file.open(QIODevice::ReadOnly)) {
        data = file.readAll();
        file.close();
    }
    return data;
}

/*--------------
 * Name: convertListToCsv
 * Usage: Converts a list of string lists to a CSV formatted QByteArray.
 * Parameters:
 * - const QList<QStringList> &list: The list of string lists to be converted to CSV.
 * Return: QByteArray: The CSV formatted data as a byte array.
 *-----------------*/
QByteArray DataManager::convertListToCsv(const QList<QStringList> &list) {
    QByteArray data;
    QTextStream out(&data);
    for (const QStringList &row : list) {
        out << row.join(",") << "\n";
    }
    return data;
}

/*--------------
 * Name: convertCsvToList
 * Usage: Converts a CSV formatted QByteArray to a list of string lists.
 * Parameters:
 * - const QByteArray &csvData: The CSV formatted data as a byte array.
 * Return: QList<QStringList>: The list of string lists obtained from the CSV data.
 *-----------------*/
QList<QStringList> DataManager::convertCsvToList(const QByteArray &csvData) {
    QList<QStringList> list;
    QTextStream in(csvData);
    while (!in.atEnd()) {
        QString line = in.readLine();
        list.append(line.split(","));
    }
    return list;
}


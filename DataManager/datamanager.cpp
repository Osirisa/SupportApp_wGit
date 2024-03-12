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

    QDir dir(standardAppDataPath);
    if (!dir.exists()) {
        dir.mkpath(standardAppDataPath);
    }
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
 * Name: hasKey
 * Usage: Checks if a given key is already present in the file map.
 * Parameters:
 * - const QString &key: The unique key to check in the file map.
 * Return: bool: Returns true if the key exists; otherwise, false.
 *-----------------*/
bool DataManager::hasKey(const QString &key) const {
    return fileMap.contains(key);
}

/*--------------
 * Name: saveToFile
 * Usage: Saves data to a file associated with a key. It creates the directory path if it does not exist.
 *       The file is saved relative to the executable's directory.
 * Parameters:
 * - const QString &filename: The filename or path where data will be saved.
 * - const QByteArray &data: The data to be saved in the file.
 * Return: bool: Returns true if the data was successfully saved; otherwise, false.
 *-----------------*/
bool DataManager::saveToFile(const QString &key, const QByteArray &data) {

    QString filePath = fileMap[key]; // Assume fileMap contains an absolute path


    if (!QFileInfo(filePath).isAbsolute()) {
        // If not an absolute path, then prepend the executable's directory
        filePath = standardAppDataPath + "/" + filePath;
        //qDebug()<<filePath;
    }

    QFileInfo fileInfo(filePath);
    QDir dir;
    // Check if the directory exists, create it if it doesn't
    if (!dir.exists(fileInfo.dir().absolutePath())) {
        dir.mkpath(fileInfo.dir().absolutePath());
        //qDebug()<<"Test";
    }
    else{
        qDebug()<< fileInfo.dir().absolutePath();
    }

    QFile file(filePath);
    // Attempt to open the file and write the data
    if (file.open(QIODevice::WriteOnly)) {
        if (file.write(data) != -1) {
            file.close();
            return true; // Data was successfully saved
        }
        file.close();
    }

    return false; // Return false if unable to save the data
}

bool DataManager::appendToFile(const QString &key, const QByteArray &data) {
    QString filePath = standardAppDataPath + "/" + fileMap[key]; // Construct the absolute file path

    QFile file(filePath);
    QFileInfo fileInfo(file.fileName());
    QDir dir;

    // Check if the directory exists, create it if it doesn't
    if (!dir.exists(fileInfo.dir().absolutePath())) {
        dir.mkpath(fileInfo.dir().absolutePath());
    }

    // Attempt to open the file in append mode
    if (file.open(QIODevice::Append)) {
        if (file.write(data) != -1) {
            file.close();
            return true; // Data was successfully appended
        }
        file.close(); // Close the file if append failed
    }

    return false; // Return false if unable to append the data
}

/*--------------
 * Name: loadFromFile
 * Usage: Loads data from a file associated with a key.
 * Parameters:
 * - const QString &filename: The filename or path from where data will be loaded.
 * Return: QByteArray: The data loaded from the file.
 *-----------------*/
QByteArray DataManager::loadFromFile(const QString &key) {

    QString filePath = standardAppDataPath + "/" + fileMap[key]; // Construct the absolute file path

    QFile file(filePath);
    if (!file.exists()) {
        qDebug() << "File does not exist:" << filePath;
        return QByteArray();
    }
    if (!file.open(QIODevice::ReadOnly)) {
        qDebug() << "Unable to open file:" << filePath;
        return QByteArray();
    }
    QByteArray data = file.readAll();
    file.close();
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

/*--------------
 * JsonManager Class Implementation
 *-----------------*/

DataManager::JsonManager::JsonManager(DataManager *parent) : parent(parent) {}

void DataManager::JsonManager::save(const QString &key, const QJsonDocument &jsonDoc) {
    parent->saveToFile(key, jsonDoc.toJson());
}

void DataManager::JsonManager::append(const QString &key, const QJsonDocument &jsonDoc){
    parent->appendToFile(key, jsonDoc.toJson());
}

QJsonDocument DataManager::JsonManager::load(const QString &key) {
    QByteArray jsonData = parent->loadFromFile(key);
    return QJsonDocument::fromJson(jsonData);
}

/*--------------
 * CsvManager Class Implementation
 *-----------------*/

DataManager::CsvManager::CsvManager(DataManager *parent) : parent(parent) {}

void DataManager::CsvManager::save(const QString &key, const QList<QStringList> &data) {
    QByteArray csvData = parent->convertListToCsv(data);
    parent->saveToFile(key, csvData);
}

QList<QStringList> DataManager::CsvManager::load(const QString &key) {
    QByteArray csvData = parent->loadFromFile(key);
    return parent->convertCsvToList(csvData);
}

/*--------------
 * TxtManager Class Implementation
 *-----------------*/

DataManager::TxtManager::TxtManager(DataManager *parent) : parent(parent) {}

void DataManager::TxtManager::save(const QString &key, const QString &text) {
    QByteArray textData = text.toUtf8();
    parent->saveToFile(key, textData);
}

QString DataManager::TxtManager::load(const QString &key) {
    QByteArray textData = parent->loadFromFile(key);
    return QString::fromUtf8(textData);

}

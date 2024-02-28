#include "setapikey.h"
#include "ui_setapikey.h"

SetAPIKey::SetAPIKey(DataManager* dataManager, EncryptionHelper* encryptionHelper,QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SetAPIKey),
    dataManager(dataManager),
    encryptionHelper(encryptionHelper)
{

    ui->setupUi(this);
}

SetAPIKey::~SetAPIKey()
{
    delete ui;
}
void SetAPIKey::saveAPIKey(const QByteArray &apiKey)
{
    QByteArray encryptedKey = encryptionHelper->encrypt(apiKey, "hello");

    // Convert the encrypted data to Base64 for safe storage
    QByteArray base64EncryptedKey = encryptedKey.toBase64();
    qDebug() << base64EncryptedKey;

    // Save the Base64 encoded encrypted key
    dataManager->txt->save("AdtractionKey", base64EncryptedKey);

    emit apiKeyChanged(apiKey);
}

QString SetAPIKey::loadAPIKey()
{
    // Load the encrypted API key
    QByteArray encryptedData = dataManager->txt->load("AdtractionKey").toUtf8();

    // Assuming the data is stored in Base64 encoding
    QByteArray decodedData = QByteArray::fromBase64(encryptedData);

    // Decrypt the API key
    QString decryptedKey = encryptionHelper->decrypt(decodedData, "hello");
    return decryptedKey;
}

void SetAPIKey::FillLE(){
    QString key = loadAPIKey();
    ui->LE_APIKey->setText(key);
}

void SetAPIKey::on_PB_Cancel_clicked()
{
    hide();
}


void SetAPIKey::on_PB_Save_clicked()
{
    saveAPIKey(ui->LE_APIKey->text().toUtf8());
    hide();
}

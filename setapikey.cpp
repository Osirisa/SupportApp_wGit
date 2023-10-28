#include "setapikey.h"
#include "ui_setapikey.h"

SetAPIKey::SetAPIKey(DataManager* dataManager, EncryptionHelper* encryptionHelper,QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SetAPIKey),
    dataManager(dataManager),
    encryptionHelper(encryptionHelper)
{

    ui->setupUi(this);
    ui->LE_APIKey->setText(loadAPIKey());
}

SetAPIKey::~SetAPIKey()
{
    delete ui;
}

const QString SetAPIKey::loadAPIKey()
{

    QString decryptedKey = encryptionHelper->encryptDecrypt(dataManager->loadFromFile("apiKey.txt"));

    return decryptedKey;
}

void SetAPIKey::on_PB_Cancel_clicked()
{
    hide();
}


void SetAPIKey::on_PB_Save_clicked()
{
    saveAPIKey(ui->LE_APIKey->text());
    hide();
}

void SetAPIKey::saveAPIKey(const QString &apiKey)
{
    QString encryptedKey = encryptionHelper->encryptDecrypt(apiKey);
    dataManager->saveToFile("apiKey.txt",encryptedKey.toUtf8());

    emit apiKeyChanged(apiKey);
}

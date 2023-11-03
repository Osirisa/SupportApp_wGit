#include "setapikey.h"
#include "ui_setapikey.h"

SetAPIKey::SetAPIKey(DataManager* dataManager, EncryptionHelper* encryptionHelper,QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SetAPIKey),
    dataManager(dataManager),
    encryptionHelper(encryptionHelper)
{
        dataManager->registerFile("adtractionKey","dataAdtraction/adtractionKey.txt");

    ui->setupUi(this);
}

SetAPIKey::~SetAPIKey()
{
    delete ui;
}

QString SetAPIKey::loadAPIKey()
{
    QString decryptedKey = encryptionHelper->encryptDecrypt(dataManager->txt->load("adtractionKey"));
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
    saveAPIKey(ui->LE_APIKey->text());
    hide();
}
void SetAPIKey::saveAPIKey(const QString &apiKey)
{
    QString encryptedKey = encryptionHelper->encryptDecrypt(apiKey);
    dataManager->txt->save("adtractionKey",encryptedKey.toUtf8());

//    emit apiKeyChanged(apiKey);
}

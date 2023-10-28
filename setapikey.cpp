#include "setapikey.h"
#include "ui_setapikey.h"

SetAPIKey::SetAPIKey(DataManager* dataManager, EncryptionHelper* encryptionHelper,QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SetAPIKey)
{
    ui->setupUi(this);
}

SetAPIKey::~SetAPIKey()
{
    delete ui;
}

void SetAPIKey::on_PB_Cancel_clicked()
{
    hide();
}


void SetAPIKey::on_PB_Save_clicked()
{
    ui->LE_APIKey;
}


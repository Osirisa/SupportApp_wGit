#include "setapikey.h"
#include "ui_setapikey.h"

SetAPIKey::SetAPIKey(QString network, DataManager* dataManager, EncryptionHelper* encryptionHelper,QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SetAPIKey),
    dataManager(dataManager),
    encryptionHelper(encryptionHelper),
    network(network)
{

    ui->setupUi(this);
    initUi();
}

SetAPIKey::~SetAPIKey()
{
    delete ui;
}

void SetAPIKey::initUi()
{
    this->setWindowTitle(network + " API Token");

    if(network == "Adcell"){

    }
    else if(network == "Adtraction"){

        this->setWindowIcon(QIcon(QPixmap(":/Logos/img/adtractionLogo_trans.png")));
    }
    else if(network == "Awin"){

        this->setWindowIcon(QIcon(QPixmap(":/Logos/img/AwinLogo_trans.png")));
    }

    fillKeyText();
}



void SetAPIKey::saveAPIKey(const QByteArray &apiKey)
{

    QByteArray encryptedKey = encryptionHelper->encrypt(apiKey, "hello");

    // Convert the encrypted data to Base64 for safe storage
    QByteArray base64EncryptedKey = encryptedKey.toBase64();
    //qDebug() << base64EncryptedKey;

    if(network == "Adcell"){

    }
    else if(network == "Adtraction"){

        dataManager->txt->save("AdtractionKey", base64EncryptedKey);
    }
    else if(network == "Awin"){

        dataManager->txt->save("AwinKey", base64EncryptedKey);
    }

    // Save the Base64 encoded encrypted key

    emit apiKeyChanged(apiKey);
}

QString SetAPIKey::loadAPIKey()
{
    QByteArray encryptedData;

    // Load the encrypted API key from the selected API
    if(network == "Adcell"){

    }
    else if(network == "Adtraction"){
        encryptedData = dataManager->txt->load("AdtractionKey").toUtf8();
    }
    else if(network == "Awin"){
        encryptedData = dataManager->txt->load("AwinKey").toUtf8();
    }



    // Assuming the data is stored in Base64 encoding
    QByteArray decodedData = QByteArray::fromBase64(encryptedData);

    // Decrypt the API key
    QString decryptedKey = encryptionHelper->decrypt(decodedData, "hello");
    return decryptedKey;
}

void SetAPIKey::fillKeyText(){
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


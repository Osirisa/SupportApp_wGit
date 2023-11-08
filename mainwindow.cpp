#include "mainwindow.h"
#include "ui_mainwindow.h"

//-------------Constructor----------------
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //TBD: OPENSSL Library Import -> autogenerating KEY Or work with server/password
    encryptionHelper = new EncryptionHelper("End,aw..daw;d");

    dataManager = new DataManager(this);
    apiManager = new APIManager(dataManager,encryptionHelper);


    //Windows
    SetAPIKeyWindow = new SetAPIKey(dataManager,encryptionHelper, this);
    networkChannelWindow = new NetworkChannels(dataManager,this);



    //TBD: FileManager dataBank or soemthing like that
    dataManager->registerFile("NetworkChannels","Admin/Networkchannels.csv");
    dataManager->registerFile("currenciesAdtraction","dataAdtraction/currencies.txt");
    dataManager->registerFile("advertisersAdtraction","dataAdtraction/advertisersAdtraction.json");
    dataManager->registerFile("adtractionKey","dataAdtraction/adtractionKey.txt");
}

//-------------Destructor----------------
MainWindow::~MainWindow()
{
    delete SetAPIKeyWindow;
    delete dataManager;
    delete apiManager;
    delete encryptionHelper;
    delete ui;
}

//Public Slots
void MainWindow::updateApiKey(const QString &newKey)
{

}

//Private Slots
void MainWindow::on_actionAPI_Key_triggered()
{
    //Opens teh Set APIKEY window
    SetAPIKeyWindow->setModal(true);
    SetAPIKeyWindow->FillLE();
    SetAPIKeyWindow->exec();
}

void MainWindow::on_actionUpdate_Shops_triggered()
{
    if(apiManager->adtraction){
        apiManager->adtraction->updater.allChannels();
    }
    else{
        qDebug()<<"not init";
    }
}


void MainWindow::on_actionAdd_ChannelIds_triggered()
{
    networkChannelWindow->setModal(true);
    networkChannelWindow->exec();
}


void MainWindow::on_actionChannelIDs_triggered()
{
    networkChannelWindow->setModal(true);
    networkChannelWindow->exec();
}


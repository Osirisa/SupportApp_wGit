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
    suppPage = new P_SupportPage(dataManager,apiManager,this);

    //Signal
    connect(SetAPIKeyWindow,SIGNAL(apiKeyChanged(QString)),this,SLOT(updateApiKey(QString)));
    connect(networkChannelWindow,SIGNAL(onNetworkDataSaved()),this,SLOT(updateNetworks()));

    ui->stackedWidget->addWidget(suppPage);
    ui->stackedWidget->setCurrentWidget(suppPage);

    //TBD: FileManager dataBank or soemthing like that
    dataManager->registerFile("NetworkChannels","Admin/Networkchannels.csv");
    dataManager->registerFile("currenciesAdtraction","dataAdtraction/currenciesAdtraction.txt");
    dataManager->registerFile("adtractionKey","dataAdtraction/adtractionKey.txt");
    dataManager->registerFile("NetworkSuppAnswers","Admin/NetworkSuppAnswers.json");

    updateNetworks();
   // ui->stackedWidget->addWidget();
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
    apiManager->refreshApiManager();
}

void MainWindow::updateNetworks()
{

    QList<QStringList> csvData = dataManager->csv->load("NetworkChannels");

    for (const QStringList &rowData : csvData) {
        dataManager->registerFile(QString(rowData.at(2)),"dataAdtraction/"+QString(rowData.at(2)+".json"));
        qDebug()<<QString(rowData.at(2));
    }
    suppPage->refreshNetworkList();
}

void MainWindow::forwardSuppMessage()
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
    QList<QStringList> csvData = dataManager->csv->load("NetworkChannels");

    for (const QStringList &rowData : csvData) {
        apiManager->adtraction->updater.byChannel(rowData.at(2).toInt());
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


void MainWindow::on_actionUpdate_Currencies_triggered()
{
    apiManager->adtraction->updater.currencies();
}


void MainWindow::on_actionUpdate_All_triggered()
{

}


#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "suppeventbus.h"

//-------------Constructor----------------
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setWindowTitle("Network Supporthelper");
    //TBD: OPENSSL Library Import -> autogenerating KEY Or work with server/password
    encryptionHelper = new EncryptionHelper("End,aw..daw;d");

    dataManager = new DataManager(this);

    //TBD: FileManager dataBank or soemthing like that
    dataManager->registerFile("NetworkChannels","Admin/Networkchannels.csv");
    dataManager->registerFile("currenciesAdtraction","dataAdtraction/currenciesAdtraction.txt");
    dataManager->registerFile("adtractionKey","dataAdtraction/adtractionKey.txt");
    dataManager->registerFile("NetworkSuppAnswers","Admin/NetworkSuppAnswers.json");

    apiManager = new APIManager(dataManager,encryptionHelper);


    //Windows
    SetAPIKeyWindow = new SetAPIKey(dataManager,encryptionHelper, this);
    networkChannelWindow = new NetworkChannels(dataManager,this);
    adtractionSuppPage = new P_SupportPageAdtraction(dataManager,apiManager,this);

    //Signal
    connect(SetAPIKeyWindow,SIGNAL(apiKeyChanged(QString)),this,SLOT(updateApiKey(QString)));
    connect(networkChannelWindow,SIGNAL(onNetworkDataSaved()),this,SLOT(updateNetworks()));

    ui->stackedWidget->addWidget(adtractionSuppPage);
    ui->stackedWidget->setCurrentWidget(adtractionSuppPage);


    QObject::connect(SuppEventBus::instance(), &SuppEventBus::eventPublished, [this](const QString& eventName) {
        if (eventName == "shopsUpdated") {

            //qDebug() << "Received networkResponse:" << responseCode;
            adtractionSuppPage->refreshShops();

        }
    });
    updateNetworks();

    initUI();
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
    adtractionSuppPage->refreshNetworkList();
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

void MainWindow::initUI()
{
    //General Buttons
    ui->PB_Home_Page->setIcon(QIcon(":/img/img/Haus_trans.png"));
    ui->PB_settings_Page->setIcon(QIcon(":/img/img/Zahnrad_trans.png"));
    ui->PB_statistics_Page->setIcon(QIcon(":/img/img/statistics_color_Trans.png"));
    ui->PB_networkStatus_Page->setIcon(QIcon(":/img/img/networkstatusPic__colortrans.png"));
    ui->PB_archiv_Page->setIcon(QIcon(":/img/img/Buecher_trans.png"));

    //NetworkButtons
    ui->PB_adcell_suppPageBTN->setIcon(QIcon(":/Logos/img/AdcellLogo_trans.png"));
    ui->PB_adtraction_suppPage->setIcon(QIcon(":/Logos/img/adtractionLogo_trans.png"));
    ui->PB_awin_suppPage->setIcon(QIcon(":/Logos/img/AwinLogo_trans.png"));
    ui->PB_cj_suppPage->setIcon(QIcon(":/Logos/img/CJ_Logo.png"));
    ui->PB_tradeDoubler_suppPage->setIcon(QIcon(":/Logos/img/tradeDoublerLogo_trans.png"));
    ui->PB_webgains_suppPage->setIcon(QIcon(":/Logos/img/WebGainsLogo.png"));

}


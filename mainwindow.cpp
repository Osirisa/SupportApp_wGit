#include "mainwindow.h"
#include "qjsonarray.h"
#include "qjsonobject.h"
#include "ui_mainwindow.h"
#include <QMessageBox>
#include <QProcess>
#include "suppeventbus.h"
#include "DataManager/standardfiles.h"

//-------------Constructor----------------
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setWindowTitle("Network Supporthelper");
    //TBD: OPENSSL Library Import -> autogenerating KEY Or work with server/password
    encryptionHelper = new EncryptionHelper();

    dataManager = new DataManager(this);

    StandardFiles standardFiles(dataManager);

    apiManager = new APIManager(dataManager,encryptionHelper);

    //Windows
    networkChannelWindow    = new NetworkChannels(dataManager,this);

    //Pages
    adtractionSuppPage      = new P_SupportPageAdtraction(dataManager,apiManager,this);
    awinSuppPage            = new p_SupportPageAwin(dataManager,apiManager,this);

    //Signal
    connect(networkChannelWindow,SIGNAL(onNetworkDataSaved()),this,SLOT(updateNetworks()));

    ui->stackedWidget->addWidget(adtractionSuppPage);
    ui->stackedWidget->addWidget(awinSuppPage);

    //TBD implement HomePage
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

    delete adtractionSuppPage;
    delete awinSuppPage;
}

//Public Slots
void MainWindow::updateApiKey(const QString& newKey)
{
    apiManager->refreshApiManager();
}

void MainWindow::updateNetworks()
{
    QJsonDocument networksDoc = dataManager->json->load("NetworkChannels");
    QJsonObject networksObj = networksDoc.object();

    for (auto network = networksObj.begin(); network != networksObj.end(); ++network) {
        // Each network's key is the network name, and its value is another JSON object containing channels
        QJsonObject channelsObj = network.value().toObject();

        // Iterate through each channel in the network
        for (auto channel = channelsObj.begin(); channel != channelsObj.end(); ++channel) {
            // Each channel's key is the channel ID, and its value is another JSON object containing channel details
            QJsonObject channelDetails = channel.value().toObject();

            // Extract channel details
            QString channelID = channel.key();
            dataManager->registerFile(network.key()+channelID,"dataAdtraction/"+ network.key()+channelID +".json");
        }
    }

    adtractionSuppPage->refreshNetworkList();
}

//Private Slots


void MainWindow::on_actionUpdate_Shops_triggered()
{
    QJsonDocument networksDoc = dataManager->json->load("NetworkChannels");
    QJsonObject networksObj = networksDoc.object();

    QJsonDocument marketRegionsAdtraction = dataManager->json->load("AdtractionRegions");

    QJsonArray marketArray = marketRegionsAdtraction.array();


    QString marketShort;

    for (auto network = networksObj.begin(); network != networksObj.end(); ++network) {
        // Each network's key is the network name, and its value is another JSON object containing channels
        QJsonObject channelsObj = network.value().toObject();

        // Iterate through each channel in the network
        for (auto channel = channelsObj.begin(); channel != channelsObj.end(); ++channel) {
            // Each channel's key is the channel ID, and its value is another JSON object containing channel details
            QJsonObject channelDetails = channel.value().toObject();

            // Extract channel details
            QString channelID = channel.key();
            QString channelName = channelDetails["channelName"].toString();
            QString channelRegion = channelDetails["channelRegion"].toString();

            for(int i = 0; i< marketArray.size(); ++i){
                QJsonObject marketObject = marketArray[i].toObject();

                if(channelRegion == marketObject["marketName"].toString()){
                    marketShort = marketObject["market"].toString();
                    break;
                }
            }

            apiManager->adtraction->updater.byChannel(channelID.toInt(),marketShort);
        }
    }
}


void MainWindow::on_actionAdd_ChannelIds_triggered()
{
    updateRegions();
    networkChannelWindow->setModal(true);
    networkChannelWindow->exec();

}


void MainWindow::on_actionChannelIDs_triggered()
{
    updateRegions();
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

    //General Buttons
    ui->PB_Home_Page->setDisabled(true);
    ui->PB_settings_Page->setDisabled(true);
    ui->PB_statistics_Page->setDisabled(true);
    ui->PB_networkStatus_Page->setDisabled(true);
    ui->PB_archiv_Page->setDisabled(true);

    //NetworkButtons
    ui->PB_adcell_suppPageBTN->setDisabled(true);
    ui->PB_awin_suppPage->setDisabled(false);
    ui->PB_cj_suppPage->setDisabled(true);
    ui->PB_tradeDoubler_suppPage->setDisabled(true);
    ui->PB_webgains_suppPage->setDisabled(true);
}

void MainWindow::updateRegions()
{
    apiManager->adtraction->updater.regions();
    apiManager->adtraction->getter.listChannels();
}


void MainWindow::on_actionUpdate_triggered()
{
    checkForApplicationUpdates();
}

void MainWindow::checkForApplicationUpdates()
{
    QUrl versionUrl("https://osirisa.github.io/updates/updatesSuppHelper/extra/currentVersion.txt");
    QNetworkAccessManager* manager = new QNetworkAccessManager(this);
    connect(manager, &QNetworkAccessManager::finished, this, &MainWindow::onVersionCheckFinished);
    manager->get(QNetworkRequest(versionUrl));
}

void MainWindow::onVersionCheckFinished(QNetworkReply* reply)
{
    if (reply->error() == QNetworkReply::NoError) {
        QString latestVersion = QString::fromUtf8(reply->readAll().trimmed());
        QString currentVersion = QApplication::applicationVersion();

        if (latestVersion > currentVersion) {
            // New version is available
            //QMessageBox::information(this,"Current Version is up to Date","The Current Version is up to Date! current vers:" + currentVersion + "latest vers:" +latestVersion);
            launchMaintenanceTool();

        } else {
            // No new version available
            QMessageBox::information(this,"Current Version is up to Date","The Current Version is up to Date!");
        }
    }
    reply->deleteLater();
}

void MainWindow::launchMaintenanceTool()
{
    QString maintenanceToolPath = QCoreApplication::applicationDirPath() + QDir::separator() + "maintenancetool";

#ifdef Q_OS_WIN
    maintenanceToolPath += ".exe";
#endif

    if (!QFile::exists(maintenanceToolPath)) {
        qDebug() << "Maintenance Tool does not exist.";
        return;
    }

    QStringList arguments;
    arguments << "--updater";

    QProcess::startDetached(maintenanceToolPath, arguments);
}


void MainWindow::on_PB_adtraction_suppPage_clicked()
{
    ui->stackedWidget->setCurrentWidget(adtractionSuppPage);
}


void MainWindow::on_PB_awin_suppPage_clicked()
{
    ui->stackedWidget->setCurrentWidget(awinSuppPage);
}


void MainWindow::on_actionAPI_Key_adtraction_triggered()
{
    //Opens teh Set APIKEY window
    SetAPIKeyWindow = new SetAPIKey("Adtraction", dataManager,encryptionHelper, this);
    connect(SetAPIKeyWindow,SIGNAL(apiKeyChanged(QString)),this,SLOT(updateApiKey(QString)));
    SetAPIKeyWindow->setModal(true);
    SetAPIKeyWindow->exec();
}


void MainWindow::on_actionAPI_Key_awin_triggered()
{
    SetAPIKeyWindow = new SetAPIKey("Awin", dataManager,encryptionHelper, this);
    connect(SetAPIKeyWindow,SIGNAL(apiKeyChanged(QString)),this,SLOT(updateApiKey(QString)));
    SetAPIKeyWindow->setModal(true);
    SetAPIKeyWindow->exec();
}


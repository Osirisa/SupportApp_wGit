#include "mainwindow.h"
#include "ui_mainwindow.h"

//-------------Constructor----------------
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    dataManager = new DataManager(this);
    networkManager = new NetworkManager(this);
    adtractionApi = new AdtractionAPI(networkManager, dataManager, SetAPIKeyWindow->loadAPIKey(),this);

    //TBD: KEY autogenerated on first opening and gets loaded via File (with second encryption
    EncryptionHelper* encryptionHelper = new EncryptionHelper("End.T9@c;b!keLM");
    SetAPIKeyWindow = new SetAPIKey(dataManager,encryptionHelper, this);
}

//-------------Destructor----------------
MainWindow::~MainWindow()
{
    delete SetAPIKeyWindow;
    delete dataManager;
    delete networkManager;
    delete adtractionApi;
    delete ui;
}

//Public Slots
void MainWindow::updateApiKey(const QString &newKey)
{
    if (adtractionApi)
    {
        delete adtractionApi;
    }
    adtractionApi = new AdtractionAPI(networkManager, dataManager, newKey, this);
}

//Private Slots
void MainWindow::on_actionAPI_Key_triggered()
{
    //Opens teh Set APIKEY window
    SetAPIKeyWindow->setModal(true);
    SetAPIKeyWindow->exec();
}

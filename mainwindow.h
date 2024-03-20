#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "setapikey.h"
#include "apimanager.h"
#include "networkchannels.h"

#include "supportPages/p_supportpageAdtraction.h"
#include "supportPages/p_supportpageawin.h"


QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public slots:
    void updateApiKey(const QString& newKey);
    void updateNetworks();

private slots:

    void on_actionUpdate_Shops_triggered();
    void on_actionAdd_ChannelIds_triggered();
    void on_actionChannelIDs_triggered();
    void on_actionUpdate_Currencies_triggered();
    void on_actionUpdate_All_triggered();
    void on_actionUpdate_triggered();
    void onVersionCheckFinished(QNetworkReply* reply);

    void on_PB_adtraction_suppPage_clicked();
    void on_PB_awin_suppPage_clicked();

    void on_actionAPI_Key_adtraction_triggered();
    void on_actionAPI_Key_awin_triggered();

private:
    Ui::MainWindow *ui;


    void checkForApplicationUpdates();
    void launchMaintenanceTool();
    void initUI();
    void updateRegions();

    //Windows
    SetAPIKey* SetAPIKeyWindow;
    NetworkChannels* networkChannelWindow;
    P_SupportPageAdtraction* adtractionSuppPage;
    p_SupportPageAwin* awinSuppPage;


    //GeneralClasses
    DataManager* dataManager;
    EncryptionHelper* encryptionHelper;

    //API Class
    APIManager* apiManager;
};
#endif // MAINWINDOW_H

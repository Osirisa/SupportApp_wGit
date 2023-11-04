#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "setapikey.h"
#include "apimanager.h"
#include "networkchannels.h"


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
    void updateApiKey(const QString &newKey);

private slots:
    void on_actionAPI_Key_triggered();

    void on_actionUpdate_Shops_triggered();

    void on_actionAdd_ChannelIds_triggered();

private:
    Ui::MainWindow *ui;

    SetAPIKey* SetAPIKeyWindow;
    NetworkChannels* networkChannelWindow;

    DataManager* dataManager;
    EncryptionHelper* encryptionHelper;

    APIManager* apiManager;
};
#endif // MAINWINDOW_H

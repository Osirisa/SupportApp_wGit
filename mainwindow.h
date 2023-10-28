#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "setapikey.h"
#include "networkmanager.h"
#include "adtractionapi.h"


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

private:
    Ui::MainWindow *ui;
    SetAPIKey* SetAPIKeyWindow;
    NetworkManager* networkManager;
    DataManager* dataManager;
    AdtractionAPI* adtractionApi;
};
#endif // MAINWINDOW_H

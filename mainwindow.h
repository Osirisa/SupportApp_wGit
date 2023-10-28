#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "setapikey.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_actionAPI_Key_triggered();

private:
    Ui::MainWindow *ui;
    SetAPIKey* SetAPIKeyWindow;
};
#endif // MAINWINDOW_H

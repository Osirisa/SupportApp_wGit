#include "mainwindow.h"
#include "ui_mainwindow.h"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    SetAPIKeyWindow = new SetAPIKey;
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_actionAPI_Key_triggered()
{
    SetAPIKeyWindow->setModal(true);
    SetAPIKeyWindow->exec();
}


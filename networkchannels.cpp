#include "NetworkChannels.h"
#include "ui_NetworkChannels.h"

NetworkChannels::NetworkChannels(DataManager *dataManager, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::NetworkChannels),
    dataManager(dataManager)
{
    ui->setupUi(this);

    // Connect the buttons to their respective slot functions
    connect(ui->PB_NWC_Add, &QPushButton::clicked, this, &NetworkChannels::on_addButton_clicked);
    connect(ui->PB_NWC_Save, &QPushButton::clicked, this, &NetworkChannels::on_saveButton_clicked);
}

NetworkChannels::~NetworkChannels() {
    delete ui;
}

void NetworkChannels::on_addButton_clicked() {
    // Check if any of the LineEdits are empty
    if (ui->LE_NWC_ChannelName->text().isEmpty() || ui->LE_NWC_ChannelID->text().isEmpty()) {
        // Handle the empty case here if needed
        return;
    }

    int rowCount = ui->T_NWC_NetworkChannels->rowCount();
    ui->T_NWC_NetworkChannels->setRowCount(rowCount + 1);

    QTableWidgetItem *items [] = {
        new QTableWidgetItem(QString(ui->CB_NWC_Network->currentText())),
        new QTableWidgetItem(QString(ui->LE_NWC_ChannelName->text())),
        new QTableWidgetItem(QString(ui->LE_NWC_ChannelID->text())),

        new QTableWidgetItem
    };

    const size_t count = sizeof(items) / sizeof(QTableWidgetItem*);
    for(size_t column = 0; column < count; column++){
        ui->T_NWC_NetworkChannels->setItem(rowCount,column,items[column]);
    }

    QPushButton *deleteBTN = new QPushButton;
    deleteBTN->setText("X");
    deleteBTN->setStyleSheet("QPushButton { color: red; }");
    ui->T_NWC_NetworkChannels->setCellWidget(rowCount,3,deleteBTN);

    QObject::connect(deleteBTN, &QPushButton::clicked, this, &NetworkChannels::on_deleteBTN_clicked);
}

void NetworkChannels::on_saveButton_clicked() {
    //saveChannelsToCsv();
    hide();
}

void NetworkChannels::saveChannelsToCsv() {
    // Your code to save data from tableWidget to CSV using DataManager
    // ...
}
void NetworkChannels::on_deleteBTN_clicked(){

    ui->T_NWC_NetworkChannels->removeRow(ui->T_NWC_NetworkChannels->currentRow());

}

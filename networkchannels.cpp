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

    // Add items to the table
    int row = ui->T_NWC_NetworkChannels->rowCount();
    ui->T_NWC_NetworkChannels->insertRow(row);
    ui->T_NWC_NetworkChannels->setItem(row, 0, new QTableWidgetItem(ui->CB_NWC_Network->currentText()));
    ui->T_NWC_NetworkChannels->setItem(row, 1, new QTableWidgetItem(ui->LE_NWC_ChannelName->text()));
    ui->T_NWC_NetworkChannels->setItem(row, 2, new QTableWidgetItem(ui->LE_NWC_ChannelID->text()));
}

void NetworkChannels::on_saveButton_clicked() {
    //saveChannelsToCsv();
    hide();
}

void NetworkChannels::saveChannelsToCsv() {
    // Your code to save data from tableWidget to CSV using DataManager
    // ...
}

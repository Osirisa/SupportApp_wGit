#include "networkchannels.h"
#include "ui_networkchannels.h"

NetworkChannels::NetworkChannels(DataManager *dataManager, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::NetworkChannels),
    dataManager(dataManager)
{
    ui->setupUi(this);

    // Connect the buttons to their respective slot functions
    connect(ui->PB_NWC_Add, &QPushButton::clicked, this, &NetworkChannels::on_addButton_clicked);
    connect(ui->PB_NWC_Save, &QPushButton::clicked, this, &NetworkChannels::on_saveButton_clicked);

    fillComboBox();

    QTimer::singleShot(300, this, &NetworkChannels::updateTableWidget);
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

    sortTableWidget();
}

void NetworkChannels::on_saveButton_clicked() {
    saveChannelsToCsv();
    hide();
}

void NetworkChannels::saveChannelsToCsv() {

    QList<QStringList> csvData;

    for(int row = 0; row < ui->T_NWC_NetworkChannels->rowCount();++row){
        QStringList rowData;
        for (int column = 0; column< (ui->T_NWC_NetworkChannels->columnCount()-1);++column){
            QTableWidgetItem *item = ui->T_NWC_NetworkChannels->item(row,column);
            rowData <<(item ? item->text() : QString());

        }
        csvData.append(rowData);
    }

    dataManager->csv->save("NetworkChannels", csvData);
}

void NetworkChannels::fillComboBox()
{
    // Create a QStringList with the given items
    QStringList channels = {"Adtraction", "AWIN", "Webgains", "CJ", "TradeDoubler", "Adcell"};

    // Sort the list alphabetically
    std::sort(channels.begin(), channels.end());

    // Assuming 'myComboBox' is a pointer to your QComboBox
    ui->CB_NWC_Network->clear(); // Clear the combo box before adding new items
    ui->CB_NWC_Network->addItems(channels); // Add the sorted items to the combo box
}
void NetworkChannels::on_deleteBTN_clicked(){

    ui->T_NWC_NetworkChannels->removeRow(ui->T_NWC_NetworkChannels->currentRow());

}
void NetworkChannels::sortTableWidget() {
    // This function assumes that the table has been already filled with items.

    // Disable sorting to manually handle the sort process
    ui->T_NWC_NetworkChannels->setSortingEnabled(false);

    // Extract the data from the table and sort it
    QList<QStringList> tableData;
    for (int i = 0; i < ui->T_NWC_NetworkChannels->rowCount(); ++i) {
        QStringList row;
        for (int j = 0; j < ui->T_NWC_NetworkChannels->columnCount(); ++j) {
            QTableWidgetItem *item = ui->T_NWC_NetworkChannels->item(i, j);
            row << (item ? item->text() : QString());
        }
        tableData << row;
    }

    // Sort the data
    std::sort(tableData.begin(), tableData.end(), [](const QStringList &a, const QStringList &b) {
        // Network is in the first column, Channel Name in the second, and Channel ID in the third
        if (a.at(0) == b.at(0)) {
            if (a.at(1) == b.at(1)) {
                return a.at(2) < b.at(2); // Sort by Channel ID if Network and Channel Name are equal
            }
            return a.at(1) < b.at(1); // Sort by Channel Name if Network is equal
        }
        return a.at(0) < b.at(0); // Sort by Network otherwise
    });

    // Clear the table and repopulate it with sorted data
    ui->T_NWC_NetworkChannels->clearContents();
    ui->T_NWC_NetworkChannels->setRowCount(tableData.size());

    // Assuming the delete button is in the 4th column (index 3)
    const int deleteButtonColumnIndex = 3;

    for (int i = 0; i < tableData.size(); ++i) {
        const QStringList &row = tableData[i];
        for (int j = 0; j < row.size(); ++j) {
            if (j != deleteButtonColumnIndex) { // Skip the delete button's column
                ui->T_NWC_NetworkChannels->setItem(i, j, new QTableWidgetItem(row.at(j)));
            }
        }
        // Add the delete button
        addDeleteButton(i);
    }

    // Re-enable sorting
    ui->T_NWC_NetworkChannels->setSortingEnabled(true);
}

void NetworkChannels::loadChannelsFromCsv() {
    // Load your CSV data into a QList<QStringList>
    QList<QStringList> csvData = dataManager->csv->load("NetworkChannels");

    // Populate the table with this data
    for (const QStringList &rowData : csvData) {
        int rowCount = ui->T_NWC_NetworkChannels->rowCount();
        ui->T_NWC_NetworkChannels->insertRow(rowCount);
        for (int column = 0; column < rowData.size(); ++column) {
            ui->T_NWC_NetworkChannels->setItem(rowCount, column, new QTableWidgetItem(rowData.at(column)));
        }
    }
    // Sort the table after loading the data
    sortTableWidget();
}
void NetworkChannels::addDeleteButton(int row) {
    QPushButton *deleteBTN = new QPushButton("X");
    deleteBTN->setStyleSheet("QPushButton { color: red; }");
    ui->T_NWC_NetworkChannels->setCellWidget(row, 3, deleteBTN);
    connect(deleteBTN, &QPushButton::clicked, this, [this, row]() {
        ui->T_NWC_NetworkChannels->removeRow(row);
        // Call the sorting function again if you want to maintain the sorted order after deletion.
        sortTableWidget();
    });
}

void NetworkChannels::on_PB_NWC_Cancel_clicked()
{
    ui->T_NWC_NetworkChannels->setRowCount(0);
    loadChannelsFromCsv();
    hide();
}

void NetworkChannels::updateTableWidget()
{
    loadChannelsFromCsv();
}


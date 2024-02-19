#include "networkchannels.h"
#include "ui_networkchannels.h"

#include <QJsonArray>
#include <QJsonObject>
#include <QRegularExpressionValidator>

enum ColumnIndexes{

    //visible
    eColCh_Network          = 0,
    eColCh_ChannelName      = 1,
    eColCh_ChannelId        = 2,
    eColCh_ChannelRegion    = 3,
    eColCh_DeleteBtn        = 4

};

NetworkChannels::NetworkChannels(DataManager *dataManager, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::NetworkChannels),
    dataManager(dataManager)
{
    ui->setupUi(this);

    // Connect the buttons to their respective slot functions
    connect(ui->PB_NWC_Add, &QPushButton::clicked, this, &NetworkChannels::on_addButton_clicked);
    connect(ui->PB_NWC_Save, &QPushButton::clicked, this, &NetworkChannels::on_saveButton_clicked);

    setupRegionsComboBox();
    initUiElements();

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

    int actRowCount = ui->T_NWC_NetworkChannels->rowCount();
    ui->T_NWC_NetworkChannels->insertRow(actRowCount); // Add a new row

    // Assuming you have the columns set as follows, adjust based on your actual setup
    QString networkCBText = ui->CB_NWC_Network->currentText();
    QString channelName = ui->LE_NWC_ChannelName->text();
    QString channelId = ui->LE_NWC_ChannelID->text();
    QString channelRegionCBText = ui->CB_NWC_regions->currentText();

    ui->T_NWC_NetworkChannels->setItem(actRowCount, eColCh_Network,     new QTableWidgetItem(networkCBText));
    ui->T_NWC_NetworkChannels->setItem(actRowCount, eColCh_ChannelName,     new QTableWidgetItem(channelName));
    ui->T_NWC_NetworkChannels->setItem(actRowCount, eColCh_ChannelId,     new QTableWidgetItem(channelId));
    ui->T_NWC_NetworkChannels->setItem(actRowCount, eColCh_ChannelRegion,     new QTableWidgetItem(channelRegionCBText));

    addDeleteButton(actRowCount);

    sortTableWidget();
}

void NetworkChannels::on_saveButton_clicked() {
    saveChannelsToCsv();
    hide();
}

void NetworkChannels::saveChannelsToCsv() {

    QList<QStringList> csvData;

    dataManager->csv->save("NetworkChannels", csvData);

    for(int row = 0; row < ui->T_NWC_NetworkChannels->rowCount();++row){
        QStringList rowData;
        for (int column = 0; column < (ui->T_NWC_NetworkChannels->columnCount()-1);++column){

            QTableWidgetItem *item = ui->T_NWC_NetworkChannels->item(row,column);
            rowData <<(item ? item->text() : QString());

        }
        csvData.append(rowData);
    }

    dataManager->csv->save("NetworkChannels", csvData);
    emit onNetworkDataSaved();
}

void NetworkChannels::fillComboBox()
{

    QStringList channels = {"Adcell", "Adtraction", "AWIN", "Webgains", "CJ", "TradeDoubler", "Adcell"};

    // Sort the list alphabetically
    std::sort(channels.begin(), channels.end());

    ui->CB_NWC_Network->clear();
    ui->CB_NWC_Network->addItems(channels);
}

void NetworkChannels::setupRegionsComboBox()
{

    connect(ui->CB_NWC_Network, QOverload<int>::of(&QComboBox::activated), [this](int) {

        ui->CB_NWC_regions->clear();
        QStringList markets;

        QString currNetwork = ui->CB_NWC_Network->currentText();

        if(currNetwork == "Adcell"){

        }else if (currNetwork == "Adtraction") {

            QJsonDocument adtractionRegions = dataManager->json->load("AdtractionRegions");
            QJsonArray marketsArray = adtractionRegions.array();

            for(int i = 0; i < marketsArray.size(); ++i){
                QJsonObject markteObject = marketsArray[i].toObject();
                QString marketName = markteObject["marketName"].toString();

                markets.append(marketName);
            }

            std::sort(markets.begin(), markets.end(), [](const QString &a, const QString &b) {
                return QString::compare(a, b, Qt::CaseInsensitive) < 0;
            });

            for(const QString &market : markets){

                ui->CB_NWC_regions->addItem(market);
            }

        }else if (currNetwork == "AWIN") {

        }else if (currNetwork == "Webgains") {

        }else if (currNetwork == "CJ") {

        }else if (currNetwork == "TradeDoubler") {

        }
    });
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
        if (a.at(eColCh_Network) == b.at(eColCh_Network)) {
            if (a.at(eColCh_ChannelName) == b.at(eColCh_ChannelName)) {
                return a.at(eColCh_ChannelId) < b.at(eColCh_ChannelId); // Sort by Channel ID if Network and Channel Name are equal
            }
            return a.at(eColCh_ChannelName) < b.at(eColCh_ChannelName); // Sort by Channel Name if Network is equal
        }
        return a.at(eColCh_Network) < b.at(eColCh_Network); // Sort by Network otherwise
    });

    // Clear the table and repopulate it with sorted data
    ui->T_NWC_NetworkChannels->clearContents();
    ui->T_NWC_NetworkChannels->setRowCount(tableData.size());

    for (int i = 0; i < tableData.size(); ++i) {
        const QStringList &row = tableData[i];
        for (int j = 0; j < row.size(); ++j) {
            if (j != eColCh_DeleteBtn) { // Skip the delete button's column
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

    ui->T_NWC_NetworkChannels->setCellWidget(row, eColCh_DeleteBtn, deleteBTN);

    connect(deleteBTN, &QPushButton::clicked, this, [this, row]() {

        ui->T_NWC_NetworkChannels->removeRow(row);
        sortTableWidget();
    });
}

void NetworkChannels::initUiElements()
{
    fillComboBox();

    QRegularExpression regExp("[A-Za-z0-9 ]*");                             //ban everything except A-Z a-z 0-9 and space
    QValidator* validator = new QRegularExpressionValidator(regExp, this);

    ui->LE_NWC_ChannelName->setValidator(validator);
    ui->LE_NWC_ChannelName->setMaxLength(14);


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



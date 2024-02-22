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
    saveChannelsToJson();
    hide();
}

void NetworkChannels::saveChannelsToJson() {

    QJsonDocument networksDoc = dataManager->json->load("NetworkChannels");
    QJsonObject networksObj = networksDoc.object();

    for(int row = 0; row < ui->T_NWC_NetworkChannels->rowCount(); ++row){

        QString network = ui->T_NWC_NetworkChannels->item(row, eColCh_Network)->text();
        QString channelID = ui->T_NWC_NetworkChannels->item(row, eColCh_ChannelId)->text();
        QString channelName = ui->T_NWC_NetworkChannels->item(row, eColCh_ChannelName)->text();
        QString channelRegion = ui->T_NWC_NetworkChannels->item(row, eColCh_ChannelRegion)->text();

        QJsonObject channelDetails;
        channelDetails["channelName"] = channelName;
        channelDetails["channelRegion"] = channelRegion;

        QJsonObject networkObj;
        if(networksObj.contains(network)){
            networkObj = networksObj[network].toObject();
        }
        networkObj.insert(channelID, channelDetails);
        networksObj.insert(network, networkObj);            // Make sure the updated networkObj is put back into networksObj
    }

    QJsonDocument jsonDoc(networksObj);
    dataManager->json->save("NetworkChannels", jsonDoc);

    emit onNetworkDataSaved();
}

void NetworkChannels::fillComboBox()
{

    QStringList channels = {"Adcell", "Adtraction", "AWIN", "Webgains", "CJ", "TradeDoubler"};

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

void NetworkChannels::loadChannelsFromJson() {

    // Clear existing rows in the table
    ui->T_NWC_NetworkChannels->setRowCount(0);

    // Load the JSON document
    QJsonDocument networksDoc = dataManager->json->load("NetworkChannels");
    QJsonObject networksObj = networksDoc.object();

    // Iterate through each network in the JSON object
    for (auto network = networksObj.begin(); network != networksObj.end(); ++network) {
        // Each network's key is the network name, and its value is another JSON object containing channels
        QJsonObject channelsObj = network.value().toObject();

        // Iterate through each channel in the network
        for (auto channel = channelsObj.begin(); channel != channelsObj.end(); ++channel) {
            // Each channel's key is the channel ID, and its value is another JSON object containing channel details
            QJsonObject channelDetails = channel.value().toObject();

            // Extract channel details
            QString channelID = channel.key();
            QString channelName = channelDetails["channelName"].toString();
            QString channelRegion = channelDetails["channelRegion"].toString();

            // Insert a new row at the end of the table
            int newRow = ui->T_NWC_NetworkChannels->rowCount();
            ui->T_NWC_NetworkChannels->insertRow(newRow);

            // Create new QTableWidgetItem for each piece of data
            QTableWidgetItem *newNetworkItem = new QTableWidgetItem(network.key());
            QTableWidgetItem *newChannelIDItem = new QTableWidgetItem(channelID);
            QTableWidgetItem *newChannelNameItem = new QTableWidgetItem(channelName);
            QTableWidgetItem *newChannelRegionItem = new QTableWidgetItem(channelRegion);

            // Add the items to the table
            ui->T_NWC_NetworkChannels->setItem(newRow, eColCh_Network, newNetworkItem);
            ui->T_NWC_NetworkChannels->setItem(newRow, eColCh_ChannelId, newChannelIDItem);
            ui->T_NWC_NetworkChannels->setItem(newRow, eColCh_ChannelName, newChannelNameItem);
            ui->T_NWC_NetworkChannels->setItem(newRow, eColCh_ChannelRegion, newChannelRegionItem);
        }
    }
    sortTableWidget();
}

void NetworkChannels::addDeleteButton(int row) {

    QPushButton *deleteBTN = new QPushButton("X");
    deleteBTN->setStyleSheet("QPushButton { color: red; }");

    ui->T_NWC_NetworkChannels->setCellWidget(row, eColCh_DeleteBtn, deleteBTN);

    connect(deleteBTN, &QPushButton::clicked, this, [this, row]() {

        QString channelID = ui->T_NWC_NetworkChannels->item(row, eColCh_ChannelId)->text();
        QString networkName = ui->T_NWC_NetworkChannels->item(row, eColCh_Network)->text();

        QJsonDocument networksDoc = dataManager->json->load("NetworkChannels");
        QJsonObject networksObj = networksDoc.object();

        if (networksObj.contains(networkName)) {
            // Get the network object
            QJsonObject networkObj = networksObj[networkName].toObject();

            // Remove the channel from the network object
            networkObj.remove(channelID);

            // Update the network object in the main JSON object
            networksObj[networkName] = networkObj;

            // Assuming you have a method to save your JSON object, for example:
            // saveJson(networksObj); // Implement this function to save the modified JSON object
        }
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
    loadChannelsFromJson();
    hide();
}

void NetworkChannels::updateTableWidget()
{
    loadChannelsFromJson();
}



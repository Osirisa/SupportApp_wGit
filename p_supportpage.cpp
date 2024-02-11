#include "p_supportpage.h"
#include "ui_p_supportpage.h"
#include "qjsonobject.h"
#include <QJsonValue>
#include <QJsonArray>
#include <qjsonobject.h>
#include <QTimer>
#include <suppeventbus.h>
#include <QMessageBox>
#include <QFileDialog>

/*
 *
 * Constructor / Destructor
 *
 */

//TBD: Delete Network in Table (this whole page is only for adtracion
//TBD: Dynamic Channel ID for sending supps ->on_PB_SendOverAPI_clicked | on_sendBTNTable_clicked
//TBD: NetworkStatus sortable
//TBD: Rest of the Sorting

//TBD: Import CSV

P_SupportPage::P_SupportPage(DataManager* dataManager, APIManager* apiManager,QWidget *parent) :
    QWidget(parent),
    ui(new Ui::P_SupportPage),
    dataManager(dataManager),
    apiManager(apiManager)
{
    ui->setupUi(this);

    //init the whole Page (has to be on a delay, otherwise problem with fileloading)
    QTimer::singleShot(300, this, &P_SupportPage::initPage);

    //Eventbus connection
    QObject::connect(SuppEventBus::instance(), &SuppEventBus::eventPublished, [this](const QString& eventName, const QVariant& eventData1, const QVariant& eventData2, const QVariant& eventData3) {
        if (eventName == "networkResponse") {
            QString responseCode = eventData1.toString();
            QString userId = eventData2.toString();
            QString orderId = eventData3.toString();

            qDebug() << "Received networkResponse:" << responseCode;
            networkRequestMessageReceived(responseCode, userId, orderId);
        }
    });
}

//Destructor
P_SupportPage::~P_SupportPage()
{
    delete ui;
}


/*
 *
 * INIT - Stuff
 *
 */

//Inits all the Elements on the page
void P_SupportPage::initPage()
{

    //Fill Comboboxes

    //In Adtraction there is only one type (maybe delete completely?
    //TBD:
    ui->CB_SuppType->addItem("Untracked");

    fillShopComboBox();
    fillCurrencyComboBox();
    fillNetworkComboBox();

    initTable();

    initInputElements();

    setupComboBoxConnections();
    ui->CB_shop->activated(ui->CB_shop->currentIndex());

    QTimer::singleShot(300, this, &P_SupportPage::fillTableWithJson);
}

//Inits the Table (Column Size)
void P_SupportPage::initTable()
{
    //standard size is 150 for the columns
    for(int i=0;i<ui->T_NachbuchungsanfragenListe->columnCount();++i){
        ui->T_NachbuchungsanfragenListe->setColumnWidth(i,150);
    }

    //rest is seted seperately
    ui->T_NachbuchungsanfragenListe->setColumnWidth(eCol_Network,80);
    ui->T_NachbuchungsanfragenListe->setColumnWidth(eCol_Channel,80);
    ui->T_NachbuchungsanfragenListe->setColumnWidth(eCol_Shop,175);
    ui->T_NachbuchungsanfragenListe->setColumnWidth(eCol_Value,80);
    ui->T_NachbuchungsanfragenListe->setColumnWidth(eCol_ExpProv,80);
    ui->T_NachbuchungsanfragenListe->setColumnWidth(eCol_Currency,60);
    ui->T_NachbuchungsanfragenListe->setColumnWidth(eCol_OrderId,80);
    ui->T_NachbuchungsanfragenListe->setColumnWidth(eCol_UserId,100);
    ui->T_NachbuchungsanfragenListe->setColumnWidth(eCol_Date,75);
    ui->T_NachbuchungsanfragenListe->setColumnWidth(eCol_CommissionId,100);
    ui->T_NachbuchungsanfragenListe->setColumnWidth(eCol_CommissionType,90);
    ui->T_NachbuchungsanfragenListe->setColumnWidth(eCol_DaysOld,75);
    ui->T_NachbuchungsanfragenListe->setColumnWidth(eCol_Networkstatus,90);
    ui->T_NachbuchungsanfragenListe->setColumnWidth(eCol_SendBTN,70);
    ui->T_NachbuchungsanfragenListe->setColumnWidth(eCol_DeleteBTN,70);

    //Hidden Column for Internal Stats
    ui->T_NachbuchungsanfragenListe->hideColumn(eCol_H_Nstat);
}

//Sets up all the input elements
void P_SupportPage::initInputElements()
{

    QDoubleValidator *dvalidator = new QDoubleValidator(this);
    ui->LE_value->setValidator(dvalidator);
    ui->LE_expectedProv_Currency->setValidator(dvalidator);
    ui->LE_expectedProv_Percent->setValidator(dvalidator);

    ui->RB_expProvCur->setChecked(true);
    ui->LE_expectedProv_Percent->setEnabled(false);

    ui->DE_transactionDate->setDate(QDate::currentDate());
    ui->DE_transactionDate->setMaximumDate(QDate::currentDate());

    //Add To list Button
    QPixmap pm_addToList(":/img/img/plusIcon_trans.png");
    QIcon i_addToList(pm_addToList);
    ui->PB_AddToList->setIcon(i_addToList);
    ui->PB_AddToList->setToolTip("Adds the input to the table");

    //Export List to CSV Button
    QPixmap pm_exportList(":/img/img/document_trans.png");
    QIcon i_exportList(pm_exportList);
    ui->PB_ExportList->setIcon(i_exportList);
    ui->PB_ExportList->setToolTip("Exports either all or only selected Items of the Table to a CSV");

    //Delte All Button
    QPixmap pm_deleteAllBTN(":/img/img/3x_trans.png");
    QIcon i_deleteAllBTN(pm_deleteAllBTN);
    ui->pb_deleteAll->setIcon(i_deleteAllBTN);
    ui->pb_deleteAll->setToolTip("Delets either all or only selected Items of the table");

    QPixmap pm_soaButton(":/img/img/arrowCircle_trans.png");
    QIcon i_soaButton(pm_soaButton);
    ui->PB_SendOverAPI->setIcon(i_soaButton);
    ui->PB_SendOverAPI->setToolTip("Sends either all or only selected Contents of the Table");
}

//Fills the shop Combobox with the shops from Adtraction
void P_SupportPage::fillShopComboBox()
{
    QList<QStringList> csvData = dataManager->csv->load("NetworkChannels");
    QStringList comboBoxItems;

    for (const QStringList &rowData : csvData) {
        QJsonDocument doc = dataManager->json->load(QString(rowData.at(2)));

        for(const QJsonValue &value : doc.array()){
            QJsonObject obj = value.toObject();
            shopToProgramIdHash.insert(obj["programName"].toString(), obj["programId"].toInt());

            // Collect items in the list
            comboBoxItems.append(obj["programName"].toString() + ": (" + rowData.at(1) + ")");
        }

        channelToId.insert(rowData.at(1), rowData.at(2));
        allDocs.insert(rowData.at(2), doc);
    }

    // Sort the items alphabetically
    std::sort(comboBoxItems.begin(), comboBoxItems.end());

    // Add sorted items to the combo box
    for (const QString &item : comboBoxItems) {
        ui->CB_shop->addItem(item);
    }
}

//Fills the Currency Combobox with all the Currencies from Adtraction
void P_SupportPage::fillCurrencyComboBox()
{
    //load the CurrencyFile for adtraction
    cur = dataManager->json->load("currenciesAdtraction");


    //take the json an load it into 2 lists (Preferred / Rest)
    for(const QJsonValue &value: cur.array()){
        QJsonObject obj = value.toObject();
        QString currency = obj["currency"].toString();
        if(prefferedCurrencies.contains(currency)){
            prefferedList.append(currency);
        }else{
            otherList.append(currency);
        }
    }

    //Sort the "rest"-list
    std::sort(otherList.begin(), otherList.end());

    //Populate the combobox
    for(const QString &currency : prefferedList){
        ui->CB_Currency->addItem(currency);
    }
    for(const QString &currency : otherList){
        ui->CB_Currency->addItem(currency);
    }
}

//Fills the Network Combobox with all the ChannelIDs Inputted in the Adtraction->ChannelIds Window
void P_SupportPage::fillNetworkComboBox()
{
    ui->CB_Network->clear();

    QStringList allNetworks;
    QStringList networkPlusALL;

    QList<QStringList> csvData = dataManager->csv->load("NetworkChannels");

    for (const QStringList &rowData : csvData) {

        if(!(networkPlusALL.contains(rowData.at(0)+": ALL"))){
            networkPlusALL.append(rowData.at(0)+": ALL");
        }
    }
    // Populate the table with this data
    for (const QStringList &rowData : csvData) {

        QString CombinedText = rowData.at(0)+ ": " + rowData.at(1);
        allNetworks.append(CombinedText);
    }

    for (int i=0; i< networkPlusALL.size();++i){
        allNetworks.append(networkPlusALL.at(i));
    }

    allNetworks.sort();
    for (int i=0; i< allNetworks.size();++i){
        ui->CB_Network->addItem(allNetworks.at(i));
    }
}

//Sets up the Connections from the ShopCombobox to the CommissionId and the CommissionId to the Expected Commission [Currency] / %
void P_SupportPage::setupComboBoxConnections()
{

    connect(ui->CB_shop, QOverload<int>::of(&QComboBox::activated),[this](int) {
        ui->CB_ComissionID->clear();

        int colonSpaceIndex = ui->CB_shop->currentText().indexOf(": ");

        QString channel;
        QString currShop;

        if(colonSpaceIndex != -1){
            channel = ui->CB_shop->currentText().mid(colonSpaceIndex + 3); // +3 to skip ": ("
            channel = channel.removeLast();
        }
        if (colonSpaceIndex != -1) {
            currShop = ui->CB_shop->currentText().left(colonSpaceIndex);

        }

        QJsonDocument commDoc = allDocs.value(channelToId.value(channel));
        QJsonArray programsArray = commDoc.array();

        for(const QJsonValue& programValue : programsArray){

            QJsonObject programObj = programValue.toObject();
            if(programObj["programName"].toString() == currShop){

                QJsonArray commissions = programObj["commissions"].toArray();
                for (const QJsonValue& value : commissions) {
                    QJsonObject obj = value.toObject();
                    QString itemId = obj["id"].toString();
                    QString itemName = obj["name"].toString();
                    QString itemText = QString("%1: %2").arg(itemName, itemId);

                    ui->CB_ComissionID->addItem(itemText);
                }
                break;
            }
        }

        int currIndex = ui->CB_ComissionID->currentIndex();
        ui->CB_ComissionID->activated(currIndex);
        ui->LE_value->editingFinished();
    });

    connect(ui->CB_ComissionID, QOverload<int>::of(&QComboBox::activated), [this](int) {

        QString currCommissionIDText = ui->CB_ComissionID->currentText();
        QString currShopText = ui->CB_shop->currentText();

        int colonSpaceIndex = currCommissionIDText.lastIndexOf(": "); // Find the last ": " to ensure correct splitting
        int colonSpaceIndex2 = currShopText.indexOf(": ");

        QString currCommissionID;
        QString currShop;


        if (colonSpaceIndex != -1) {
            currCommissionID = currCommissionIDText.mid(colonSpaceIndex + 2);
            // Assuming the ID is after ": ", adjust if necessary
        }

        if (colonSpaceIndex2 != -1) {
            currShop = currShopText.mid(colonSpaceIndex2 + 3).chopped(1);
            // Assuming the ID is after ": (", adjust if necessary
        }

        // Now currCommissionID should correctly hold the ID string
        QJsonDocument commDoc = allDocs.value(channelToId.value(currShop));
        // Assuming you need to search through all programs for the correct commission
        for (const QJsonValue& programValue : commDoc.array()) {
            QJsonObject programObj = programValue.toObject();
            QJsonArray commissions = programObj["commissions"].toArray();

            for (const QJsonValue& value : commissions) {
                QJsonObject obj = value.toObject();

                if (currCommissionID == obj["id"].toString()) {
                    // Found the matching commission
                    QString typeExpProv = obj["type"].toString();
                    double valExpProv = obj["value"].toDouble();

                    if(typeExpProv == "%"){
                        ui->LE_expectedProv_Percent->setText(QString::number(valExpProv));
                        ui->LE_expectedProv_Percent->editingFinished();
                        ui->RB_expProvPer->click();
                    }
                    else{
                        ui->LE_expectedProv_Currency->setText(QString::number(valExpProv));
                        ui->RB_expProvCur->click();;
                    }
                    // Update UI elements as before
                    break; // Exit the loop once the correct commission is found
                }
            }
        }
    });
}

//on Startup fills the Table with the last session / if not deleted
void P_SupportPage::fillTableWithJson() {

    QJsonDocument suppAnswers = dataManager->json->load("NetworkSuppAnswers");

    //qDebug()<<suppAnswers;

    if (suppAnswers.isNull()) {
        //qDebug() << "Error loading JSON file or is Empty";
        return;
    }

    QJsonObject jObj = suppAnswers.object();
    ui->T_NachbuchungsanfragenListe->setRowCount(0); // Clear existing rows

    for (auto userID : jObj.keys()) {
        QJsonObject userObj = jObj.value(userID).toObject();
        QJsonArray ordersArray = userObj["orders"].toArray();

        for(const QJsonValue &ordersValue : ordersArray) {
            QJsonObject orderObj = ordersValue.toObject();
            QDate orderDate = QDate::fromString(orderObj["date"].toString(), "dd.MM.yyyy");

            int rowCount = ui->T_NachbuchungsanfragenListe->rowCount();
            ui->T_NachbuchungsanfragenListe->insertRow(rowCount); // Add a new row

            // Assuming you have the columns set as follows, adjust based on your actual setup
            ui->T_NachbuchungsanfragenListe->setItem(rowCount, eCol_Network, new QTableWidgetItem(orderObj["network"].toString()));
            ui->T_NachbuchungsanfragenListe->setItem(rowCount, eCol_Channel, new QTableWidgetItem(orderObj["channel"].toString()));
            ui->T_NachbuchungsanfragenListe->setItem(rowCount, eCol_Shop, new QTableWidgetItem(orderObj["shop"].toString()));
            ui->T_NachbuchungsanfragenListe->setItem(rowCount, eCol_Value, new QTableWidgetItem(orderObj["value"].toString()));
            ui->T_NachbuchungsanfragenListe->setItem(rowCount, eCol_ExpProv, new QTableWidgetItem(orderObj["expProv"].toString()));
            ui->T_NachbuchungsanfragenListe->setItem(rowCount, eCol_Currency, new QTableWidgetItem(orderObj["currency"].toString()));
            ui->T_NachbuchungsanfragenListe->setItem(rowCount, eCol_OrderId, new QTableWidgetItem(orderObj["orderID"].toString()));
            ui->T_NachbuchungsanfragenListe->setItem(rowCount, eCol_UserId, new QTableWidgetItem(userID));
            ui->T_NachbuchungsanfragenListe->setItem(rowCount, eCol_Date, new QTableWidgetItem(orderObj["date"].toString()));
            ui->T_NachbuchungsanfragenListe->setItem(rowCount, eCol_CommissionId, new QTableWidgetItem(orderObj["comissionID"].toString()));
            ui->T_NachbuchungsanfragenListe->setItem(rowCount, eCol_CommissionType, new QTableWidgetItem(orderObj["suppType"].toString()));
            ui->T_NachbuchungsanfragenListe->setItem(rowCount, eCol_DaysOld, new QTableWidgetItem(QString::number(orderDate.daysTo(QDate::currentDate()))));

            if(!orderObj["networkStatusCode"].isNull()){

                QString responseCode = orderObj["networkStatusCode"].toString();
                QString nreply = orderObj["networkReply"].toString();

                if(responseCode == "200"){

                    QPushButton *networkStatusBTN = new QPushButton;
                    QPixmap pm_networkStatus(":/img/img/ausrufezeichen_green_trans.png");
                    QIcon i_networkSatus(pm_networkStatus);
                    networkStatusBTN->setIcon(i_networkSatus);
                    networkStatusBTN->setToolTip(nreply);

                    ui->T_NachbuchungsanfragenListe->setCellWidget(rowCount,eCol_Networkstatus,networkStatusBTN);
                }
                else if(responseCode == "409"|| responseCode == "204" || responseCode == "201"){
                    QPushButton *networkStatusBTN = new QPushButton;
                    QPixmap pm_networkStatus(":/img/img/ausrufezeichen_orange_trans.png");
                    QIcon i_networkSatus(pm_networkStatus);
                    networkStatusBTN->setIcon(i_networkSatus);
                    networkStatusBTN->setToolTip(nreply);

                    ui->T_NachbuchungsanfragenListe->setCellWidget(rowCount,eCol_Networkstatus,networkStatusBTN);
                }
                else {
                    QPushButton *networkStatusBTN = new QPushButton;
                    QPixmap pm_networkStatus(":/img/img/ausrufezeichen_trans.png");
                    QIcon i_networkSatus(pm_networkStatus);
                    networkStatusBTN->setIcon(i_networkSatus);
                    networkStatusBTN->setToolTip(nreply);

                    ui->T_NachbuchungsanfragenListe->setCellWidget(rowCount,eCol_Networkstatus,networkStatusBTN);
                }
            }
            else{

                // Adding buttons to specific columns
                QPushButton *networkStatusBTN = new QPushButton;
                networkStatusBTN->setIcon(QIcon(":/img/img/3Dots_trans.png"));
                networkStatusBTN->setToolTip("Network Status");
                ui->T_NachbuchungsanfragenListe->setCellWidget(rowCount, eCol_Networkstatus, networkStatusBTN);
            }
            QPushButton *sendBTN = new QPushButton;
            sendBTN->setIcon(QIcon(":/img/img/Send_trans.png"));
            ui->T_NachbuchungsanfragenListe->setCellWidget(rowCount, eCol_SendBTN, sendBTN);

            QPushButton *deleteBTN = new QPushButton;
            deleteBTN->setIcon(QIcon(":/img/img/Kreuz_trans.png"));
            ui->T_NachbuchungsanfragenListe->setCellWidget(rowCount, eCol_DeleteBTN, deleteBTN);

            // Connect button signals to slots for handling user actions
            QObject::connect(deleteBTN, &QPushButton::clicked, this, &P_SupportPage::on_deleteBTNTable_clicked);
            QObject::connect(sendBTN, &QPushButton::clicked, this, &P_SupportPage::on_sendBTNTable_clicked);
        }
    }
}

/*
 *
 * Public Functions
 *
 */

void P_SupportPage::refreshNetworkList()
{
    fillNetworkComboBox();
}

/*
 *
 * Private Functions
 *
 */

void P_SupportPage::outputRowsToCSV(const QString &fileName)
{
    QList<QTableWidgetSelectionRange> rowSelections = ui->T_NachbuchungsanfragenListe->selectedRanges();
    QList<QStringList> csvData;

    for(const QTableWidgetSelectionRange &rowSelect : rowSelections){
        for(int row = rowSelect.topRow(); row <= rowSelect.bottomRow();++row){

            QStringList rowData;

            QString currCommissionIDText = ui->T_NachbuchungsanfragenListe->item(row,eCol_CommissionId)->text();
            QString currShopText = ui->T_NachbuchungsanfragenListe->item(row,eCol_Shop)->text();

            int colonSpaceIndex = currCommissionIDText.lastIndexOf(": ");       // Find the last ": " to ensure correct splitting
            int colonSpaceIndex2 = currShopText.indexOf(": ");


            QString currCommissionID;
            QString currShop;


            if (colonSpaceIndex != -1) {
                currCommissionID = currCommissionIDText.mid(colonSpaceIndex + 2);
            }

            if (colonSpaceIndex2 != -1) {
                currShop = currShopText.mid(colonSpaceIndex2 + 3).chopped(1);
            }

            //orderValue
            QTableWidgetItem *itemToCSV = ui->T_NachbuchungsanfragenListe->item(row,eCol_Value);
            rowData << (itemToCSV ? itemToCSV->text() : QString());


            //ChannelID
            rowData << channelToId.value(currShop);

            //commissionId
            rowData << currCommissionID;

            //currency
            itemToCSV = ui->T_NachbuchungsanfragenListe->item(row,eCol_Currency);
            rowData << (itemToCSV ? itemToCSV->text() : QString());

            //transactionDate
            itemToCSV = ui->T_NachbuchungsanfragenListe->item(row,eCol_Date);
            QString originalDateString = itemToCSV ? itemToCSV->text() : QString();
            QDate date = QDate::fromString(originalDateString, "dd.MM.yyyy"); // Adjust this format
            QString formattedDateString = date.toString("yyyy-MM-dd");
            rowData << formattedDateString;

            //orderID
            itemToCSV = ui->T_NachbuchungsanfragenListe->item(row,eCol_OrderId);
            rowData << (itemToCSV ? itemToCSV->text() : QString());

            //epi (user)
            itemToCSV = ui->T_NachbuchungsanfragenListe->item(row,eCol_UserId);
            rowData << (itemToCSV ? itemToCSV->text() : QString());

            //productcategoryId -- EMPTY
            rowData << "";

            //expectedCommission
            itemToCSV = ui->T_NachbuchungsanfragenListe->item(row,eCol_ExpProv);
            rowData << (itemToCSV ? itemToCSV->text() : QString());

            csvData.append(rowData);
        }
    }

    dataManager->registerFile("csvExportFile",fileName);
    dataManager->csv->save("csvExportFile",csvData);
}

//Event after the request was received from the server
void P_SupportPage::networkRequestMessageReceived(const QString responseCode, const QString userId, const QString orderId)
{
    QJsonDocument suppAnswers = dataManager->json->load("NetworkSuppAnswers");
    QJsonObject jObj;

    QString nreply;

    if (!suppAnswers.isNull()) {
        // If the JSON document is not null, use its object
        jObj = suppAnswers.object();
    }

    if(jObj.contains(userId)){
        QJsonObject userObj = jObj[userId].toObject();
        QJsonArray ordersArray = userObj["orders"].toArray();

        for(int i = 0; i<ordersArray.size(); ++i){
            QJsonObject orderObj = ordersArray[i].toObject();


            if(orderObj["orderID"].toString() == orderId){

                nreply = orderObj["networkReply"].toString();
                break;
            }
        }
    }

    for (int i = 0; i < ui->T_NachbuchungsanfragenListe->rowCount();++i){
        if (userId == ui->T_NachbuchungsanfragenListe->item(i,eCol_UserId)->text()&& orderId == ui->T_NachbuchungsanfragenListe->item(i,eCol_OrderId)->text()){
            if(responseCode == "200"){

                QPushButton *networkStatusBTN = new QPushButton;
                QPixmap pm_networkStatus(":/img/img/ausrufezeichen_green_trans.png");
                QIcon i_networkSatus(pm_networkStatus);
                networkStatusBTN->setIcon(i_networkSatus);
                networkStatusBTN->setToolTip(nreply);

                ui->T_NachbuchungsanfragenListe->setCellWidget(i,eCol_Networkstatus,networkStatusBTN);
            }
            else if(responseCode == "409"|| responseCode == "204" || responseCode == "201"){
                QPushButton *networkStatusBTN = new QPushButton;
                QPixmap pm_networkStatus(":/img/img/ausrufezeichen_orange_trans.png");
                QIcon i_networkSatus(pm_networkStatus);
                networkStatusBTN->setIcon(i_networkSatus);
                networkStatusBTN->setToolTip(nreply);

                ui->T_NachbuchungsanfragenListe->setCellWidget(i,eCol_Networkstatus,networkStatusBTN);
            }
            else {
                QPushButton *networkStatusBTN = new QPushButton;
                QPixmap pm_networkStatus(":/img/img/ausrufezeichen_trans.png");
                QIcon i_networkSatus(pm_networkStatus);
                networkStatusBTN->setIcon(i_networkSatus);
                networkStatusBTN->setToolTip(nreply);

                ui->T_NachbuchungsanfragenListe->setCellWidget(i,eCol_Networkstatus,networkStatusBTN);
            }
        }
    }
}


/*
 *
 * SLOTS - Buttons
 *
 */

//---------------TableStuff----------------

//Toggle and Sort:
void P_SupportPage::on_pb_toggleTable_clicked()
{
    if(ui->T_NachbuchungsanfragenListe->isColumnHidden(1)){

        ui->T_NachbuchungsanfragenListe->setColumnHidden(eCol_Channel,false);
        ui->T_NachbuchungsanfragenListe->setColumnHidden(eCol_ExpProv,false);
        ui->T_NachbuchungsanfragenListe->setColumnHidden(eCol_CommissionId,false);
        ui->T_NachbuchungsanfragenListe->setColumnHidden(eCol_CommissionType,false);
        ui->T_NachbuchungsanfragenListe->setColumnHidden(eCol_DaysOld,false);
    }else{

        ui->T_NachbuchungsanfragenListe->setColumnHidden(eCol_Channel,true);
        ui->T_NachbuchungsanfragenListe->setColumnHidden(eCol_ExpProv,true);
        ui->T_NachbuchungsanfragenListe->setColumnHidden(eCol_CommissionId,true);
        ui->T_NachbuchungsanfragenListe->setColumnHidden(eCol_CommissionType,true);
        ui->T_NachbuchungsanfragenListe->setColumnHidden(eCol_DaysOld,true);
    }
}

//Inserting, Sending, Deleting direct out of the table
void P_SupportPage::on_PB_AddToList_clicked()
{
    if(ui->LE_orderId->text().isEmpty() || ui->LE_User->text().isEmpty() || ui->LE_value->text().isEmpty() || ui->LE_expectedProv_Currency->text().isEmpty()){

        QMessageBox::critical(this,"Text Boxes Empty","At least one of the text boxes is empty, please fill out every text box!");
    }
    else{

        int colonSpaceIndex = ui->CB_shop->currentText().indexOf(": ");
        QString channel;

        if(colonSpaceIndex != -1){
            channel = ui->CB_shop->currentText().mid(colonSpaceIndex + 3); // +3 to skip ": ("
            channel = channel.removeLast();
        }

        int rowCount = ui->T_NachbuchungsanfragenListe->rowCount();
        ui->T_NachbuchungsanfragenListe->setRowCount(rowCount + 1);

        size_t colonPos = ui->CB_Network->currentText().toStdString().find(":");
        std::string extractedText;

        if (colonPos != std::string::npos) {
            // Extract the substring from the beginning to the colon
            extractedText = ui->CB_Network->currentText().toStdString().substr(0, colonPos);
        }
        QString qExtractedText = QString::fromStdString(extractedText);

        QTableWidgetItem *items [] = {
            new QTableWidgetItem(QString(qExtractedText)),                                                                  //0     --> Network
            new QTableWidgetItem(QString(channel)),                                                                         //1     -->
            new QTableWidgetItem(QString(ui->CB_shop->currentText())),                                                      //2     -->
            new QTableWidgetItem(QString(ui->LE_value->text())),                                                            //3     -->
            new QTableWidgetItem(QString(ui->LE_expectedProv_Currency->text())),                                            //4     -->
            new QTableWidgetItem(QString(ui->CB_Currency->currentText())),                                                  //5     -->
            new QTableWidgetItem(QString(ui->LE_orderId->text())),                                                          //6     -->
            new QTableWidgetItem(QString(ui->LE_User->text())),                                                             //7     -->
            new QTableWidgetItem(QString(ui->DE_transactionDate->date().toString("dd.MM.yyyy"))),                           //8     -->
            new QTableWidgetItem(QString(ui->CB_ComissionID->currentText())),                                               //9     -->
            new QTableWidgetItem(QString(ui->CB_SuppType->currentText())),                                                  //10    -->
            new QTableWidgetItem(QString::number(ui->DE_transactionDate->date().daysTo(QDate::currentDate()))),             //11    -->
            new QTableWidgetItem,                                                                                           //12    --> networkstatus
            new QTableWidgetItem,                                                                                           //13    --> send
            new QTableWidgetItem                                                                                            //14    --> delete
        };



        QJsonDocument suppData = dataManager->json->load("NetworkSuppAnswers");
        QJsonObject jObj;

        if (!suppData.isNull()) {
            // If the JSON document is not null, use its object
            jObj = suppData.object();
        }

        QString userID = ui->LE_User->text();

        if(!jObj.contains(ui->LE_User->text())){
            QJsonObject userObj;
            userObj["userID"] = userID;
            userObj["orders"] = QJsonArray();
            jObj[userID]=userObj;
        }

        QJsonObject userObj = jObj[userID].toObject();

        QJsonObject orderObj;
        orderObj["orderID"] = ui->LE_orderId->text();
        orderObj["network"] = qExtractedText;
        orderObj["channel"] = channel;
        orderObj["shop"] = ui->CB_shop->currentText();
        orderObj["value"] = ui->LE_value->text();
        orderObj["expProv"] = ui->LE_expectedProv_Currency->text();
        orderObj["currency"] = ui->CB_Currency->currentText();
        orderObj["date"] = ui->DE_transactionDate->date().toString("dd.MM.yyyy");
        orderObj["comissionID"] = ui->CB_ComissionID->currentText();
        orderObj["suppType"] = ui->CB_SuppType->currentText();

        QJsonArray ordersArray = userObj["orders"].toArray();
        ordersArray.append(orderObj);

        userObj["orders"] = ordersArray;
        jObj[userID] = userObj;

        QJsonDocument suppDoc(jObj);

        dataManager->json->save("NetworkSuppAnswers",suppDoc);

        const size_t count = sizeof(items) / sizeof(QTableWidgetItem*);
        for(size_t column = 0; column < count; column++){
            ui->T_NachbuchungsanfragenListe->setItem(rowCount,column,items[column]);
        }

        QPushButton *networkStatusBTN = new QPushButton;
        QPixmap pm_networkStatus(":/img/img/3Dots_trans.png");
        QIcon i_networkSatus(pm_networkStatus);
        networkStatusBTN->setIcon(i_networkSatus);
        networkStatusBTN->setToolTip("test");

        ui->T_NachbuchungsanfragenListe->setCellWidget(rowCount,eCol_Networkstatus,networkStatusBTN);


        QPushButton *sendBTN = new QPushButton;
        QPixmap sendPixmap(":/img/img/Send_trans.png");
        QIcon sendIcon(sendPixmap);
        sendBTN->setIcon(sendIcon);
        ui->T_NachbuchungsanfragenListe->setCellWidget(rowCount,eCol_SendBTN,sendBTN);

        QPushButton *deleteBTN = new QPushButton;
        QPixmap xPixmap(":/img/img/Kreuz_trans.png");
        QIcon xIcon(xPixmap);
        deleteBTN->setIcon(xIcon);
        ui->T_NachbuchungsanfragenListe->setCellWidget(rowCount,eCol_DeleteBTN,deleteBTN);



        QObject::connect(deleteBTN, &QPushButton::clicked, this, &P_SupportPage::on_deleteBTNTable_clicked);
        QObject::connect(sendBTN, &QPushButton::clicked, this, &P_SupportPage::on_sendBTNTable_clicked);


        //Delete contents
        ui->LE_value->clear();
        ui->LE_orderId->clear();
        ui->LE_User->clear();
        ui->DE_transactionDate->setDate(QDate::currentDate());
    }
}

//deleting one Element from the Table directly
void P_SupportPage::on_deleteBTNTable_clicked()
{
    QJsonDocument suppData = dataManager->json->load("NetworkSuppAnswers");
    QJsonObject jObj;

    int currRow = ui->T_NachbuchungsanfragenListe->currentRow();

    QString userId = ui->T_NachbuchungsanfragenListe->item(currRow,eCol_UserId)->text();
    QString orderId = ui->T_NachbuchungsanfragenListe->item(currRow,eCol_OrderId)->text();

    if (!suppData.isNull()) {
        // If the JSON document is not null, use its object
        jObj = suppData.object();
    }

    if(jObj.contains(userId)){
        QJsonObject userObj = jObj[userId].toObject();
        QJsonArray ordersArray = userObj["orders"].toArray();

        for(int i = 0; i<ordersArray.size(); ++i){
            QJsonObject orderObj = ordersArray[i].toObject();

            if(orderObj["orderID"].toString() == orderId){
                ordersArray.removeAt(i);
                break;
            }
        }

        userObj["orders"] = ordersArray;

        jObj[userId] = userObj;

        QJsonDocument suppdoc(jObj);

        dataManager->json->save("NetworkSuppAnswers",suppdoc);
    }

    ui->T_NachbuchungsanfragenListe->removeRow(currRow);
}

//sending one Element from the Table directly
void P_SupportPage::on_sendBTNTable_clicked(){

    int row = ui->T_NachbuchungsanfragenListe->currentRow();

    QString currShopText = ui->T_NachbuchungsanfragenListe->item(row,eCol_Shop)->text();

    int colonSpaceIndex2 = currShopText.indexOf(": ");
    QString currShop;

    if (colonSpaceIndex2 != -1) {
        currShop = currShopText.mid(colonSpaceIndex2 + 3).chopped(1);
    }

    QStringList programIdStrL = ui->T_NachbuchungsanfragenListe->item(row, eCol_Shop)->text().split(":");
    QString programIdStr = programIdStrL.first().trimmed();
    int programId = shopToProgramIdHash.value(programIdStr);
    int channelId = channelToId.value(currShop).toInt();
    QString orderId= ui->T_NachbuchungsanfragenListe->item(row, eCol_OrderId)->text();
    int commissionId= ui->T_NachbuchungsanfragenListe->item(row, eCol_CommissionId)->text().split(":").at(1).trimmed().toInt();
    double expecetedCom = ui->T_NachbuchungsanfragenListe->item(row, eCol_ExpProv)->text().toDouble();


    QDate date = QDate::fromString(ui->T_NachbuchungsanfragenListe->item(row, eCol_Date)->text(),"dd.MM.yyyy");
    QString transactionDate = date.toString("yyyy-MM-dd");
    transactionDate += "T23:59:59+0100";

    double orderVal = ui->T_NachbuchungsanfragenListe->item(row, eCol_Value)->text().toDouble();
    QString currency = ui->T_NachbuchungsanfragenListe->item(row, eCol_Currency)->text();
    QString userId = ui->T_NachbuchungsanfragenListe->item(row, eCol_UserId)->text();

    // qDebug()<<programId;
    // qDebug()<<channelId;
    // qDebug()<<orderId;
    // qDebug()<<commissionId;
    // qDebug()<<expecetedCom;
    // qDebug()<<transactionDate;
    // qDebug()<<orderVal;
    // qDebug()<<currency;
    // qDebug()<<userId;
    // qDebug()<<"------------------------------SEND------------------------------";
    apiManager->adtraction->sendSuppData(programId,channelId, orderId, commissionId, expecetedCom, transactionDate, orderVal, currency, userId);
}

//Sending selection / all
void P_SupportPage::on_PB_SendOverAPI_clicked()
{
    //Checks if the Table has at least one Entry
    if (ui->T_NachbuchungsanfragenListe->rowCount() < 1) {
        QMessageBox::information(this, tr("No Contents"), tr("There are no Contents in the Table"));
        return;
    }

    if (ui->T_NachbuchungsanfragenListe->rowCount() == 1){
        ui->T_NachbuchungsanfragenListe->selectAll();
    }
    else if (!ui->T_NachbuchungsanfragenListe->selectionModel()->hasSelection()) {

        QMessageBox::StandardButton reply;
        reply = QMessageBox::question(this, "Send all Contents to the Network", "Are you sure you want to send all the Contents to the Network?",
                                      QMessageBox::Yes|QMessageBox::Cancel);

        if (reply == QMessageBox::Yes) {

            ui->T_NachbuchungsanfragenListe->selectAll();
        }
        else {
            return;
        }
    }

    QList<QTableWidgetSelectionRange> rowSelections = ui->T_NachbuchungsanfragenListe->selectedRanges();

    for(const QTableWidgetSelectionRange &rowSelect : rowSelections){
        for(int row = rowSelect.topRow(); row <= rowSelect.bottomRow();++row){

            QString currShopText = ui->T_NachbuchungsanfragenListe->item(row,eCol_Shop)->text();

            int colonSpaceIndex2 = currShopText.indexOf(": ");
            QString currShop;

            if (colonSpaceIndex2 != -1) {
                currShop = currShopText.mid(colonSpaceIndex2 + 3).chopped(1);
            }

            QStringList programIdStrL = ui->T_NachbuchungsanfragenListe->item(row, eCol_Shop)->text().split(":");
            QString programIdStr = programIdStrL.first().trimmed();
            int programId = shopToProgramIdHash.value(programIdStr);

            int channelId = channelToId.value(currShop).toInt();
            QString orderId= ui->T_NachbuchungsanfragenListe->item(row, eCol_OrderId)->text();
            int commissionId= ui->T_NachbuchungsanfragenListe->item(row, eCol_CommissionId)->text().split(":").at(1).trimmed().toInt();
            double expecetedCom = ui->T_NachbuchungsanfragenListe->item(row, eCol_ExpProv)->text().toDouble();


            QDate date = QDate::fromString(ui->T_NachbuchungsanfragenListe->item(row, eCol_Date)->text(),"dd.MM.yyyy");
            QString transactionDate = date.toString("yyyy-MM-dd");
            transactionDate += "T23:59:59+0100";

            double orderVal = ui->T_NachbuchungsanfragenListe->item(row, eCol_Value)->text().toDouble();
            QString currency = ui->T_NachbuchungsanfragenListe->item(row, eCol_Currency)->text();
            QString userId = ui->T_NachbuchungsanfragenListe->item(row, eCol_UserId)->text();

            // qDebug()<<programId;
            // qDebug()<<channelId;
            // qDebug()<<orderId;
            // qDebug()<<commissionId;
            // qDebug()<<expecetedCom;
            // qDebug()<<transactionDate;
            // qDebug()<<orderVal;
            // qDebug()<<currency;
            // qDebug()<<userId;
            // qDebug()<<"------------------------------SEND------------------------------";
            apiManager->adtraction->sendSuppData(programId,channelId, orderId, commissionId, expecetedCom, transactionDate, orderVal, currency, userId);
        }
    }
}

//Exporting selection / all
//goes into windows explorer to get the path to save
void P_SupportPage::on_PB_ExportList_clicked()
{

    //Checks if the Table has at least one Entry
    if (ui->T_NachbuchungsanfragenListe->rowCount() < 1) {
        QMessageBox::information(this, tr("No Contents"), tr("There are no Contents in the Table"));
        return;
    }

    if (ui->T_NachbuchungsanfragenListe->rowCount() == 1){
        ui->T_NachbuchungsanfragenListe->selectAll();
    }
    else if (!ui->T_NachbuchungsanfragenListe->selectionModel()->hasSelection()) {

        QMessageBox::StandardButton reply;
        reply = QMessageBox::question(this, "Export all Elements to CSV", "Are you sure you want to export all Table Contents into a CSV?",
                                      QMessageBox::Yes|QMessageBox::Cancel);

        if (reply == QMessageBox::Yes) {

            ui->T_NachbuchungsanfragenListe->selectAll();
        }
        else {
            return;
        }
    }
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save CSV"), QDir::homePath(),tr("CSV Files (*.csv"));
    if(!fileName.isEmpty()){
        if(!fileName.endsWith(".csv",Qt::CaseInsensitive)){
            fileName+= ".csv";
        }
        outputRowsToCSV(fileName);
    }
}

//Deleting selection / all
void P_SupportPage::on_pb_deleteAll_clicked()
{
    QJsonDocument suppData = dataManager->json->load("NetworkSuppAnswers");
    QJsonObject jObj;

    if (!suppData.isNull()) {
        jObj = suppData.object();
    } else {
        // Handle error: JSON data is null
        QMessageBox::critical(this, tr("Error"), tr("Failed to load data."));
        return;
    }

    // Checks if the Table has at least one Entry
    if (ui->T_NachbuchungsanfragenListe->rowCount() < 1) {
        QMessageBox::information(this, tr("No Contents"), tr("There are no Contents in the Table"));
        return;
    }

    QMessageBox::StandardButton reply;
    if (!ui->T_NachbuchungsanfragenListe->selectionModel()->hasSelection()) {
        // If nothing is selected, ask if the user wants to delete all contents
        reply = QMessageBox::question(this, "Delete all Contents", "Are you sure you want to delete all the Contents?",
                                      QMessageBox::Yes | QMessageBox::Cancel);
        if (reply == QMessageBox::Yes) {
            ui->T_NachbuchungsanfragenListe->selectAll(); // Select all rows to delete
        } else {
            return; // Exit if the user decides not to delete
        }
    } else {
        // Confirm deletion of selected contents
        reply = QMessageBox::question(this, "Delete Selected Contents", "Are you sure you want to delete the selected Content/s?",
                                      QMessageBox::Yes | QMessageBox::Cancel);
        if (reply != QMessageBox::Yes) {
            return; // Exit if the user decides not to delete
        }
    }

    QModelIndexList rows = ui->T_NachbuchungsanfragenListe->selectionModel()->selectedRows();
    // Sort QModelIndexList in descending order by row numbers
    std::sort(rows.begin(), rows.end(), [](const QModelIndex &a, const QModelIndex &b) {
        return a.row() > b.row();
    });

    for (const QModelIndex &index : rows) {
        int i = index.row();
        QString userId = ui->T_NachbuchungsanfragenListe->item(i, eCol_UserId)->text();
        QString orderId = ui->T_NachbuchungsanfragenListe->item(i, eCol_OrderId)->text();

        if (jObj.contains(userId)) {
            QJsonObject userObj = jObj.value(userId).toObject();
            QJsonArray ordersArray = userObj["orders"].toArray();

            for (int v = ordersArray.size() - 1; v >= 0; --v) {
                QJsonObject orderObj = ordersArray.at(v).toObject();
                if (orderObj["orderID"].toString() == orderId) {
                    ordersArray.removeAt(v);
                    break; // Assuming orderIDs are unique and only one needs to be removed
                }
            }

            userObj["orders"] = ordersArray;
            jObj[userId] = userObj; // Update the modified user object back into the main JSON object
        }

        ui->T_NachbuchungsanfragenListe->removeRow(i);
    }

    // Save the modified JSON object back to the file
    QJsonDocument suppdoc(jObj);
    dataManager->json->save("NetworkSuppAnswers", suppdoc);
}


/*
 *
 * SLOTS - LineEdits
 *
 */

void P_SupportPage::on_LE_expectedProv_Percent_editingFinished()
{
    double valueCalc = ui->LE_value->text().toDouble()*(ui->LE_expectedProv_Percent->text().toDouble()/100);
    ui->LE_expectedProv_Currency->setText(QString::number(valueCalc,'f',2));
}

void P_SupportPage::on_LE_value_editingFinished()
{
    if(ui->RB_expProvPer->isChecked()){
        on_LE_expectedProv_Percent_editingFinished();
    }
}

/*
 *
 * SLOTS - RadioButtons
 *
 */

void P_SupportPage::on_RB_expProvCur_clicked()
{
    ui->LE_expectedProv_Percent->clear();
    ui->LE_expectedProv_Percent->setEnabled(false);
    ui->LE_expectedProv_Currency->setEnabled(true);
}

void P_SupportPage::on_RB_expProvPer_clicked()
{
    ui->LE_expectedProv_Currency->clear();
    ui->LE_expectedProv_Percent->setEnabled(true);
    ui->LE_expectedProv_Currency->setEnabled(false);
}

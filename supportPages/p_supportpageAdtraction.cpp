#include "supportPages/p_supportpageAdtraction.h"
#include "DataManager/adtractionsuppdatamanager.h"
#include "ui_p_supportpageAdtraction.h"

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

P_SupportPageAdtraction::P_SupportPageAdtraction(DataManager* dataManager, APIManager* apiManager,QWidget *parent) :
    NetworkPageBase(parent),
    ui(new Ui::P_SupportPageAdtraction),
    dataManager(dataManager),
    apiManager(apiManager)
{
    ui->setupUi(this);

    suppdataManager = new AdtractionSuppDataManager(dataManager);

    //init the whole Page (has to be on a delay, otherwise problem with fileloading)
    QTimer::singleShot(300, this, &P_SupportPageAdtraction::initPage);

    //Eventbus connection
    QObject::connect(SuppEventBus::instance(), &SuppEventBus::eventPublished, [this](const QString& eventName, const QVariant& eventData1, const QVariant& eventData2, const QVariant& eventData3) {
        if (eventName == "networkResponse") {
            QString responseCode = eventData1.toString();
            QString userId = eventData2.toString();
            QString orderId = eventData3.toString();

            //qDebug() << "Received networkResponse:" << responseCode;
            networkRequestMessageReceived(responseCode, userId, orderId);
        }
    });
    QObject::connect(SuppEventBus::instance(), &SuppEventBus::eventPublished, [this](const QString& eventName){
        if (eventName == "currenciesUpdated") {
            fillCurrencyComboBox();
        }
    });


}

//Destructor
P_SupportPageAdtraction::~P_SupportPageAdtraction()
{
    delete ui;
}

/*
 *
 * INIT - Stuff
 *
 */

//Inits all the Elements on the page
void P_SupportPageAdtraction::initPage()
{
    //set the adtraction logo on top of the input fields
    QPixmap networkPixmap(":/Logos/img/adtractionLogo_trans.png");
    networkPixmap = networkPixmap.scaledToHeight(20*1.33, Qt::SmoothTransformation); //scale it down to 20pt
    ui->L_NetworkNamePic->setPixmap(networkPixmap);


    fillCurrencyComboBox();
    fillNetworkComboBox();

    initTable();
    initInputElements();

    setupComboBoxConnections();

    ui->CB_Network->activated(0);
    ui->CB_shop->activated(ui->CB_shop->currentIndex());

    QTimer::singleShot(300, this, &P_SupportPageAdtraction::fillTableWithJson);
}

//Inits the Table (Column Size)
void P_SupportPageAdtraction::initTable()
{
    initializeColumnSortStates();

    //standard size is 150 for the columns
    for(int i=0;i<ui->T_NachbuchungsanfragenListe->columnCount();++i){
        ui->T_NachbuchungsanfragenListe->setColumnWidth(i,150);
    }

    //rest is seted seperately
    //ui->T_NachbuchungsanfragenListe->setColumnWidth(eCol_Network,80);
    ui->T_NachbuchungsanfragenListe->setColumnWidth(eCol_Channel,80);
    ui->T_NachbuchungsanfragenListe->setColumnWidth(eCol_Shop,175);
    ui->T_NachbuchungsanfragenListe->setColumnWidth(eCol_Value,80);
    ui->T_NachbuchungsanfragenListe->setColumnWidth(eCol_ExpProv,80);
    ui->T_NachbuchungsanfragenListe->setColumnWidth(eCol_Currency,60);
    ui->T_NachbuchungsanfragenListe->setColumnWidth(eCol_OrderId,80);
    ui->T_NachbuchungsanfragenListe->setColumnWidth(eCol_UserId,100);
    ui->T_NachbuchungsanfragenListe->setColumnWidth(eCol_Date,75);
    ui->T_NachbuchungsanfragenListe->setColumnWidth(eCol_CommissionText,100);
    ui->T_NachbuchungsanfragenListe->setColumnWidth(eCol_DaysOld,75);
    ui->T_NachbuchungsanfragenListe->setColumnWidth(eCol_Networkstatus,90);
    ui->T_NachbuchungsanfragenListe->setColumnWidth(eCol_SendBTN,70);
    ui->T_NachbuchungsanfragenListe->setColumnWidth(eCol_DeleteBTN,70);

    //Hidden Column for Internal Stats
    ui->T_NachbuchungsanfragenListe->hideColumn(eCol_H_Nstat);
    ui->T_NachbuchungsanfragenListe->hideColumn(eCol_H_Network);
    ui->T_NachbuchungsanfragenListe->hideColumn(eCol_H_ProgramId);
    ui->T_NachbuchungsanfragenListe->hideColumn(eCOl_H_CommissionId);
}

//Sets up all the input elements
void P_SupportPageAdtraction::initInputElements()
{
    QRegularExpressionValidator *validator = new QRegularExpressionValidator(QRegularExpression("^\\d*\\.?\\d*$"), this);

    ui->LE_value->setValidator(validator);
    ui->LE_expectedProv_Currency->setValidator(validator);
    ui->LE_expectedProv_Percent->setValidator(validator);

    ui->RB_expProvCur->setChecked(true);
    ui->LE_expectedProv_Percent->setEnabled(false);

    ui->DE_transactionDate->setDate(QDate::currentDate());
    ui->DE_transactionDate->setMaximumDate(QDate::currentDate());

    addKeyWordMapping("UserID"      ,eCol_UserId);
    addKeyWordMapping("Currency"    ,eCol_Currency);
    addKeyWordMapping("Value"       ,eCol_Value);
    addKeyWordMapping("OrderID"     ,eCol_OrderId);
    addKeyWordMapping("Date"        ,eCol_Date);

    StandardButtons stdBTNs;

    stdBTNs.std_addToListBTN    = ui->PB_AddToList;
    stdBTNs.std_deleteAllBTN    = ui->pb_deleteAll;
    stdBTNs.std_exportToCSV     = ui->PB_ExportList;
    stdBTNs.std_select_Days     = ui->pb_select_90Days;
    stdBTNs.std_select_Green    = ui->pb_select_Green;
    stdBTNs.std_select_Orange   = ui->pb_select_Orange;
    stdBTNs.std_select_Red      = ui->pb_select_Red;
    stdBTNs.std_sendOverAPI     = ui->PB_SendOverAPI;

    initStandardButtons(stdBTNs);
}
//Fills the Network Combobox with all the ChannelIDs Inputted in the Adtraction->ChannelIds Window
void P_SupportPageAdtraction::fillNetworkComboBox()
{
    ui->CB_Network->clear();
    channelToId.clear();
    idtoChannel.clear();

    QList<AdtractionNetworkChannelData> networksChannel = suppdataManager->getAdtractionNetworkChannels();

    QStringList cbItems;
    // Iterate through each channel in the network

    for(AdtractionNetworkChannelData networkData : networksChannel){
        cbItems.append(networkData.channelName);
        channelToId.insert(networkData.channelName,networkData.channelID);
        idtoChannel.insert(networkData.channelID,networkData.channelName);
    }

    cbItems.sort(Qt::CaseInsensitive);
    ui->CB_Network->addItem("All");

    for(QString &cbItem : cbItems){
        QString extraText = channelToId.value(cbItem);
        ui->CB_Network->addItem(cbItem, QVariant(extraText));
    }
}

//Fills the shop Combobox with the shops from Adtraction
void P_SupportPageAdtraction::fillShopComboBox(QString &fi_channel, QString &fi_channelID)
{
    ui->CB_shop->clear();
    QList<Shop> shops;
    QMap<QString, QString> channelIdToName; // For mapping channel IDs to names

    if(fi_channel == "All"){
        QList<AdtractionNetworkChannelData> networksChannel = suppdataManager->getAdtractionNetworkChannels();

        for (const AdtractionNetworkChannelData &networkChannel : networksChannel) {
            channelIdToName[networkChannel.channelID] = networkChannel.channelName; // Store channel names
            QList<Shop> channelShops = suppdataManager->getShopsByChannelID(networkChannel.channelID);
            shops.append(channelShops);
        }
    } else {
        shops = suppdataManager->getShopsByChannelID(fi_channelID);
        // Optionally populate channelIdToName for the single channel case if needed
        channelIdToName[fi_channelID] = fi_channel; // Assuming fi_channel is the name, not ID here
    }

    // Sort the shops list by programName
    std::sort(shops.begin(), shops.end(), [](const Shop &a, const Shop &b) {
        return a.programName.compare(b.programName, Qt::CaseInsensitive) < 0;
    });

    // Now add sorted items to the combobox
    for (const Shop &shop : shops) {
        QString channelName = (fi_channel == "All") ? channelIdToName[shop.channelID] : fi_channel;
        QString itemText = QString("%1: (%2)").arg(shop.programName, channelName);
        ui->CB_shop->addItem(itemText, QVariant::fromValue(shop));
    }
}


//Fills the Currency Combobox with all the Currencies from Adtraction
void P_SupportPageAdtraction::fillCurrencyComboBox()
{
    QStringList sortedCurrencies = suppdataManager->getSortedCurrencies();

    for(const QString& currency : sortedCurrencies){
        ui->CB_Currency->addItem(currency);
    }
}

//Sets up the Connections from the ShopCombobox to the CommissionId and the CommissionId to the Expected Commission [Currency] / %
void P_SupportPageAdtraction::setupComboBoxConnections()
{

    connect(ui->CB_shop, QOverload<int>::of(&QComboBox::activated),[this](int index) {

        QVariant variant = ui->CB_shop->itemData(index);
        Shop selectedShop = variant.value<Shop>();

        ui->CB_ComissionID->clear();
        for (const Commission& commission : selectedShop.commissions) {
            QString itemText = QString("%1: %2").arg(commission.name, commission.id);
            ui->CB_ComissionID->addItem(itemText, QVariant::fromValue(commission));
        }
        ui->CB_ComissionID->activated(ui->CB_ComissionID->currentIndex());
    });

    connect(ui->CB_ComissionID, QOverload<int>::of(&QComboBox::activated), [this](int index) {

        QVariant variant = ui->CB_ComissionID->itemData(index);
        Commission commission = variant.value<Commission>();

        if (commission.type == "%") {
            ui->LE_expectedProv_Percent->setText(QString::number(commission.value));
            ui->LE_expectedProv_Percent->editingFinished(); // Trigger any related updates
            ui->RB_expProvPer->click();
            ui->LE_value->editingFinished();
        } else {
            ui->LE_expectedProv_Currency->setText(QString::number(commission.value));
            ui->LE_expectedProv_Currency->editingFinished(); // Trigger any related updates
            ui->RB_expProvCur->click();
        }
    });

    connect(ui->CB_Network, QOverload<int>::of(&QComboBox::activated), [this](int index) {

        QVariant channelIdInvisible = ui->CB_Network->itemData(index,Qt::UserRole);
        QString channelId = channelIdInvisible.toString();
        QString channel = ui->CB_Network->itemText(index);

        fillShopComboBox(channel,channelId);
        ui->CB_shop->activated(0);
    });
}

//on Startup fills the Table with the last session / if not deleted
void P_SupportPageAdtraction::fillTableWithJson() {

    ui->T_NachbuchungsanfragenListe->setRowCount(0); // Clear existing rows

    QList<AdtractionSuppCase> suppCases = suppdataManager->loadAllObjectsFromSessionFile();

    for (auto suppCase : suppCases) {

        addItemToTable(suppCase);
    }
}

/*
 *
 * Public Functions
 *
 */

void P_SupportPageAdtraction::refreshNetworkList()
{
    fillNetworkComboBox();
}

void P_SupportPageAdtraction::refreshShops()
{
    ui->CB_Network->setCurrentIndex(0);
    ui->CB_Network->activated(0);
}

/*
 *
 * Private Functions
 *
 */

suppNetStatus P_SupportPageAdtraction::convertNetworkStatusCodeToNstat(const QString& suppNetStatString)
{
    if (suppNetStatString == "0") {
        return eNstat_NotSend;
    } else if (suppNetStatString == "200") {
        return eNstat_Good;
    } else if (suppNetStatString == "201" || suppNetStatString == "204" || suppNetStatString == "409") {
        return eNstat_Okay;
    } else{
        return eNstat_Error;
    }
}

void P_SupportPageAdtraction::addItemToTable(const AdtractionSuppCase &suppCase)
{
    ui->T_NachbuchungsanfragenListe->blockSignals(true);

    int newRow = ui->T_NachbuchungsanfragenListe->rowCount();
    ui->T_NachbuchungsanfragenListe->insertRow(newRow);

    // Create and set QTableWidgetItem for each detail of suppCase
    ui->T_NachbuchungsanfragenListe->setItem(newRow, eCol_Channel,          new QTableWidgetItem(suppCase.getChannel()));
    ui->T_NachbuchungsanfragenListe->setItem(newRow, eCol_Shop,             new QTableWidgetItem(suppCase.getShop()));
    ui->T_NachbuchungsanfragenListe->setItem(newRow, eCol_Value,            new QTableWidgetItem(QString::number(suppCase.getOrderVal())));
    ui->T_NachbuchungsanfragenListe->setItem(newRow, eCol_ExpProv,          new QTableWidgetItem(QString::number(suppCase.getExpProv())));
    ui->T_NachbuchungsanfragenListe->setItem(newRow, eCol_Currency,         new QTableWidgetItem(suppCase.getCurrency()));
    ui->T_NachbuchungsanfragenListe->setItem(newRow, eCol_OrderId,          new QTableWidgetItem(suppCase.getOrderId()));
    ui->T_NachbuchungsanfragenListe->setItem(newRow, eCol_UserId,           new QTableWidgetItem(suppCase.getUserId()));
    ui->T_NachbuchungsanfragenListe->setItem(newRow, eCol_Date,             new QTableWidgetItem(suppCase.getDate()));
    ui->T_NachbuchungsanfragenListe->setItem(newRow, eCol_CommissionText,   new QTableWidgetItem(suppCase.getCommissionText()));

    ui->T_NachbuchungsanfragenListe->setItem(newRow, eCol_H_Nstat,          new QTableWidgetItem(suppCase.getNetworkStatus()));
    ui->T_NachbuchungsanfragenListe->setItem(newRow, eCol_H_Network,        new QTableWidgetItem(suppCase.getNetwork()));
    ui->T_NachbuchungsanfragenListe->setItem(newRow, eCol_H_ProgramId,      new QTableWidgetItem(suppCase.getProgramId()));
    ui->T_NachbuchungsanfragenListe->setItem(newRow, eCOl_H_CommissionId,   new QTableWidgetItem(suppCase.getCommissionId()));

    QDate suppDate = QDate::fromString(suppCase.getDate(),"dd.MM.yyyy");
    QString daysOld = QString::number(suppDate.daysTo(QDate::currentDate()));
    ui->T_NachbuchungsanfragenListe->setItem(newRow, eCol_DaysOld, new QTableWidgetItem(daysOld));

    // Adjust addNStatButton and action button adding methods as needed
    addNStatButton(ui->T_NachbuchungsanfragenListe, newRow, eCol_Networkstatus, convertNetworkStatusCodeToNstat(suppCase.getNetworkStatus()), suppCase.getNetworkStatusText());

    QPushButton *sendBTN = new QPushButton;
    QPixmap sendPixmap(":/img/img/Send_trans.png");
    QIcon sendIcon(sendPixmap);
    sendBTN->setIcon(sendIcon);
    ui->T_NachbuchungsanfragenListe->setCellWidget(newRow,eCol_SendBTN,sendBTN);

    QPushButton *deleteBTN = new QPushButton;
    QPixmap xPixmap(":/img/img/Kreuz_trans.png");
    QIcon xIcon(xPixmap);
    deleteBTN->setIcon(xIcon);
    ui->T_NachbuchungsanfragenListe->setCellWidget(newRow,eCol_DeleteBTN,deleteBTN);

    disableEditingForRow(newRow);

    QObject::connect(deleteBTN, &QPushButton::clicked, this, &P_SupportPageAdtraction::on_deleteBTNTable_clicked);
    QObject::connect(sendBTN, &QPushButton::clicked, this, &P_SupportPageAdtraction::on_sendBTNTable_clicked);

    ui->T_NachbuchungsanfragenListe->blockSignals(false);
}

bool P_SupportPageAdtraction::addItemToSessionJson(const AdtractionSuppCase &suppCase)
{
    return suppdataManager->saveObjectToSessionFile(suppCase);
}

void P_SupportPageAdtraction::disableEditingForRow(const int currentRow)
{
    QList<int> columnsToDisable = {eCol_UserId, eCol_OrderId, eCol_Shop, eCol_DaysOld, eCol_Networkstatus, eCol_CommissionText, eCOl_H_CommissionId, eCol_H_Nstat, eCol_H_ProgramId, eCol_H_Network};

    for (int col : columnsToDisable) {
        QTableWidgetItem *item = ui->T_NachbuchungsanfragenListe->item(currentRow, col);
        if (item) { // Make sure the item exists
            Qt::ItemFlags flags = item->flags();
            item->setFlags(flags & ~Qt::ItemIsEditable);
        }
    }

}

void P_SupportPageAdtraction::initializeColumnSortStates()
{
    columnSortStates[1] = ColumnSortState(eCol_Shop);
    columnSortStates[9] = ColumnSortState(eCol_DaysOld);
    columnSortStates[13] = ColumnSortState(eCol_H_Nstat);
}

void P_SupportPageAdtraction::outputRowsToCSV(const QString &fileName)
{
    QList<QTableWidgetSelectionRange> rowSelections = ui->T_NachbuchungsanfragenListe->selectedRanges();
    QList<QStringList> csvData;

    for(const QTableWidgetSelectionRange &rowSelect : rowSelections){
        for(int row = rowSelect.topRow(); row <= rowSelect.bottomRow();++row){

            QStringList rowData;

            //orderValue
            QTableWidgetItem *itemToCSV = ui->T_NachbuchungsanfragenListe->item(row,eCol_Value);
            rowData << (itemToCSV ? itemToCSV->text() : QString());


            //ChannelID
            rowData << channelToId.value(ui->T_NachbuchungsanfragenListe->item(row,eCol_Channel)->text());

            //commissionId
            rowData << ui->T_NachbuchungsanfragenListe->item(row,eCOl_H_CommissionId)->text();

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

//TBD:: OBJECTIFY TBD TBD TBD
//Event after the request was received from the server
void P_SupportPageAdtraction::networkRequestMessageReceived(const QString responseCode, const QString userId, const QString orderId)
{
    QJsonDocument suppAnswers = dataManager->json->load("AdtractionSessionFile");
    QJsonObject jObj;

    QString nreply;

    suppNetStatus netStatForObject;

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

    for (int currentRow = 0; currentRow < ui->T_NachbuchungsanfragenListe->rowCount();++currentRow){
        if (userId == ui->T_NachbuchungsanfragenListe->item(currentRow,eCol_UserId)->text()&& orderId == ui->T_NachbuchungsanfragenListe->item(currentRow,eCol_OrderId)->text()){

            if(responseCode == "200"){
                netStatForObject = eNstat_Good;
            }
            else if(responseCode == "409"|| responseCode == "204" || responseCode == "201"){
                netStatForObject = eNstat_Okay;
            }
            else {
                netStatForObject = eNstat_Error;
            }

            addNStatButton(ui->T_NachbuchungsanfragenListe, currentRow, eCol_Networkstatus, netStatForObject,nreply);
            ui->T_NachbuchungsanfragenListe->item(currentRow,eCol_H_Nstat)->setText(QString::number(netStatForObject));
            break;
        }
    }
}

/*
 *
 * SLOTS - Buttons
 *
 */

//---------------TableStuff----------------

//---Toggle and Sort---
//Toggle specific Columns to be visible
void P_SupportPageAdtraction::on_pb_toggleTable_clicked()
{

    switch(toggleStatusTable){
    case eTT_Normal:

        ui->T_NachbuchungsanfragenListe->setColumnHidden(eCol_Channel,true);
        ui->T_NachbuchungsanfragenListe->setColumnHidden(eCol_ExpProv,true);
        ui->T_NachbuchungsanfragenListe->setColumnHidden(eCol_CommissionText,true);
        ui->T_NachbuchungsanfragenListe->setColumnHidden(eCol_DaysOld,true);

        toggleStatusTable = eTT_Small;
        break;

    case eTT_Small:

        ui->T_NachbuchungsanfragenListe->setColumnHidden(eCol_Channel,false);
        ui->T_NachbuchungsanfragenListe->setColumnHidden(eCol_ExpProv,false);
        ui->T_NachbuchungsanfragenListe->setColumnHidden(eCol_CommissionText,false);
        ui->T_NachbuchungsanfragenListe->setColumnHidden(eCol_DaysOld,false);

        ui->T_NachbuchungsanfragenListe->setColumnHidden(eCol_H_ProgramId,false);
        ui->T_NachbuchungsanfragenListe->setColumnHidden(eCOl_H_CommissionId,false);
        ui->T_NachbuchungsanfragenListe->setColumnHidden(eCol_H_Network,false);

        toggleStatusTable = eTT_Big;
        break;

    case eTT_Big:

        ui->T_NachbuchungsanfragenListe->setColumnHidden(eCol_H_ProgramId,true);
        ui->T_NachbuchungsanfragenListe->setColumnHidden(eCOl_H_CommissionId,true);
        ui->T_NachbuchungsanfragenListe->setColumnHidden(eCol_H_Network,true);

        toggleStatusTable = eTT_Normal;
        break;

    default:
        qDebug()<<"Error in toggleStatus-Statemachine";
        break;
    }
}

void P_SupportPageAdtraction::on_pb_select_90Days_clicked()
{
    int overdueDays = 90;
    selectRowsBasedOnCondition(ui->T_NachbuchungsanfragenListe, eCol_DaysOld, [overdueDays](const QString &value) {
        int days = value.toInt();
        //qDebug() << "Days old:" << days << ", Overdue threshold:" << overdueDays;
        return days >= overdueDays;
    });
}

void P_SupportPageAdtraction::on_pb_select_Red_clicked()
{
    selectRowsBasedOnCondition(ui->T_NachbuchungsanfragenListe, eCol_H_Nstat, [](const QString &value) {
        //qDebug() << "Checking value:" << value << ", Converted to Int:" << value.toInt();
        int valueInt = value.toInt();
        bool isSelected = (valueInt != 0 && valueInt != 200 && valueInt != 201 && valueInt != 204 && valueInt != 409);
        //qDebug() << "Is Selected:" << isSelected;
        return isSelected;
    });
}

void P_SupportPageAdtraction::on_pb_select_Orange_clicked()
{
    selectRowsBasedOnCondition(ui->T_NachbuchungsanfragenListe, eCol_H_Nstat, [](const QString &value) {
        //qDebug() << "Checking value:" << value << ", Converted to Int:" << value.toInt();
        int valueInt = value.toInt();
        bool isSelected = ((valueInt == 201) || (valueInt == 204) || (valueInt == 409));
        //qDebug() << "Is Selected:" << isSelected;
        return isSelected;
    });
}

void P_SupportPageAdtraction::on_pb_select_Green_clicked()
{
    selectRowsBasedOnCondition(ui->T_NachbuchungsanfragenListe, eCol_H_Nstat, [](const QString &value) {
        //qDebug() << "Checking value:" << value << ", Converted to Int:" << value.toInt();
        int valueInt = value.toInt();
        bool isSelected = (valueInt == 200);
        //qDebug() << "Is Selected:" << isSelected;
        return isSelected;
    });

}

void P_SupportPageAdtraction::on_PB_select_search_clicked()
{
    selectSearch(ui->T_NachbuchungsanfragenListe,ui->LE_SearchBar);
}

//Sort by NetworkStatus
void P_SupportPageAdtraction::on_PB_SortNetworkStatus_clicked()
{
    sortColumn(ui->T_NachbuchungsanfragenListe,eCol_H_Nstat);
}

//Sort by Shop
void P_SupportPageAdtraction::on_PB_SortShop_clicked()
{

    sortColumn(ui->T_NachbuchungsanfragenListe,eCol_Shop);
}

//Sort by Date
void P_SupportPageAdtraction::on_PB_SortDate_clicked()
{
    sortColumn(ui->T_NachbuchungsanfragenListe,eCol_DaysOld);
}
//------------------

//Inserting, Sending, Deleting direct out of the table
void P_SupportPageAdtraction::on_PB_AddToList_clicked()
{
    if(ui->LE_orderId->text().isEmpty() || ui->LE_User->text().isEmpty() || ui->LE_value->text().isEmpty() || ui->LE_expectedProv_Currency->text().isEmpty()){

        QMessageBox::critical(this,"Text Boxes Empty","At least one of the text boxes is empty, please fill out every text box!");
    }
    else{

        //bool answer;
        AdtractionParams suppCaseParams;

        suppCaseParams.orderVal         = ui->LE_value->text().toDouble();
        suppCaseParams.expProv          = ui->LE_expectedProv_Currency->text().toDouble();
        suppCaseParams.currency         = ui->CB_Currency->currentText();

        suppCaseParams.orderId          = ui->LE_orderId->text();
        suppCaseParams.userId           = ui->LE_User->text();
        suppCaseParams.date             = ui->DE_transactionDate->date().toString("dd.MM.yyyy");

        suppCaseParams.lastEditDate     = QDate::currentDate().toString();


        suppCaseParams.networkStatus    = "0";
        suppCaseParams.network          = "Adtraction";

        QVariant variant = ui->CB_shop->itemData(ui->CB_shop->currentIndex());
        Shop selectedShop = variant.value<Shop>();

        suppCaseParams.channel          = idtoChannel.value(selectedShop.channelID);
        suppCaseParams.shop             = selectedShop.programName;
        suppCaseParams.programId        = QString::number(selectedShop.programId);
        suppCaseParams.commissionId     = selectedShop.commissions.at(ui->CB_ComissionID->currentIndex()).id;
        suppCaseParams.commissionText   = selectedShop.commissions.at(ui->CB_ComissionID->currentIndex()).name; //TBD:: change name of ComboBox

        AdtractionSuppCase suppCase(suppCaseParams);

        //answer = addItemToSessionJson(suppCase);
        addItemToSessionJson(suppCase);
        //qDebug()<<answer;

        fillTableWithJson();

        //Delete contents
        ui->LE_value->clear();
        ui->LE_orderId->clear();
        ui->LE_User->clear();
        ui->DE_transactionDate->setDate(QDate::currentDate());
    }
}

void P_SupportPageAdtraction::on_deleteBTNTable_clicked()
{
    int currRow = ui->T_NachbuchungsanfragenListe->currentRow();

    QString userId = ui->T_NachbuchungsanfragenListe->item(currRow,eCol_UserId)->text();
    //qDebug()<<"--DELETE--";
    //qDebug()<<userId;
    QString orderId = ui->T_NachbuchungsanfragenListe->item(currRow,eCol_OrderId)->text();
    //qDebug()<<orderId;
    //qDebug()<<"--DELETE--";

    suppdataManager->deleteOrder(userId,orderId);
    ui->T_NachbuchungsanfragenListe->removeRow(currRow);
}

//sending one Element from the Table directly
void P_SupportPageAdtraction::on_sendBTNTable_clicked(){

    int row = ui->T_NachbuchungsanfragenListe->currentRow();

    if(!(ui->T_NachbuchungsanfragenListe->item(row,eCol_H_Nstat)->text().toInt() == eNstat_NotSend ||
          ui->T_NachbuchungsanfragenListe->item(row,eCol_H_Nstat)->text().toInt() == eNstat_Error)){
        QMessageBox::StandardButton reply;
        reply = QMessageBox::question(this, "Send already sent Data", "Are you sure you want to send already sent Data to the network?",
                                      QMessageBox::Yes|QMessageBox::Cancel);

        if (reply == QMessageBox::Yes) {

        }
        else {
            return;
        }
    }

    int programId = ui->T_NachbuchungsanfragenListe->item(row, eCol_H_ProgramId)->text().toInt();

    QString channel = ui->T_NachbuchungsanfragenListe->item(row,eCol_Channel)->text();
    int channelId = channelToId.value(channel).toInt();

    QString orderId= ui->T_NachbuchungsanfragenListe->item(row, eCol_OrderId)->text();
    int commissionId= ui->T_NachbuchungsanfragenListe->item(row, eCOl_H_CommissionId)->text().toInt();
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

void P_SupportPageAdtraction::on_PB_SendOverAPI_clicked()
{
    bool filter = true;
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
        reply = QMessageBox::question(this, "Send all unsent contents to the network", "Are you sure you want to send all the unsent contents to the network?",
                                      QMessageBox::Yes|QMessageBox::Cancel);

        if (reply == QMessageBox::Yes) {
            ui->T_NachbuchungsanfragenListe->selectAll();
            filter = true;
        }
        else {
            return;
        }
    }
    else{
        filter = false;
    }

    QList<QTableWidgetSelectionRange> rowSelections = ui->T_NachbuchungsanfragenListe->selectedRanges();

    for(const QTableWidgetSelectionRange &rowSelect : rowSelections){
        for(int row = rowSelect.topRow(); row <= rowSelect.bottomRow();++row){
            if(filter){
                if(!(ui->T_NachbuchungsanfragenListe->item(row,eCol_H_Nstat)->text().toInt() == eNstat_NotSend)){
                    continue;
                }
            }
            else if(!(ui->T_NachbuchungsanfragenListe->item(row,eCol_H_Nstat)->text().toInt() == eNstat_NotSend ||
                       ui->T_NachbuchungsanfragenListe->item(row,eCol_H_Nstat)->text().toInt() == eNstat_Error)){
                continue;
            }

            int programId = ui->T_NachbuchungsanfragenListe->item(row, eCol_H_ProgramId)->text().toInt();

            QString channel = ui->T_NachbuchungsanfragenListe->item(row,eCol_Channel)->text();
            int channelId = channelToId.value(channel).toInt();

            QString orderId= ui->T_NachbuchungsanfragenListe->item(row, eCol_OrderId)->text();
            int commissionId= ui->T_NachbuchungsanfragenListe->item(row, eCOl_H_CommissionId)->text().toInt();
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

//TBD:: OBJECTIFY
void P_SupportPageAdtraction::on_PB_ExportList_clicked()
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

    QFileDialog dialog(this);
    dialog.setFileMode(QFileDialog::AnyFile);           // Allows selection of any file, existing or not
    dialog.setAcceptMode(QFileDialog::AcceptSave);      // Set the dialog to save mode
    dialog.setWindowTitle(tr("Save CSV"));
    dialog.setDirectory(QDir::homePath());
    dialog.setNameFilter(tr("CSV Files (*.csv)"));
    dialog.setDefaultSuffix("csv");                     // Ensure the file is saved with a .csv extension

    if(dialog.exec() == QDialog::Accepted) {
        QString fileName = dialog.selectedFiles().first();
        if (!fileName.isEmpty()) {
            // Check if the file ends with .csv, add if not
            if (!fileName.endsWith(".csv", Qt::CaseInsensitive)) {
                fileName += ".csv";
            }
            outputRowsToCSV(fileName);
        }
    }
}

//Deleting selection / all

void P_SupportPageAdtraction::on_pb_deleteAll_clicked()
{

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

        suppdataManager->deleteOrder(userId,orderId);
        ui->T_NachbuchungsanfragenListe->removeRow(i);
    }
}

/*
 *
 * SLOTS - LineEdits
 *
 */

void P_SupportPageAdtraction::on_LE_expectedProv_Percent_editingFinished()
{
    double valueCalc = ui->LE_value->text().toDouble()*(ui->LE_expectedProv_Percent->text().toDouble()/100);
    ui->LE_expectedProv_Currency->setText(QString::number(valueCalc,'f',2));
}

void P_SupportPageAdtraction::on_LE_value_editingFinished()
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

void P_SupportPageAdtraction::on_RB_expProvCur_clicked()
{
    ui->LE_expectedProv_Percent->clear();
    ui->LE_expectedProv_Percent->setEnabled(false);
    ui->LE_expectedProv_Currency->setEnabled(true);
}

void P_SupportPageAdtraction::on_RB_expProvPer_clicked()
{
    ui->LE_expectedProv_Currency->clear();
    ui->LE_expectedProv_Percent->setEnabled(true);
    ui->LE_expectedProv_Currency->setEnabled(false);
}

void P_SupportPageAdtraction::on_LE_SearchBar_textChanged(const QString &arg1)
{
    searchBarActivity(ui->T_NachbuchungsanfragenListe, arg1);
}



void P_SupportPageAdtraction::on_T_NachbuchungsanfragenListe_itemChanged(QTableWidgetItem *item)
{
    int currentCol = item->column();
    int currentRow = item->row();
    bool editCorrect = false;

    QRegularExpression regExp("^\\d+(\\.\\d+)?$");
    QRegularExpression dateRegExp("^\\d{2}\\.\\d{2}\\.\\d{4}$");

    switch (currentCol) {

    case eCol_Channel: {

        bool isEqualToOneItem = false;
        QJsonDocument networksDoc= suppdataManager->getNetworkChannelsDoc();
        QJsonObject networksObj = networksDoc.object();
        QJsonObject channelsObj = networksObj["Adtraction"].toObject();

        for (auto channel = channelsObj.begin(); channel != channelsObj.end(); ++channel) {

            QJsonObject channelDetails = channel.value().toObject();

            QString channelName = channelDetails["channelName"].toString();

            if(item->text() == channelName){
                isEqualToOneItem = true;
                break;
            }
        }

        if(!isEqualToOneItem){
            QMessageBox::critical(this,"Does not match channels","The input does not match previously input Channels (NetworkChannels)");
        }
        else{
            editCorrect = true;
        }

        break;
    }
    case eCol_Value:
    case eCol_ExpProv: {
        // Check if the item's text matches the required format
        QRegularExpressionMatch match = regExp.match(item->text());
        if (!match.hasMatch()) {
            QMessageBox::critical(this, "Invalid Format", "The input must be a number with or without decimal points (e.g., 9, 8.75, 66.50).");
        } else {
            editCorrect = true;
        }
        break;
    }

    case eCol_Currency: {
        QStringList currencies = suppdataManager->getSortedCurrencies();

        // Check if the entered currency is in the list of valid currencies
        if (!currencies.contains(item->text())) {
            QMessageBox::critical(this, "Invalid Currency", "The entered currency is not valid. Please enter a currency from the list.");

        } else {
            editCorrect = true; // Mark the edit as correct
        }
        break;
    }

    case eCol_Date: {
        // Regular expression for date format dd.mm.yyyy
        QRegularExpression dateRegExp("^\\d{2}\\.\\d{2}\\.\\d{4}$");
        QRegularExpressionMatch match = dateRegExp.match(item->text());

        if (!match.hasMatch()) {
            QMessageBox::critical(this, "Invalid Date Format", "The input must be a date in the format dd.mm.yyyy (e.g., 31.12.2024).");
        } else {
            // Check if the date is valid and not in the future
            QDate date = QDate::fromString(item->text(), "dd.MM.yyyy");
            QDate today = QDate::currentDate();
            if (!date.isValid()) {
                QMessageBox::critical(this, "Invalid Date", "The entered date does not exist. Please enter a valid date in the format dd.mm.yyyy.");
            } else if (date > today) {
                QMessageBox::critical(this, "Date in the Future", "The entered date cannot be in the future. Please enter a date that is today or in the past.");
            } else {
                editCorrect = true;
            }
        }
        break;
    }

    default:
        break;
    }

    if(editCorrect){


        AdtractionParams suppCaseParams;

        suppCaseParams.orderVal         = ui->T_NachbuchungsanfragenListe->item(currentRow,eCol_Value)->text().toDouble();
        suppCaseParams.expProv          = ui->T_NachbuchungsanfragenListe->item(currentRow,eCol_ExpProv)->text().toDouble();
        suppCaseParams.currency         = ui->T_NachbuchungsanfragenListe->item(currentRow,eCol_Currency)->text();

        suppCaseParams.orderId          = ui->T_NachbuchungsanfragenListe->item(currentRow,eCol_OrderId)->text();
        suppCaseParams.userId           = ui->T_NachbuchungsanfragenListe->item(currentRow,eCol_UserId)->text();
        suppCaseParams.date             = ui->T_NachbuchungsanfragenListe->item(currentRow,eCol_Date)->text();

        suppCaseParams.lastEditDate     = QDate::currentDate().toString();


        suppCaseParams.networkStatus    = ui->T_NachbuchungsanfragenListe->item(currentRow,eCol_H_Nstat)->text();
        suppCaseParams.network          = ui->T_NachbuchungsanfragenListe->item(currentRow,eCol_H_Network)->text();

        QVariant variant = ui->CB_shop->itemData(ui->CB_shop->currentIndex());
        Shop selectedShop = variant.value<Shop>();

        suppCaseParams.channel          = ui->T_NachbuchungsanfragenListe->item(currentRow,eCol_Channel)->text();
        suppCaseParams.shop             = ui->T_NachbuchungsanfragenListe->item(currentRow,eCol_Shop)->text();
        suppCaseParams.programId        = ui->T_NachbuchungsanfragenListe->item(currentRow,eCol_H_ProgramId)->text();
        suppCaseParams.commissionId     = ui->T_NachbuchungsanfragenListe->item(currentRow,eCOl_H_CommissionId)->text();
        suppCaseParams.commissionText   = ui->T_NachbuchungsanfragenListe->item(currentRow,eCol_CommissionText)->text();

        AdtractionSuppCase suppCase(suppCaseParams);

        QTimer::singleShot(0, [this, suppCase = std::move(suppCase)]() mutable {
            addItemToSessionJson(suppCase);
            fillTableWithJson();
        });
    }
    else{
        QTimer::singleShot(0, [this]() { fillTableWithJson(); });
    }
}


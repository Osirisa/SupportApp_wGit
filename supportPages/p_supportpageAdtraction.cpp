
#include "supportPages/p_supportpageAdtraction.h"
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
    QWidget(parent),
    ui(new Ui::P_SupportPageAdtraction),
    dataManager(dataManager),
    apiManager(apiManager)
{
    ui->setupUi(this);

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

    QPixmap networkPixmap(":/Logos/img/adtractionLogo_trans.png");
    networkPixmap = networkPixmap.scaledToHeight(20*1.33, Qt::SmoothTransformation); //scale it down to 20pt
    ui->L_NetworkNamePic->setPixmap(networkPixmap);

    //Fill Comboboxes

    //In Adtraction there is only one type (maybe delete completely?
    //TBD:
    ui->CB_SuppType->addItem("Untracked");

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
    ui->T_NachbuchungsanfragenListe->setColumnWidth(eCol_CommissionType,90);
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

    //send over API Button
    QPixmap pm_soaButton(":/img/img/arrowCircle_trans.png");
    QIcon i_soaButton(pm_soaButton);
    ui->PB_SendOverAPI->setIcon(i_soaButton);
    ui->PB_SendOverAPI->setToolTip("Sends either all or only selected Contents of the Table");

    //select "Red" Button
    QPixmap pm_select_Red(":/img/img/ausrufezeichen_trans.png");
    QIcon i_select_Red(pm_select_Red);
    ui->pb_select_Red->setIcon(i_select_Red);
    ui->pb_select_Red->setToolTip("Selects all the not accepted Supports");

    //select "Orange" Button
    QPixmap pm_select_Orange(":/img/img/ausrufezeichen_orange_trans.png");
    QIcon i_select_Orange(pm_select_Orange);
    ui->pb_select_Orange->setIcon(i_select_Orange);
    ui->pb_select_Orange->setToolTip("Selects all the accepted but not good Supports");

    //select "Green" Button
    QPixmap pm_select_Green(":/img/img/ausrufezeichen_green_trans.png");
    QIcon i_select_Green(pm_select_Green);
    ui->pb_select_Green->setIcon(i_select_Green);
    ui->pb_select_Green->setToolTip("Selects all the accepted Supports");

    //select "90Days" Button
    QPixmap pm_select_90Days(":/img/img/Uhr_trans.png");
    QIcon i_select_90Days(pm_select_90Days);
    ui->pb_select_90Days->setIcon(i_select_90Days);
    ui->pb_select_90Days->setToolTip("Selects all the Supports which are older than 90 Days");
}
//Fills the Network Combobox with all the ChannelIDs Inputted in the Adtraction->ChannelIds Window
void P_SupportPageAdtraction::fillNetworkComboBox()
{
    ui->CB_Network->clear();

    QJsonDocument networksDoc = dataManager->json->load("NetworkChannels");
    QJsonObject networksObj = networksDoc.object();

    // Each network's key is the network name, and its value is another JSON object containing channels
    QJsonObject channelsObj = networksObj["Adtraction"].toObject();

    QStringList cbItems;
    QHash<QString,QString> channelChannelIDMap;
    // Iterate through each channel in the network
    for (auto channel = channelsObj.begin(); channel != channelsObj.end(); ++channel) {
        // Each channel's key is the channel ID, and its value is another JSON object containing channel details
        QJsonObject channelDetails = channel.value().toObject();

        // Extract channel details
        QString channelID = channel.key();
        QString channelName = channelDetails["channelName"].toString();

        cbItems.append(channelName);
        channelChannelIDMap.insert(channelName,channelID);
        }

    cbItems.sort(Qt::CaseInsensitive);
    ui->CB_Network->addItem("All");

    for(QString &cbItem : cbItems){
        QString extraText = channelChannelIDMap.value(cbItem);
        ui->CB_Network->addItem(cbItem, QVariant(extraText));
    }
}

//Fills the shop Combobox with the shops from Adtraction
void P_SupportPageAdtraction::fillShopComboBox(QString &fi_channel, QString &fi_channelID)
{
    QStringList comboBoxItems;

    channelToId.clear();

    ui->CB_shop->clear();
    if(fi_channel == "All"){

        QJsonDocument networksDoc = dataManager->json->load("NetworkChannels");
        QJsonObject networksObj = networksDoc.object();
        QJsonObject channelsObj = networksObj["Adtraction"].toObject();

        for (auto channel = channelsObj.begin(); channel != channelsObj.end(); ++channel) {
            // Each channel's key is the channel ID, and its value is another JSON object containing channel details
            QJsonObject channelDetails = channel.value().toObject();

            // Extract channel details
            QString channelID = channel.key();
            QString channelName = channelDetails["channelName"].toString();

            QJsonDocument channelShopsDoc = dataManager->json->load("Adtraction"+channelID);

            for(const QJsonValue &value : channelShopsDoc.array()){
                QJsonObject obj = value.toObject();
                shopToProgramIdHash.insert(obj["programName"].toString(), obj["programId"].toInt());

                // Collect items in the list
                comboBoxItems.append(obj["programName"].toString() + ": (" + channelName + ")");

            }
            channelToId.insert(channelName,channelID);
            allDocs.insert(channelID,channelShopsDoc);
        }
    }
    else{
        QJsonDocument channelShopsDoc = dataManager->json->load("Adtraction"+fi_channelID);

        for(const QJsonValue &value : channelShopsDoc.array()){
            QJsonObject obj = value.toObject();
            shopToProgramIdHash.insert(obj["programName"].toString(), obj["programId"].toInt());

            // Collect items in the list
            comboBoxItems.append(obj["programName"].toString() + ": (" + fi_channel + ")");
        }

        channelToId.insert(fi_channel,fi_channelID);
    }

    std::sort(comboBoxItems.begin(), comboBoxItems.end(), [](const QString &a, const QString &b) {
        return QString::compare(a, b, Qt::CaseInsensitive) < 0;
    });

    // Add sorted items to the combo box
    for (const QString &item : comboBoxItems) {
        ui->CB_shop->addItem(item);
    }
}

//Fills the Currency Combobox with all the Currencies from Adtraction
void P_SupportPageAdtraction::fillCurrencyComboBox()
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

//Sets up the Connections from the ShopCombobox to the CommissionId and the CommissionId to the Expected Commission [Currency] / %
void P_SupportPageAdtraction::setupComboBoxConnections()
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

                //fill AD_advertData for later (filling the Table)
                aD_advertData.programName = programObj["programName"].toString();
                aD_advertData.programId = programObj["programId"].toInt();
                aD_advertData.commissions.clear();

                QJsonArray commissions = programObj["commissions"].toArray();
                for (const QJsonValue& value : commissions) {

                    QJsonObject obj = value.toObject();
                    Commission aD_commission;

                    aD_commission.id    = obj["id"].toString();
                    aD_commission.name  = obj["name"].toString();

                    QString itemId      = aD_commission.id;
                    QString itemName    = aD_commission.name;
                    QString itemText = QString("%1: %2").arg(itemName, itemId);

                    aD_advertData.commissions.append(aD_commission);
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
                    ui->LE_value->editingFinished();
                    // Update UI elements as before
                    break; // Exit the loop once the correct commission is found
                }
            }
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

            QString channel         = orderObj["channel"].toString();
            QString shop            = orderObj["shop"].toString();
            QString value           = orderObj["value"].toString();
            QString expProv         = orderObj["expProv"].toString();
            QString currency        = orderObj["currency"].toString();
            QString orderId         = orderObj["orderID"].toString();
            QString juserId         = userID;
            QString date            = orderObj["date"].toString();
            QString commissionText  = orderObj["commissionText"].toString();
            QString suppType        = orderObj["suppType"].toString();
            QString network         = "Adtraction"; //TBD:: make dynamic
            QString programId       = orderObj["shopID"].toString();
            QString commissionId    = orderObj["comissionID"].toString();

            QString nStat=QString::number(eNstat_NotSend);
            QString nStatText = orderObj["networkReply"].toString();
            if(!orderObj["networkStatusCode"].isNull()){

                QString responseCode = orderObj["networkStatusCode"].toString();

                if(responseCode == "200"){

                    nStat = QString::number(eNstat_Good);
                }
                else if(responseCode == "409"|| responseCode == "204" || responseCode == "201"){
                    nStat = QString::number(eNstat_Okay);
                }
                else {
                    nStat = QString::number(eNstat_Error);

                }
            }
            SuppDetail tableSuppDetails(channel,shop,value,expProv,currency,orderId,juserId,date,commissionText,suppType,nStat,network,programId,commissionId);

            addItemToTable(tableSuppDetails,false, nStatText);
        }
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

suppNetStatus P_SupportPageAdtraction::convStringTosuppNetStat(const QString& suppNetStatString)
{
    if (suppNetStatString == "0") {
        return eNstat_NotSend;
    } else if (suppNetStatString == "1") {
        return eNstat_Good;
    } else if (suppNetStatString == "2") {
        return eNstat_Okay;
    } else if (suppNetStatString == "3") {
        return eNstat_Error;
    } else {
        // Handle unknown status, could return a default value or throw an exception
        qDebug() << "Unknown network status string:" << suppNetStatString;
        return eNstat_NotSend; // Assuming eNstat_NotSend as a default or error case
    }
}

void P_SupportPageAdtraction::addItemToTable(const SuppDetail &suppDetails, const bool addOrderToSessionJson, const QString &nStatText)
{
    bool success = true;
    if(addOrderToSessionJson){
        success = addItemToSessionJson(suppDetails);
    }

    if(success){
        int actRowCount = ui->T_NachbuchungsanfragenListe->rowCount();
        ui->T_NachbuchungsanfragenListe->insertRow(actRowCount); // Add a new row

        // Assuming you have the columns set as follows, adjust based on your actual setup
        QDate suppDate = QDate::fromString(suppDetails.date,"dd.MM.yyyy");
        QString daysOld = QString::number(suppDate.daysTo(QDate::currentDate()));

        ui->T_NachbuchungsanfragenListe->setItem(actRowCount, eCol_Channel,         new QTableWidgetItem(suppDetails.channel));
        ui->T_NachbuchungsanfragenListe->setItem(actRowCount, eCol_Shop,            new QTableWidgetItem(suppDetails.shop));
        ui->T_NachbuchungsanfragenListe->setItem(actRowCount, eCol_Value,           new QTableWidgetItem(suppDetails.value));
        ui->T_NachbuchungsanfragenListe->setItem(actRowCount, eCol_ExpProv,         new QTableWidgetItem(suppDetails.expProv));
        ui->T_NachbuchungsanfragenListe->setItem(actRowCount, eCol_Currency,        new QTableWidgetItem(suppDetails.currency));
        ui->T_NachbuchungsanfragenListe->setItem(actRowCount, eCol_OrderId,         new QTableWidgetItem(suppDetails.orderId));
        ui->T_NachbuchungsanfragenListe->setItem(actRowCount, eCol_UserId,          new QTableWidgetItem(suppDetails.userId));
        ui->T_NachbuchungsanfragenListe->setItem(actRowCount, eCol_Date,            new QTableWidgetItem(suppDetails.date));
        ui->T_NachbuchungsanfragenListe->setItem(actRowCount, eCol_CommissionText,  new QTableWidgetItem(suppDetails.commissionText));
        ui->T_NachbuchungsanfragenListe->setItem(actRowCount, eCol_CommissionType,  new QTableWidgetItem(suppDetails.suppType));
        ui->T_NachbuchungsanfragenListe->setItem(actRowCount, eCol_DaysOld,         new QTableWidgetItem(daysOld));

        ui->T_NachbuchungsanfragenListe->setItem(actRowCount, eCol_H_Nstat,         new QTableWidgetItem(suppDetails.nStat));
        ui->T_NachbuchungsanfragenListe->setItem(actRowCount, eCol_H_Network,       new QTableWidgetItem(suppDetails.network));
        ui->T_NachbuchungsanfragenListe->setItem(actRowCount, eCol_H_ProgramId,     new QTableWidgetItem(suppDetails.programId));
        ui->T_NachbuchungsanfragenListe->setItem(actRowCount, eCOl_H_CommissionId,  new QTableWidgetItem(suppDetails.commissionId));

        //qDebug()<<suppDetails.nStat;
        suppNetStatus networkSatusForCurrObj = convStringTosuppNetStat(suppDetails.nStat);
        addNStatButton(actRowCount,nStatText,networkSatusForCurrObj);

        QPushButton *sendBTN = new QPushButton;
        QPixmap sendPixmap(":/img/img/Send_trans.png");
        QIcon sendIcon(sendPixmap);
        sendBTN->setIcon(sendIcon);
        ui->T_NachbuchungsanfragenListe->setCellWidget(actRowCount,eCol_SendBTN,sendBTN);

        QPushButton *deleteBTN = new QPushButton;
        QPixmap xPixmap(":/img/img/Kreuz_trans.png");
        QIcon xIcon(xPixmap);
        deleteBTN->setIcon(xIcon);
        ui->T_NachbuchungsanfragenListe->setCellWidget(actRowCount,eCol_DeleteBTN,deleteBTN);

        disableEditingForRow(actRowCount);

        QObject::connect(deleteBTN, &QPushButton::clicked, this, &P_SupportPageAdtraction::on_deleteBTNTable_clicked);
        QObject::connect(sendBTN, &QPushButton::clicked, this, &P_SupportPageAdtraction::on_sendBTNTable_clicked);
    }
}

void P_SupportPageAdtraction::addNStatButton(const int currentRow,const QString &netReply ,const suppNetStatus currentStat)
{
    QPushButton *networkStatusBTN = new QPushButton;

    switch(currentStat){
    case eNstat_Good:


        networkStatusBTN->setIcon(QIcon(":/img/img/ausrufezeichen_green_trans.png"));
        networkStatusBTN->setToolTip(netReply);

        ui->T_NachbuchungsanfragenListe->setCellWidget(currentRow,eCol_Networkstatus,networkStatusBTN);

        break;

    case eNstat_Okay:


        networkStatusBTN->setIcon(QIcon(":/img/img/ausrufezeichen_orange_trans.png"));
        networkStatusBTN->setToolTip(netReply);

        ui->T_NachbuchungsanfragenListe->setCellWidget(currentRow,eCol_Networkstatus,networkStatusBTN);

        break;

    case eNstat_Error:

        networkStatusBTN->setIcon(QIcon(":/img/img/ausrufezeichen_trans.png"));
        networkStatusBTN->setToolTip(netReply);

        ui->T_NachbuchungsanfragenListe->setCellWidget(currentRow,eCol_Networkstatus,networkStatusBTN);

        break;

    case eNstat_NotSend:

        networkStatusBTN->setIcon(QIcon(":/img/img/3Dots_trans.png"));
        networkStatusBTN->setToolTip("This Item has not been send");

        ui->T_NachbuchungsanfragenListe->setCellWidget(currentRow, eCol_Networkstatus, networkStatusBTN);

        break;

    default:

        qDebug()<<"Error in AddNstatButton-Statemachine";
        delete networkStatusBTN;

        break;
    }
}

bool P_SupportPageAdtraction::addItemToSessionJson(const SuppDetail &suppDetails)
{


    QJsonDocument suppDataDoc = dataManager->json->load("NetworkSuppAnswers");
    QJsonObject suppDataObj;

    if (!suppDataDoc.isNull()) {
        // If the JSON document is not null, use its object
        suppDataObj = suppDataDoc.object();
    }

    QString userID = suppDetails.userId;
    QString orderID = suppDetails.orderId;

    if(!suppDataObj.contains(userID)){
        QJsonObject userObj;
        userObj["userID"] = userID;
        userObj["orders"] = QJsonArray();
        suppDataObj[userID] = userObj;
    }
    else{

        QJsonObject userObj = suppDataObj[userID].toObject();
        QJsonArray ordersArray = userObj["orders"].toArray();


        for(int i = 0; i<ordersArray.size();++i){

            QJsonObject orderObj = ordersArray[i].toObject();

            if(orderObj["orderID"].toString() == orderID){

                QMessageBox::StandardButton reply;

                // If nothing is selected, ask if the user wants to delete all contents
                reply = QMessageBox::question(this, "OrderId Already Exists", "This orderID: " + suppDetails.orderId + " already exists for this user, do you want to override it?",
                                              QMessageBox::Yes | QMessageBox::Cancel);
                if (reply == QMessageBox::Yes) {

                    for (int currentRow = 0; currentRow < ui->T_NachbuchungsanfragenListe->rowCount();++currentRow){
                        if (userID == ui->T_NachbuchungsanfragenListe->item(currentRow,eCol_UserId)->text()&& orderID == ui->T_NachbuchungsanfragenListe->item(currentRow,eCol_OrderId)->text()){

                            ui->T_NachbuchungsanfragenListe->removeRow(currentRow);
                            break;
                        }
                    }
                    ordersArray.removeAt(i);

                    userObj["orders"] = ordersArray;
                    suppDataObj[userID] = userObj;
                    QJsonDocument suppDoc(suppDataObj);

                    dataManager->json->save("NetworkSuppAnswers",suppDoc);
                } else {

                    return false; // Exit if the user decides not to delete
                }

                break;
            }
        }
    }

    suppDataDoc = dataManager->json->load("NetworkSuppAnswers");
    if (!suppDataDoc.isNull()) {
        // If the JSON document is not null, use its object
        suppDataObj = suppDataDoc.object();
    }
    QJsonObject userObj = suppDataObj[userID].toObject();

    QJsonObject orderObj;


    //save all the data into the SessionJson
    orderObj["channel"]         = suppDetails.channel;
    orderObj["commissionText"]  = suppDetails.commissionText;
    orderObj["comissionID"]     = suppDetails.commissionId;
    orderObj["currency"]        = suppDetails.currency;
    orderObj["date"]            = suppDetails.date;
    orderObj["expProv"]         = suppDetails.expProv;
    orderObj["network"]         = suppDetails.network;
    orderObj["orderID"]         = suppDetails.orderId;
    orderObj["shop"]            = suppDetails.shop;
    orderObj["shopID"]          = suppDetails.programId;
    orderObj["suppType"]        = suppDetails.suppType;
    orderObj["value"]           = suppDetails.value;


    QJsonArray ordersArray = userObj["orders"].toArray();
    ordersArray.append(orderObj);

    userObj["orders"] = ordersArray;
    suppDataObj[userID] = userObj;

    QJsonDocument suppDoc(suppDataObj);

    dataManager->json->save("NetworkSuppAnswers",suppDoc);
    return true;
}

void P_SupportPageAdtraction::disableEditingForRow(const int currentRow)
{
    for(int column = 0; column < ui->T_NachbuchungsanfragenListe->columnCount();++column){

        QTableWidgetItem* item = ui->T_NachbuchungsanfragenListe->item(currentRow,column);
        if(item){

            item->setFlags(item->flags()& ~Qt::ItemIsEditable);
        }
    }

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

//Event after the request was received from the server
void P_SupportPageAdtraction::networkRequestMessageReceived(const QString responseCode, const QString userId, const QString orderId)
{
    QJsonDocument suppAnswers = dataManager->json->load("NetworkSuppAnswers");
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

            addNStatButton(currentRow,nreply,netStatForObject);
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
        ui->T_NachbuchungsanfragenListe->setColumnHidden(eCol_CommissionType,true);
        ui->T_NachbuchungsanfragenListe->setColumnHidden(eCol_DaysOld,true);

        toggleStatusTable = eTT_Small;
        break;

    case eTT_Small:

        ui->T_NachbuchungsanfragenListe->setColumnHidden(eCol_Channel,false);
        ui->T_NachbuchungsanfragenListe->setColumnHidden(eCol_ExpProv,false);
        ui->T_NachbuchungsanfragenListe->setColumnHidden(eCol_CommissionText,false);
        ui->T_NachbuchungsanfragenListe->setColumnHidden(eCol_CommissionType,false);
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
    ui->T_NachbuchungsanfragenListe->setSelectionMode(QAbstractItemView::MultiSelection);
    ui->T_NachbuchungsanfragenListe->clearSelection();

    for (int currRow = 0; currRow < ui->T_NachbuchungsanfragenListe->rowCount(); ++currRow){

        if(ui->T_NachbuchungsanfragenListe->item(currRow,eCol_DaysOld)->text().toInt() >= 90){
            ui->T_NachbuchungsanfragenListe->selectRow(currRow);
        }
    }
    ui->T_NachbuchungsanfragenListe->setSelectionMode(QAbstractItemView::ExtendedSelection);
}

void P_SupportPageAdtraction::on_pb_select_Red_clicked()
{
    ui->T_NachbuchungsanfragenListe->setSelectionMode(QAbstractItemView::MultiSelection);
    ui->T_NachbuchungsanfragenListe->clearSelection();

    for (int currRow = 0; currRow < ui->T_NachbuchungsanfragenListe->rowCount(); ++currRow){

        if(ui->T_NachbuchungsanfragenListe->item(currRow,eCol_H_Nstat)->text().toInt() == eNstat_Error){
            ui->T_NachbuchungsanfragenListe->selectRow(currRow);
        }
    }
    ui->T_NachbuchungsanfragenListe->setSelectionMode(QAbstractItemView::ExtendedSelection);
}

void P_SupportPageAdtraction::on_pb_select_Orange_clicked()
{
    ui->T_NachbuchungsanfragenListe->setSelectionMode(QAbstractItemView::MultiSelection);
    ui->T_NachbuchungsanfragenListe->clearSelection();

    for (int currRow = 0; currRow < ui->T_NachbuchungsanfragenListe->rowCount(); ++currRow){

        if(ui->T_NachbuchungsanfragenListe->item(currRow,eCol_H_Nstat)->text().toInt() == eNstat_Okay){
            qDebug()<<"notopk";
            ui->T_NachbuchungsanfragenListe->selectRow(currRow);
        }
    }
    ui->T_NachbuchungsanfragenListe->setSelectionMode(QAbstractItemView::ExtendedSelection);
}

void P_SupportPageAdtraction::on_pb_select_Green_clicked()
{
    ui->T_NachbuchungsanfragenListe->clearSelection();

    for (int currRow = 0; currRow < ui->T_NachbuchungsanfragenListe->rowCount(); ++currRow){

        if(ui->T_NachbuchungsanfragenListe->item(currRow,eCol_H_Nstat)->text().toInt() == eNstat_Good){
            ui->T_NachbuchungsanfragenListe->selectRow(currRow);
        }
    }
}

//Sort by NetworkStatus
void P_SupportPageAdtraction::on_PB_SortNetworkStatus_clicked()
{
    switch(sortStat_networkStat){
    case eSStat_None:
        ui->T_NachbuchungsanfragenListe->sortItems(eCol_H_Nstat,Qt::AscendingOrder);

        sortStat_networkStat = eSStat_Ascending;
        sortStat_shop = eSStat_None;
        sortStat_date = eSStat_None;
        break;

    case eSStat_Ascending:
        ui->T_NachbuchungsanfragenListe->sortItems(eCol_H_Nstat,Qt::DescendingOrder);

        sortStat_networkStat = eSStat_Descending;
        break;

    case eSStat_Descending:

        fillTableWithJson();
        sortStat_networkStat = eSStat_None;
        break;

    default:
        qDebug()<<"Error in NetworkStatus-Sorting-Statemachine";
        break;
    }

}

//Sort by Shop
void P_SupportPageAdtraction::on_PB_SortShop_clicked()
{
    switch(sortStat_shop){
    case eSStat_None:
        ui->T_NachbuchungsanfragenListe->sortItems(eCol_Shop,Qt::AscendingOrder);

        sortStat_shop = eSStat_Ascending;
        sortStat_networkStat = eSStat_None;
        sortStat_date = eSStat_None;

        break;

    case eSStat_Ascending:
        ui->T_NachbuchungsanfragenListe->sortItems(eCol_Shop,Qt::DescendingOrder);

        sortStat_shop = eSStat_Descending;
        break;

    case eSStat_Descending:

        fillTableWithJson();
        sortStat_shop = eSStat_None;
        break;

    default:
        qDebug()<<"Error in Shop-Sorting-Statemachine";
        break;
    }
}

//Sort by Date
void P_SupportPageAdtraction::on_PB_SortDate_clicked()
{
    switch(sortStat_date){
    case eSStat_None:
        ui->T_NachbuchungsanfragenListe->sortItems(eCol_Date,Qt::AscendingOrder);

        sortStat_date = eSStat_Ascending;
        sortStat_networkStat = eSStat_None;
        sortStat_shop = eSStat_None;
        break;

    case eSStat_Ascending:
        ui->T_NachbuchungsanfragenListe->sortItems(eCol_Date,Qt::DescendingOrder);

        sortStat_date = eSStat_Descending;
        break;

    case eSStat_Descending:

        fillTableWithJson();
        sortStat_date = eSStat_None;
        break;

    default:
        qDebug()<<"Error in Date-Sorting-Statemachine";

        sortStat_networkStat = eSStat_None;
        sortStat_shop = eSStat_None;
        sortStat_date = eSStat_None;
        break;
    }
}
//------------------

//Inserting, Sending, Deleting direct out of the table
void P_SupportPageAdtraction::on_PB_AddToList_clicked()
{
    if(ui->LE_orderId->text().isEmpty() || ui->LE_User->text().isEmpty() || ui->LE_value->text().isEmpty() || ui->LE_expectedProv_Currency->text().isEmpty()){

        QMessageBox::critical(this,"Text Boxes Empty","At least one of the text boxes is empty, please fill out every text box!");
    }
    else{

        int colonSpaceIndex = ui->CB_shop->currentText().indexOf(": ");
        QString channel;
        QString shop;
        if(colonSpaceIndex != -1){

            shop = ui->CB_shop->currentText().first(colonSpaceIndex);

            channel = ui->CB_shop->currentText().mid(colonSpaceIndex + 3); // +3 to skip ": ("
            channel = channel.removeLast();
        }

        QString value           = ui->LE_value->text();
        QString expProv         = ui->LE_expectedProv_Currency->text();
        QString currency        = ui->CB_Currency->currentText();
        QString orderId         = ui->LE_orderId->text();
        QString userId          = ui->LE_User->text();
        QString date            = ui->DE_transactionDate->date().toString("dd.MM.yyyy");
        QString commissionText  = aD_advertData.commissions.at(ui->CB_ComissionID->currentIndex()).name; //TBD:: change name of ComboBox
        QString suppType        = ui->CB_SuppType->currentText();
        QString nStat           = QString::number(eNstat_NotSend);
        QString network         = "Adtraction";                                                         //TBD:: make dynamic
        QString programId       = QString::number(aD_advertData.programId);
        QString commissionId    = aD_advertData.commissions.at(ui->CB_ComissionID->currentIndex()).id;

        SuppDetail tableSuppDetails(channel,shop,value,expProv,currency,orderId,userId,date,commissionText,suppType,nStat,network,programId,commissionId);

        addItemToTable(tableSuppDetails,true);

        //Delete contents
        ui->LE_value->clear();
        ui->LE_orderId->clear();
        ui->LE_User->clear();
        ui->DE_transactionDate->setDate(QDate::currentDate());
    }
}

//deleting one Element from the Table directly
void P_SupportPageAdtraction::on_deleteBTNTable_clicked()
{
    QJsonDocument suppData = dataManager->json->load("NetworkSuppAnswers");
    QJsonObject jObj;

    int currRow = ui->T_NachbuchungsanfragenListe->currentRow();

    QString userId = ui->T_NachbuchungsanfragenListe->item(currRow,eCol_UserId)->text();
    //qDebug()<<"--DELETE--";
    //qDebug()<<userId;
    QString orderId = ui->T_NachbuchungsanfragenListe->item(currRow,eCol_OrderId)->text();
    //qDebug()<<orderId;
    //qDebug()<<"--DELETE--";

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

    //****OLD*****//
    // QString fileName = QFileDialog::getSaveFileName(this, tr("Save CSV"), QDir::homePath(),tr("CSV Files (*.csv"));
    // if(!fileName.isEmpty()){
    //     if(!fileName.endsWith(".csv",Qt::CaseInsensitive)){
    //         fileName+= ".csv";
    //     }
    //     outputRowsToCSV(fileName);
    // }
}

//Deleting selection / all
void P_SupportPageAdtraction::on_pb_deleteAll_clicked()
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
    for (int i = 0; i < ui->T_NachbuchungsanfragenListe->rowCount(); ++i) {
        bool match = false;
        for (int j = 0; j < ui->T_NachbuchungsanfragenListe->columnCount(); ++j) {
            QTableWidgetItem *item = ui->T_NachbuchungsanfragenListe->item(i, j);
            if (item && item->text().contains(arg1, Qt::CaseInsensitive)) {
                match = true;
                break; // No need to check other columns if we have a match
            }
        }
        ui->T_NachbuchungsanfragenListe->setRowHidden(i, !match); // Hide the row if there is no match
    }
}


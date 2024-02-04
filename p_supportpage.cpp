#include "p_supportpage.h"
#include "ui_p_supportpage.h"
#include "qjsonobject.h"
#include <QJsonValue>
#include <QJsonArray>
#include <qjsonobject.h>
#include <QTimer>
#include <suppeventbus.h>


P_SupportPage::P_SupportPage(DataManager* dataManager, APIManager* apiManager,QWidget *parent) :
    QWidget(parent),
    ui(new Ui::P_SupportPage),
    dataManager(dataManager),
    apiManager(apiManager)
{
    ui->setupUi(this);

    QDoubleValidator *dvalidator = new QDoubleValidator(this);
    ui->LE_value->setValidator(dvalidator);
    ui->LE_expectedProv_Currency->setValidator(dvalidator);
    ui->LE_expectedProv_Percent->setValidator(dvalidator);

    ui->RB_expProvCur->setChecked(true);
    ui->LE_expectedProv_Percent->setEnabled(false);

    ui->DE_transactionDate->setDate(QDate::currentDate());
    ui->DE_transactionDate->setMaximumDate(QDate::currentDate());

    initTable();

    ui->CB_SuppType->addItem("Untracked");
    QTimer::singleShot(300, this, &P_SupportPage::initPage);
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

P_SupportPage::~P_SupportPage()
{
    delete ui;
}

void P_SupportPage::refreshNetworkList()
{
    fillNetworkComboBox();
}

void P_SupportPage::initTable()
{
    //standard size is 150 for the columns
    for(int i=0;i<ui->T_NachbuchungsanfragenListe->columnCount();++i){
        ui->T_NachbuchungsanfragenListe->setColumnWidth(i,150);
    }

    //rest is seted seperately
    ui->T_NachbuchungsanfragenListe->setColumnWidth(0,80);
    ui->T_NachbuchungsanfragenListe->setColumnWidth(1,80);
    ui->T_NachbuchungsanfragenListe->setColumnWidth(2,175);
    ui->T_NachbuchungsanfragenListe->setColumnWidth(3,80);
    ui->T_NachbuchungsanfragenListe->setColumnWidth(4,80);
    ui->T_NachbuchungsanfragenListe->setColumnWidth(5,60);
    ui->T_NachbuchungsanfragenListe->setColumnWidth(6,80);
    ui->T_NachbuchungsanfragenListe->setColumnWidth(7,100);
    ui->T_NachbuchungsanfragenListe->setColumnWidth(8,75);
    ui->T_NachbuchungsanfragenListe->setColumnWidth(9,100);
    ui->T_NachbuchungsanfragenListe->setColumnWidth(10,90);
    ui->T_NachbuchungsanfragenListe->setColumnWidth(11,75);
    ui->T_NachbuchungsanfragenListe->setColumnWidth(12,90);
    ui->T_NachbuchungsanfragenListe->setColumnWidth(13,70);
    ui->T_NachbuchungsanfragenListe->setColumnWidth(14,70);
}

void P_SupportPage::initPage()
{
    cur = dataManager->json->load("currenciesAdtraction");


    //Fill Comboboxes
    fillShopComboBox();
    fillCurrencyComboBox();
    fillNetworkComboBox();

    setupComboBoxConnections();
}

void P_SupportPage::fillShopComboBox()
{
    QList<QStringList> csvData = dataManager->csv->load("NetworkChannels");

    for (const QStringList &rowData : csvData) {

        doc = dataManager->json->load(QString(rowData.at(2)));

        for(const QJsonValue &value : doc.array()){
            QJsonObject obj = value.toObject();
            shopToProgramIdHash.insert(obj["programName"].toString(),obj["programId"].toInt());
            ui->CB_shop->addItem(obj["programName"].toString()+": ("+rowData.at(1) + ")");
        }
        channelToId.insert(rowData.at(1),rowData.at(2));
        allDocs.insert(rowData.at(2),doc);
    }
}

void P_SupportPage::setupComboBoxConnections()
{

    connect(ui->CB_shop, QOverload<int>::of(&QComboBox::activated),[this](int index) {
        ui->CB_ComissionID->clear();

        int colonSpaceIndex = ui->CB_shop->currentText().indexOf(": ");
        QString channel;

        if(colonSpaceIndex != -1){
            channel = ui->CB_shop->currentText().mid(colonSpaceIndex + 3); // +3 to skip ": ("
            channel = channel.removeLast();
        }

        QJsonDocument commDoc = allDocs.value(channelToId.value(channel));
        QJsonObject selectedProgram = commDoc.array().at(index).toObject();
        QJsonArray commissions = selectedProgram["commissions"].toArray();

        for (const QJsonValue &value : commissions) {
            QJsonObject obj = value.toObject();
            QString itemId = obj["id"].toString();
            QString itemName = obj["name"].toString();
            QString itemText = QString("%1: %2").arg(itemName, itemId);
            ui->CB_ComissionID->addItem(itemText);
        }
    });
}

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
        if (userId == ui->T_NachbuchungsanfragenListe->item(i,7)->text()&& orderId == ui->T_NachbuchungsanfragenListe->item(i,6)->text()){
            if(responseCode == "200"){

                QPushButton *networkStatusBTN = new QPushButton;
                QPixmap pm_networkStatus(":/img/img/ausrufezeichen_green_trans.png");
                QIcon i_networkSatus(pm_networkStatus);
                networkStatusBTN->setIcon(i_networkSatus);
                networkStatusBTN->setToolTip(nreply);

                ui->T_NachbuchungsanfragenListe->setCellWidget(i,12,networkStatusBTN);
            }
            else if(responseCode != "200"){
                QPushButton *networkStatusBTN = new QPushButton;
                QPixmap pm_networkStatus(":/img/img/ausrufezeichen_orange_trans.png");
                QIcon i_networkSatus(pm_networkStatus);
                networkStatusBTN->setIcon(i_networkSatus);
                networkStatusBTN->setToolTip(nreply);

                ui->T_NachbuchungsanfragenListe->setCellWidget(i,12,networkStatusBTN);
            }
        }
    }
}

void P_SupportPage::fillCurrencyComboBox()
{
    for(const QJsonValue &value: cur.array()){
        QJsonObject obj = value.toObject();
        QString currency = obj["currency"].toString();
        if(prefferedCurrencies.contains(currency)){
            prefferedList.append(currency);
        }else{
            otherList.append(currency);
        }
    }

    std::sort(otherList.begin(), otherList.end());

    for(const QString &currency : prefferedList){
        ui->CB_Currency->addItem(currency);
    }
    for(const QString &currency : otherList){
        ui->CB_Currency->addItem(currency);
    }
}

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

//Private Slots
void P_SupportPage::on_RB_expProvCur_clicked()
{
    ui->LE_expectedProv_Percent->setEnabled(false);
    ui->LE_expectedProv_Currency->setEnabled(true);
}

void P_SupportPage::on_RB_expProvPer_clicked()
{
    ui->LE_expectedProv_Percent->setEnabled(true);
    ui->LE_expectedProv_Currency->setEnabled(false);
}
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


void P_SupportPage::on_PB_AddToList_clicked()
{
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

    ui->T_NachbuchungsanfragenListe->setCellWidget(rowCount,12,networkStatusBTN);


    QPushButton *sendBTN = new QPushButton;
    QPixmap sendPixmap(":/img/img/Send_trans.png");
    QIcon sendIcon(sendPixmap);
    sendBTN->setIcon(sendIcon);
    ui->T_NachbuchungsanfragenListe->setCellWidget(rowCount,13,sendBTN);

    QPushButton *deleteBTN = new QPushButton;
    QPixmap xPixmap(":/img/img/Kreuz_trans.png");
    QIcon xIcon(xPixmap);
    deleteBTN->setIcon(xIcon);
    ui->T_NachbuchungsanfragenListe->setCellWidget(rowCount,14,deleteBTN);



    QObject::connect(deleteBTN, &QPushButton::clicked, this, &P_SupportPage::on_deleteBTN_clicked);
    QObject::connect(sendBTN, &QPushButton::clicked, this, &P_SupportPage::on_sendBTN_clicked);


    //Delete contents
    ui->LE_value->clear();
    ui->LE_expectedProv_Currency->clear();
    ui->LE_expectedProv_Percent->clear();
    ui->LE_orderId->clear();
    ui->LE_User->clear();
    ui->DE_transactionDate->setDate(QDate::currentDate());
}

void P_SupportPage::on_deleteBTN_clicked()
{
    QJsonDocument suppData = dataManager->json->load("NetworkSuppAnswers");
    QJsonObject jObj;

    int currRow = ui->T_NachbuchungsanfragenListe->currentRow();

    QString userId = ui->T_NachbuchungsanfragenListe->item(currRow,7)->text();
    QString orderId = ui->T_NachbuchungsanfragenListe->item(currRow,6)->text();

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

void P_SupportPage::on_sendBTN_clicked(){

    int row = ui->T_NachbuchungsanfragenListe->currentRow();

    QStringList programIdStrL = ui->T_NachbuchungsanfragenListe->item(row, 2)->text().split(":");
    QString programIdStr = programIdStrL.first().trimmed();
    int programId = shopToProgramIdHash.value(programIdStr);
    qDebug()<<programId;
    int channelId = 1592293656;
    QString orderId= ui->T_NachbuchungsanfragenListe->item(row, 6)->text();
    int commissionId= ui->T_NachbuchungsanfragenListe->item(row, 9)->text().split(":").at(1).trimmed().toInt();
    double expecetedCom = ui->T_NachbuchungsanfragenListe->item(row, 4)->text().toDouble();


    QDate date = QDate::fromString(ui->T_NachbuchungsanfragenListe->item(row, 8)->text(),"dd.MM.yyyy");
    QString transactionDate = date.toString("yyyy-MM-dd");
    transactionDate += "T23:59:59+0100";

    double orderVal = ui->T_NachbuchungsanfragenListe->item(row, 3)->text().toDouble();
    QString currency = ui->T_NachbuchungsanfragenListe->item(row, 5)->text();
    QString userId = ui->T_NachbuchungsanfragenListe->item(row, 7)->text();
    apiManager->adtraction->sendSuppData(programId,channelId, orderId, commissionId, expecetedCom, transactionDate, orderVal, currency, userId);
}


void P_SupportPage::on_PB_SendOverAPI_clicked()
{

    for(int row = 0; row < ui->T_NachbuchungsanfragenListe->rowCount(); ++row){
        QStringList programIdStrL = ui->T_NachbuchungsanfragenListe->item(row, 2)->text().split(":");
        QString programIdStr = programIdStrL.first().trimmed();
        int programId = shopToProgramIdHash.value(programIdStr);
        int channelId = 1592293656;
        QString orderId= ui->T_NachbuchungsanfragenListe->item(row, 6)->text();
        int commissionId= ui->T_NachbuchungsanfragenListe->item(row, 9)->text().split(":").at(1).trimmed().toInt();
        double expecetedCom = ui->T_NachbuchungsanfragenListe->item(row, 4)->text().toDouble();


        QDate date = QDate::fromString(ui->T_NachbuchungsanfragenListe->item(row, 8)->text(),"dd.MM.yyyy");
        QString transactionDate = date.toString("yyyy-MM-dd");
        transactionDate += "T23:59:59+0100";

        double orderVal = ui->T_NachbuchungsanfragenListe->item(row, 3)->text().toDouble();
        QString currency = ui->T_NachbuchungsanfragenListe->item(row, 5)->text();
        QString userId = ui->T_NachbuchungsanfragenListe->item(row, 7)->text();

        apiManager->adtraction->sendSuppData(programId,channelId, orderId, commissionId, expecetedCom, transactionDate, orderVal, currency, userId);
    }

}

void P_SupportPage::on_pb_toggleTable_clicked()
{
    if(ui->T_NachbuchungsanfragenListe->isColumnHidden(1)){

        ui->T_NachbuchungsanfragenListe->setColumnHidden(1,false);
        ui->T_NachbuchungsanfragenListe->setColumnHidden(4,false);
        ui->T_NachbuchungsanfragenListe->setColumnHidden(9,false);
        ui->T_NachbuchungsanfragenListe->setColumnHidden(10,false);
        ui->T_NachbuchungsanfragenListe->setColumnHidden(11,false);
    }else{

        ui->T_NachbuchungsanfragenListe->setColumnHidden(1,true);
        ui->T_NachbuchungsanfragenListe->setColumnHidden(4,true);
        ui->T_NachbuchungsanfragenListe->setColumnHidden(9,true);
        ui->T_NachbuchungsanfragenListe->setColumnHidden(10,true);
        ui->T_NachbuchungsanfragenListe->setColumnHidden(11,true);
    }
}


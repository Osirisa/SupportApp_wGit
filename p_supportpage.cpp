#include "p_supportpage.h"
#include "ui_p_supportpage.h"
#include <QJsonValue>
#include <QJsonArray>
#include <qjsonobject.h>
#include <QTimer>


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
    ui->T_NachbuchungsanfragenListe->setColumnWidth(5,75);
    ui->T_NachbuchungsanfragenListe->setColumnWidth(11,75);
    ui->T_NachbuchungsanfragenListe->setColumnWidth(13,75);
    ui->T_NachbuchungsanfragenListe->setColumnWidth(14,75);
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

    QTableWidgetItem *items [] = {
        new QTableWidgetItem(QString(ui->CB_Network->currentText())),
        new QTableWidgetItem(QString(channel)),
        new QTableWidgetItem(QString(ui->CB_shop->currentText())),
        new QTableWidgetItem(QString(ui->LE_value->text())),
        new QTableWidgetItem(QString(ui->LE_expectedProv_Currency->text())),
        new QTableWidgetItem(QString(ui->CB_Currency->currentText())),
        new QTableWidgetItem(QString(ui->LE_orderId->text())),
        new QTableWidgetItem(QString(ui->LE_User->text())),
        new QTableWidgetItem(QString(ui->DE_transactionDate->date().toString("dd.MM.yyyy"))),
        new QTableWidgetItem(QString(ui->CB_ComissionID->currentText())),
        new QTableWidgetItem(QString(ui->CB_SuppType->currentText())),
        new QTableWidgetItem(QString::number(ui->DE_transactionDate->date().daysTo(QDate::currentDate()))),
        new QTableWidgetItem,
        new QTableWidgetItem
    };

    const size_t count = sizeof(items) / sizeof(QTableWidgetItem*);
    for(size_t column = 0; column < count; column++){
        ui->T_NachbuchungsanfragenListe->setItem(rowCount,column,items[column]);
    }



    QPushButton *sendBTN = new QPushButton;
    sendBTN->setText("->");
    sendBTN->setStyleSheet("QPushButton { color: orange; }");
    ui->T_NachbuchungsanfragenListe->setCellWidget(rowCount,13,sendBTN);

    QPushButton *deleteBTN = new QPushButton;
    deleteBTN->setText("X");
    deleteBTN->setStyleSheet("QPushButton { color: red; }");
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
    ui->T_NachbuchungsanfragenListe->removeRow(ui->T_NachbuchungsanfragenListe->currentRow());
}

void P_SupportPage::on_sendBTN_clicked(){

    int row = ui->T_NachbuchungsanfragenListe->currentRow();

    int programId = shopToProgramIdHash.value(ui->T_NachbuchungsanfragenListe->item(row, 1)->text());
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
        int programId = shopToProgramIdHash.value(ui->T_NachbuchungsanfragenListe->item(row, 1)->text());
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


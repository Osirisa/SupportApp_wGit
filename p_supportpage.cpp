#include "p_supportpage.h"
#include "ui_p_supportpage.h"
#include <QJsonValue>
#include <QJsonArray>
#include <qjsonobject.h>
#include <QTimer>

P_SupportPage::P_SupportPage(DataManager* dataManager,QWidget *parent) :
    QWidget(parent),
    ui(new Ui::P_SupportPage),
    dataManager(dataManager)
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

    QTimer::singleShot(300, this, &P_SupportPage::initPage);
}

P_SupportPage::~P_SupportPage()
{
    delete ui;
}

void P_SupportPage::initTable()
{
    //standard size is 150 for the columns
    for(int i=0;i<ui->T_NachbuchungsanfragenListe->columnCount();++i){
        ui->T_NachbuchungsanfragenListe->setColumnWidth(i,150);
    }

    //rest is seted seperately
    ui->T_NachbuchungsanfragenListe->setColumnWidth(4,75);
    ui->T_NachbuchungsanfragenListe->setColumnWidth(9,75);
    ui->T_NachbuchungsanfragenListe->setColumnWidth(10,75);
}

void P_SupportPage::initPage()
{
    doc = dataManager->json->load("advertisersAdtraction");
    fillShopComboBox();
    setupComboBoxConnections();
}

void P_SupportPage::fillShopComboBox()
{

    for(const QJsonValue &value : doc.array()){
        QJsonObject obj = value.toObject();
        ui->CB_shop->addItem(obj["programName"].toString(),obj["programId"].toInt());
    }
}

void P_SupportPage::setupComboBoxConnections()
{
    connect(ui->CB_shop, QOverload<int>::of(&QComboBox::activated),[this](int index) {
        ui->CB_ComissionID->clear();
        QJsonObject selectedProgram = doc.array().at(index).toObject();
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
    int rowCount = ui->T_NachbuchungsanfragenListe->rowCount();
    ui->T_NachbuchungsanfragenListe->setRowCount(rowCount + 1);

    QTableWidgetItem *items [] = {
        new QTableWidgetItem(QString(ui->CB_Network->currentText())),
        new QTableWidgetItem(QString(ui->CB_shop->currentText())),
        new QTableWidgetItem(QString(ui->LE_value->text())),
        new QTableWidgetItem(QString(ui->LE_expectedProv_Currency->text())),
        new QTableWidgetItem(QString(ui->CB_Currency->currentText())),
        new QTableWidgetItem(QString(ui->LE_User->text())),
        new QTableWidgetItem(QString(ui->DE_transactionDate->date().toString())),
        new QTableWidgetItem(QString(ui->CB_ComissionID->currentText())),
        new QTableWidgetItem(QString(ui->CB_SuppType->currentText())),
        new QTableWidgetItem(QString::number(ui->DE_transactionDate->date().daysTo(QDate::currentDate()))),
        new QTableWidgetItem
    };

    const size_t count = sizeof(items) / sizeof(QTableWidgetItem*);
    for(size_t column = 0; column < count; column++){
        ui->T_NachbuchungsanfragenListe->setItem(rowCount,column,items[column]);
    }

    QPushButton *deleteBTN = new QPushButton;
    deleteBTN->setText("X");
    deleteBTN->setStyleSheet("QPushButton { color: red; }");
    ui->T_NachbuchungsanfragenListe->setCellWidget(rowCount,10,deleteBTN);

    QObject::connect(deleteBTN, &QPushButton::clicked, this, &P_SupportPage::on_deleteBTN_clicked);
}

void P_SupportPage::on_deleteBTN_clicked()
{
    ui->T_NachbuchungsanfragenListe->removeRow(ui->T_NachbuchungsanfragenListe->currentRow());
}


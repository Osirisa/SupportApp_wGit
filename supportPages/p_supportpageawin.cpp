#include "p_supportpageawin.h"
#include "ui_p_supportpageawin.h"

p_SupportPageAwin::p_SupportPageAwin(DataManager* dataManager, APIManager* apiManager,QWidget *parent) :
    NetworkPageBase(parent),
    ui(new Ui::p_SupportPageAwin),
    dataManager(dataManager),
    apiManager(apiManager)
{
    ui->setupUi(this);
    QTimer::singleShot(300, this, &p_SupportPageAwin::initPage);
}

p_SupportPageAwin::~p_SupportPageAwin()
{
    delete ui;
}

void p_SupportPageAwin::initPage()
{
    //set the adtraction logo on top of the input fields
    QPixmap networkPixmap(":/Logos/img/AwinLogo_trans.png");
    networkPixmap = networkPixmap.scaledToHeight(20*1.33, Qt::SmoothTransformation); //scale it down to 20pt
    ui->L_NetworkNamePic->setPixmap(networkPixmap);


    // fillCurrencyComboBox();
    // fillNetworkComboBox();

    // initTable();
    initInputElements();

    // setupComboBoxConnections();

    ui->CB_Network->activated(0);
    ui->CB_shop->activated(ui->CB_shop->currentIndex());

    //QTimer::singleShot(300, this, &P_SupportPageAdtraction::fillTableWithJson);
}
void p_SupportPageAwin::initInputElements()
{
    QRegularExpressionValidator *validator = new QRegularExpressionValidator(QRegularExpression("^\\d*\\.?\\d*$"), this);

    ui->LE_value->setValidator(validator);
    ui->LE_expectedProv_Currency->setValidator(validator);
    ui->LE_expectedProv_Percent->setValidator(validator);

    ui->RB_expProvCur->setChecked(true);
    ui->LE_expectedProv_Percent->setEnabled(false);

    ui->DE_transactionDate->setDate(QDate::currentDate());
    ui->DE_transactionDate->setMaximumDate(QDate::currentDate());

    QStringList enquiryTypes = {"untracked", "declined", "incorrect"};
    ui->CB_enquiryType->addItems(enquiryTypes);

    //Add To list Button
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

void p_SupportPageAwin::initializeColumnSortStates()
{
    columnSortStates[1] = ColumnSortState(eCol_Shop);
    columnSortStates[9] = ColumnSortState(eCol_DaysOld);
    columnSortStates[13] = ColumnSortState(eCol_H_Nstat); //TBD: is valid?
}

void p_SupportPageAwin::on_pb_select_90Days_clicked()
{
    int overdueDays = 90; //TBD: how many Days?
    selectRowsBasedOnCondition(ui->T_NachbuchungsanfragenListe, eCol_DaysOld, [overdueDays](const QString &value) {
        int days = value.toInt();
        //qDebug() << "Days old:" << days << ", Overdue threshold:" << overdueDays;
        return days >= overdueDays;
    });
}


void p_SupportPageAwin::on_pb_select_Red_clicked()
{
    selectRowsBasedOnCondition(ui->T_NachbuchungsanfragenListe, eCol_H_Nstat, [](const QString &value) {
        //qDebug() << "Checking value:" << value << ", Converted to Int:" << value.toInt();
        int valueInt = value.toInt();
        bool isSelected = (valueInt != 0 && valueInt != 200 && valueInt != 201 && valueInt != 204 && valueInt != 409);//TBD: Define Value
        //qDebug() << "Is Selected:" << isSelected;
        return isSelected;
    });
}


void p_SupportPageAwin::on_pb_select_Orange_clicked()
{
    selectRowsBasedOnCondition(ui->T_NachbuchungsanfragenListe, eCol_H_Nstat, [](const QString &value) {
        //qDebug() << "Checking value:" << value << ", Converted to Int:" << value.toInt();
        int valueInt = value.toInt();
        bool isSelected = ((valueInt == 201) || (valueInt == 204) || (valueInt == 409));//TBD: Define Value
        //qDebug() << "Is Selected:" << isSelected;
        return isSelected;
    });
}


void p_SupportPageAwin::on_pb_select_Green_clicked()
{
    selectRowsBasedOnCondition(ui->T_NachbuchungsanfragenListe, eCol_H_Nstat, [](const QString &value) {
        //qDebug() << "Checking value:" << value << ", Converted to Int:" << value.toInt();
        int valueInt = value.toInt();
        bool isSelected = (valueInt == 200); //TBD: Define Value
        //qDebug() << "Is Selected:" << isSelected;
        return isSelected;
    });
}


void p_SupportPageAwin::on_PB_select_search_clicked()
{
    selectSearch(ui->T_NachbuchungsanfragenListe,ui->LE_SearchBar);
}


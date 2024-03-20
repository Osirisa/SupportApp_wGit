#include "p_supportpageawin.h"
#include "ui_p_supportpageawin.h"

p_SupportPageAwin::p_SupportPageAwin(DataManager* dataManager, APIManager* apiManager,QWidget *parent) :
    QWidget(parent),
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

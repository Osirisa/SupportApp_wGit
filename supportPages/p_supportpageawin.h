#ifndef P_SUPPORTPAGEAWIN_H
#define P_SUPPORTPAGEAWIN_H

#include <QWidget>
//#include "DataManager/adtractionsuppdatamanager.h"
#include "supportPages/networkpagebase.h"
#include "apimanager.h"
#include "Enums.h"
#include "DataManager/datamanager.h"
//#include "supportClasses/adtractionsuppcase.h"

namespace Ui {
class p_SupportPageAwin;
}

class p_SupportPageAwin : public NetworkPageBase
{
    Q_OBJECT

public:
    explicit p_SupportPageAwin(DataManager* dataManager,APIManager* apiManager,QWidget *parent = nullptr);
    ~p_SupportPageAwin();

private slots:
    void on_pb_select_90Days_clicked();

    void on_pb_select_Red_clicked();

    void on_pb_select_Orange_clicked();

    void on_pb_select_Green_clicked();

    void on_PB_select_search_clicked();

private:
    Ui::p_SupportPageAwin *ui;

    DataManager* dataManager;
    APIManager* apiManager;
    //AwinSuppDataManager* suppdataManager;

    void initPage();
    void initInputElements();
    void initializeColumnSortStates() override;

    enum ColumnIndexesAwin{

        //visible
        eCol_Channel,
        eCol_Shop,
        eCol_Enquiry,
        eCol_EnquiryType,
        eCol_Value,
        eCol_ExpProv,
        eCol_Currency,
        eCol_OrderId,
        eCol_UserId,
        eCol_Date,
        eCol_CommissionText,
        eCol_DaysOld,
        eCol_Networkstatus,
        eCol_SendBTN,
        eCol_DeleteBTN,

        //invisible
        eCol_H_Nstat,
        eCol_H_Network,
        eCol_H_ProgramId,
        eCOl_H_CommissionId
    };
};

#endif // P_SUPPORTPAGEAWIN_H

#ifndef P_SUPPORTPAGEADTRACTION_H
#define P_SUPPORTPAGEADTRACTION_H

#include <QWidget>
#include <QTableWidgetItem>
#include <QString>
#include <QDate>

#include "supportPages/networkpagebase.h"
#include "DataManager/adtractionsuppdatamanager.h"
#include "apimanager.h"
#include "Enums.h"
#include "DataManager/datamanager.h"
#include "supportClasses/adtractionsuppcase.h"



namespace Ui {
class P_SupportPageAdtraction;
}

class P_SupportPageAdtraction : public NetworkPageBase
{
    Q_OBJECT

public:
    P_SupportPageAdtraction(DataManager* dataManager,APIManager* apiManager,QWidget *parent = nullptr);
    ~P_SupportPageAdtraction();

    void refreshNetworkList();
    void refreshShops();

private:

    //QT Stuff
    Ui::P_SupportPageAdtraction *ui;

    //Methods
    void initTable();
    void initPage();
    void initInputElements();

    void fillTableWithJson();
    void fillShopComboBox(QString &channel,QString &channelID);
    void fillCurrencyComboBox();
    void fillNetworkComboBox();

    void setupComboBoxConnections();

    void addItemToTable(const AdtractionSuppCase &suppCase);
    bool addItemToSessionJson(const AdtractionSuppCase &suppCase);

    void disableEditingForRow(const int currentRow);
    void initializeColumnSortStates() override;

    suppNetStatus convertNetworkStatusCodeToNstat(const QString& suppNetStatString);

    void outputRowsToCSV(const QString &fileName);

    //Custom Eventsubscribe
    void networkRequestMessageReceived(const QString response, const QString userId, const QString orderId);

    //Objects which are needed
    DataManager* dataManager;
    APIManager* apiManager;
    AdtractionSuppDataManager* suppdataManager;

    //QHashes, QMaps, QSets, QLists
    QHash<QString,QString> channelToId;
    QHash<QString,QString> idtoChannel;

    //Currency Sorting
    QList<QString> prefferedList;
    QList<QString> otherList;


    toggleTable toggleStatusTable = eTT_Normal;

    //Enum for every column in the table for better Code transparency
    enum ColumnIndexesAdtraction{

        //visible
        eCol_Channel,
        eCol_Shop,
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

public slots:
private slots:

    //RadioButtons
    void on_RB_expProvCur_clicked();
    void on_RB_expProvPer_clicked();

    //Line Edits
    void on_LE_value_editingFinished();
    void on_LE_expectedProv_Percent_editingFinished();
    void on_LE_SearchBar_textChanged(const QString &arg1);

    //PushButtons
    void on_PB_AddToList_clicked();
    void on_PB_SendOverAPI_clicked();
    void on_pb_toggleTable_clicked();
    void on_PB_ExportList_clicked();
    void on_pb_deleteAll_clicked();

    void on_PB_SortNetworkStatus_clicked();
    void on_PB_SortShop_clicked();
    void on_PB_SortDate_clicked();

    void on_pb_select_90Days_clicked();
    void on_pb_select_Green_clicked();
    void on_pb_select_Orange_clicked();
    void on_pb_select_Red_clicked();
    void on_PB_select_search_clicked();

    //In Table PushButtons
    void on_deleteBTNTable_clicked();
    void on_sendBTNTable_clicked();

    //In Table row manipulation
    void on_T_NachbuchungsanfragenListe_itemChanged(QTableWidgetItem *item);
};

#endif // P_SUPPORTPAGEADTRACTION_H

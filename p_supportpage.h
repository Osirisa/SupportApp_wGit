#ifndef P_SUPPORTPAGE_H
#define P_SUPPORTPAGE_H

#include <QWidget>
#include <QTableWidgetItem>
#include <QString>
#include <QDate>

#include "apimanager.h"
#include "DataManager/datamanager.h"

namespace Ui {
class P_SupportPage;
}

class P_SupportPage : public QWidget
{
    Q_OBJECT

public:
    explicit P_SupportPage(DataManager* dataManager,APIManager* apiManager,QWidget *parent = nullptr);
    ~P_SupportPage();

    void refreshNetworkList();
private:
    Ui::P_SupportPage *ui;

    void initTable();
    void initPage();
    void initInputElements();
    void fillShopComboBox();
    void fillCurrencyComboBox();
    void fillNetworkComboBox();
    void setupComboBoxConnections();
    void fillTableWithJson();
    void outputRowsToCSV(const QString &fileName);

    //Custom Eventsubscribe
    void networkRequestMessageReceived(const QString response, const QString userId, const QString orderId);

    //Objects which are needed
    DataManager* dataManager;
    APIManager* apiManager;

    //JsonDocuments which will be used "globally" in this class
    QJsonDocument doc;
    QJsonDocument cur;

    //QHashes, QMaps, QSets, QLists
    QHash<QString,int> shopToProgramIdHash;
    QHash<QString,QString> channelToId;

    QMap<QString, QJsonDocument> allDocs;

    QSet<QString> prefferedCurrencies = {"EUR","CHF","USD"};

    QList<QString> prefferedList;
    QList<QString> otherList;

    //Enum for every column in the table for better Code transparency
    enum ColumnIndexes{
        eCol_Network        = 0,
        eCol_Channel        = 1,
        eCol_Shop           = 2,
        eCol_Value          = 3,
        eCol_ExpProv        = 4,
        eCol_Currency       = 5,
        eCol_OrderId        = 6,
        eCol_UserId         = 7,
        eCol_Date           = 8,
        eCol_CommissionId   = 9,
        eCol_CommissionType = 10,
        eCol_DaysOld        = 11,
        eCol_Networkstatus  = 12,
        eCol_SendBTN        = 13,
        eCol_DeleteBTN      = 14,
        eCol_H_Nstat        = 15
    };

    enum suppNetStatus{
        eNstat_NotSend      = 0,
        eNstat_Good         = 1,
        eNstat_Okay         = 2,
        eNstat_Error        = 3
    };

public slots:
private slots:
    void on_RB_expProvCur_clicked();
    void on_RB_expProvPer_clicked();
    void on_LE_expectedProv_Percent_editingFinished();
    void on_LE_value_editingFinished();
    void on_PB_AddToList_clicked();
    void on_deleteBTNTable_clicked();
    void on_sendBTNTable_clicked();
    void on_PB_SendOverAPI_clicked();
    void on_pb_toggleTable_clicked();
    void on_PB_ExportList_clicked();
    void on_pb_deleteAll_clicked();
};

#endif // P_SUPPORTPAGE_H

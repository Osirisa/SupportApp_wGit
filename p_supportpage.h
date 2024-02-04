#ifndef P_SUPPORTPAGE_H
#define P_SUPPORTPAGE_H

#include <QWidget>
#include <QTableWidgetItem>
#include <QString>
#include <QDate>

#include "apimanager.h"
#include "advertiserdata.h"
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
    void fillShopComboBox();
    void fillCurrencyComboBox();
    void fillNetworkComboBox();
    void setupComboBoxConnections();

    void networkRequestMessageReceived(const QString response, const QString userId, const QString orderId);

    DataManager* dataManager;
    APIManager* apiManager;
    QJsonDocument doc;
    QJsonDocument cur;

    QHash<QString,int> shopToProgramIdHash;
    QHash<QString,QString> channelToId;
    QMap<QString, QJsonDocument> allDocs;

    QSet<QString> prefferedCurrencies = {"EUR","CHF","USD"};

    QList<QString> prefferedList;
    QList<QString> otherList;

public slots:
   // void onShopsUpdated(const QHash<QString, AdvertiserData> &advertisers);
private slots:
    void on_RB_expProvCur_clicked();
    void on_RB_expProvPer_clicked();
    void on_LE_expectedProv_Percent_editingFinished();
    void on_LE_value_editingFinished();
    void on_PB_AddToList_clicked();
    void on_deleteBTN_clicked();
    void on_sendBTN_clicked();
    void on_PB_SendOverAPI_clicked();
    void on_pb_toggleTable_clicked();
};

#endif // P_SUPPORTPAGE_H

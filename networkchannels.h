#ifndef NETWORKCHANNELS_H
#define NETWORKCHANNELS_H

#include <QDialog>
#include <QString>
#include <QStringList>
#include <QTimer>

#include "DataManager/DataManager.h"

namespace Ui {
class NetworkChannels;
}

class NetworkChannels : public QDialog {
    Q_OBJECT

public:
    explicit NetworkChannels(DataManager* dataManager, QWidget* parent = nullptr);
    ~NetworkChannels();
signals:
    void onNetworkDataSaved();

private slots:
    void on_addButton_clicked();
    void on_saveButton_clicked();
    void on_deleteBTN_clicked();
    void on_PB_NWC_Cancel_clicked();
    void updateTableWidget();

private:
    Ui::NetworkChannels* ui;
    DataManager* dataManager;
    void addEntryToTable();
    void saveChannelsToCsv();

    void loadChannelsFromCsv();
    void sortTableWidget();
    void fillComboBox();
    void setupRegionsComboBox();
    void addDeleteButton(int row);
    void initUiElements();

};

#endif // NETWORKCHANNELS_H

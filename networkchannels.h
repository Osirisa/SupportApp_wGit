#ifndef NETWORKCHANNELS_H
#define NETWORKCHANNELS_H

#include <QDialog>
#include "DataManager/DataManager.h"

namespace Ui {
class NetworkChannels;
}

class NetworkChannels : public QDialog {
    Q_OBJECT

public:
    explicit NetworkChannels(DataManager* dataManager, QWidget* parent = nullptr);
    ~NetworkChannels();

private slots:
    void on_addButton_clicked();
    void on_saveButton_clicked();

    void on_deleteBTN_clicked();

private:
    Ui::NetworkChannels* ui;
    DataManager* dataManager;
    void addEntryToTable();
    void saveChannelsToCsv();
};

#endif // NETWORKCHANNELS_H

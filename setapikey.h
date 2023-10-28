#ifndef SETAPIKEY_H
#define SETAPIKEY_H

#include <QDialog>
#include "datamanager.h"
#include "encryptionhelper.h"


namespace Ui {
class SetAPIKey;
}

class SetAPIKey : public QDialog
{
    Q_OBJECT

public:
    explicit SetAPIKey(DataManager* dataManager, EncryptionHelper* encryptionHelper,QWidget *parent = nullptr);
    ~SetAPIKey();

private slots:
    void on_PB_Cancel_clicked();
    void on_PB_Save_clicked();

private:
    Ui::SetAPIKey *ui;
    DataManager* dataManager;
    EncryptionHelper* encryptionHelper;
};

#endif // SETAPIKEY_H

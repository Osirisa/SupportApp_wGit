#ifndef NETWORKPAGEBASE_H
#define NETWORKPAGEBASE_H

#include <QObject>
#include <QWidget>
#include <QPushButton>



class NetworkPageBase  : public QWidget
{
    Q_OBJECT
public:
    explicit NetworkPageBase(QWidget *parent =nullptr);

protected:
    struct StandardButtons{
        QPushButton* std_addToListBTN;
        QPushButton* std_exportToCSV;
        QPushButton* std_deleteAllBTN;
        QPushButton* std_sendOverAPI;
        QPushButton* std_select_Green;
        QPushButton* std_select_Orange;
        QPushButton* std_select_Red;
        QPushButton* std_select_Days;

        StandardButtons()
            : std_addToListBTN(nullptr), std_exportToCSV(nullptr), std_deleteAllBTN(nullptr),std_sendOverAPI(nullptr),std_select_Green(nullptr),std_select_Orange(nullptr),std_select_Red(nullptr),std_select_Days(nullptr) {}
    };

    void initStandardButtons(StandardButtons stdBtNs);
};

#endif // NETWORKPAGEBASE_H

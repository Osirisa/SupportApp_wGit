#ifndef P_SUPPORTPAGEAWIN_H
#define P_SUPPORTPAGEAWIN_H

#include <QWidget>
//#include "DataManager/adtractionsuppdatamanager.h"
#include "apimanager.h"
#include "Enums.h"
#include "DataManager/datamanager.h"
//#include "supportClasses/adtractionsuppcase.h"

namespace Ui {
class p_SupportPageAwin;
}

class p_SupportPageAwin : public QWidget
{
    Q_OBJECT

public:
    explicit p_SupportPageAwin(DataManager* dataManager,APIManager* apiManager,QWidget *parent = nullptr);
    ~p_SupportPageAwin();

private:
    Ui::p_SupportPageAwin *ui;

    DataManager* dataManager;
    APIManager* apiManager;
    //AwinSuppDataManager* suppdataManager;

    void initPage();
    void initInputElements();
};

#endif // P_SUPPORTPAGEAWIN_H

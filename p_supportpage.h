#ifndef P_SUPPORTPAGE_H
#define P_SUPPORTPAGE_H

#include <QWidget>
#include "advertiserdata.h"

namespace Ui {
class P_SupportPage;
}

class P_SupportPage : public QWidget
{
    Q_OBJECT

public:
    explicit P_SupportPage(QWidget *parent = nullptr);
    ~P_SupportPage();

private:
    Ui::P_SupportPage *ui;

public slots:
    void onShopsUpdated(const QHash<QString, AdvertiserData> &advertisers);
};

#endif // P_SUPPORTPAGE_H

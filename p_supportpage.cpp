#include "p_supportpage.h"
#include "ui_p_supportpage.h"

P_SupportPage::P_SupportPage(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::P_SupportPage)
{
    ui->setupUi(this);
}

P_SupportPage::~P_SupportPage()
{
    delete ui;
}

#include "mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QIcon appIcon(":/img/img/appLogo.png");
    a.setWindowIcon(appIcon);

    QCoreApplication::setApplicationName("SupportHelper");
    QCoreApplication::setApplicationVersion("0.1.0");
    QCoreApplication::setOrganizationName("Osirisa Software");

    MainWindow w;
    w.show();


    return a.exec();
}

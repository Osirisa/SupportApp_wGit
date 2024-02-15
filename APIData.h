#ifndef APIDATA_H
#define APIDATA_H

#include <QString>
#include <QList>

struct APIData {
    QString id;
    QString type;
    QString name;
    double value;
    int transactionType;
};

#endif // APIDATA_H

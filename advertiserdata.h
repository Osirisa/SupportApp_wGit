// advertiserdata.h
#ifndef ADVERTISERDATA_H
#define ADVERTISERDATA_H

#include <QString>
#include <QList>

struct Commission {
    QString id;
    QString type;
    QString name;
    double value;
    int transactionType;
};

struct AdvertiserData {
    int programId;
    QString market;
    QString currency;
    QString programName;
    QList<Commission> commissions;
};

#endif  // ADVERTISERDATA_H

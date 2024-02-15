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

struct SuppDetail{
    QString channel;
    QString shop;
    QString value;
    QString expProv;
    QString currency;
    QString orderId;
    QString userId;
    QString date;
    QString commissionText;
    QString suppType;
    QString nStat;
    QString network;
    QString programId;
    QString commissionId;

    SuppDetail(QString channel, QString shop, QString value, QString expProv, QString currency, QString orderId, QString userId, QString date, QString commissionText, QString suppType, QString nStat, QString network, QString programId, QString commissionId)
        : channel(std::move(channel)), shop(std::move(shop)), value(std::move(value)), expProv(std::move(expProv)), currency(std::move(currency)), orderId(std::move(orderId)), userId(std::move(userId)),
        date(std::move(date)), commissionText(std::move(commissionText)), suppType(std::move(suppType)), nStat(std::move(nStat)),network(std::move(network)),programId(std::move(programId)),commissionId(std::move(commissionId)) {}
};

//Enum for every column in the table for better Code transparency
enum ColumnIndexes{

    //visible
    eCol_Channel        = 0,
    eCol_Shop           = 1,
    eCol_Value          = 2,
    eCol_ExpProv        = 3,
    eCol_Currency       = 4,
    eCol_OrderId        = 5,
    eCol_UserId         = 6,
    eCol_Date           = 7,
    eCol_CommissionText = 8,
    eCol_CommissionType = 9,
    eCol_DaysOld        = 10,
    eCol_Networkstatus  = 11,
    eCol_SendBTN        = 12,
    eCol_DeleteBTN      = 13,

    //invisible
    eCol_H_Nstat        = 14,
    eCol_H_Network      = 15,
    eCol_H_ProgramId    = 16,
    eCOl_H_CommissionId = 17
};

enum suppNetStatus{
    eNstat_NotSend      = 0,
    eNstat_Good         = 1,
    eNstat_Okay         = 2,
    eNstat_Error        = 3
};

enum sortingStatus{
    eSStat_None,
    eSStat_Ascending,
    eSStat_Descending
};

enum toggleTable{
    eTT_Small,
    eTT_Normal,
    eTT_Big
};

#endif  // ADVERTISERDATA_H

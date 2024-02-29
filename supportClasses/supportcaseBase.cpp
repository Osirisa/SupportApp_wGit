#include "supportcaseBase.h"
#include <QDebug>
#include <QUuid>

SupportCaseBase::SupportCaseBase(const BaseParams& baseParams)
    :orderId(baseParams.orderId), currency(baseParams.currency), userId(baseParams.userId), orderVal(baseParams.orderVal){

    uuId = QUuid::createUuid().toString(QUuid::WithoutBraces);
}

SupportCaseBase::~SupportCaseBase(){
    //Destructor
}

void SupportCaseBase::printDetailsDebug() const {

    qDebug()<<"orderId: "<< orderId
             <<", userId: "<< userId
             <<", orderVal: "<< orderVal
             <<", currency: "<< currency;
}
BaseParams SupportCaseBase::getAllAttributes() const {

    BaseParams params;

    params.network = network;
    params.orderId = orderId;
    params.userId = userId;
    params.orderVal = orderVal;
    params.currency = currency;

    return params;
}

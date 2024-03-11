#include "supportClasses/adtractionsuppcase.h"

AdtractionSuppCase::AdtractionSuppCase(const AdtractionParams adtractionParams)
    :SupportCaseBase(adtractionParams),
    networkStatus(adtractionParams.networkStatus),networkStatusText(adtractionParams.networkStatusText),
    channel(adtractionParams.channel), date(adtractionParams.date),lastEditDate(adtractionParams.lastEditDate), shop(adtractionParams.shop),
    programId(adtractionParams.programId),commissionId(adtractionParams.commissionId), commissionText(adtractionParams.commissionText),
    expProv(adtractionParams.expProv){

    network = "Adtraction";
}

AdtractionSuppCase::~AdtractionSuppCase()
{

}

AdtractionParams AdtractionSuppCase::getAllAttributes() const
{

    AdtractionParams params;
    BaseParams baseParams = SupportCaseBase::getAllAttributes();

    //first all BaseParameters:
    params.network = baseParams.network;
    params.orderId = baseParams.orderId;
    params.userId = baseParams.userId;
    params.orderVal = baseParams.orderVal;
    params.currency = baseParams.currency;

    // Now, add derived attributes
    params.networkStatus = networkStatus;
    params.networkStatusText = networkStatusText;
    params.channel = channel;

    params.date = date;
    params.lastEditDate = lastEditDate;

    params.shop = shop;
    params.programId = programId;
    params.commissionId = commissionId;
    params.commissionText = commissionText;

    params.expProv = expProv;

    return params;
}



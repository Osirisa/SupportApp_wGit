#include "adtractionsuppcase.h"

AdtractionSuppCase::AdtractionSuppCase(const AdtractionParams adtractionParams)
    :SupportCaseBase(adtractionParams),
    network(adtractionParams.network), channel(adtractionParams.channel), date(adtractionParams.date),
    shop(adtractionParams.shop), programId(adtractionParams.programId),commissionId(adtractionParams.commissionId),
    commissionText(adtractionParams.commissionText),expProv(adtractionParams.expProv){}

AdtractionParams AdtractionSuppCase::getAllAttributes() const
{

    AdtractionParams params;
    BaseParams baseParams = SupportCaseBase::getAllAttributes();

    //first all BaseParameters:
    params.orderId = baseParams.orderId;
    params.userId = baseParams.userId;
    params.orderVal = baseParams.orderVal;
    params.currency = baseParams.currency;

    // Now, add derived attributes
    params.channel = channel;
    params.shop = shop;
    params.expProv = expProv;
    params.date = date;
    params.commissionText = commissionText;
    params.network = network;
    params.programId = programId;
    params.commissionId = commissionId;

    return params;

}



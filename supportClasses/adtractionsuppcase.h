#ifndef ADTRACTIONSUPPCASE_H
#define ADTRACTIONSUPPCASE_H

#include "supportClasses/supportcaseBase.h"

struct AdtractionParams : BaseParams {

    QString networkStatus;
    QString networkStatusText;
    QString channel;

    QString date;
    QString lastEditDate;

    QString shop;
    QString programId;
    QString commissionId;
    QString commissionText;

    double expProv;
};

class AdtractionSuppCase : public SupportCaseBase
{
public:
    AdtractionSuppCase(const AdtractionParams adtractionParams);
    virtual ~AdtractionSuppCase();


    //----------------Getters & Setters----------------------
    //Getters
    AdtractionParams getAllAttributes() const;

    QString getNetworkStatus()      const noexcept { return networkStatus; }
    QString getNetworkStatusText()  const noexcept { return networkStatusText; }
    QString getChannel()            const noexcept {return channel;};

    QString getDate()               const noexcept {return date;};
    QString getLastEditDate()       const noexcept { return lastEditDate; }

    QString getShop()               const noexcept {return shop;};
    QString getProgramId()          const noexcept {return programId;};
    QString getCommissionId()       const noexcept {return commissionId;};
    QString getCommissionText()     const noexcept {return commissionText;};

    double getExpProv()             const noexcept {return expProv;};

    // Setters

    void setNetworkStatus(const QString& newNetworkStatus)          noexcept { networkStatus = newNetworkStatus; }
    void setNetworkStatusText(const QString& newNetworkStatusText)  noexcept { networkStatusText = newNetworkStatusText; }
    void setChannel(const QString& newChannel)                      noexcept { channel = newChannel; }

    void setDate(const QString& newDate)                            noexcept { date = newDate; }
    void setLastEditDate(const QString& newLastEditDate)            noexcept { lastEditDate = newLastEditDate; }

    void setShop(const QString& newShop)                            noexcept { shop = newShop; }
    void setProgramId(const QString& newProgramId)                  noexcept { programId = newProgramId; }
    void setCommissionId(const QString& newCommissionId)            noexcept { commissionId = newCommissionId; }
    void setCommissionText(const QString& newCommissionText)        noexcept { commissionText = newCommissionText; }

    void setExpProv(double newExpProv)                              noexcept { expProv = newExpProv; }

private:

    QString networkStatus;
    QString networkStatusText;
    QString channel;

    QString date;
    QString lastEditDate;

    QString shop;
    QString programId;
    QString commissionId;
    QString commissionText;

    double expProv;
};

#endif // ADTRACTIONSUPPCASE_H

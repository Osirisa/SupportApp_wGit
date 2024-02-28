#ifndef ADTRACTIONSUPPCASE_H
#define ADTRACTIONSUPPCASE_H

#include "supportClasses/supportcaseBase.h"

struct AdtractionParams : BaseParams {

    QString network;
    QString channel;

    QString date;

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
    QString getNetwork()        const noexcept {return network;};
    QString getChannel()        const noexcept {return channel;};

    QString getDate()           const noexcept {return date;};

    QString getShop()           const noexcept {return shop;};
    QString getProgramId()      const noexcept {return programId;};
    QString getCommissionId()   const noexcept {return commissionId;};
    QString getCommissionText() const noexcept {return commissionText;};

    double getExpProv()         const noexcept {return expProv;};

    // Setters
    void setNetwork(const QString& newNetwork) noexcept { network = newNetwork; }
    void setChannel(const QString& newChannel) noexcept { channel = newChannel; }

    void setDate(const QString& newDate) noexcept { date = newDate; }

    void setShop(const QString& newShop) noexcept { shop = newShop; }
    void setProgramId(const QString& newProgramId) noexcept { programId = newProgramId; }
    void setCommissionId(const QString& newCommissionId) noexcept { commissionId = newCommissionId; }
    void setCommissionText(const QString& newCommissionText) noexcept { commissionText = newCommissionText; }

    void setExpProv(double newExpProv) noexcept { expProv = newExpProv; }

private:
    QString network;
    QString channel;

    QString date;

    QString shop;
    QString programId;
    QString commissionId;
    QString commissionText;

    double expProv;
};

#endif // ADTRACTIONSUPPCASE_H

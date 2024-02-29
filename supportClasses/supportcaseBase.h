#ifndef SUPPORTCASEBASE_H
#define SUPPORTCASEBASE_H

#include <qstring.h>

struct BaseParams{

    QString network;
    QString orderId;
    QString userId;

    double orderVal;
    QString currency;
};

class SupportCaseBase
{
public:
    SupportCaseBase(const BaseParams& baseParams);

    virtual ~SupportCaseBase();
    virtual void printDetailsDebug() const;

    //Getters
    BaseParams getAllAttributes()           const;

    const QString getNetwork()              const noexcept {return network;};
    const QString& getOrderId()             const noexcept { return orderId; }
    const QString& getUserId()              const noexcept { return userId; }
    double getOrderVal()                    const noexcept { return orderVal; }
    const QString& getCurrency()            const noexcept { return currency; }

    //uuID
    QString getUuId()                       const noexcept { return uuId; }


    // Setters
    void setNetwork(const QString& newNetwork)      noexcept { network = newNetwork; }
    void setOrderId(const QString& newOrderId)      noexcept { orderId = newOrderId; }
    void setUserId(const QString& newUserId)        noexcept { userId = newUserId; }
    void setOrderVal(double newOrderVal)            noexcept { orderVal = newOrderVal; }
    void setCurrency(const QString& newCurrency)    noexcept { currency = newCurrency; }

protected:
    QString uuId;

    QString network;
    QString orderId;
    QString currency;
    QString userId;

    double orderVal;
};

#endif // SUPPORTCASEBASE_H

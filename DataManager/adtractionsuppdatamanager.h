#ifndef ADTRACTIONSUPPDATAMANAGER_H
#define ADTRACTIONSUPPDATAMANAGER_H

#include <QObject>
#include "basesuppdatamanager.h"
#include "supportClasses/adtractionsuppcase.h"

class AdtractionSuppDataManager : public BaseSuppDataManager
{
public:
    AdtractionSuppDataManager(DataManager *dataManager);

    void saveObjectToSessionFile(const AdtractionSuppCase& adtractionObject);
    void saveObjectToSessionFile(const QList<AdtractionSuppCase>& adtractionObjects);

    AdtractionSuppCase loadObjectFromSessionFile(QString& userId, QString& orderId);
    QList<AdtractionSuppCase> loadAllObjectsFromSessionFile();

    void deleteOrder(const QString& userId, const QString& orderId);
};

#endif // ADTRACTIONSUPPDATAMANAGER_H

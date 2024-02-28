#ifndef BASESUPPDATAMANAGER_H
#define BASESUPPDATAMANAGER_H

#include <QObject>

class BaseSuppDataManager : public QObject
{
    Q_OBJECT
public:
    BaseSuppDataManager(QObject *parent = nullptr);
    virtual ~BaseSuppDataManager();



protected:

    QString pathToSessionJson;
};

#endif // BASESUPPDATAMANAGER_H

// suppeventbus.h
#ifndef SUPPEVENTBUS_H
#define SUPPEVENTBUS_H

#include <QObject>
#include <QVariant>

class SuppEventBus : public QObject
{
    Q_OBJECT
public:
    static SuppEventBus* instance();

    // Custom method to publish events
    void publish(const QString& eventName, const QVariant& eventData1 = QVariant(), const QVariant& eventData2 = QVariant(), const QVariant& eventData3 = QVariant());

signals:
    // Signal emitted when an event is published
    void eventPublished(const QString& eventName,  const QVariant& eventData1, const QVariant& eventData2, const QVariant& eventData3);

private:
    SuppEventBus(QObject *parent = nullptr);
    SuppEventBus(const SuppEventBus&) = delete;
    SuppEventBus& operator=(const SuppEventBus&) = delete;
};

#endif // SUPPEVENTBUS_H

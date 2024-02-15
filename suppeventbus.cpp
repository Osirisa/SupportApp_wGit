// suppeventbus.cpp
#include "suppeventbus.h"

SuppEventBus* SuppEventBus::instance() {
    static SuppEventBus bus;
    return &bus;
}

SuppEventBus::SuppEventBus(QObject *parent) : QObject(parent) {}

void SuppEventBus::publish(const QString& eventName, const QVariant& eventData1, const QVariant& eventData2, const QVariant& eventData3) {
    emit eventPublished(eventName, eventData1 , eventData2, eventData3);
}

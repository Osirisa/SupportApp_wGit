#include "Networks/adtraction.h"

// Implementation for RefreshAdvertisersDerived methods

void Adtraction::UpdateAdvertisersDerived::byChannel(int channelID) {

}

void Adtraction::UpdateAdvertisersDerived::allChannels() {

}


QStringList Adtraction::GetAdvertisersDerived::fromChannel(int channelID) {
    QStringList advertisers;

    advertisers << "Advertiser1" << "Advertiser2"; // Example data
    return advertisers;
}

QStringList Adtraction::GetAdvertisersDerived::fromNetwork() {
    QStringList advertisers;

    advertisers << "AdvertiserA" << "AdvertiserB" << "AdvertiserC"; // Example data
    return advertisers;
}

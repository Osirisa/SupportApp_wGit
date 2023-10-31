#include "APIManager.h"

APIManager::APIManager() : getAdvertisers(this), refreshAdvertisers(this), admin(this) {}

// Implementations for Admin
void APIManager::Admin::addAPIKey(const QString& apiName, const QString& key) {
    apiManager->networks[apiName].apiKey = key;
}

void APIManager::Admin::addAPIChannelID(const QString& apiName, int channelID) {
    apiManager->networks[apiName].channelIDs.append(channelID);
}

// Implementations for GetAdvertisers
QStringList APIManager::GetAdvertisers::all() {
    // ... Logic to get all advertisers
    QStringList result;
    return result;
}

QStringList APIManager::GetAdvertisers::fromChannel(const QString& apiName, int channelID) {
    // ... Logic to get advertisers from specific channel
    QStringList result;
    return result;
}

QStringList APIManager::GetAdvertisers::fromNetwork(const QString& apiName) {
    // ... Logic to get advertisers from a specific network
    QStringList result;
    return result;
}

// Implementations for RefreshAdvertisers
void APIManager::RefreshAdvertisers::all() {
    // ... Logic to refresh all advertisers
}

void APIManager::RefreshAdvertisers::fromChannel(const QString& apiName, int channelID) {
    // ... Logic to refresh advertisers from specific channel
}

void APIManager::RefreshAdvertisers::fromNetwork(const QString& apiName) {
    // ... Logic to refresh advertisers from a specific network
}

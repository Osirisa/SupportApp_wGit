#ifndef ADTRACTION_H
#define ADTRACTION_H

#include "BaseClasses/BaseNetwork.h"

class Adtraction : public BaseNetwork {
public:
    Adtraction();

    class UpdateAdvertisersDerived : public UpdateAdvertisersBase {
    public:
       UpdateAdvertisersDerived(Adtraction* parent);

        void byChannel(int channelID) override;
        void allChannels() override;

    private:
        Adtraction* parentAdtraction;
    } UpdateAdvs;

    class GetAdvertisersDerived : public GetAdvertisersBase {
    public:
        GetAdvertisersDerived(Adtraction* parent);

        QStringList fromChannel(int channelID) override;
        QStringList fromNetwork() override;

    private:
        Adtraction* parentAdtraction;
    } GetAdvs;
};

#endif // ADTRACTION_H

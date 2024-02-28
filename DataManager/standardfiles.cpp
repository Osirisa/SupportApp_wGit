#include "standardfiles.h"

StandardFiles::StandardFiles(DataManager *dataManager) {

    //General NetworkFiles
    dataManager->registerFile("NetworkChannels","NetworkFiles/NetworkChannels.json");

    //AdcellFiles

    //AdtracionFiles
    dataManager->registerFile("AdtractionSessionFile",  "dataAdtraction/AdtractionSessionFile.json");
    dataManager->registerFile("AdtractionCurrencies",   "dataAdtraction/AdtractionCurrencies.txt");
    dataManager->registerFile("AdtractionKey",          "dataAdtraction/AdtractionApiKey.txt");
    dataManager->registerFile("AdtractionRegions",      "dataAdtraction/AdtractionRegions.json");

    //AwinFiles



    //CJFiles

    //WebgainsFile

    //TradeDoublerFiles

}

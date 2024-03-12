#include "DataManager/adtractionsuppdatamanager.h"

#include "qjsonarray.h"
#include "qjsonobject.h"
#include "qmessagebox.h"

AdtractionSuppDataManager::AdtractionSuppDataManager(DataManager* dataManager) : BaseSuppDataManager(dataManager) {

    qRegisterMetaType<Shop>("Shop");
    qRegisterMetaType<Commission>("Commission");

    //init document
    QJsonDocument sessionDoc = dataManager->json->load("AdtractionSessionFile");
    if (sessionDoc.isNull()) {
        // Handle error
        QJsonDocument startDoc;
        dataManager->json->save("AdtractionSessionFile",startDoc);
    }


}
AdtractionSuppDataManager::~AdtractionSuppDataManager(){}

bool AdtractionSuppDataManager::saveObjectToSessionFile(const AdtractionSuppCase &adtractionObject)
{
    // Load the existing session document
    QJsonDocument sessionDoc = dataManager->json->load("AdtractionSessionFile");
    // if (sessionDoc.isNull()) {
    //     qDebug() << "Error: File could not be loaded.";
    //     return false;
    // }

    // Extract the root object
    QJsonObject sessionObj = sessionDoc.object();
    // Extract or initialize the users object
    QJsonObject users = sessionObj["users"].toObject();
    // Extract attributes from the adtractionObject
    AdtractionParams suppParams = adtractionObject.getAllAttributes();

    // Prepare the new order object
    QJsonObject newOrderObj = {
        {"networkStatus",       suppParams.networkStatus},
        {"networkStatusText",   suppParams.networkStatusText},
        {"channel",             suppParams.channel},
        {"date",                suppParams.date},
        {"lastEditDate",        suppParams.lastEditDate},
        {"shop",                suppParams.shop},
        {"programId",           suppParams.programId},
        {"commissionId",        suppParams.commissionId},
        {"commissionText",      suppParams.commissionText},
        {"expProv",             suppParams.expProv},
        {"network",             suppParams.network},
        {"orderId",             suppParams.orderId},
        {"currency",            suppParams.currency},
        {"userId",              suppParams.userId},
        {"orderVal",            suppParams.orderVal}
    };

    // Check if the user already exists
    if (users.contains(suppParams.userId)) {
        QJsonObject userObj = users[suppParams.userId].toObject();
        QJsonArray ordersArray = userObj["orders"].toArray();

        // Check if the order already exists for this user
        for (int i = 0; i < ordersArray.size(); ++i) {
            QJsonObject orderObj = ordersArray[i].toObject();
            if (orderObj["orderId"].toString() == suppParams.orderId) {

                QMessageBox::StandardButton reply = QMessageBox::question(nullptr, "Order ID Already Exists",
                                                                          "This order ID: " + suppParams.orderId + " already exists for this user. Do you want to override it?",
                                                                          QMessageBox::Yes | QMessageBox::No);
                if (reply == QMessageBox::No) {
                    return false;
                } else {
                    ordersArray.removeAt(i); // Remove the existing order
                    break;
                }
            }
        }


        ordersArray.append(newOrderObj); // Add or update the order


        userObj["orders"] = ordersArray; // Update the user's orders
        users[suppParams.userId] = userObj; // Update the user object in users
    } else {
        // User does not exist, create a new user object with the order
        QJsonObject newUserObj;
        QJsonArray newOrdersArray = {newOrderObj}; // Initialize with the new order
        newUserObj["orders"] = newOrdersArray; // Add orders to the new user object
        users[suppParams.userId] = newUserObj; // Add the new user object to users
    }

    // Update the session object with the new or updated users object
    sessionObj["users"] = users;

    // Save the updated session document
    dataManager->json->save("AdtractionSessionFile", QJsonDocument(sessionObj));

    return true;
}


bool AdtractionSuppDataManager::saveObjectToSessionFile(const QList<AdtractionSuppCase> &adtractionObjects)
{
    for (const AdtractionSuppCase& object : adtractionObjects) {
        saveObjectToSessionFile(object);
    }
    return true;
}

AdtractionSuppCase AdtractionSuppDataManager::loadObjectFromSessionFile(const QString &userId, const QString &orderId)
{
    QJsonDocument sessionDoc = dataManager->json->load("AdtractionSessionFile");

    if (sessionDoc.isNull()) {
        qDebug() << "Error loading JSON file or it is Empty";
        //return AdtractionSuppCase();
    }

    QJsonObject sessionObj = sessionDoc.object();
    QJsonObject users = sessionObj["users"].toObject();

    if (!users.contains(userId)) {
        qDebug() << "User ID not found.";
        //return AdtractionSuppCase();
    }

    QJsonObject userObj = users[userId].toObject();
    QJsonArray ordersArray = userObj["orders"].toArray();

    for (const QJsonValue &orderVal : ordersArray) {
        QJsonObject orderObj = orderVal.toObject();
        if (orderObj["orderId"].toString() == orderId) {
            AdtractionParams params;
            params.networkStatus = orderObj["networkStatus"].toString();
            params.networkStatusText = orderObj["networkStatusText"].toString();
            params.channel = orderObj["channel"].toString();
            params.date = orderObj["date"].toString();
            params.lastEditDate = orderObj["lastEditDate"].toString();
            params.shop = orderObj["shop"].toString();
            params.programId = orderObj["programId"].toString();
            params.commissionId = orderObj["commissionId"].toString();
            params.commissionText = orderObj["commissionText"].toString();
            params.expProv = orderObj["expProv"].toDouble();
            params.network = orderObj["network"].toString();
            params.orderId = orderObj["orderId"].toString();
            params.currency = orderObj["currency"].toString();
            params.userId = orderObj["userId"].toString();
            params.orderVal = orderObj["orderVal"].toDouble();

            return AdtractionSuppCase(params);
        }
    }
    AdtractionParams noParams;
    qDebug() << "Order ID not found for given user ID.";
    return AdtractionSuppCase(noParams);
}


QList<AdtractionSuppCase> AdtractionSuppDataManager::loadAllObjectsFromSessionFile()
{
    QJsonDocument sessionDoc = dataManager->json->load("AdtractionSessionFile");

    if (sessionDoc.isNull()) {
        qDebug() << "Error loading JSON file or it is Empty";
        return QList<AdtractionSuppCase>(); // Return an empty list in case of error
    }

    QList<AdtractionSuppCase> cases;
    QJsonObject sessionObj = sessionDoc.object();
    QJsonObject users = sessionObj["users"].toObject();

    for (auto it = users.begin(); it != users.end(); ++it) {
        QString userId = it.key();
        QJsonObject userObj = it.value().toObject();
        QJsonArray ordersArray = userObj["orders"].toArray();

        for (const QJsonValue &orderVal : ordersArray) {
            QJsonObject orderObj = orderVal.toObject();
            AdtractionParams params;
            params.networkStatus = orderObj["networkStatus"].toString();
            params.networkStatusText = orderObj["networkStatusText"].toString();
            params.channel = orderObj["channel"].toString();
            params.date = orderObj["date"].toString();
            params.lastEditDate = orderObj["lastEditDate"].toString();
            params.shop = orderObj["shop"].toString();
            params.programId = orderObj["programId"].toString();
            params.commissionId = orderObj["commissionId"].toString();
            params.commissionText = orderObj["commissionText"].toString();
            params.expProv = orderObj["expProv"].toDouble();
            params.network = orderObj["network"].toString();
            params.orderId = orderObj["orderId"].toString();
            params.currency = orderObj["currency"].toString();
            params.userId = userId; // Since the userId is known from the loop
            params.orderVal = orderObj["orderVal"].toDouble();

            cases.append(AdtractionSuppCase(params));
        }
    }

    return cases;
}


QList<AdtractionNetworkChannelData> AdtractionSuppDataManager::getAdtractionNetworkChannels()
{
    QJsonDocument networksDoc = this->getNetworkChannelsDoc();
    QJsonObject networksObj = networksDoc.object();

    // Each network's key is the network name, and its value is another JSON object containing channels
    QJsonObject channelsObj = networksObj["Adtraction"].toObject();

    QList<AdtractionNetworkChannelData> networksData;

    for (auto channel = channelsObj.begin(); channel != channelsObj.end(); ++channel) {
        // Each channel's key is the channel ID, and its value is another JSON object containing channel details
        QJsonObject channelDetails = channel.value().toObject();

        AdtractionNetworkChannelData channelData;

        // Extract channel details
        channelData.channelID = channel.key();
        channelData.channelName = channelDetails["channelName"].toString();

        networksData.append(AdtractionNetworkChannelData(channelData));
    }

    return networksData;
}

QList<Shop> AdtractionSuppDataManager::getShopsByChannelID(const QString& channelID) {
    QList<Shop> shops;

    // Load the document that contains shops for the specified channelID
    QJsonDocument channelShopsDoc = dataManager->json->load("Adtraction" + channelID);
    QJsonArray shopsArray = channelShopsDoc.array();

    // Iterate through each shop in the array
    for (const QJsonValue &value : shopsArray) {
        QJsonObject shopObj = value.toObject();

        Shop shop;
        shop.programName = shopObj["programName"].toString();
        shop.programId = shopObj["programId"].toInt();
        shop.channelID = channelID; // Assuming each shop in this document belongs to the same channel

        // Commissions
        QJsonArray commissionsArray = shopObj["commissions"].toArray();
        for (const QJsonValue &commissionValue : commissionsArray) {
            QJsonObject commissionObj = commissionValue.toObject();
            Commission commission;
            commission.id = commissionObj["id"].toString();
            commission.name = commissionObj["name"].toString();
            commission.type = commissionObj["type"].toString();
            commission.value = commissionObj["value"].toDouble();

            shop.commissions.append(commission);
        }

        shops.append(shop);
    }

    return shops;
}


QStringList AdtractionSuppDataManager::getSortedCurrencies()
{
    QStringList sortedCurrencies;
    QSet<QString> preferredCurrencies = {"EUR", "CHF", "USD"};
    QStringList otherCurrencies;

    QJsonDocument currencyDoc = dataManager->json->load("AdtractionCurrencies");
    if (currencyDoc.isNull()) {
        qDebug() << "Error loading currency JSON file or it is empty";
        return QStringList(); // Return an empty list
    }

    for (const QJsonValue &value : currencyDoc.array()) {
        QJsonObject obj = value.toObject();
        QString currency = obj["currency"].toString();
        if (preferredCurrencies.contains(currency)) {
            sortedCurrencies.append(currency);
        } else {
            otherCurrencies.append(currency);
        }
    }

    // Sort the "other" currencies list and append to the sortedCurrencies list
    std::sort(otherCurrencies.begin(), otherCurrencies.end());
    sortedCurrencies.append(otherCurrencies);

    return sortedCurrencies;
}

void AdtractionSuppDataManager::deleteOrder(const QString &userId, const QString &orderId)
{
    // Load the existing session document
    QJsonDocument sessionDoc = dataManager->json->load("AdtractionSessionFile");

    if (sessionDoc.isNull()) {
        qDebug() << "Error: File could not be loaded.";
        return;
    }

    QJsonObject sessionObj = sessionDoc.object();
    QJsonObject users = sessionObj["users"].toObject();

    // Check if the specified user exists
    if (users.contains(userId)) {
        QJsonObject userObj = users[userId].toObject();
        QJsonArray ordersArray = userObj["orders"].toArray();

        bool orderFound = false;
        for (int i = 0; i < ordersArray.size(); ++i) {
            QJsonObject orderObj = ordersArray[i].toObject();
            if (orderObj["orderId"].toString() == orderId) {
                ordersArray.removeAt(i); // Remove the matching order
                orderFound = true;
                break;
            }
        }

        if (!orderFound) {
            qDebug() << "Order ID: " << orderId << " does not exist for user ID: " << userId;
            return;
        }

        if (ordersArray.isEmpty()) {
            // If user has no more orders, you might choose to remove the user as well
            users.remove(userId);
        } else {
            // Otherwise, update the user's orders array
            userObj["orders"] = ordersArray;
            users[userId] = userObj; // Reassign the modified user object back to the users object
        }

        // Update the session object with the modified users object
        sessionObj["users"] = users;
    } else {
        qDebug() << "User ID: " << userId << " not found.";
        return;
    }

    // Save the updated session document
    dataManager->json->save("AdtractionSessionFile", QJsonDocument(sessionObj));
}


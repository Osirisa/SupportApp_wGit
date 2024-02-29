#include "adtractionsuppdatamanager.h"
#include "qjsonarray.h"
#include "qjsonobject.h"

AdtractionSuppDataManager::AdtractionSuppDataManager(DataManager* dataManager) : BaseSuppDataManager(dataManager) {}

void AdtractionSuppDataManager::saveObjectToSessionFile(const AdtractionSuppCase &adtractionObject)
{
    QJsonDocument sessionDoc = dataManager.json->load("AdtractionSessionFile");

    if(sessionDoc.isNull()){
        //qDebug() << "Error loading JSON file or is Empty";
        return;
    }

    QJsonObject sessionObj = sessionDoc.object();

    // Assuming AdtractionParams is a struct or class that holds all attributes of AdtractionSuppCase
    AdtractionParams suppParams = adtractionObject.getAllAttributes();

    // Update or add user
    QJsonObject users = sessionObj["users"].toObject();
    QJsonObject user = users[suppParams.userId].toObject(); // Get the specific user object
    QJsonArray userOrders = user["orderIDs"].toArray();
    if (!userOrders.contains(QJsonValue(suppParams.orderId))) {
        userOrders.append(QJsonValue(suppParams.orderId));
    }
    user["orderIDs"] = userOrders; // Update the user's orders array
    users[suppParams.userId] = user; // IMPORTANT: Reassign the updated user object back to the users object
    sessionObj["users"] = users; // Reassign the updated users object back to the sessionObj

    // Update or add order
    QJsonObject orders = sessionObj["orders"].toObject();
    QJsonObject orderDetails = {
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
    orders[suppParams.orderId] = orderDetails;
    sessionObj["orders"] = orders; // Update the session object with the new orders

    // Save the updated document
    dataManager.json->save("AdtractionSessionFile", QJsonDocument(sessionObj));
}

void AdtractionSuppDataManager::saveObjectToSessionFile(const QList<AdtractionSuppCase> &adtractionObjects)
{
    for (const AdtractionSuppCase& object : adtractionObjects) {
        saveObjectToSessionFile(object);
    }
}

AdtractionSuppCase AdtractionSuppDataManager::loadObjectFromSessionFile(QString &userId, QString &orderId)
{
    QJsonDocument sessionDoc = dataManager.json->load("AdtractionSessionFile");

    if (sessionDoc.isNull()) {
        qDebug() << "Error loading JSON file or it is Empty";
        // Return a default or null AdtractionSuppCase object
        //TBD:: return AdtractionSuppCase(); // Assuming AdtractionSuppCase has a default constructor
    }

    QJsonObject sessionObj = sessionDoc.object();
    QJsonObject orders = sessionObj["orders"].toObject();
    QJsonObject orderDetails = orders.value(orderId).toObject();

    // Check if the order belongs to the requested userId
    if (orderDetails["userId"].toString() != userId) {
        qDebug() << "Order ID does not belong to the user ID";
        // Return a default or null AdtractionSuppCase object
        //TBD:: return AdtractionSuppCase(); // Assuming AdtractionSuppCase has a default constructor
    }

    AdtractionParams params;
    // Populate params with orderDetails
    params.networkStatus        = orderDetails["networkStatus"].toString();
    params.networkStatusText    = orderDetails["networkStatusText"].toString();
    params.channel              = orderDetails["channel"].toString();
    params.date                 = orderDetails["date"].toString();
    params.lastEditDate         = orderDetails["lastEditDate"].toString();
    params.shop                 = orderDetails["shop"].toString();
    params.programId            = orderDetails["programId"].toString();
    params.commissionId         = orderDetails["commissionId"].toString();
    params.commissionText       = orderDetails["commissionText"].toString();
    params.expProv              = orderDetails["expProv"].toDouble();
    params.network              = orderDetails["network"].toString();
    params.orderId              = orderDetails["orderId"].toString();
    params.currency             = orderDetails["currency"].toString();
    params.userId               = orderDetails["userId"].toString();
    params.orderVal             = orderDetails["orderVal"].toDouble();

    // Assuming AdtractionSuppCase can be constructed using AdtractionParams
    return AdtractionSuppCase(params);
}

QList<AdtractionSuppCase> AdtractionSuppDataManager::loadAllObjectsFromSessionFile()
{
    QJsonDocument sessionDoc = dataManager.json->load("AdtractionSessionFile");

    if (sessionDoc.isNull()) {
        // qDebug() << "Error loading JSON file or it is Empty";
        return QList<AdtractionSuppCase>(); // Return an empty list in case of error
    }

    QList<AdtractionSuppCase> cases;
    QJsonObject sessionObj = sessionDoc.object();
    QJsonObject orders = sessionObj["orders"].toObject();

    for (const QString& orderId : orders.keys()) {
        QJsonObject orderDetails = orders[orderId].toObject();

        AdtractionParams params;
        // Populate params with orderDetails
        params.networkStatus        = orderDetails["networkStatus"].toString();
        params.networkStatusText    = orderDetails["networkStatusText"].toString();
        params.channel              = orderDetails["channel"].toString();
        params.date                 = orderDetails["date"].toString();
        params.lastEditDate         = orderDetails["lastEditDate"].toString();
        params.shop                 = orderDetails["shop"].toString();
        params.programId            = orderDetails["programId"].toString();
        params.commissionId         = orderDetails["commissionId"].toString();
        params.commissionText       = orderDetails["commissionText"].toString();
        params.expProv              = orderDetails["expProv"].toDouble();
        params.network              = orderDetails["network"].toString();
        params.orderId              = orderId;
        params.currency             = orderDetails["currency"].toString();
        params.userId               = orderDetails["userId"].toString();
        params.orderVal             = orderDetails["orderVal"].toDouble();

        cases.append(AdtractionSuppCase(params));
    }

    return cases;
}

void AdtractionSuppDataManager::deleteOrder(const QString &userId, const QString &orderId)
{

    QJsonDocument sessionDoc = dataManager.json->load("AdtractionSessionFile");

    if (sessionDoc.isNull()) {
        // Handle error: JSON file load failed
        return;
    }

    QJsonObject sessionObj = sessionDoc.object();
    QJsonObject orders = sessionObj["orders"].toObject();
    QJsonObject users = sessionObj["users"].toObject();

    // Step 1: Delete the specified order
    if (orders.contains(orderId)) {
        orders.remove(orderId); // Remove the order
        sessionObj["orders"] = orders; // Update the session object with the modified orders

        // Step 2: Update user's orders list
        QJsonObject user = users[userId].toObject();
        QJsonArray userOrders = user["orderIDs"].toArray();
        QJsonArray updatedUserOrders;
        foreach (const QJsonValue &value, userOrders) {
            if (value.toString() != orderId) {
                updatedUserOrders.append(value);
            }
        }

        if (updatedUserOrders.isEmpty()) {
            // If user has no more orders, remove the user
            users.remove(userId);
        } else {
            // Otherwise, update the user's orders
            user["orderIDs"] = updatedUserOrders;
            users[userId] = user; // Make sure to reassign the modified user object back to the users object
        }

        sessionObj["users"] = users; // Update the session object with the modified users

        // Save the updated JSON back to the file
        dataManager.json->save("AdtractionSessionFile", QJsonDocument(sessionObj));
    } else {
        qDebug()<<orderId << " doesnt exist";
        // Handle error: Order ID not found
    }
}

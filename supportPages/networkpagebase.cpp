#include "networkpagebase.h"
#include "qlineedit.h"
#include <QMessageBox>

NetworkPageBase::NetworkPageBase(QWidget *parent) : QWidget(parent)
{

}

NetworkPageBase::~NetworkPageBase()
{

}

void NetworkPageBase::initStandardButtons(StandardButtons stdBTNs)
{
    //Add To list Button
    QPixmap pm_addToList(":/img/img/plusIcon_trans.png");
    QIcon i_addToList(pm_addToList);

    stdBTNs.std_addToListBTN->setIcon(i_addToList);
    stdBTNs.std_addToListBTN->setToolTip("Adds the input to the table");


    //Export List to CSV Button
    QPixmap pm_exportList(":/img/img/document_trans.png");
    QIcon i_exportList(pm_exportList);
    stdBTNs.std_exportToCSV->setIcon(i_exportList);
    stdBTNs.std_exportToCSV->setToolTip("Exports either all or only selected Items of the Table to a CSV");

    //Delte All Button
    QPixmap pm_deleteAllBTN(":/img/img/3x_trans.png");
    QIcon i_deleteAllBTN(pm_deleteAllBTN);
    stdBTNs.std_deleteAllBTN ->setIcon(i_deleteAllBTN);
    stdBTNs.std_deleteAllBTN ->setToolTip("Delets either all or only selected Items of the table");

    //send over API Button
    QPixmap pm_soaButton(":/img/img/arrowCircle_trans.png");
    QIcon i_soaButton(pm_soaButton);
    stdBTNs.std_sendOverAPI->setIcon(i_soaButton);
    stdBTNs.std_sendOverAPI->setToolTip("Sends either all or only selected Contents of the Table");

    //select "Red" Button
    QPixmap pm_select_Red(":/img/img/ausrufezeichen_trans.png");
    QIcon i_select_Red(pm_select_Red);
    stdBTNs.std_select_Red->setIcon(i_select_Red);
    stdBTNs.std_select_Red->setToolTip("Selects all the not accepted Supports");

    //select "Orange" Button
    QPixmap pm_select_Orange(":/img/img/ausrufezeichen_orange_trans.png");
    QIcon i_select_Orange(pm_select_Orange);
    stdBTNs.std_select_Orange->setIcon(i_select_Orange);
    stdBTNs.std_select_Orange->setToolTip("Selects all the accepted but not good Supports");

    //select "Green" Button
    QPixmap pm_select_Green(":/img/img/ausrufezeichen_green_trans.png");
    QIcon i_select_Green(pm_select_Green);
    stdBTNs.std_select_Green->setIcon(i_select_Green);
    stdBTNs.std_select_Green->setToolTip("Selects all the accepted Supports");

    //select "90Days" Button
    QPixmap pm_select_90Days(":/img/img/Uhr_trans.png");
    QIcon i_select_90Days(pm_select_90Days);
    stdBTNs.std_select_Days->setIcon(i_select_90Days);
    stdBTNs.std_select_Days->setToolTip("Selects all the Supports which are older than 90 Days");
}

void NetworkPageBase::addNStatButton(QTableWidget* table, const int currentRow,const int nStatColumn, const suppNetStatus currentStat, const QString &netReply)
{
    QPushButton *networkStatusBTN = new QPushButton;

    switch(currentStat){
    case eNstat_Good:


        networkStatusBTN->setIcon(QIcon(":/img/img/ausrufezeichen_green_trans.png"));
        networkStatusBTN->setToolTip(netReply);

        table->setCellWidget(currentRow,nStatColumn,networkStatusBTN);

        break;

    case eNstat_Okay:


        networkStatusBTN->setIcon(QIcon(":/img/img/ausrufezeichen_orange_trans.png"));
        networkStatusBTN->setToolTip(netReply);

        table->setCellWidget(currentRow,nStatColumn,networkStatusBTN);

        break;

    case eNstat_Error:

        networkStatusBTN->setIcon(QIcon(":/img/img/ausrufezeichen_trans.png"));
        networkStatusBTN->setToolTip(netReply);

        table->setCellWidget(currentRow,nStatColumn,networkStatusBTN);

        break;

    case eNstat_NotSend:

        networkStatusBTN->setIcon(QIcon(":/img/img/3Dots_trans.png"));
        networkStatusBTN->setToolTip("This Item has not been send");

        table->setCellWidget(currentRow, nStatColumn, networkStatusBTN);

        break;

    default:

        qDebug()<<"Error in AddNstatButton-Statemachine";
        delete networkStatusBTN;

        break;
    }
}



void NetworkPageBase::selectRowsBasedOnCondition(QTableWidget *table, int column, std::function<bool (const QString &)> condition)
{
    table->setSelectionMode(QAbstractItemView::MultiSelection);
    table->clearSelection();

    for (int currRow = 0; currRow < table->rowCount(); ++currRow) {
        QTableWidgetItem *item = table->item(currRow, column);
        if (item && condition(item->text())) {
            table->selectRow(currRow);
        }
    }

    table->setSelectionMode(QAbstractItemView::ExtendedSelection);
}

void NetworkPageBase::selectSearch(QTableWidget *table, QLineEdit *searchBar)
{
    if(searchBar->text().isEmpty()){
        QMessageBox::critical(this,"Search is Empty","Please put in a search before pressing this button.");
        return;
    }

    table->setSelectionMode(QAbstractItemView::MultiSelection);
    for (int i = 0; i < table->rowCount(); ++i) {

        if(!table->isRowHidden(i)){

            table->selectRow(i);
        }
    }
    table->setSelectionMode(QAbstractItemView::ExtendedSelection);
}

void NetworkPageBase::addKeyWordMapping(const QString &keyword, int columnIndex)
{
    keywordColumnMap[keyword] = columnIndex;
}

void NetworkPageBase::searchBarActivity(QTableWidget *table, const QString &searchText)
{
    QMap<QString, QString> searchParams;
    QStringList parts = searchText.split(",", Qt::SkipEmptyParts);
    foreach (QString part, parts) {
        part = part.trimmed(); // Trim whitespace
        if (part.startsWith("@")) {
            QStringList keyValue = part.mid(1).split(":");
            if (keyValue.size() == 2) {
                QString key = keyValue[0].trimmed();
                QString value = keyValue[1].trimmed();
                searchParams[key] = value;
            }
        }
    }

    for (int i = 0; i < table->rowCount(); ++i) {
        bool match = true; // Assume match for default search, opposite for keyword search

        foreach (const QString &key, searchParams.keys()) {
            const QString &value = searchParams[key];

            if (keywordColumnMap.contains(key)) {
                int columnIndex = keywordColumnMap[key];
                QTableWidgetItem *item = table->item(i, columnIndex);
                //qDebug() << "Comparing:" << item->text() << "with value:" << value;
                if (!(item && item->text().contains(value, Qt::CaseInsensitive))) {
                    match = false;
                    break;
                }
            }
        }

        // Default search logic
        if (searchParams.isEmpty()) {
            match = false; // Reset for default search
            for (int j = 0; j < table->columnCount(); ++j) {
                QTableWidgetItem *item = table->item(i, j);
                if (item && item->text().contains(searchText, Qt::CaseInsensitive)) {
                    match = true; // Match found
                    break;
                }
            }
        }

        // Hide or show the row based on match result
        table->setRowHidden(i, !match);
    }
}

void NetworkPageBase::sortColumn(QTableWidget *table,const int columnIndex)
{
    if (!columnSortStates.contains(columnIndex)) {
        // Initialize the state if it does not exist
        columnSortStates[columnIndex] = ColumnSortState(columnIndex);
    }

    ColumnSortState &state = columnSortStates[columnIndex];

    // Debugging: Log the current state before sorting
    //qDebug() << "Current state before sorting:" << state.currentState;

    // Toggle the sorting order based on the current state
    if (state.currentState == eSStat_None || state.currentState == eSStat_Descending) {
        // If currently not sorted or descending, sort ascending
        table->sortItems(columnIndex, Qt::AscendingOrder);
        state.currentState = eSStat_Ascending; // Prepare for next action
    } else if (state.currentState == eSStat_Ascending) {
        // If currently ascending, sort descending
        table->sortItems(columnIndex, Qt::DescendingOrder);
        state.currentState = eSStat_Descending; // Prepare for next action
    }

    // Debugging: Log the updated state after sorting
    //qDebug() << "Current state after sorting:" << state.currentState;
}


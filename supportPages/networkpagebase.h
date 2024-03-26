#ifndef NETWORKPAGEBASE_H
#define NETWORKPAGEBASE_H

#include <QObject>
#include <QWidget>
#include <QPushButton>

#include "Enums.h"
#include "qtablewidget.h"

class NetworkPageBase  : public QWidget
{
    Q_OBJECT
public:
    NetworkPageBase(QWidget *parent =nullptr);
    virtual ~NetworkPageBase();

protected:
    struct ColumnSortState {
        int column;
        sortingStatus currentState;
        ColumnSortState() : column(-1), currentState(eSStat_None) {}
        ColumnSortState(int col) : column(col), currentState(eSStat_None) {}
    };
    struct StandardButtons{
        QPushButton* std_addToListBTN;
        QPushButton* std_exportToCSV;
        QPushButton* std_deleteAllBTN;
        QPushButton* std_sendOverAPI;
        QPushButton* std_select_Green;
        QPushButton* std_select_Orange;
        QPushButton* std_select_Red;
        QPushButton* std_select_Days;

        StandardButtons()
            : std_addToListBTN(nullptr), std_exportToCSV(nullptr), std_deleteAllBTN(nullptr),std_sendOverAPI(nullptr),std_select_Green(nullptr),std_select_Orange(nullptr),std_select_Red(nullptr),std_select_Days(nullptr) {}
    };

    //UI Edits
    void initStandardButtons(StandardButtons stdBtNs);

    //Table Edits
    virtual void initializeColumnSortStates() = 0;
    void sortColumn(QTableWidget *table,const int columnIndex);

    virtual void addNStatButton(QTableWidget* table, const int currentRow,const int nStatColumn, const suppNetStatus currentStat, const QString &netReply);

    void selectSearch(QTableWidget *table, QLineEdit *searchBar);
    void selectRowsBasedOnCondition (QTableWidget *table, int column, std::function<bool(const QString&)> condition);

    //search
    void addKeyWordMapping(const QString& keyword, int columnIndex);
    void searchBarActivity(QTableWidget *table, const QString &searchText);

    QMap<QString, int> keywordColumnMap;
    QMap<int, ColumnSortState> columnSortStates; // Maps column index to its sort state
private:
};

#endif // NETWORKPAGEBASE_H

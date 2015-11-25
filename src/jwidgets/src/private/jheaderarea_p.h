#ifndef JHEADERAREA_P_H
#define JHEADERAREA_P_H

#include "../jwidgets_global.h"
#include "../jheaderarea.h"
#include <QStandardItemModel>
#include <QAbstractItemView>
#include <QSortFilterProxyModel>

// - class JHeaderAreaPrivate -

class QAbstractItemModel;
class QAbstractItemView;
class QHeaderView;
class QVBoxLayout;
class QLabel;
class QAction;

class JHeaderAreaPrivate : public QObject
{
    Q_OBJECT
    J_DECLARE_PUBLIC(JHeaderArea)
    friend class JTableView;
    friend class JTreeView;
public:
    explicit JHeaderAreaPrivate(JHeaderArea *parent);
    ~JHeaderAreaPrivate();

    bool filterItemVisible(int column) const;
    QStringList filterItem(int column) const;
    bool filterItemEditable(int column) const;
    void setFilterItemEditable(bool editable, int column = -1);
    void setFilterItem(int column, const QString &text);
    void setFilterItem(int column, const QStringList &texts = QStringList());
    void removeFilterItem(int column, bool update = true);
    void clearFilterItem(bool update = true);

    bool filterVisible() const;
    void setFilterVisible(bool visible);

    bool attach(QAbstractItemView *view);
    void detach();

    void setAutpUpdateTitle(bool enable = true);
    void setSwitchEnable(bool enable = true);

Q_SIGNALS:
    void textChanged(int column, const QString &text);

public Q_SLOTS:
    void updateArea();
    void updateFilters();
    void onScrollarValueChanged(int value);
    void onSectionResized(int logicalIndex, int oldSize, int newSize);
    void onSectionMoved(int logicalIndex, int oldVisualIndex, int newVisualIndex);
    void onSectionDoubleClicked(int logicalIndex);
    void onCellChanged(int row, int column);
    void onFilterTextChanged(const QString &text);
    void onChangeFilter();

    void setFilterRegExp(int column, const QString &text);
    void updateFilterModel(bool visible);

    void replaceFilterItem();
    void replaceFilterItemWithLineEdit();
    void replaceFilterItemWithComboBox();

    void updateFilterItem();
    void updateFilterItem(int column);

    // from view -- start

    void _emit_insertRow(int row);
    void _emit_insertColumn(int column);
    void _emit_removeRow(int row);
    void _emit_removeColumn(int column);
    void _emit_clear();
    void _emit_clearContents();

    // from view -- end

    void _emit_filterChanged(int column, int type, bool visible);

public:
    void init();

private:
    void createFilterItem(int column, QWidget *widget, const char *asignal);
    void disconnectFromThis(QWidget *widget);

private:
    QVBoxLayout *vertLayoutMain;
    QLabel *titleArea;
    QWidget *filterArea;
    QAbstractItemView *view;
    QHeaderView *horiHeader;
    QHeaderView *vertheader;
    QAction *actionEnable;
    QAction *actionReplace;
    QMap<int, QWidget *> filterItems;
    QStandardItemModel *sourceModel;        // source of model
    JSortFilterProxyModel sortModel;        // default model
    JSortFilterProxyModel *cusSortModel;    // custom sort model
    int leftOffset;
    bool switchEnable;
    bool autoUpdateTitle;
};

#endif // JHEADERAREA_P_H

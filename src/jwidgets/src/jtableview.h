#ifndef JTABLEVIEW_H
#define JTABLEVIEW_H

#include "jwidgets_global.h"
#include <QTableView>
#include <QStandardItemModel>

// - class JTableViewSelectionRange -

class JWIDGETS_EXPORT JTableViewSelectionRange
{
public:
    JTableViewSelectionRange();
    JTableViewSelectionRange(int top, int left, int bottom, int right);
    JTableViewSelectionRange(const JTableViewSelectionRange &other);
    ~JTableViewSelectionRange();

    inline int topRow() const { return top; }
    inline int bottomRow() const { return bottom; }
    inline int leftColumn() const { return left; }
    inline int rightColumn() const { return right; }
    inline int rowCount() const { return bottom - top + 1; }
    inline int columnCount() const { return right - left + 1; }

    JTableViewSelectionRange &operator =(const JTableViewSelectionRange &other);

private:
    int top, left, bottom, right;
};

// - class JTableView -

class JTableViewPrivate;

class JWIDGETS_EXPORT JTableView : public QTableView
{
    Q_OBJECT
    Q_PROPERTY(int rowCount READ rowCount WRITE setRowCount)
    Q_PROPERTY(int columnCount READ columnCount WRITE setColumnCount)
    friend class JHeaderArea;
public:
    explicit JTableView(QWidget *parent);
    virtual ~JTableView();

    int rowCount() const;
    void setRowCount(int rows);

    int columnCount() const;
    void setColumnCount(int columns);

    void setVerticalHeaderLabels(const QStringList &labels);
    void setHorizontalHeaderLabels(const QStringList &labels);
    void setVerticalHeaderVisible(bool visible);

    QStandardItem *item(int row, int column) const;
    void setItem(int row, int column, QStandardItem *item);

    QVariant itemData(int row, int column, int role = Qt::DisplayRole) const;
    bool setItemData(int row, int column, const QVariant &value, int role = Qt::EditRole);

    bool insertRow(int row, QStandardItem *item);
    bool insertRow(int row, const QList<QStandardItem *> &items);

    int currentRow() const;
    int currentColumn() const;
    QStandardItem *currentItem() const;
    void setCurrentItem(QStandardItem *item);
    void setCurrentCell(int row, int column);

    QWidget *cellWidget(int row, int column) const;
    void setCellWidget(int row, int column, QWidget *widget);
    inline void removeCellWidget(int row, int column);

    bool isItemSelected(const QStandardItem *item) const;
    void setItemSelected(const QStandardItem *item, bool select);
    void setRangeSelected(const JTableViewSelectionRange &range, bool select);

    QList<JTableViewSelectionRange> selectRanges() const;

    QStandardItemModel *sourceModel() const;

Q_SIGNALS:
    void itemPressed(QStandardItem *item);
    void itemClicked(QStandardItem *item);
    void itemDoubleClicked(QStandardItem *item);

    void itemActivated(QStandardItem *item);
    void itemEntered(QStandardItem *item);
    void itemChanged(QStandardItem *item);

    void currentItemChanged(QStandardItem *current, QStandardItem *previous);
    void itemSelectionChanged();

    void cellPressed(int row, int column);
    void cellClicked(int row, int column);
    void cellDoubleClicked(int row, int column);

    void cellActivated(int row, int column);
    void cellEntered(int row, int column);
    void cellChanged(int row, int column);

    void currentCellChanged(int currentRow, int currentColumn, int previousRow, int previuosColumn);

public Q_SLOTS:
    void scrollToItem(const QStandardItem *item, QAbstractItemView::ScrollHint hint = EnsureVisible);
    void scrollToRow(int row, QAbstractItemView::ScrollHint hint = EnsureVisible);
    void scrollToColumn(int column, QAbstractItemView::ScrollHint hint = EnsureVisible);
    void insertRow(int row);
    void insertColumn(int column);
    void removeRow(int row);
    void removeColumn(int column);
    void clear();
    void clearContents();

protected Q_SLOTS:
    virtual void selectionChanged(const QItemSelection &selected, const QItemSelection &deselected);
    virtual void currentChanged(const QModelIndex &current, const QModelIndex &previous);

private:
    void setModel(QAbstractItemModel *model);

public:
    JTableViewPrivate *privateData();

private:
    J_DECLARE_PRIVATE(JTableView)
};

inline void JTableView::removeCellWidget(int row, int column)
{ setCellWidget(row, column, 0); }

#endif // JTABLEVIEW_H

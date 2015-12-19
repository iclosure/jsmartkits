#ifndef JTABLEVIEW_P_H
#define JTABLEVIEW_P_H

#include "../jwidgets_global.h"
#include "../jtableview.h"

// - class JTableViewPrivate -

class JTableViewPrivate : public QObject
{
    Q_OBJECT
    J_DECLARE_PUBLIC(JTableView)
public:
    explicit JTableViewPrivate(JTableView *parent);

    void insertRow(int row, QStandardItem *item);
    void insertRow(int row, const QList<QStandardItem *> &items);
    bool insertRows(int row, int count = 1, const QModelIndex &parent = QModelIndex());
    bool insertColumns(int column, int count = 1, const QModelIndex &parent = QModelIndex());

    bool removeRows(int row, int count = 1, const QModelIndex &parent = QModelIndex());
    bool removeColumns(int column, int count = 1, const QModelIndex &parent = QModelIndex());

    int currentRow() const;
    int currentColumn() const;

    void clear();
    void clearContents();

Q_SIGNALS:
    void _signal_insertRow(int row);
    void _signal_insertColumn(int column);
    void _signal_removeRow(int row);
    void _signal_removeColumn(int column);
    void _signal_clear();
    void _signal_clearContents();

    void _signal_updateFilterArea();

public Q_SLOTS:
    void _emit_itemPressed(const QModelIndex &index);
    void _emit_itemClicked(const QModelIndex &index);
    void _emit_itemDoubleClicked(const QModelIndex &index);
    void _emit_itemActivated(const QModelIndex &index);
    void _emit_itemEntered(const QModelIndex &index);

    void _emit_itemChanged(const QModelIndex &index);

    void _emit_currentItemChanged(const QModelIndex &current, const QModelIndex &previous);

    void _emit_dataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight);
    void _emit_sort();

private:
    void init();

    QModelIndex mapToSource(const QModelIndex &index) const;
    QStandardItem *modelItem(const QModelIndex &index) const;

private:
    QStandardItemModel *sourceModel;
};

#endif // JTABLEVIEW_P_H

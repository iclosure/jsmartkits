#include "precomp.h"
#include "jtableview_p.h"
#include "../jtableview.h"
#include "jheaderarea_p.h"

// - class JTableViewPrivate -

JTableViewPrivate::JTableViewPrivate(JTableView *parent)
    : QObject(parent)
    , q_ptr(parent)
{

}

void JTableViewPrivate::insertRow(int row, QStandardItem *item)
{
    sourceModel->insertRow(row, item);
    Q_EMIT _signal_insertRow(row);
}

void JTableViewPrivate::insertRow(int row, const QList<QStandardItem *> &items)
{
    sourceModel->insertRow(row, items);
    Q_EMIT _signal_insertRow(row);
}

bool JTableViewPrivate::insertRows(int row, int count, const QModelIndex &parent)
{
    bool result = sourceModel->insertRows(row, count, parent);
    if (result) {
        Q_EMIT _signal_insertRow(row);
    }

    return result;
}

bool JTableViewPrivate::insertColumns(int column, int count, const QModelIndex &parent)
{
    bool result = sourceModel->insertColumns(column, count, parent);
    if (result) {
        Q_EMIT _signal_insertColumn(column);
    }

    return result;
}

bool JTableViewPrivate::removeRows(int row, int count, const QModelIndex &parent)
{
    bool result = sourceModel->removeRows(row, count, parent);
    if (result) {
        Q_EMIT _signal_removeRow(row);
    }

    return result;
}

bool JTableViewPrivate::removeColumns(int column, int count, const QModelIndex &parent)
{
    bool result = sourceModel->removeColumns(column, count, parent);
    if (result) {
        Q_EMIT _signal_removeColumn(column);
    }

    return result;
}

int JTableViewPrivate::currentRow() const
{
    Q_Q(const JTableView);
    const QModelIndex currentIndex = mapToSource(q->currentIndex());
    return currentIndex.row();
}

int JTableViewPrivate::currentColumn() const
{
    Q_Q(const JTableView);
    const QModelIndex currentIndex = mapToSource(q->currentIndex());
    return currentIndex.column();
}

void JTableViewPrivate::clear()
{
    sourceModel->clear();
    Q_EMIT _signal_clear();
}

void JTableViewPrivate::clearContents()
{
    sourceModel->removeRows(0, sourceModel->rowCount());
    Q_EMIT _signal_clearContents();
}

void JTableViewPrivate::_emit_itemPressed(const QModelIndex &index)
{
    Q_Q(JTableView);
    const QModelIndex sourceIndex = mapToSource(index);
    if (QStandardItem *item = modelItem(sourceIndex)) {
        Q_EMIT q->itemPressed(item);
    }

    Q_EMIT q->cellPressed(sourceIndex.row(), sourceIndex.column());
}

void JTableViewPrivate::_emit_itemClicked(const QModelIndex &index)
{
    Q_Q(JTableView);
    const QModelIndex sourceIndex = mapToSource(index);
    if (QStandardItem *item = modelItem(sourceIndex)) {
        Q_EMIT q->itemClicked(item);
    }

    Q_EMIT q->cellClicked(sourceIndex.row(), sourceIndex.column());
}

void JTableViewPrivate::_emit_itemDoubleClicked(const QModelIndex &index)
{
    Q_Q(JTableView);
    const QModelIndex sourceIndex = mapToSource(index);
    if (QStandardItem *item = modelItem(sourceIndex)) {
        Q_EMIT q->itemDoubleClicked(item);
    }

    Q_EMIT q->cellDoubleClicked(sourceIndex.row(), sourceIndex.column());
}

void JTableViewPrivate::_emit_itemActivated(const QModelIndex &index)
{
    Q_Q(JTableView);
    const QModelIndex sourceIndex = mapToSource(index);
    if (QStandardItem *item = modelItem(sourceIndex)) {
        Q_EMIT q->itemActivated(item);
    }

    Q_EMIT q->cellActivated(sourceIndex.row(), sourceIndex.column());
}

void JTableViewPrivate::_emit_itemEntered(const QModelIndex &index)
{
    Q_Q(JTableView);
    const QModelIndex sourceIndex = mapToSource(index);
    if (QStandardItem *item = modelItem(sourceIndex)) {
        Q_EMIT q->itemEntered(item);
    }

    Q_EMIT q->cellEntered(sourceIndex.row(), sourceIndex.column());
}

void JTableViewPrivate::_emit_itemChanged(const QModelIndex &index)
{
    Q_Q(JTableView);
    const QModelIndex sourceIndex = mapToSource(index);
    if (QStandardItem *item = modelItem(sourceIndex)) {
        Q_EMIT q->itemChanged(item);
    }

    Q_EMIT q->cellChanged(sourceIndex.row(), sourceIndex.column());
}

void JTableViewPrivate::_emit_currentItemChanged(const QModelIndex &current, const QModelIndex &previous)
{
    Q_Q(JTableView);
    const QModelIndex currentIndex = mapToSource(current);
    const QModelIndex previousIndex = mapToSource(previous);
    QStandardItem *currentItem = modelItem(currentIndex);
    QStandardItem *previousItem = modelItem(previousIndex);
    if (currentItem || previousItem) {
        Q_EMIT q->currentItemChanged(currentItem, previousItem);
    }

    Q_EMIT q->currentCellChanged(currentIndex.row(), currentIndex.column(),
                                 previousIndex.row(), previousIndex.column());
}

void JTableViewPrivate::_emit_dataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight)
{
    Q_Q(JTableView);
    if (q->isSortingEnabled() && topLeft.isValid() && bottomRight.isValid()) {
        int column = q->horizontalHeader()->sortIndicatorSection();
        if (column >= topLeft.column() && column <= bottomRight.column()) {
            Qt::SortOrder order = q->horizontalHeader()->sortIndicatorOrder();
            //
            Q_UNUSED(order);
        }
    }
}

void JTableViewPrivate::_emit_sort()
{
    Q_Q(JTableView);
    if (q->isSortingEnabled()) {
        int column = q->horizontalHeader()->sortIndicatorSection();
        Qt::SortOrder order = q->horizontalHeader()->sortIndicatorOrder();
        sourceModel->sort(column, order);
    }
}

void JTableViewPrivate::init()
{
    Q_Q(JTableView);

    sourceModel = qobject_cast<QStandardItemModel *>(q->model());

    // signals of view
    connect(q, SIGNAL(pressed(QModelIndex)),
            SLOT(_emit_itemPressed(QModelIndex)));
    connect(q, SIGNAL(clicked(QModelIndex)),
            SLOT(_emit_itemClicked(QModelIndex)));
    connect(q, SIGNAL(doubleClicked(QModelIndex)),
            SLOT(_emit_itemDoubleClicked(QModelIndex)));
    connect(q, SIGNAL(activated(QModelIndex)),
            SLOT(_emit_itemActivated(QModelIndex)));
    connect(q, SIGNAL(entered(QModelIndex)),
            SLOT(_emit_itemEntered(QModelIndex)));
    // signals of model
    connect(sourceModel, SIGNAL(dataChanged(QModelIndex,QModelIndex)),
            SLOT(_emit_dataChanged(QModelIndex,QModelIndex)));
    connect(sourceModel, SIGNAL(itemChanged(QStandardItem*)),
            q, SIGNAL(itemChanged(QStandardItem*)));
    // signals of selection
    //!
    // signals of sorting
    connect(sourceModel, SIGNAL(dataChanged(QModelIndex,QModelIndex)),
            SLOT(_emit_dataChanged(QModelIndex,QModelIndex)));
    connect(sourceModel, SIGNAL(columnsRemoved(QModelIndex,int,int)),
            SLOT(_emit_sort()));
}

QModelIndex JTableViewPrivate::mapToSource(const QModelIndex &index) const
{
    Q_Q(const JTableView);
    if (q->model()->inherits("QSortFilterProxyModel")) {
        return qobject_cast<QSortFilterProxyModel *>(q->model())->mapToSource(index);
    } else {
        return index;
    }
}

QStandardItem *JTableViewPrivate::modelItem(const QModelIndex &index) const
{
    //Q_Q(const JTableView);
    return sourceModel->item(index.row(), index.column());
}

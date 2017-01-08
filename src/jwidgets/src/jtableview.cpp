#include "precomp.h"
#include "jtableview.h"
#include "private/jtableview_p.h"

// - class JTableViewSelectionRange -

JTableViewSelectionRange::JTableViewSelectionRange()
    : top(-1), left(-1), bottom(-2), right(-2)
{

}

JTableViewSelectionRange::JTableViewSelectionRange(int top, int left, int bottom, int right)
    : top(top), left(left), bottom(bottom), right(right)
{

}

JTableViewSelectionRange::JTableViewSelectionRange(const JTableViewSelectionRange &other)
{
    *this = other;
}

JTableViewSelectionRange::~JTableViewSelectionRange()
{

}

JTableViewSelectionRange &JTableViewSelectionRange::operator =(const JTableViewSelectionRange &other)
{
    if (this == &other) {
        return *this;
    }

    top = other.top;
    left = other.left;
    bottom = other.bottom;
    right = other.right;

    return *this;
}

// - class JTableView -

JTableView::JTableView(QWidget *parent)
    : QTableView(parent)
    , d_ptr(new JTableViewPrivate(this))
{
    Q_D(JTableView);

    QTableView::setModel(new QStandardItemModel(this));

    d->init();

    // default properties
#if QT_VERSION >= 0x050000
    horizontalHeader()->setSectionsMovable(true);
    verticalHeader()->setMinimumWidth(60);
#else
    horizontalHeader()->setMovable(true);
#endif
    horizontalHeader()->setStretchLastSection(true);
    horizontalHeader()->setDefaultSectionSize(150);
    //verticalHeader()->setDefaultAlignment(Qt::AlignRight | Qt::AlignVCenter);
    setSelectionBehavior(QAbstractItemView::SelectRows);
    setSelectionMode(QAbstractItemView::SingleSelection);
    setAlternatingRowColors(true);
    setSortingEnabled(true);
}

JTableView::~JTableView()
{
    d_ptr->deleteLater();
}

int JTableView::rowCount() const
{
    Q_D(const JTableView);
    return d->sourceModel->rowCount();
}

void JTableView::setRowCount(int rows)
{
    Q_D(JTableView);
    d->sourceModel->setRowCount(rows);
}

int JTableView::columnCount() const
{
    Q_D(const JTableView);
    return d->sourceModel->columnCount();
}

void JTableView::setColumnCount(int columns)
{
    Q_D(JTableView);
    d->sourceModel->setColumnCount(columns);
}

QStandardItem *JTableView::horizontalHeaderItem(int column) const
{
    Q_D(const JTableView);
    return d->sourceModel->horizontalHeaderItem(column);
}

QStandardItem *JTableView::verticalHeaderItem(int row) const
{
    Q_D(const JTableView);
    return d->sourceModel->verticalHeaderItem(row);
}

void JTableView::setVerticalHeaderLabels(const QStringList &labels)
{
    Q_D(JTableView);
    d->sourceModel->setVerticalHeaderLabels(labels);
}

void JTableView::setHorizontalHeaderLabels(const QStringList &labels)
{
    Q_D(JTableView);
    d->sourceModel->setHorizontalHeaderLabels(labels);
}

void JTableView::setVerticalHeaderVisible(bool visible)
{
    Q_D(JTableView);
    verticalHeader()->setVisible(visible);
    Q_EMIT d->_signal_updateFilterArea();
}

QStandardItem *JTableView::item(int row, int column) const
{
    Q_D(const JTableView);
    return d->sourceModel->item(row, column);
}

void JTableView::setItem(int row, int column, QStandardItem *item)
{
    Q_D(JTableView);
    d->sourceModel->setItem(row, column, item);
}

QVariant JTableView::itemData(int row, int column, int role) const
{
    Q_D(const JTableView);
    return d->sourceModel->data(d->sourceModel->index(row, column), role);
}

bool JTableView::setItemData(int row, int column, const QVariant &value, int role)
{
    Q_D(JTableView);
    return d->sourceModel->setData(d->sourceModel->index(row, column), value, role);
}

bool JTableView::insertRow(int row, QStandardItem *item)
{
    Q_D(JTableView);
    d->insertRow(row, item);
    return true;
}

bool JTableView::insertRow(int row, const QList<QStandardItem *> &items)
{
    Q_D(JTableView);
    d->insertRow(row, items);

    return true;
}

int JTableView::currentRow() const
{
    Q_D(const JTableView);
    return d->currentRow();
}

int JTableView::currentColumn() const
{
    Q_D(const JTableView);
    return d->currentColumn();
}

QStandardItem *JTableView::currentItem() const
{
    Q_D(const JTableView);
    return d->sourceModel->item(currentRow(), currentColumn());
}

void JTableView::setCurrentItem(QStandardItem *item)
{
    Q_ASSERT(item);
    if (item) {
        setCurrentIndex(item->index());
    }
}

void JTableView::setCurrentCell(int row, int column)
{
    Q_D(JTableView);
    setCurrentIndex(d->sourceModel->index(row, column));
}

QWidget *JTableView::cellWidget(int row, int column) const
{
    Q_D(const JTableView);
    return indexWidget(d->sourceModel->index(row, column));
}

void JTableView::setCellWidget(int row, int column, QWidget *widget)
{
    Q_D(JTableView);
    setIndexWidget(d->sourceModel->index(row, column), widget);
}

bool JTableView::isItemSelected(const QStandardItem *item) const
{
    Q_D(const JTableView);
    Q_ASSERT(item);
    return selectionModel()->isSelected(d->sourceModel->index(item->row(), item->column()));
}

void JTableView::setItemSelected(const QStandardItem *item, bool select)
{
    Q_D(JTableView);
    selectionModel()->select(d->sourceModel->index(item->row(), item->column()),
                             select
                             ? QItemSelectionModel::Select
                             : QItemSelectionModel::Deselect);
}

void JTableView::setRangeSelected(const JTableViewSelectionRange &range, bool select)
{
    Q_D(JTableView);
    const QModelIndex index = rootIndex();
    if (!d->sourceModel->hasIndex(range.topRow(), range.leftColumn(), index) ||
            !d->sourceModel->hasIndex(range.bottomRow(), range.rightColumn(), index)) {
        return;
    }

    const QModelIndex topleftIndex = d->sourceModel->index(range.topRow(), range.leftColumn());
    const QModelIndex bottomRightIndex = d->sourceModel->index(range.bottomRow(), range.rightColumn());

    selectionModel()->select(QItemSelection(topleftIndex, bottomRightIndex),
                             select
                             ? QItemSelectionModel::Select
                             : QItemSelectionModel::Deselect);
}

QList<JTableViewSelectionRange> JTableView::selectRanges() const
{
    const QList<QItemSelectionRange> ranges = selectionModel()->selection();
    QList<JTableViewSelectionRange> result;
    for (int i = 0; i < ranges.count(); ++i) {
        const QItemSelectionRange &range = ranges.at(i);
        result.append(JTableViewSelectionRange(range.top(), range.left(),
                                               range.bottom(), range.right()));
    }

    return result;
}

QStandardItemModel *JTableView::sourceModel() const
{
    Q_D(const JTableView);
    return d->sourceModel;
}

void JTableView::scrollToItem(const QStandardItem *item, QAbstractItemView::ScrollHint hint)
{
    if (!item) {
        return;
    }

    QTableView::scrollTo(item->index(), hint);
}

void JTableView::scrollToRow(int row, QAbstractItemView::ScrollHint hint)
{
    Q_D(JTableView);
    const QModelIndex index = d->sourceModel->index(row, 0);
    QTableView::scrollTo(index, hint);
}

void JTableView::scrollToColumn(int column, QAbstractItemView::ScrollHint hint)
{
    Q_D(JTableView);
    const QModelIndex index = d->sourceModel->index(0, column);
    QTableView::scrollTo(index, hint);
}

void JTableView::insertRow(int row)
{
    Q_D(JTableView);
    d->insertRows(row);
}

void JTableView::insertRow(int row, int count)
{
    Q_D(JTableView);
    d->insertRows(row, count);
}

void JTableView::insertColumn(int column)
{
    Q_D(JTableView);
    d->insertColumns(column);
}

void JTableView::insertColumn(int column, int count)
{
    Q_D(JTableView);
    d->insertColumns(column, count);
}

void JTableView::removeRow(int row)
{
    Q_D(JTableView);
    d->removeRows(row);
}

void JTableView::removeRow(int row, int count)
{
    Q_D(JTableView);
    d->removeRows(row, count);
}

void JTableView::removeColumn(int column)
{
    Q_D(JTableView);
    d->removeColumns(column);
}

void JTableView::removeColumn(int column, int count)
{
    Q_D(JTableView);
    d->removeColumns(column, count);
}

void JTableView::clear()
{
    Q_D(JTableView);
    QItemSelectionModel *selModel = selectionModel();
    if (selModel) {
        selModel->clear();
    }

    d->clear();
}

void JTableView::clearContents()
{
    Q_D(JTableView);
    QItemSelectionModel *selModel = selectionModel();
    if (selModel) {
        selModel->clear();
    }

    d->clearContents();
}

void JTableView::selectionChanged(const QItemSelection &selected, const QItemSelection &deselected)
{
    QTableView::selectionChanged(selected, deselected);
    Q_EMIT itemSelectionChanged();
}

void JTableView::currentChanged(const QModelIndex &current, const QModelIndex &previous)
{
    Q_D(JTableView);
    QTableView::currentChanged(current, previous);
    d->_emit_currentItemChanged(current, previous);
}

void JTableView::setModel(QAbstractItemModel *model)
{
    QTableView::setModel(model);
}

JTableViewPrivate *JTableView::privateData()
{
    Q_D(JTableView);
    return d;
}

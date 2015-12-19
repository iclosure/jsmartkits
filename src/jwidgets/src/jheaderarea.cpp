#include "precomp.h"
#include "jheaderarea.h"
#include "private/jheaderarea_p.h"

// - JSortFilterProxyModelData -

class JSortFilterProxyModelData
{
public:
    QMap<int/*column*/, QRegExp> mapRegExp;
};

// - class JSortFilterProxyModel -

JSortFilterProxyModel::JSortFilterProxyModel(QObject *parent)
    : QSortFilterProxyModel(parent)
{
    d = new JSortFilterProxyModelData();
}

JSortFilterProxyModel::~JSortFilterProxyModel()
{
    delete d;
}

void JSortFilterProxyModel::setFilterRegExp(int column, const QRegExp &regExp)
{
    d->mapRegExp[column] = regExp;
    invalidateFilter();
}

void JSortFilterProxyModel::removeFilter(int column)
{
    if (d->mapRegExp.contains(column)) {
        d->mapRegExp.remove(column);
        invalidateFilter();
    }
}

bool JSortFilterProxyModel::filterAcceptsRow(int source_row, const QModelIndex &source_parent) const
{
    QAbstractItemModel *model = sourceModel();
    if (!model) {
        return true;
    }

    bool bResult = true;
    QMapIterator<int, QRegExp> iter(d->mapRegExp);
    while (iter.hasNext()) {
        iter.next();
        QModelIndex index = model->index(source_row, iter.key(), source_parent);
        if (!index.isValid()) {
            return true;
        }

        bResult = bResult && model->data(index).toString().contains(iter.value());
    }

    return bResult;
}

// - class JHeaderArea -

JHeaderArea::JHeaderArea(QWidget *parent)
    : QFrame(parent)
    , d_ptr(new JHeaderAreaPrivate(this))
{
    Q_D(JHeaderArea);
    d->init();
}

JHeaderArea::~JHeaderArea()
{
    d_ptr->deleteLater();
}

bool JHeaderArea::autoUpdateTitle() const
{
    Q_D(const JHeaderArea);
    return d->autoUpdateTitle;
}

bool JHeaderArea::titleVisible() const
{
    Q_D(const JHeaderArea);
    return d->titleArea->isVisible();
}

QString JHeaderArea::title() const
{
    Q_D(const JHeaderArea);
    return d->titleArea->text();
}

int JHeaderArea::titleHeight() const
{
    Q_D(const JHeaderArea);
    return d->titleArea->height();
}

QFont JHeaderArea::titleFont() const
{
    Q_D(const JHeaderArea);
    return d->titleArea->font();
}

Qt::Alignment JHeaderArea::titleAlignment() const
{
    Q_D(const JHeaderArea);
    return d->titleArea->alignment();
}

bool JHeaderArea::filterVisible() const
{
    Q_D(const JHeaderArea);
    return d->filterVisible();
}

int JHeaderArea::filterHeight() const
{
    Q_D(const JHeaderArea);
    return d->filterArea->height();
}

void JHeaderArea::setTitleStyle(const QString &styleSheet)
{
    Q_D(JHeaderArea);
    d->titleArea->setStyleSheet(styleSheet);
}

void JHeaderArea::setFilterStyle(const QString &styleSheet)
{
    Q_D(JHeaderArea);
    d->filterArea->setStyleSheet(styleSheet);
}

bool JHeaderArea::filterItemVisible(int column) const
{
    Q_D(const JHeaderArea);
    return d->filterItemVisible(column);
}

QStringList JHeaderArea::filterItem(int column) const
{
    Q_D(const JHeaderArea);
    return d->filterItem(column);
}

bool JHeaderArea::filterItemEditable(int column) const
{
    Q_D(const JHeaderArea);
    return d->filterItemEditable(column);
}

void JHeaderArea::setFilterItemEditable(bool editable, int column)
{
    Q_D(JHeaderArea);
    d->setFilterItemEditable(editable, column);
}

bool JHeaderArea::setFilterItem(int column, const QString &text)
{
    Q_D(JHeaderArea);
    if (!d->sourceModel) {
        return false;
    }

    if (column < 0 || column >= d->sourceModel->columnCount()) {
        return false;
    }

    d->setFilterItem(column, text);

    return true;
}

bool JHeaderArea::setFilterItem(int column, const QStringList &texts)
{
    Q_D(JHeaderArea);
    if (!d->sourceModel) {
        return false;
    }

    if (column < 0 || column >= d->sourceModel->columnCount()) {
        return false;
    }

    d->setFilterItem(column, texts);

    return true;
}

void JHeaderArea::setFilterItem(const QList<int> &columns)
{
    Q_D(JHeaderArea);
    if (!d->sourceModel) {
        return;
    }

    QListIterator<int> iter(columns);
    while (iter.hasNext()) {
        int column = iter.next();
        if (column < 0 || column >= d->sourceModel->columnCount()) {
            continue;
        }

        d->setFilterItem(column, "");
    }
}

void JHeaderArea::setFilterItem(const QList<QPair<int, QString> > &columns)
{
    Q_D(JHeaderArea);
    if (!d->sourceModel) {
        return;
    }

    QListIterator<QPair<int, QString> > iter(columns);
    while (iter.hasNext()) {
        const QPair<int, QString> &column = iter.next();
        int col = column.first;
        if (col < 0 || col >= d->sourceModel->columnCount()) {
            continue;
        }

        d->setFilterItem(col, column.second);
    }
}

void JHeaderArea::setFilterItem(const QList<QPair<int, QStringList> > &columns)
{
    Q_D(JHeaderArea);
    if (!d->sourceModel) {
        return;
    }

    QListIterator<QPair<int, QStringList> > iter(columns);
    while (iter.hasNext()) {
        const QPair<int, QStringList> &column = iter.next();
        int col = column.first;
        if (col < 0 || col >= d->sourceModel->columnCount()) {
            continue;
        }

        d->setFilterItem(col, column.second);
    }
}

void JHeaderArea::setFilterItemAsComboBox(const QList<int> columns)
{
    Q_D(JHeaderArea);
    if (!d->sourceModel) {
        return;
    }

    QListIterator<int> iter(columns);
    while (iter.hasNext()) {
        int column = iter.next();
        if (column < 0 || column >= d->sourceModel->columnCount()) {
            continue;
        }

        d->setFilterItem(column);
    }
}

void JHeaderArea::setAllFilterItemWithLineEdit()
{
    Q_D(JHeaderArea);
    d->replaceFilterItemWithLineEdit();
}

void JHeaderArea::setAllFilterItemWithComboBox()
{
    Q_D(JHeaderArea);
    d->replaceFilterItemWithComboBox();
}

void JHeaderArea::removeFilterItem(int column)
{
    Q_D(JHeaderArea);
    if (!d->sourceModel) {
        return;
    }

    if (column < 0 || column >= d->sourceModel->columnCount()) {
        return;
    }

    d->removeFilterItem(column);
}

void JHeaderArea::clearFilterItem()
{
    Q_D(JHeaderArea);
    d->clearFilterItem();
}

JSortFilterProxyModel *JHeaderArea::filterModel()
{
    Q_D(JHeaderArea);
    if (d->sourceModel) {
        return d->cusSortModel;
    } else {
        return &d->sortModel;
    }
}

void JHeaderArea::setFilterProxyModel(JSortFilterProxyModel *model)
{
    Q_D(JHeaderArea);
    d->cusSortModel = model;
    d->updateFilterModel(filterVisible());
}

bool JHeaderArea::attach(QAbstractItemView *view)
{
    Q_D(JHeaderArea);
    if (d->attach(view)) {
        Q_EMIT attached();
        return true;
    } else {
        return false;
    }
}

void JHeaderArea::detach()
{
    Q_D(JHeaderArea);
    d->detach();
    Q_EMIT detached();
}

void JHeaderArea::setAutoUpdateTitle(bool enable)
{
    Q_D(JHeaderArea);
    d->setAutpUpdateTitle(enable);
}

void JHeaderArea::setTitleVisible(bool visible)
{
    Q_D(JHeaderArea);
    d->titleArea->setVisible(visible);
    Q_EMIT titleVisibleChanged(visible);
}

void JHeaderArea::setTitle(const QString &text)
{
    Q_D(JHeaderArea);
    if (d->titleArea->text() != text) {
        d->titleArea->setText(text);
        Q_EMIT titleChanged(text);
    }
}

void JHeaderArea::setTitleHeight(int height)
{
    Q_D(JHeaderArea);
    if (d->titleArea->height() != height) {
        d->titleArea->setFixedHeight(height);
        Q_EMIT titleHeightChanged(height);
    }
}

void JHeaderArea::setTitleFont(const QFont &font)
{
    Q_D(JHeaderArea);
    d->titleArea->setFont(font);
}

void JHeaderArea::setTitleAlignment(Qt::Alignment alignment)
{
    Q_D(JHeaderArea);
    d->titleArea->setAlignment(alignment);
}

void JHeaderArea::setFilterVisible(bool visible)
{
    Q_D(JHeaderArea);
    d->setFilterVisible(visible);
    Q_EMIT filterVisibleChanged(visible);
}

void JHeaderArea::setFilterHeight(int height)
{
    Q_D(JHeaderArea);
    d->filterArea->setFixedHeight(height);
    d->updateArea();
    Q_EMIT filterHeightChanged(height);
}

void JHeaderArea::setSwitchEnabled(bool enable)
{
    Q_D(JHeaderArea);
    d->setSwitchEnable(enable);
}

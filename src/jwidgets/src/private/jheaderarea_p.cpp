#include "precomp.h"
#include "jheaderarea_p.h"
#include <QAbstractItemView>
#include "../jheaderarea.h"

//! jtableview
#include "jtableview_p.h"
#include "../jtableview.h"

//! jtreeview
#include "jtreeview_p.h"
#include "../jtreeview.h"

//! jxmltable
#include "../jxmltable.h"

// - class JHeaderAreaPrivate -

JHeaderAreaPrivate::JHeaderAreaPrivate(JHeaderArea *parent)
    : QObject(parent)
    , q_ptr(parent)
    , view(0)
    , horiHeader(0)
    , vertheader(0)
    , actionEnable(0)
    , actionReplace(0)
    , sourceModel(0)
    , cusSortModel(&sortModel)
    , leftOffset(0)
    , switchEnable(false)
    , autoUpdateTitle(true)
{
    //
    titleArea = new QLabel("# Table", parent);
    filterArea = new QWidget(parent);

    // layout
    vertLayoutMain = new QVBoxLayout(parent);
    vertLayoutMain->setContentsMargins(0, 0, 0, 0);
    vertLayoutMain->setSpacing(0);
    vertLayoutMain->addWidget(titleArea);
    vertLayoutMain->addWidget(filterArea);

    // signals
    connect(this, SIGNAL(textChanged(int,QString)), SLOT(setFilterRegExp(int,QString)));
}

JHeaderAreaPrivate::~JHeaderAreaPrivate()
{
    clearFilterItem(false);
}

bool JHeaderAreaPrivate::filterItemVisible(int column) const
{
    return filterItems.contains(column);
}

QStringList JHeaderAreaPrivate::filterItem(int column) const
{
    QStringList items;
    if (!filterItems.contains(column)) {
        return items;
    }

    QWidget *widget = filterItems[column];
    if (widget->inherits("QLineEdit")) {
        items.append(qobject_cast<QLineEdit *>(widget)->text());
    } else if (widget->inherits("QComboBox")) {
        QComboBox *comboBox = qobject_cast<QComboBox *>(widget);
        for (int i = 1; i < comboBox->count(); ++i) {
            items.append(comboBox->itemText(i));
        }
    } else {
        Q_ASSERT("JHeaderAreaPrivate::filterItem() - "
                 "Not supported type for widget.");
    }

    return items;
}

bool JHeaderAreaPrivate::filterItemEditable(int column) const
{
    if (!filterItems.contains(column)) {
        return false;
    }

    QWidget *widget = filterItems[column];
    if (!widget) {
        return false;
    }

    if (widget->inherits("QLineEdit")) {
        QLineEdit *lineEdit = qobject_cast<QLineEdit *>(widget);
        if (lineEdit->isEnabled()) {
            return !lineEdit->isReadOnly();
        } else {
            return false;
        }
    } else if (widget->inherits("QComboBox")) {
        QComboBox *comboBox = qobject_cast<QComboBox *>(widget);
        if (comboBox->isEnabled()) {
            return comboBox->isEditable();
        } else {
            return false;
        }
    } else {
        return false;
    }
}

void JHeaderAreaPrivate::setFilterItemEditable(bool editable, int column)
{
    if (column == -1) {
        QMapIterator<int, QWidget *> iter(filterItems);
        while (iter.hasNext()) {
            iter.next();
            QWidget *widget = iter.value();
            if (!widget) {
                continue;
            }

            if (widget->inherits("QLineEdit")) {
                qobject_cast<QLineEdit *>(widget)->setReadOnly(!editable);
            } else if (widget->inherits("QcombBox")) {
                qobject_cast<QComboBox *>(widget)->setEditable(editable);
            }
        }
    } else {
        if (!filterItems.contains(column)) {
            return;
        }

        QWidget *widget = filterItems[column];
        if (!widget) {
            return;
        }

        if (widget->inherits("QLineEdit")) {
            qobject_cast<QLineEdit *>(widget)->setReadOnly(!editable);
        } else if (widget->inherits("QComboBox")) {
            qobject_cast<QComboBox *>(widget)->setEditable(editable);
        }
    }
}

void JHeaderAreaPrivate::setFilterItem(int column, const QString &text)
{
    QWidget *widget = 0;

    if (filterItems.contains(column)) {
        widget = filterItems[column];
    } else {
        widget = new QLineEdit(filterArea);
        createFilterItem(column, widget, SIGNAL(textChanged(QString)));
    }

    if (!widget->inherits("QLineEdit")) {
        removeFilterItem(column);
        widget = new QLineEdit(filterArea);
        createFilterItem(column, widget, SIGNAL(textChanged(QString)));
    }

    QLineEdit *lineEdit = qobject_cast<QLineEdit *>(widget);
    lineEdit->setText(text);

    Q_EMIT textChanged(column, text);

    updateArea();
}

void JHeaderAreaPrivate::setFilterItem(int column, const QStringList &texts)
{
    QWidget *widget = 0;

    if (filterItems.contains(column)) {
        widget = filterItems[column];
    } else {
        QComboBox *comboBox = new QComboBox(filterArea);
        comboBox->setEditable(true);
        comboBox->setAutoCompletion(true);
        comboBox->setInsertPolicy(QComboBox::NoInsert);
        createFilterItem(column, comboBox, SIGNAL(editTextChanged(QString)));
        widget = comboBox;
    }

    if (!widget->inherits("QComboBox")) {
        removeFilterItem(column);
        QComboBox *comboBox = new QComboBox(filterArea);
        comboBox->setEditable(true);
        comboBox->setAutoCompletion(true);
        comboBox->setInsertPolicy(QComboBox::NoInsert);
        createFilterItem(column, comboBox, SIGNAL(editTextChanged(QString)));
        widget = comboBox;
    }

    QComboBox *comboBox = qobject_cast<QComboBox *>(widget);
    if (texts.isEmpty()) {
        updateFilterItem(column);
    } else {
        comboBox->clear();
        comboBox->addItem(tr("#All#"));
        comboBox->addItems(texts);
    }

    Q_EMIT textChanged(column, comboBox->currentText());

    updateArea();
}

void JHeaderAreaPrivate::removeFilterItem(int column, bool update)
{
    if (filterItems.contains(column)) {
        cusSortModel->removeFilter(column);
        QWidget *widget = filterItems[column];
        disconnectFromThis(widget);
        widget->deleteLater();
        filterItems.remove(column);
    }

    if (update) {
        updateArea();
    }
}

void JHeaderAreaPrivate::clearFilterItem(bool update)
{
    int column = 0;
    QWidget *widget = 0;
    QMapIterator<int, QWidget *> iter(filterItems);
    while (iter.hasNext()) {
        iter.next();
        column = iter.key();
        cusSortModel->removeFilter(column);
        widget = filterItems[column];
        disconnectFromThis(widget);
        widget->deleteLater();
        filterItems.remove(column);
    }

    if (update) {
        updateArea();
    }
}

bool JHeaderAreaPrivate::filterVisible() const
{
    return actionEnable ? actionEnable->isCheckable() : false;
}

void JHeaderAreaPrivate::setFilterVisible(bool visible)
{
    if (actionEnable) {
        actionEnable->setChecked(visible);
    }
}

void JHeaderAreaPrivate::updateArea()
{
    if (!horiHeader) {
        return;
    }

    int height = filterArea->height();
    int left_offset = horiHeader->geometry().left();
    int offset = left_offset - horiHeader->offset();

    QMapIterator<int, QWidget *> iter(filterItems);
    while (iter.hasNext()) {
        iter.next();
        int index = iter.key();
        QWidget *item = iter.value();
        int x = offset += horiHeader->sectionPosition(index);
        if (x < left_offset) {
            item->setVisible(false);
        } else {
            item->setVisible(true);
            item->setGeometry(x, 0, horiHeader->sectionSize(index), height);
        }
    }
}

void JHeaderAreaPrivate::updateFilters()
{
    QMapIterator<int, QWidget *> iter(filterItems);
    while (iter.hasNext()) {
        iter.next();
        QWidget *widget = iter.value();
        if (widget->inherits("QLineEdit")) {
            setFilterRegExp(iter.key(), qobject_cast<QLineEdit *>(widget)->text());
        } else if (widget->inherits("QComboBox")) {
            setFilterRegExp(iter.key(), qobject_cast<QComboBox *>(widget)->currentText());
        }
    }
}

void JHeaderAreaPrivate::onScrollarValueChanged(int value)
{
    Q_UNUSED(value);

    updateArea();
}

void JHeaderAreaPrivate::onSectionResized(int logicalIndex, int oldSize, int newSize)
{
    Q_UNUSED(logicalIndex);
    Q_UNUSED(oldSize);
    Q_UNUSED(newSize);

    updateArea();
}

void JHeaderAreaPrivate::onSectionDoubleClicked(int logicalIndex)
{
    if (!switchEnable) {
        return;
    }

    if (!filterVisible()) {
        return;
    }

    if (filterItems.contains(logicalIndex)) {
        removeFilterItem(logicalIndex);
    } else {
        setFilterItem(logicalIndex);
    }
}

void JHeaderAreaPrivate::onSectionMoved(int logicalIndex, int oldVisualIndex, int newVisualIndex)
{
    Q_UNUSED(logicalIndex);
    Q_UNUSED(oldVisualIndex);
    Q_UNUSED(newVisualIndex);

    if (!filterVisible()) {
        return;
    }

    updateArea();
}

void JHeaderAreaPrivate::onCellChanged(int row, int column)
{
    Q_UNUSED(row);
    updateFilterItem(column);
}

void JHeaderAreaPrivate::onFilterTextChanged(const QString &text)
{
    setFilterRegExp(sender()->property("column").toInt(), text);
}

void JHeaderAreaPrivate::setFilterRegExp(int column, const QString &text)
{
    QString pattern;
    if (text == tr("#All#")) {
        pattern = "";
    } else {
        pattern = text;
    }

    cusSortModel->setFilterRegExp(column, QRegExp(pattern, Qt::CaseInsensitive,
                                                  QRegExp::WildcardUnix));
}

void JHeaderAreaPrivate::onChangeFilter()
{
    QAction *action = qobject_cast<QAction *>(sender());
    if (!action) {
        return;
    }

    QVariant variant = action->data();
    if (variant.isNull()) {
        return;
    }

    QWidget *widget = reinterpret_cast<QWidget *>(variant.value<void *>());
    if (!widget) {
        return;
    }

    variant = widget->property("column");
    if (variant.isNull()) {
        return;
    }

    int column = variant.toInt();
    if (widget->inherits("QLineEdit")) {
        setFilterItem(column, QStringList());
    } else if (widget->inherits("QComboBox")) {
        setFilterItem(column, "");
    }
}

void JHeaderAreaPrivate::updateFilterModel(bool visible)
{
    Q_ASSERT(view && filterArea);
    QAbstractItemModel *model = view->model();
    filterArea->setVisible(visible);
    if (visible) {
        if (model != cusSortModel) {
            cusSortModel->setSourceModel(sourceModel);
            view->setModel(cusSortModel);
            updateFilterItem();
            updateFilters();
        }
    } else {
        if (model != sourceModel) {
            view->setModel(sourceModel);
        }
    }
}

void JHeaderAreaPrivate::replaceFilterItem()
{
    QAction *action = qobject_cast<QAction *>(sender());
    if (!action) {
        return;
    }

    QString text = action->text();
    if (text.contains(tr("QLineEdit"))) {
        text.replace(tr("QLineEdit"), tr("QComboBox"));
        for (int i = 0; i < sourceModel->columnCount(); ++i) {
            setFilterItem(i, "");
        }
    } else if (text.contains(tr("QComboBox"))) {
        text.replace(tr("QComboBox"), tr("QLineEdit"));
        for (int i = 0; i < sourceModel->columnCount(); ++i) {
            setFilterItem(i);
        }
    }

    action->setText(text);
}

void JHeaderAreaPrivate::replaceFilterItemWithLineEdit()
{
    if (!sourceModel) {
        return;
    }

    QString text = actionReplace->text();
    text.replace(tr("QLineEdit"), tr("QComboBox"));
    for (int i = 0; i < sourceModel->columnCount(); ++i) {
        setFilterItem(i, "");
    }

    actionReplace->setText(text);
}

void JHeaderAreaPrivate::replaceFilterItemWithComboBox()
{
    if (!sourceModel) {
        return;
    }

    QString text = actionReplace->text();
    for (int i = 0; i < sourceModel->columnCount(); ++i) {
        setFilterItem(i);
    }

    actionReplace->setText(text);
}

void JHeaderAreaPrivate::updateFilterItem()
{
    QMapIterator<int, QWidget *> iter(filterItems);
    while (iter.hasNext()) {
        iter.next();
        updateFilterItem(iter.key());
    }
}

void JHeaderAreaPrivate::updateFilterItem(int column)
{
    if (!filterItems.contains(column)) {
        return;
    }

    QWidget *widget = filterItems[column];
    if (!(widget && widget->inherits("QComboBox"))) {
        return;
    }

    QComboBox *comboBox = qobject_cast<QComboBox *>(widget);
    if (!comboBox) {
        return;
    }

    comboBox->clear();
    comboBox->addItem(tr("#All#"));
    if (view && view->inherits("JXmlTable")) {
        QVariant items = qobject_cast<JXmlTable *>(view)->itemProperty(-1, column, "items");
        if (items.isValid() && items.canConvert(QVariant::StringList)) {
            comboBox->addItems(items.toStringList());
            return;
        }
    }

    //
    int rowCount = sourceModel->rowCount();
    for (int i = 0; i < rowCount; ++i) {
        QStandardItem *item = sourceModel->item(i, column);
        if (item) {
            QString text = item->text();
            if (text.isEmpty()) {
                continue;
            }
            if (comboBox->findText(text) == -1) {
                comboBox->addItem(text);
            }
        }
    }
}

void JHeaderAreaPrivate::_emit_insertRow(int row)
{
    Q_UNUSED(row);
    if (!filterVisible()) {
        return;
    }
}

void JHeaderAreaPrivate::_emit_insertColumn(int column)
{
    if (!filterVisible()) {
        return;
    }

    if (filterItems.isEmpty()) {
        return;
    }

    QMutableMapIterator<int, QWidget *> iter(filterItems);
    iter.toBack();
    while (iter.hasPrevious()) {
        iter.previous();
        if (column > iter.key()) {
            break;
        }

        filterItems.insert(iter.key() + 1, iter.value());
        iter.remove();
    }
}

void JHeaderAreaPrivate::_emit_removeRow(int row)
{
    Q_UNUSED(row);
    if (!filterVisible()) {
        return;
    }

    updateFilterItem();
}

void JHeaderAreaPrivate::_emit_removeColumn(int column)
{
    if (!filterVisible()) {
        return;
    }

    if (filterItems.isEmpty()) {
        return;
    }

    removeFilterItem(column, false);

    QMutableMapIterator<int, QWidget *> iter(filterItems);
    while (iter.hasNext()) {
        iter.next();
        if (column < iter.key()) {
            filterItems.insert(iter.key() - 1, iter.value());
            iter.remove();
        }
    }

    updateArea();
}

void JHeaderAreaPrivate::_emit_clear()
{
    if (!filterVisible()) {
        return;
    }

    clearFilterItem();
}

void JHeaderAreaPrivate::_emit_clearContents()
{
    if (!filterVisible()) {
        return;
    }

    updateFilterItem();
}

void JHeaderAreaPrivate::init()
{
    Q_Q(JHeaderArea);

    q->setObjectName("JHeaderArea");

    // default properties
    q->setTitleAlignment(Qt::AlignCenter);
    q->setTitleHeight(25);
    q->setTitleStyle("border-top-left-radius:3px;"
                     "border-top-right-radius:3px;"
                     "border-top:1px solid #666;"
                     "border-left:1px solid #666;"
                     "border-right:1px solid #666;"
                     "margin-left:0;margin-right:0;"
                     "background:qlineargradient(x1:0,y1:0,x2:0,y2:1,"
                     "stop:0#44595e,"
                     "stop:1#44595e);"
                     "color:#dbdbdb;");
    q->setTitleVisible(true);
    q->setFilterHeight(25);
    q->setFilterVisible(false);
}

void JHeaderAreaPrivate::createFilterItem(int column, QWidget *widget, const char *asignal)
{
    Q_ASSERT(widget);
    filterItems[column] = widget;
    widget->setProperty("column", column);
    widget->setContextMenuPolicy(switchEnable
                                 ? Qt::ActionsContextMenu
                                 : Qt::DefaultContextMenu);
    widget->show();
    QAction *action = 0;
    if (widget->inherits("QLineEdit")) {
        action = new QAction(tr("Change to QComboBox"), widget);
    } else if (widget->inherits("QComboBox")) {
        action = new QAction(tr("Change to QLineEdit"), widget);
    }

    if (action) {
        action->setData(QVariant::fromValue<void *>(widget));
        widget->addAction(action);
        connect(action, SIGNAL(triggered(bool)), SLOT(onChangeFilter()));
    }

    connect(widget, asignal, SLOT(onFilterTextChanged(QString)));
}

void JHeaderAreaPrivate::disconnectFromThis(QWidget *widget)
{
    if (!widget) {
        return;
    }

    if (widget->inherits("QLineEdit")) {
        disconnect(widget, SIGNAL(textChanged(QString)),
                   this, SLOT(onFilterTextChanged(QString)));
    } else if (widget->inherits("QComboBox")) {
        disconnect(widget, SIGNAL(currentIndexChanged(QString)),
                   this, SLOT(onFilterTextChanged(QString)));
    }

    QListIterator<QAction *> iter(widget->actions());
    while (iter.hasNext()) {
        disconnect(iter.next(), SIGNAL(triggered(bool)),
                   this, SLOT(onChangeFilter()));
    }
}

bool JHeaderAreaPrivate::attach(QAbstractItemView *view)
{
    if (this->view) {
        return this->view == view;
    }

    if (!view) {
        return false;
    }

    this->view = view;

    if (view->inherits("JTableView")) {
        JTableView *tableView = qobject_cast<JTableView *>(view);
        if (!tableView) {
            return false;
        }

        horiHeader = tableView->horizontalHeader();
        vertheader = tableView->verticalHeader();
        // signal - slot
        connect(tableView->privateData(), SIGNAL(_signal_insertRow(int)),
                SLOT(_emit_insertRow(int)), Qt::QueuedConnection);
        connect(tableView->privateData(), SIGNAL(_signal_insertColumn(int)),
                SLOT(_emit_insertColumn(int)), Qt::QueuedConnection);
        connect(tableView->privateData(), SIGNAL(_signal_removeRow(int)),
                SLOT(_emit_removeRow(int)), Qt::QueuedConnection);
        connect(tableView->privateData(), SIGNAL(_signal_removeColumn(int)),
                SLOT(_emit_removeColumn(int)), Qt::QueuedConnection);
        connect(tableView->privateData(), SIGNAL(_signal_clear()),
                SLOT(_emit_clear())/*, Qt::QueuedConnection*/);
        connect(tableView->privateData(), SIGNAL(_signal_clearContents()),
                SLOT(_emit_clearContents()), Qt::QueuedConnection);
        connect(tableView->privateData(), SIGNAL(_signal_updateFilterArea()),
                SLOT(updateArea()), Qt::QueuedConnection);
    } else if (view->inherits("JTreeView")) {
        JTreeView *treeView = qobject_cast<JTreeView *>(view);
        if (!treeView) {
            return false;
        }

        horiHeader = treeView->header();
        // signal - slot
    } else {
        return false;
    }

    setAutpUpdateTitle(autoUpdateTitle);

    // add to layout
    vertLayoutMain->addWidget(view);

    //
    sourceModel = qobject_cast<QStandardItemModel *>(view->model());
    Q_ASSERT(sourceModel);

    horiHeader->setContextMenuPolicy(switchEnable
                                     ? Qt::ActionsContextMenu
                                     : Qt::DefaultContextMenu);
    actionEnable = new QAction(tr("Enable fitler"), horiHeader);
    actionEnable->setCheckable(true);
    actionReplace = new QAction(tr("Replace all with ") + tr("QLineEdit"), horiHeader);
    actionReplace->setVisible(actionEnable->isChecked());
    horiHeader->addAction(actionEnable);
    horiHeader->addAction(actionReplace);

    //
    connect(view->horizontalScrollBar(), SIGNAL(valueChanged(int)),
            SLOT(onScrollarValueChanged(int)), Qt::QueuedConnection);
    connect(horiHeader, SIGNAL(sectionResized(int,int,int)),
            SLOT(onSectionResized(int,int,int)), Qt::QueuedConnection);
    connect(horiHeader, SIGNAL(sectionDoubleClicked(int)),
            SLOT(onSectionDoubleClicked(int)), Qt::QueuedConnection);
    connect(horiHeader, SIGNAL(sectionMoved(int,int,int)),
            SLOT(onSectionMoved(int,int,int)), Qt::QueuedConnection);
    connect(view, SIGNAL(cellChanged(int, int)),
            SLOT(onCellChanged(int,int)), Qt::QueuedConnection);
    connect(actionEnable, SIGNAL(toggled(bool)), SLOT(updateFilterModel(bool)));
    connect(actionEnable, SIGNAL(toggled(bool)), actionReplace, SLOT(setVisible(bool)));
    connect(actionReplace, SIGNAL(triggered(bool)), SLOT(replaceFilterItem()));

    //
    filterArea->setVisible(actionEnable->isChecked());

    return true;
}

void JHeaderAreaPrivate::detach()
{
    if (!view) {
        return;
    }

    if (view->inherits("JTableView")) {
        JTableView *tableView = qobject_cast<JTableView *>(view);
        if (tableView) {
            // disconnect all signal-slot
            disconnect(tableView->privateData(), SIGNAL(_signal_insertRow(int)),
                       this, SLOT(_emit_insertRow(int)));
            disconnect(tableView->privateData(), SIGNAL(_signal_insertColumn(int)),
                       this, SLOT(_emit_insertColumn(int)));
            disconnect(tableView->privateData(), SIGNAL(_signal_removeRow(int)),
                       this, SLOT(_emit_removeRow(int)));
            disconnect(tableView->privateData(), SIGNAL(_signal_removeColumn(int)),
                       this, SLOT(_emit_removeColumn(int)));
            disconnect(tableView->privateData(), SIGNAL(_signal_clear()),
                       this, SLOT(_emit_clear()));
            disconnect(tableView->privateData(), SIGNAL(_signal_clearContents()),
                       this, SLOT(_emit_clearContents()));
            disconnect(tableView->privateData(), SIGNAL(_signal_udpateFilterArea()),
                       this, SLOT(updateArea()));
        }
    } else if (view->inherits("JTreeView")) {
        JTreeView *treeView = qobject_cast<JTreeView *>(view);
        if (treeView) {
            // disconnect all signal-slot
        }
    }

    //
    setAutpUpdateTitle(autoUpdateTitle);

    //
    setFilterVisible(false);

    // remove from layout
    vertLayoutMain->removeWidget(view);
    horiHeader->removeAction(actionEnable);
    horiHeader->removeAction(actionReplace);
    horiHeader->setContextMenuPolicy(Qt::DefaultContextMenu);

    //
    disconnect(view->horizontalScrollBar(), SIGNAL(valueChanged(int)),
               this, SLOT(onScrollarValueChanged(int)));
    disconnect(horiHeader, SIGNAL(sectionResized(int,int,int)),
               this, SLOT(onSectionResized(int,int,int)));
    disconnect(horiHeader, SIGNAL(sectionDoubleClicked(int)),
               this, SLOT(onSectionDoubleClicked(int)));
    disconnect(horiHeader, SIGNAL(sectionMoved(int,int,int)),
               this, SLOT(onSectionMoved(int,int,int)));
    disconnect(view, SIGNAL(cellChanged(int, int)),
               this, SLOT(onCellChanged(int,int)));
    disconnect(actionEnable, SIGNAL(toggled(bool)), this, SLOT(updateFilterModel(bool)));
    disconnect(actionEnable, SIGNAL(toggled(bool)), actionReplace, SLOT(setVisible(bool)));
    disconnect(actionReplace, SIGNAL(triggered(bool)), this, SLOT(replaceFilterItem()));
    // private
    disconnect(this, SIGNAL(sortModelChanged(QSortFilterProxuModel*)),
               view, SLOT(setSortModel(QSortFilterProxuModel*)));

    actionEnable->deleteLater();
    actionReplace->deleteLater();

    actionEnable = 0;
    actionReplace  = 0;

    horiHeader = 0;
    vertheader = 0;

    view = 0;
    sourceModel = 0;
}

void JHeaderAreaPrivate::setAutpUpdateTitle(bool enable)
{
    if (!view) {
        return;
    }

    Q_Q(JHeaderArea);
    if (view->inherits("JTableView")) {
        if (view->inherits("JXmlTable")) {
            disconnect(view, SIGNAL(tableChanged(QString)), q, SLOT(setTitle(QString)));
            if (enable) {
                connect(view, SIGNAL(tableChanged(QString)), q, SLOT(setTitle(QString)));
            }
        } else {
            //
        }
    }
}

void JHeaderAreaPrivate::setSwitchEnable(bool enable)
{
    if (!view) {
        return;
    }

    if (enable == switchEnable) {
        return;
    }

    switchEnable = enable;
    //
    horiHeader->setContextMenuPolicy(enable
                                     ? Qt::ActionsContextMenu
                                     : Qt::DefaultContextMenu);
    //
    QMapIterator<int, QWidget *> iterFilter(filterItems);
    while (iterFilter.hasNext()) {
        iterFilter.next();
        QWidget *widget = iterFilter.value();
        if (widget) {
            widget->setContextMenuPolicy(enable
                                         ? Qt::ActionsContextMenu
                                         : Qt::DefaultContextMenu);
        }
    }
}

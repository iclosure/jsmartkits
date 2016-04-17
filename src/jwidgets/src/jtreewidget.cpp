#include "precomp.h"
#include "jtreewidget.h"

// - class JTreeWidget -

JTreeWidget::JTreeWidget(QWidget *parent) :
    QTreeWidget(parent),
    q_checkable(true),
    q_enableIcon(true),
    q_enableTooltip(true),
    q_enableConnection(false),
    q_isDrag(false),
    q_rootItem(0)
{
    setFrameShape(QFrame::NoFrame);

    // 用户数据（节点数据）
    q_userData.userData = new JTreeWidgetUserData<JTreeItem>();
    q_userData.bSelf = true;

    setSelectionBehavior(QAbstractItemView::SelectItems);

    //
    setAnimated(true);

    //
    enableSlotItemChanged(true);
}

JTreeWidget::~JTreeWidget()
{
    if (q_userData.userData != 0 && q_userData.bSelf) {
        delete q_userData.userData;
        q_userData.userData = 0;
    }
}

void JTreeWidget::enableTooltip(bool enabled)
{
    if (q_enableTooltip != enabled) {
        q_enableTooltip = enabled;
        //Q_EMIT
    }
}

bool JTreeWidget::loadTreeItems(QTreeWidgetItem *parentItem)
{
    return loadTreeItems(parentItem, jTreeItem<JTreeItem>());
}

bool JTreeWidget::loadTreeItems(QTreeWidgetItem *parentItem, const JTreeItem &item)
{
    if (item.text.empty()) {
        return false;
    }

    //
    QTreeWidgetItem *treeWidgetItem = 0;
    if (parentItem == 0) {
        treeWidgetItem = new QTreeWidgetItem(this);
    } else {
        treeWidgetItem = new QTreeWidgetItem(parentItem);
    }

    //
    treeWidgetItem->setData(0, Qt::UserRole, QVariant::fromValue((void *)&item.itemData));
#if 1
    treeWidgetItem->setFlags(treeWidgetItem->flags() &(~Qt::ItemIsUserCheckable));
    if (q_checkable && item.checkable) {
        QCheckBox *checkBox = new QCheckBox(QString::fromStdString(item.text), this);
        checkBox->setProperty("treeWidgetItem", QVariant::fromValue((void *)treeWidgetItem));
        checkBox->setProperty("column", 0);
        //checkBox->setStyleSheet("QCheckBox{background:transparent;}");
        checkBox->setStyleSheet("QCheckBox{border-radius:2px;}"
                                "QCheckBox:hover{color:#e9be4b;background:#545d6b;}}");
        checkBox->setAttribute(Qt::WA_TranslucentBackground);
        checkBox->setAttribute(Qt::WA_NoSystemBackground);
        JCellWidgetData *checkBoxUserData = new JCellWidgetData();
        checkBoxUserData->setData((Qt::CheckState)item.checkState);
        checkBox->setUserData(0, checkBoxUserData);
        setItemWidget(treeWidgetItem, 0, checkBox);
        connect(checkBox, SIGNAL(stateChanged(int)), this, SLOT(slotItemCheckedChanged(int)));
        checkBox->setCheckState((Qt::CheckState)item.checkState);
    } else {
        treeWidgetItem->setText(0, QString::fromStdString(item.text));
        if (q_enableIcon
                && (item.widgets.empty() || item.widgets.find(0) == item.widgets.end())) {
            treeWidgetItem->setIcon(0, QIcon(QString::fromStdString(item.icon)));
        }
    }
#else
    treeWidgetItem->setText(0, QString::fromStdString(item.name));
    if (q_checkable && item.checkable) {
        treeWidgetItem->setCheckState(0, (Qt::CheckState)item.checkState);
    } else {
        treeWidgetItem->setFlags(treeWidgetItem->flags() & (~Qt::ItemIsUserCheckable));
    }
#endif
    if (q_enableTooltip) {
        treeWidgetItem->setToolTip(0, QString::fromStdString(item.text));
    }
    // itemWidget
    std::map<int, JTreeItemWidget>::const_iterator citerWidgets =
            item.widgets.begin();
    for (; citerWidgets != item.widgets.end(); ++citerWidgets) {
        const JTreeItemWidget &itemWidget = citerWidgets->second;
        if (itemWidget.widget != 0) {
            setItemWidget(treeWidgetItem, itemWidget.column, itemWidget.widget);
        }
    }

    //
    std::list<JTreeItem>::const_iterator citerItem = item.items.begin();
    for (; citerItem != item.items.end(); ++citerItem) {
        loadTreeItems(treeWidgetItem, *citerItem);
    }

    return true;
}

bool JTreeWidget::loadTreeItems(QTreeWidgetItem *parentItem, const std::list<JTreeItem> &items)
{
    if (parentItem == 0 || items.empty()) {
        return true;
    }

    // 初始化断开消息链接
    bool enabled = q_enableConnection;
    if (enabled) {
        enableSlotItemChanged(false);
    }

    //
    std::list<JTreeItem>::const_iterator citerItem = items.begin();
    for (; citerItem != items.end(); ++citerItem) {
        loadTreeItems(parentItem, *citerItem);
    }

    // 初始化完成后恢复消息链接状态
    if (enabled) {
        enableSlotItemChanged(enabled);
    }

    return true;
}

bool JTreeWidget::loadTableItems(QTreeWidgetItem *parentItem, const JTreeItem &item)
{
    if (item.text.empty()) {
        return false;
    }

    // 生成列文本
    QStringList rowTexts;
    rowTexts.reserve(item.columnOffset + 1);
    rowTexts.last() = QString::fromStdString(item.text);
    if (item.columnOffset == 0) {
        rowTexts.last().prepend("    ");        //
    }

    //
    QTreeWidgetItem *treeWidgetItem = 0;
    if (parentItem == 0) {
        treeWidgetItem = new QTreeWidgetItem(this, rowTexts);
    } else {
        treeWidgetItem = new QTreeWidgetItem(parentItem, rowTexts);
    }

    treeWidgetItem->setData(0, Qt::UserRole, QVariant::fromValue((void *)&item.itemData));
#if 1
    treeWidgetItem->setFlags(treeWidgetItem->flags() &(~Qt::ItemIsUserCheckable));
    if (q_checkable && item.checkable) {
        QCheckBox *checkBox = new QCheckBox(this);
        checkBox->setProperty("treeWidgetItem", QVariant::fromValue((void *)treeWidgetItem));
        checkBox->setProperty("column", 0);
        /*checkBox->setStyleSheet("QCheckBox{border-radius:2px;}"
                                "QCheckBox:hover{color:#e9be4b;background:#545d6b;}}");*/
        checkBox->setAttribute(Qt::WA_TranslucentBackground);
        checkBox->setAttribute(Qt::WA_NoSystemBackground);
        JCellWidgetData *checkBoxUserData = new JCellWidgetData();
        checkBoxUserData->setData((Qt::CheckState)item.checkState);
        checkBox->setUserData(0, checkBoxUserData);
        setItemWidget(treeWidgetItem, 0/*item.columnOffset*/, checkBox);
        connect(checkBox, SIGNAL(stateChanged(int)), this, SLOT(slotItemCheckedChanged(int)));
        checkBox->setCheckState((Qt::CheckState)item.checkState);
    } else if (q_enableIcon
               && (item.widgets.empty() || item.widgets.find(0) == item.widgets.end())) {
        treeWidgetItem->setIcon(0, QIcon(QString::fromStdString(item.icon)));
    }
#else
    treeWidgetItem->setText(0, QString::fromStdString(item.name));
    if (q_checkable && item.checkable) {
        treeWidgetItem->setCheckState(0/*item.columnOffset*/, (Qt::CheckState)item.checkState);
    } else {
        treeWidgetItem->setFlags(treeWidgetItem->flags() & (~Qt::ItemIsUserCheckable));
    }
#endif
    if (q_enableTooltip) {
        treeWidgetItem->setToolTip(item.columnOffset, QString::fromStdString(item.text));
    }
    // itemWidget
    std::map<int, JTreeItemWidget>::const_iterator citerWidgets =
            item.widgets.begin();
    for (; citerWidgets != item.widgets.end(); ++citerWidgets) {
        const JTreeItemWidget &itemWidget = citerWidgets->second;
        if (itemWidget.widget != 0) {
            setItemWidget(treeWidgetItem, itemWidget.column, itemWidget.widget);
        }
    }

    //
    std::list<JTreeItem>::const_iterator citerItem = item.items.begin();
    for (; citerItem != item.items.end(); ++citerItem) {
        loadTableItems(treeWidgetItem, *citerItem);
    }

    return true;
}

bool JTreeWidget::loadTableItems(QTreeWidgetItem *parentItem, const std::list<JTreeItem> &items)
{
    if (parentItem == 0 || items.empty()) {
        return true;
    }

    // 初始化断开消息链接
    bool enabled = q_enableConnection;
    if (enabled) {
        //enableSlotItemChanged(false);
    }

    //
    std::list<JTreeItem>::const_iterator citerItem = items.begin();
    for (; citerItem != items.end(); ++citerItem) {
        loadTableItems(parentItem, *citerItem);
    }

    // 初始化完成后恢复消息链接状态
    if (enabled) {
        //enableSlotItemChanged(enabled);
    }

    return true;
}

void JTreeWidget::deleteItem(const QString &identity)
{
    QTreeWidgetItem *item = findTreeItem(identity);
    if (item == 0) {
        return;
    }

    // 判断是否存在子节点，存在则要迭代删除
    QTreeWidgetItem *pChildItem = 0;
    JTreeItemData *pItemData = 0;
    QVariant variant;
    int childCount = item->childCount();

    for (int i = 0; i < childCount; ++i) {
        pChildItem = item->child(i);
        if (pChildItem == 0) {
            continue;
        }

        //
        variant = pChildItem->data(0, Qt::UserRole);
        if (variant.isNull()) {
            continue;
        }

        //
        pItemData = reinterpret_cast<JTreeItemData *>(variant.value<void *>());
        if (pItemData == 0) {
            continue;
        }

        deleteItem(pItemData->identity);
    }

    // 删除节点方法
    QTreeWidgetItem *_treeWidgetItem = 0;
    QTreeWidgetItem *parentItem =item->parent();
    if (parentItem == 0) {
        // 如果为topLevelItem
        _treeWidgetItem = topLevelItem(indexOfTopLevelItem(item));
    } else {
        // 如果存在父节点
        _treeWidgetItem = parentItem->child(parentItem->indexOfChild(item));
    }

    //
    if (_treeWidgetItem != 0) {
        delete _treeWidgetItem;
    }
}

QTreeWidgetItem *JTreeWidget::rootItem()
{
    return q_rootItem;
}

void JTreeWidget::setRootItem(QTreeWidgetItem *item)
{
    if (q_rootItem == item) {
        q_rootItem = item;
        //Q_EMIT
    }
}

QTreeWidgetItem *JTreeWidget::findTreeItem(const QString &identity, QTreeWidgetItem *current)
{
    QTreeWidgetItem *currentItem = current;
    if (currentItem == 0) {
        currentItem = topLevelItem(0);
    }

    if (currentItem == 0) {
        return 0;
    }

    JTreeItemData *pItemData = treeItemData(currentItem);
    if (pItemData != 0) {
        if (pItemData->identity == identity) {
            return currentItem;
        }
    }

    int count = currentItem->childCount();
    for (int i = 0; i < count; ++i) {
        QTreeWidgetItem *item = findTreeItem(identity, currentItem->child(i));
        if (item != 0) {
            return item;
        }
    }

    return 0;
}

QString JTreeWidget::identityOfTreeItem(QTreeWidgetItem *item) const
{
    JTreeItemData *pItemData = treeItemData(item);
    if (pItemData == 0) {
        return QString::null;
    }

    return pItemData->identity;
}

bool JTreeWidget::treeItemData(const QTreeWidgetItem *item, JTreeItemData &itemData)
{
    JTreeItemData *pItemData = treeItemData(item);
    if (pItemData == 0) {
        return false;
    }

    itemData = *pItemData;

    return true;
}

JTreeItemData *JTreeWidget::treeItemData(const QTreeWidgetItem *item)
{
    if (item == 0) {
        return 0;
    }

    QVariant v = item->data(0, Qt::UserRole);
    if (v.isNull()) {
        return 0;
    }

    return reinterpret_cast<JTreeItemData *>(v.value<void *>());
}

JTreeItemData *JTreeWidget::currentTreeItemData()
{
    return treeItemData(currentItem());
}

JTreeItem *JTreeWidget::currentTreeItem()
{
    JTreeItemData *treeItemData = currentTreeItemData();
    if (treeItemData == 0) {
        return 0;
    }

    // 获取节点数据
    return treeItemData->item;
}

void JTreeWidget::enableSlotItemChanged(bool enable)
{
    disconnect(this, SIGNAL(itemChanged(QTreeWidgetItem*,int)),
               this, SLOT(slotItemChanged(QTreeWidgetItem*,int)));

    q_enableConnection = enable;

    if (enable) {
        connect(this, SIGNAL(itemChanged(QTreeWidgetItem*,int)),
                this, SLOT(slotItemChanged(QTreeWidgetItem*,int)));
    }
}

void JTreeWidget::enableAllCheckable(bool enable)
{
    if (q_checkable != enable) {
        q_checkable = enable;
        //Q_EMIT
    }
}

void JTreeWidget::setDefaultIconEnable(bool enable)
{
    if (q_enableIcon != enable) {
        q_enableIcon = enable;
        //Q_EMIT
    }
}

void JTreeWidget::setItemAllExpandded(QTreeWidgetItem *item, bool expand)
{
    if (item == 0) {
        return;
    }

    //
    item->setExpanded(expand);

    //
    int count = item->childCount();
    for (int i = 0; i < count; ++i) {
        setItemAllExpandded(item->child(i), expand);
    }
}

void JTreeWidget::setItemState(QTreeWidgetItem *item, int column, Qt::CheckState state)
{
    if (item == 0) {
        return;
    }

    QCheckBox *checkBox = qobject_cast<QCheckBox *>(itemWidget(item, column));
    bool checkable = checkBox == 0 ? true : (item->flags() & Qt::ItemIsUserCheckable);
    if (!checkable) {
        return;
    }

    // 更新节点数据状态
    JTreeItemData *pItemData = jTreeItem<JTreeItemData *, void *>(item, column);
    if (pItemData != 0 && pItemData->item != 0) {
        pItemData->item->checkState = state;
    }

    // 设置状态
    if (checkBox != 0) {
        checkBox->setCheckState(state);
    } else {
        item->setCheckState(column, state);
    }
}

void JTreeWidget::setAllItmeState(Qt::CheckState state, QTreeWidgetItem *item)
{
    // 初始化断开消息链接
    bool enabled = q_enableConnection;
    if (enabled) {
        enableSlotItemChanged(false);
    }

    //
    QTreeWidgetItem *pItem = item;
    if (pItem == 0) {
        pItem = topLevelItem(0);

        // 设置子节点状态
        setChildrenState(pItem, 0, state);
    } else {
        //设置子节点状态
        setChildrenState(pItem, 0, state);

        // 设置兄弟节点状态
        setSiblingState(pItem, 0, state);

        // 设置父节点状态
        setParentState(pItem, 0, state);
    }

    // 初始化完成后恢复消息链接状态
    if (enabled) {
        enableSlotItemChanged(enabled);
    }
}

void JTreeWidget::slotItemChanged(QTreeWidgetItem *item, int column)
{
    if (item == 0 || column != 0) {
        return;     //
    }

    // 初始化断开消息链接
    bool enabled = q_enableConnection;
    if (enabled) {
        enableSlotItemChanged(false);
    }

    //
    Qt::CheckState checkState = Qt::Checked;
    QCheckBox *checkBox = qobject_cast<QCheckBox *>(itemWidget(item, column));
    if (checkBox != 0) {
        JCellWidgetData *checkBoxUserData = JCellWidgetData::userData(checkBox->userData(0));
        if (checkBoxUserData != 0) {
            checkState = (Qt::CheckState)checkBoxUserData->data().toInt();
            if (checkState != Qt::Unchecked) {
                checkState = Qt::Checked;
                checkBox->setCheckState(checkState);
                checkBoxUserData->setData(checkState);
            }
        }
    } else {
        checkState = item->checkState(column);
        item->setData(column, ItemCheckableState, checkState);
    }

    // 设置子节点状态
    setChildrenState(item, column, checkState);

    // 设置兄弟节点状态
    setSiblingState(item, column, checkState);

    // 设置父节点状态
    setParentState(item, column, checkState);

    // 初始化完成后恢复消息链接状态
    if (enabled) {
        enableSlotItemChanged(enabled);
    }
}

void JTreeWidget::slotItemCheckedChanged(int state)
{
    QTreeWidgetItem *item = 0;
    if (sender()->inherits("QCheckBox")) {
        QCheckBox *checkBox = qobject_cast<QCheckBox *>(sender());
        if (checkBox != 0) {
            QVariant v = checkBox->property("treeWidgetItem");
            if (v.isValid()) {
                item = reinterpret_cast<QTreeWidgetItem *>(v.value<void *>());
                JTreeItemData *pItemData = treeItemData(item);
                if (pItemData != 0 && pItemData->item != 0) {
                    pItemData->item->checkState = state;
                }

                //
                JCellWidgetData *pUserData = JCellWidgetData::userData(checkBox->userData(0));
                if (pUserData != 0) {
                    pUserData->setData(state);
                }
            }
        }
    }

    if (item == 0) {
        return;
    }

    //
    Q_EMIT itemChanged(item, 0);
}

int JTreeWidget::uniqueIndex()
{
    Q_ASSERT(false);     // 未实现
    return 0;
}

void JTreeWidget::setChildrenState(QTreeWidgetItem *item, int column, Qt::CheckState state)
{
    if (item == 0) {
        return;
    }

    // 递归设置子节点状态
    int count = item->childCount();
    for (int i = 0; i < count; ++i) {
        QTreeWidgetItem *pChildItem = item->child(i);
        if (pChildItem == 0) {
            continue;
        }

        //
        QCheckBox *checkBox = qobject_cast<QCheckBox *>(itemWidget(pChildItem, column));
        bool checkable = (checkBox != 0) ? true : (pChildItem->flags() & Qt::ItemIsUserCheckable);
        if (checkable) {
            // 设置本节点状态
            if (checkBox != 0) {
                checkBox->setCheckState(state);
            } else {
                pChildItem->setCheckState(column, state);
            }
        }

        // 记录状态
        if (checkBox != 0) {
            JCellWidgetData *checkBoxUserData = JCellWidgetData::userData(checkBox->userData(0));
            if (checkBoxUserData != 0) {
                checkBoxUserData->setData(state);
            }
        } else {
            pChildItem->setData(column, ItemCheckableState, state);
        }

        // 设置子节点状态
        if (pChildItem->childCount() > 0) {
            setChildrenState(pChildItem, column, state);
        }
    }
}

void JTreeWidget::setSiblingState(QTreeWidgetItem *item, int column, Qt::CheckState state)
{
    if (item == 0) {
        return;
    }

    //
    QTreeWidgetItem *parentItem = item->parent();
    if (parentItem == 0) {
        return;
    }

    //
    int count = parentItem->childCount();
    for (int i = 0; i < count; ++i) {
        QTreeWidgetItem *pSiblingItem = parentItem->child(i);
        if (pSiblingItem == 0) {
            continue;
        }

        //
        QCheckBox *checkBox = qobject_cast<QCheckBox *>(itemWidget(pSiblingItem, column));
        bool checkable = (checkBox != 0) ? true : (pSiblingItem->flags() & Qt::ItemIsUserCheckable);
        if (checkable) {
            continue;       // 当前节点未启用复选框，处理下一个节点
        }

        //
        if (partCheckedDisabled(pSiblingItem)) {
            if (checkBox != 0) {
                JCellWidgetData *checkBoxUserData = JCellWidgetData::userData(checkBox->userData(0));
                if (checkBoxUserData != 0) {
                    checkBoxUserData->setData(state);
                }
            } else {
                pSiblingItem->setData(column, ItemCheckableState, state);
            }
        }
    }
}

void JTreeWidget::setParentState(QTreeWidgetItem *item, int column, Qt::CheckState state)
{
    if (item == 0) {
        return;
    }

    //
    QTreeWidgetItem *parentItem = item->parent();
    if (parentItem == 0) {
        return;
    }

    // 半选中状态不再遍历，直接设置
    QCheckBox *checkBox = qobject_cast<QCheckBox *>(itemWidget(parentItem, column));
    bool checkable = (checkBox != 0) ? true : (parentItem->flags() & Qt::ItemIsUserCheckable);
    if (state == Qt::PartiallyChecked) {
        if (checkBox != 0) {
            if (checkable) {
                checkBox->setCheckState(state);
            }

            // 记录状态
            JCellWidgetData *checkBoxUserData = JCellWidgetData::userData(checkBox->userData(0));
            if (checkBoxUserData != 0) {
                checkBoxUserData->setData(state);
            }
        } else {
            if (checkable) {
                checkBox->setCheckState(state);
            }
            parentItem->setData(column, ItemCheckableState, state);
        }

        // 设置父节点状态
        setParentState(parentItem, column, state);

        return; //
    }

    // 其他选中状态
    int checkedCount = 0;
    int childCount = parentItem->childCount();
    Qt::CheckState parentState = Qt::Unchecked;

    //
    for (int i = 0; i < childCount; ++i) {
        QTreeWidgetItem *pSiblingItem = parentItem->child(i);
        if (pSiblingItem == 0) {
            continue;
        }

        //
        QCheckBox *pChildCheckBox = qobject_cast<QCheckBox *>(itemWidget(pSiblingItem, column));
        Qt::CheckState childState = Qt::Unchecked;

        // 区分复选框是否为自定义
        if (pChildCheckBox != 0) {
            //
            JCellWidgetData *checkBoxUserData = JCellWidgetData::userData(pChildCheckBox->userData(0));
            if (checkBoxUserData != 0) {
                childState = (Qt::CheckState)checkBoxUserData->data().toInt();
            }
        } else {
            childState = (Qt::CheckState)pSiblingItem->data(column, ItemCheckableState).toInt();
        }

        // 若为半选中状态，则直接设置父节点为半选中状态；否则记录状态
        switch (childState) {
        case Qt::Unchecked:
            break;
        case Qt::PartiallyChecked:  // 若为部分选择，则父节点也应为部分选择状态
            parentState = Qt::PartiallyChecked;
            break;
        case Qt::Checked:
            ++checkedCount;
            break;
        default:
            break;
        }

        // 出现半选中状态，直接中断
        if (parentState == Qt::PartiallyChecked) {
            break;
        }
    }

    // 确定节点状态
    if (parentState != Qt::PartiallyChecked) {
        if (checkedCount < childCount) {
            if (checkedCount > 0) { // 部分选中
                parentState = Qt::PartiallyChecked;
            }
        } else {    // 全部选中
            parentState = Qt::Checked;
        }
    }

    // 设置父节点状态
    if (checkBox != 0) {
        if (checkable) {
            checkBox->setCheckState(parentState);

            // 记录状态
            JCellWidgetData *checkBoxUserData = JCellWidgetData::userData(checkBox->userData(0));
            if (checkBoxUserData != 0) {
                checkBoxUserData->setData(parentState);
            }
        }
    } else {
        if (checkable) {
            parentItem->setCheckState(column, parentState);

            // 记录状态
            parentItem->setData(column, ItemCheckableState, parentState);
        }
    }

    // 向上递归
    setParentState(parentItem, column, parentState);
}

bool JTreeWidget::partCheckedDisabled(QTreeWidgetItem *item) const
{
    if (item == 0) {
        return true;
    }

    //
    QTreeWidgetItem *parentItem = item->parent();
    if (parentItem == 0) {
        return true;
    }

    // 遍历兄弟节点
    int count = parentItem->childCount();
    for (int i = 0; i < count; ++i) {
        QTreeWidgetItem *pSiblingItem = parentItem->child(i);
        if (pSiblingItem) {
            continue;
        }

        //
        QCheckBox *checkBox = qobject_cast<QCheckBox *>(itemWidget(pSiblingItem, 0));
        bool checkable = (checkBox != 0) ? true : (pSiblingItem->flags() & Qt::ItemIsUserCheckable);
        if (checkable) {
            return false;       // 只要有一个启用了复选框则结束循环
        }

        //
        if (pSiblingItem->childCount() > 0) {
            if (!partCheckedDisabled(pSiblingItem->child(0))) {
                return false;
            }
        }
    }

    return true;
}

void JTreeWidget::paintEvent(QPaintEvent *event)
{
#if 0
    QStyleOption opt;
    opt.init(this);
    QPainter painter(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &painter, this);
#endif
    QTreeWidget::paintEvent(event);
}

void JTreeWidget::mouseMoveEvent(QMouseEvent *event)
{
    if (event->buttons() & Qt::LeftButton) {
        int dragDistance = (event->pos() - q_startDragPoint).manhattanLength();
        if (dragDistance > QApplication::startDragDistance()) {
            performDrag();
        }
    }

    QTreeWidget::mouseMoveEvent(event);
}

void JTreeWidget::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        q_isDrag = true;
        q_startDragPoint = event->pos();
    }

    QTreeWidget::mousePressEvent(event);
}

void JTreeWidget::mouseReleaseEvent(QMouseEvent *event)
{
    q_isDrag = false;

    QTreeWidget::mouseReleaseEvent(event);
}

void JTreeWidget::dragEnterEvent(QDragEnterEvent *event)
{
    QWidget *srcWidget = qobject_cast<JTreeWidget *>(event->source());
    if (srcWidget != 0) {
        event->setDropAction(Qt::CopyAction);
        event->accept();
    }
}

void JTreeWidget::dragMoveEvent(QDragMoveEvent *event)
{
    QWidget *srcWidget = qobject_cast<JTreeWidget *>(event->source());
    if (srcWidget != 0) {
        event->setDropAction(Qt::CopyAction);
        event->accept();
    }
}

void JTreeWidget::dropEvent(QDropEvent *event)
{
    QWidget *srcWidget = qobject_cast<JTreeWidget *>(event->source());
    if (srcWidget != 0) {
        event->setDropAction(Qt::CopyAction);
        event->accept();
    }

    q_isDrag = false;
}

void JTreeWidget::performDrag()
{
    if (!q_isDrag) {
        return;
    }

    //
    QTreeWidgetItem *item = currentItem();
    if (item == 0) {
        return;
    }

    //
    JTreeItemData *pItemData = currentTreeItemData();
    if (pItemData == 0
            || !pItemData->bDrag) { // 未启用拖拽
        return;
    }

    JTreeItemMimeData *pMimeData = new JTreeItemMimeData;
    pMimeData->setDragData("ItemMimeData", item);
    QDrag *drag = new QDrag(this);
    drag->setMimeData(pMimeData);
    drag->setPixmap(QPixmap(":/global/image/root.png"));
    drag->exec(Qt::CopyAction);
}

void JTreeWidget::setItemState(QTreeWidgetItem *item, Qt::CheckState state)
{
    QTreeWidgetItem *pItem = (item == 0) ? topLevelItem(0) : item;
    if (pItem == 0) {
        return;
    }

    //
    int count = pItem->columnCount();
    for (int i = 0; i < count; ++i) {
        setItemState(pItem, i, state);
    }

    //
    count = pItem->childCount();
    for (int i = 0; i < count; ++i) {
        QTreeWidgetItem *pChildItem = pItem->child(i);
        if (pChildItem == 0) {
            continue;
        }

        //
        setItemState(pChildItem, state);
    }
}

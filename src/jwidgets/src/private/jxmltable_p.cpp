#include "precomp.h"
#include "jxmltable_p.h"
#include "../jxmltable.h"
#include "jxmltable_widget.h"

// - class JXmlTablePrivate -

JXmlTablePrivate::JXmlTablePrivate(JXmlTable *parent)
    : QObject(parent)
    , q_ptr(parent)
{

}

void JXmlTablePrivate::setConfig(const QString &filePath)
{
    data.clear();
    data.filePath = filePath;
}

bool JXmlTablePrivate::loadConfig(const QString &filePath, const QString &tableName)
{
    data.clear();

    QDomElement emTable = elementTable(filePath, tableName);
    if (emTable.isNull()) {
        return false;
    }

    data.filePath = filePath;

    return loadConfig(emTable);
}

bool JXmlTablePrivate::loadConfig(const QByteArray &text)
{
    data.clear(false);

    if (data.tableName.isEmpty()) {
        return false;
    }

    if (text.isEmpty()) {
        return false;
    }

    QString errorMsg;
    int errorLine;
    int errorColumn;
    if (!data.document.setContent(text, &errorMsg, &errorLine, &errorColumn)) {
        QMessageBox::critical(0, QObject::tr("Parse xmltable data"),
                              QObject::tr("parse data failure! \n\nerrorMsg: %1\nerrorPosition: (%2, %3))")
                              .arg(errorMsg).arg(errorLine).arg(errorColumn));
        return false;
    }

    if (data.document.isNull()) {
        return false;
    }

    QDomElement emRoot = data.document.firstChildElement();
    if (emRoot.isNull()) {
        return false;
    }

    for (QDomElement emTable = emRoot.firstChildElement("table");
         !emTable.isNull();
         emTable = emTable.nextSiblingElement("table")) {
        if (emTable.attribute("name") == data.tableName) {
            return loadConfig(emTable);
        }
    }

    return false;
}

bool JXmlTablePrivate::loadConfig(const QDomElement &emTable)
{
    data.clear();

    if (emTable.isNull()) {
        return false;
    }

    QString tableName = emTable.attribute("name");
    if (tableName.isEmpty()) {
        QMessageBox::critical(0, QObject::tr("Parse xmltable"),
                              QObject::tr("name of table cannot be empty!"));
        return false;
    }

    data.tableName = tableName;

    // orientations
    if (emTable.hasAttribute("orientations")) {
        QStringList sOrientations = emTable.attribute("orientations")
                .trimmed().split('|', QString::SkipEmptyParts);
        data.orientations = (Qt::Orientations)0;
        QStringListIterator citer(sOrientations);
        while (citer.hasNext()) {
            const QString &sOrientation = citer.next();
            if (sOrientation == "horizontal") {
                data.orientations |= Qt::Horizontal;
            } else if (sOrientation == "vertical") {
                data.orientations |= Qt::Vertical;
            }
        }
    }
    // headerWidth
    if (emTable.hasAttribute("headerWidth")) {
        data.headerSize.setWidth(emTable.attribute("headerWidth").toInt());
    }
    // headerHeight
    if (emTable.hasAttribute("headerHeight")) {
        data.headerSize.setHeight(emTable.attribute("headerHeight").toInt());
    }
    // styleSheet
    if (emTable.hasAttribute("styleSheet")) {
        data.styleSheet = emTable.attribute("styleSheet");
    }
    // horiHeaderMovable
    if (emTable.hasAttribute("horiHeaderMovable")) {
        data.horiHeaderMovable = QVariant(emTable.attribute("horiHeaderMovable")).toBool();
    }
    // verticalHeaderVisible
    if (emTable.hasAttribute("verticalHeaderVisible")) {
        data.verticalHeaderVisible = QVariant(emTable.attribute("verticalHeaderVisible")).toBool();
    }
    // verticalHeaderLabel
    if (emTable.hasAttribute("verticalHeaderLabel")) {
        data.verticalHeaderLabel = QVariant(emTable.attribute("verticalHeaderLabel")).toBool();
    }
    // offsetEnabled
    if (emTable.hasAttribute("offsetEnabled")) {
        data.offsetEnabled = QVariant(emTable.attribute("offsetEnabled")).toBool();
    }
    // readOnly
    if (emTable.hasAttribute("readOnly")) {
        data.readOnly = QVariant(emTable.attribute("readOnly")).toBool();
    }
    // sync
    if (emTable.hasAttribute("sync")) {
        data.sync = QVariant(emTable.attribute("sync")).toBool();
    }
    // parse table
    if (!parse(emTable)) {
        return false;
    }

    return true;
}

bool JXmlTablePrivate::updateHeaders()
{
    Q_Q(JXmlTable);

    // window title
    q->setWindowTitle(data.tableName);

    // clear table
    q->clear();

    //
    disconnectSignals(Qt::Horizontal | Qt::Vertical);

    //
    if (data.headerSize.width() > 0) {
        q->horizontalHeader()->setDefaultSectionSize(data.headerSize.width());
    }
    if (data.headerSize.height() > 0) {
        q->verticalHeader()->setDefaultSectionSize(data.headerSize.height());
    }

    //
    q->setVerticalHeaderVisible(data.verticalHeaderVisible);

    if (!data.items.isEmpty()) {
        switch (data.orientations) {
        case Qt::Horizontal:
        {
            QMap<int, JValueBase *> &columnDatas = data.items.begin().value();
            if (columnDatas.isEmpty()) {
                break;
            }
            int columnCount = columnDatas.keys().last() + 1;
            int headerWidth = data.headerSize.width();
            q->setColumnCount(columnCount);
            QVector<QString> horiLabels(columnCount);
            QMapIterator<int, JValueBase *> iterColumn(columnDatas);
            while (iterColumn.hasNext()) {
                iterColumn.next();
                const JValueBase *valueBase = iterColumn.value();
                horiLabels[valueBase->index()] = valueBase->name();
                if (valueBase->headerSize() > 0) {
                    q->setColumnWidth(valueBase->index(), valueBase->headerSize());
                } else if (headerWidth > 0) {
                    q->setColumnWidth(valueBase->index(), headerWidth);
                }
                // section - move
                if (valueBase->offset() >= 0
                        && valueBase->offset() < columnCount
                        && valueBase->offset() != valueBase->index()) {
                    q->horizontalHeader()->moveSection(q->horizontalHeader()->visualIndex(valueBase->index()),
                                                       valueBase->offset());
                }
                // filter
                if (valueBase->filterType() != InvalidValue) {
                    Q_EMIT q->filterChanged(valueBase->index(), valueBase->filterType(), true);
                }
            }
            q->setHorizontalHeaderLabels(horiLabels.toList());
#if QT_VERSION >= 0x050000
            q->horizontalHeader()->setSectionsMovable(data.horiHeaderMovable);
#else
            q->horizontalHeader()->setMovable(data.horiHeaderMovable);
#endif
            //
            connectSignals(Qt::Horizontal);
            break;
        }
        case Qt::Vertical:
        {
            const QMap<int, JValueBase *> &rowDatas = data.items.begin().value();
            int rowCount = rowDatas.keys().last() + 1;
            int headerHeight = data.headerSize.height();
            q->setRowCount(rowCount);
            QVector<QString> vertLabels(rowCount);
            QMapIterator<int, JValueBase *> iter(rowDatas);
            while (iter.hasNext()) {
                iter.next();
                const JValueBase *valueBase = iter.value();
                vertLabels[valueBase->index()] = valueBase->name();
                if (valueBase->headerSize() > 0) {
                    q->setRowHeight(valueBase->index(), valueBase->headerSize());
                } else if (headerHeight > 0) {
                    q->setRowHeight(valueBase->index(), headerHeight);
                }
                // section - move
                if (valueBase->offset() >= 0
                        && valueBase->offset() < rowCount
                        && valueBase->offset() != valueBase->index()) {
                    q->verticalHeader()->moveSection(q->verticalHeader()->visualIndex(valueBase->index()),
                                                     valueBase->offset());
                }
            }
            q->setVerticalHeaderLabels(vertLabels.toList());
            if (data.headerSize.width() > 0) {
                q->verticalHeader()->setFixedWidth(data.headerSize.width());
            }
#if QT_VERSION >= 0x050000
            q->horizontalHeader()->setSectionsMovable(data.horiHeaderMovable);
#else
            q->horizontalHeader()->setMovable(data.horiHeaderMovable);
#endif
            //
            connectSignals(Qt::Vertical);
            break;
        }
        case JCrossOrientation:
        {
            int rowCount = 0;
            int columnCount = 0;
            // calculate rowCount and columnCount
            QMapIterator<JValueBase *, QMap<int, JValueBase *> > iterRow(data.items);
            while (iterRow.hasNext()) {
                iterRow.next();
                rowCount = qMax(rowCount, iterRow.key()->index());
                const QMap<int, JValueBase *> &columnDatas = iterRow.value();
                if (!columnDatas.isEmpty()) {
                    columnCount = qMax(columnCount, columnDatas.keys().last() + 1);
                }
            }
            rowCount = rowCount + 1;
            q->setRowCount(rowCount);
            q->setColumnCount(columnCount);
            //
            QVector<QString> horiLabels(columnCount);
            QVector<QString> vertLabels(rowCount);
            //
            const QSize &headerSize = data.headerSize;
            iterRow.toFront();
            while (iterRow.hasNext()) {
                iterRow.next();
                const JValueBase *rowValue = iterRow.key();
                if (data.verticalHeaderLabel) {
                    vertLabels[rowValue->index()] = rowValue->name();
                }
                if (rowValue->headerSize() > 0) {
                    q->setRowHeight(rowValue->index(), rowValue->headerSize());
                } else if (headerSize.height() > 0) {
                    q->setRowHeight(rowValue->index(), headerSize.height());
                }
                //
                QMapIterator<int, JValueBase *> iterColumn(iterRow.value());
                while (iterColumn.hasNext()) {
                    iterColumn.next();
                    const JItemValue *columnValue = static_cast<JItemValue *>(iterColumn.value());
                    const int column = iterColumn.key();
                    horiLabels[column] = columnValue->name();
                    if (columnValue->headerSize() > 0) {
                        q->setColumnWidth(column, columnValue->headerSize());
                    } else if (headerSize.width() > 0) {
                        q->setColumnWidth(column, headerSize.width());
                    }
                    //
                    if (!columnValue->enabled() || !columnValue->editable()) {
                        switch (columnValue->type()) {
                        case StringValue:
                        {
                            const JStringValue *stringValue = static_cast<const JStringValue *>(columnValue);
                            if (stringValue->containCSS()) {
                                q->setCellWidget(rowValue->index(), column,
                                                 new JStringLabel(const_cast<JStringValue *>(stringValue), q));
                            } else {
                                q->setItem(rowValue->index(), column,
                                           new JStringItem(const_cast<JItemValue *>(columnValue)));
                            }
                            break;
                        }
                        default:
                            q->setItem(rowValue->index(), column,
                                       new JStringItem(const_cast<JItemValue *>(columnValue)));
                            break;
                        }
                    }
                }
            }
            //
            q->setHorizontalHeaderLabels(horiLabels.toList());
            if (data.verticalHeaderLabel) {
                q->setVerticalHeaderLabels(vertLabels.toList());
            }
#if QT_VERSION >= 0x050000
            q->horizontalHeader()->setSectionsMovable(data.horiHeaderMovable);
#else
            q->horizontalHeader()->setMovable(data.horiHeaderMovable);
#endif
            //
            connectSignals(Qt::Horizontal | Qt::Vertical);
            break;
        }
        default:
            break;
        }
    }

    Q_EMIT q->tableChanged(data.tableName);

    return true;
}

QDomElement JXmlTablePrivate::elementTable()
{
    return elementTable(data.filePath, data.tableName);
}

QDomElement JXmlTablePrivate::elementTable(const QString &filePath, const QString &tableName)
{
    Q_Q(JXmlTable);
    if (filePath.isEmpty()) {
        QMessageBox::critical(q, QObject::tr("check parameters"),
                              QObject::tr("filePath cannot be empty!"));
        return QDomElement();
    }

    if (tableName.isEmpty()) {
        QMessageBox::critical(q, QObject::tr("check parameters"),
                              QObject::tr("tableName cannot be empty!"));
        return QDomElement();
    }

    QFile file(filePath);
    if (!file.open(QFile::ReadOnly)) {
        QMessageBox::critical(q, QObject::tr("open buffer file"),
                              QObject::tr("open failure!"));
        return QDomElement();
    }

    data.document.setContent(&file);
    if (data.document.isNull()) {
        return QDomElement();
    }

    QDomElement emRoot = data.document.firstChildElement();
    if (emRoot.isNull()) {
        QMessageBox::critical(q, QObject::tr("Parse xmltable"),
                              QObject::tr("root node is not exists in file %1!").arg(filePath));
        return QDomElement();
    }

    // version
#if 0
    QString ver = emRoot->attribute("version");
    if (ver != version()) {
        QMessageBox::critical(0, QObject::tr("Parse xmltable"),
                              QObject::tr("the version of '%1' is invalid for %2!")
                              .arg(ver).arg(filePath));
        return QDomElement();
    }
#endif
    QDomElement emTable = emRoot.firstChildElement("table");
    if (emTable.isNull()) {
        QMessageBox::critical(q, QObject::tr("Parse xmltable"),
                              QObject::tr("table node is not exists int file %1!").arg(filePath));
        return QDomElement();
    } else {
        for (; !emTable.isNull();
             emTable = emTable.nextSiblingElement("table")) {
            QString name = emTable.attribute("name");
            if (name == tableName) {
                break;
            }
        }

        if (emTable.isNull()) {
            QMessageBox::critical(q, QObject::tr("Parse xmltable"),
                                  QObject::tr("table node %1 is not found in file %2!")
                                  .arg(tableName).arg(filePath));
            return QDomElement();
        }
    }

    return emTable;
}

QDomElement JXmlTablePrivate::elementItem(int row, int column)
{
    return elementItem(elementTable(), row, column);
}

QDomElement JXmlTablePrivate::elementItem(QDomElement emTable, int row, int column)
{
    if (emTable.isNull()) {
        return QDomElement();
    }

    JItemValue *itemValue = itemData(row, column);
    if (!itemValue) {
        return QDomElement();
    }
    return elementItem(emTable, itemValue->name());
}

QDomElement JXmlTablePrivate::elementItem(const QString &name)
{
    return elementItem(elementTable(), name);
}

QDomElement JXmlTablePrivate::elementItem(QDomElement emTable, const QString &name)
{
    if (emTable.isElement()) {
        return QDomElement();
    }

    for (QDomElement emItem = emTable.firstChildElement("item");
         !emItem.isNull();
         emItem = emItem.nextSiblingElement("item")) {
        QString _name = emItem.attribute("name");
        if (_name == name) {
            return emItem;
        }
    }

    return QDomElement();
}

QDomElement JXmlTablePrivate::elementRow(int row)
{
    QDomElement emTable = elementTable();
    if (emTable.isNull()) {
        return QDomElement();
    }

    QDomElement emRow = emTable.firstChildElement("row");
    while (!emRow.isNull() && -- row >= 0) {
        emRow = emRow.nextSiblingElement("row");
    }

    return emRow;
}

QDomElement JXmlTablePrivate::elementRow(QDomElement emTable, int row)
{
    if (emTable.isNull()) {
        return QDomElement();
    }

    JItemValue *itemValue = itemData(row, 0);
    if (!itemValue) {
        return QDomElement();
    }

    return elementRow(emTable, itemValue->name());
}

QDomElement JXmlTablePrivate::elementRow(QDomElement emTable, const QString &name)
{
    if (emTable.isNull()) {
        return QDomElement();
    }

    for (QDomElement emRow = emTable.firstChildElement("row");
         !emRow.isNull();
         emRow = emRow.nextSiblingElement("row")) {
        QString _name = emRow.attribute("name");
        if (_name == name) {
            return emRow;
        }
    }

    return QDomElement();
}

QDomElement JXmlTablePrivate::elementColumn(int column)
{
    JItemValue *itemValue = itemData(0, column);
    if (!itemValue) {
        return QDomElement();
    }

    return elementColumn(elementTable(), itemValue->name());
}

QDomElement JXmlTablePrivate::elementColumn(QDomElement emTable, int column)
{
    if (emTable.isNull()) {
        return QDomElement();
    }

    JItemValue *itemValue = itemData(0, column);
    if (!itemValue) {
        return QDomElement();
    }

    return elementColumn(emTable, column);
}

QDomElement JXmlTablePrivate::elementColumn(QDomElement emTable, const QString &name)
{
    if (emTable.isNull()) {
        return QDomElement();
    }

    QDomElement emRow = emTable.firstChildElement("row");
    if (emRow.isNull()) {
        return QDomElement();
    }

    for (QDomElement emColumn = emRow.firstChildElement("column");
         !emColumn.isNull();
         emColumn = emColumn.nextSiblingElement("column")) {
        QString _name = emColumn.attribute("column");
        if (_name == name) {
            return emColumn;
        }
    }

    return QDomElement();
}

void JXmlTablePrivate::saveHeaderSize(int logicalIndex, int newSize, Qt::Orientation orientation)
{
    QDomElement emItem;

    switch (orientation) {
    case Qt::Horizontal:
        if (data.orientations == (Qt::Horizontal | Qt::Vertical)) {
            emItem = elementColumn(logicalIndex);
        } else {
            emItem = elementItem(0, logicalIndex);
        }
        break;
    case Qt::Vertical:
        if (data.orientations == (Qt::Horizontal | Qt::Vertical)) {
            emItem = elementRow(logicalIndex);
        } else {
            emItem = elementItem(logicalIndex, 0);
        }
        break;
    default:
        return;
    }

    if (!emItem.isNull()) {
        emItem.setAttribute("headerSize", newSize);
        QFile out(data.filePath);
        if (!out.open(QFile::WriteOnly | QFile::Text)) {
            return;
        }
        QTextStream textStream(&out);
        data.document.save(textStream, 2);
    }
}

void JXmlTablePrivate::saveSection(int logicalIndex, int oldVisualIndex, int newVisualIndex)
{
    Q_UNUSED(logicalIndex);
    Q_UNUSED(oldVisualIndex);
    Q_UNUSED(newVisualIndex);

    QDomElement emTable = elementTable();
    if (emTable.isNull()) {
        return;
    }

    Q_Q(JXmlTable);

    QHeaderView *headerView = 0;

    switch (data.orientations) {
    case Qt::Horizontal:
        headerView = q->horizontalHeader();
        break;
    case Qt::Vertical:
        headerView = q->verticalHeader();
        break;
    default:
        return;
    }

    int index = 0;
    for (QDomElement emItem = emTable.firstChildElement("item");
         !emItem.isNull();
         emItem = emItem.nextSiblingElement("item")) {
        emItem.setAttribute("offset", headerView->visualIndex(index++));
    }

    QFile out(data.filePath);
    if (!out.open(QFile::WriteOnly | QFile::Text)) {
        return;
    }
    QTextStream textStream(&out);
    data.document.save(textStream, 2);
}

bool JXmlTablePrivate::parse(const QDomElement &emTable)
{
    if (emTable.isNull()) {
        return false;
    }

    switch (data.orientations) {
    case Qt::Horizontal:
    case Qt::Vertical:
        for (QDomElement emItem = emTable.firstChildElement("item");
             !emItem.isNull();
             emItem = emItem.nextSiblingElement("item")) {
            if (!emItem.hasAttribute("type")) {
                continue;
            }
            JItemValue *itemValue = createItem(emItem, emItem.attribute("type").toStdString());
            if (itemValue) {
                appendItem(itemValue, 0);
            }
        }
        break;
    case JCrossOrientation:
    {
        // rows - columns
        for (QDomElement emRow = emTable.firstChildElement("row");
             !emRow.isNull();
             emRow = emRow.nextSiblingElement("row")) {
            JValueBase *rowValue = new JValueBase(this);
            if (!parse(emRow, *rowValue)) {
                delete rowValue;
                continue;
            }
            // update index
            if (data.items.isEmpty()) {
                if (rowValue->index() == -1) {
                    rowValue->setIndex(0);
                }
            } else {
                int maxIndex = 0;
                JValueBase *oldValue = data.rowKey(rowValue->index(), maxIndex);
                if (!oldValue) {    // not exists
                    if (rowValue->index() == -1) {
                        rowValue->setIndex(maxIndex + 1);
                    }
                } else {
                    data.items.remove(oldValue);
                    delete oldValue;
                }
            }
            // update offset
            if (true) {
                //
            }
            //
            data.items[rowValue] = QMap<int, JValueBase *>();
            for (QDomElement emColumn = emRow.firstChildElement("column");
                 !emColumn.isNull();
                 emColumn = emColumn.nextSiblingElement("column")) {
                if (!emColumn.hasAttribute("type")) {
                    continue;
                }
                JItemValue *columnValue = createItem(emColumn, emColumn.attribute("type").toStdString());
                if (columnValue) {
                    appendItem(columnValue, rowValue);
                }
            }
        }
        break;
    }
    default:
        break;
    }

    return true;
}

bool JXmlTablePrivate::parse(const QDomElement &emItem, JValueBase &value)
{
    if (emItem.isNull()) {
        return false;
    }

    // index
    if (emItem.hasAttribute("index")) {
        value.setIndex(qBound(0, emItem.attribute("index").toInt(), 100000));
    }
    // offset
    if (emItem.hasAttribute("offset")) {
        value.setOffset(qMax(0, emItem.attribute("offset").toInt()));
    }
    // headerSize
    if (emItem.hasAttribute("headerSize")) {
        value.setHeaderSize(qBound(25, emItem.attribute("headerSize").toInt(), 2000));
    }
    // name
    if (emItem.hasAttribute("name")) {
        value.setName(emItem.attribute("name"));
    }
    // filterType
    if (emItem.hasAttribute("filterType")) {
        value.setFilterType(jvaluetype_type_of_string(emItem.attribute("filterType").toStdString()));
    }

    return true;
}

bool JXmlTablePrivate::parse(const QDomElement &emItem, JItemValue &value)
{
    if (!parse(emItem, static_cast<JValueBase &>(value))) {
        return false;
    }

    // enabled
    if (emItem.hasAttribute("enabled")) {
        value.setEnabled(QVariant(emItem.attribute("enabled")).toBool());
    }
    // editable
    if (emItem.hasAttribute("editable")) {
        value.setEditable(QVariant(emItem.attribute("editable")).toBool());
    }
    // alignment
    if (emItem.hasAttribute("alignment")) {
        QStringList sAlignments = emItem.attribute("alignment")
                .trimmed().split('|', QString::SkipEmptyParts);
        Qt::Alignment alignment = (Qt::Alignment)0;
        QStringListIterator citer(sAlignments);
        while (citer.hasNext()) {
            const QString &sAlignment = citer.next();
            if (sAlignment == "left" || sAlignment == "leading") {
                alignment |= Qt::AlignLeft;
            } else if (sAlignment == "top") {
                alignment |= Qt::AlignTop;
            } else if (sAlignment == "right" || sAlignment == "trailing") {
                alignment |= Qt::AlignRight;
            } else if (sAlignment == "bototm") {
                alignment |= Qt::AlignBottom;
            } else if (sAlignment == "hCenter") {
                alignment |= Qt::AlignHCenter;
            } else if (sAlignment == "vCenter") {
                alignment |= Qt::AlignVCenter;
            }
        }
        value.setAlignment(alignment);
    }
    // styleSheet
    if (emItem.hasAttribute("styleSheet")) {
        value.setStyleSheet(emItem.attribute("styleSheet"));
    }
    // icon
    if (emItem.hasAttribute("icon")) {
        value.setIcon(emItem.attribute("icon"));
    }
    // tooltip
    if (emItem.hasAttribute("tooltip")) {
        value.setTooltip(emItem.attribute("tooltip"));
    }
    // placeHoldText
    if (emItem.hasAttribute("placeHoldText")) {
        value.setPlaceHoldText(emItem.attribute("placeHoldText"));
    }
    // whatsThis
    if (emItem.hasAttribute("whatsThis")) {
        value.setWhatsThis(emItem.attribute("whatsThis"));
    }

    return true;
}

bool JXmlTablePrivate::parse(const QDomElement &emItem, JBoolValue &value)
{
    if (!parse(emItem, static_cast<JItemValue &>(value))) {
        return false;
    }

    // original
    if (emItem.hasAttribute("default")) {
        QString text = emItem.attribute("default");
        if (text == "unchecked") {
            value.setOriginal(Qt::Unchecked);
        } else if (text == "partially") {
            value.setOriginal(Qt::PartiallyChecked);
        } else if (text == "checked") {
            value.setOriginal(Qt::Checked);
        }
    }
    // checkable
    if (emItem.hasAttribute("checkable")) {
        value.setCheckable(QVariant(emItem.attribute("checkable")).toBool());
    }
    // text
    if (emItem.hasAttribute("text")) {
        value.setText(emItem.attribute("text"));
    }

    return true;
}

bool JXmlTablePrivate::parse(const QDomElement &emItem, JNumericValue &value)
{
    if (!parse(emItem, static_cast<JItemValue &>(value))) {
        return false;
    }

    Q_Q(JXmlTable);

    // prefix
    if (emItem.hasAttribute("prefix")) {
        value.setPrefix(emItem.attribute("prefix"));
    }
    // suffix
    if (emItem.hasAttribute("suffix")) {
        value.setSuffix(emItem.attribute("suffix"));
    }
    // original
    if (value.suffix().isEmpty()) {     // is an ascii
        if (emItem.hasAttribute("default")) {
            QString text = emItem.attribute("default");
            if (text.isEmpty()) {
                value.setOriginal('\0');
            } else {
                value.setOriginal(text.at(0).toLatin1());
            }
        }
    } else {
        if (emItem.hasAttribute("default")) {
            value.setOriginal(emItem.attribute("default"));
        }
    }
    // radix
    if (emItem.hasAttribute("radix")) {
        int radix = emItem.attribute("radix").toInt();
        if (radix < 2 || radix > 36) {
            QMessageBox::critical(q, QObject::tr("Parse xmltable"),
                                  QObject::tr("parse radix failure (out of range [2, 36] '%1' in '%2/item/radix'")
                                  .arg(emItem.attribute("radix"))
                                  .arg(nodePath(emItem)));
            radix = qBound(2, radix, 36);
        }
        value.setRadix(radix);
    }
    // maskCount
    if (emItem.hasAttribute("maskCount")) {
        int maskCount = emItem.attribute("maskCount").toInt();
        if (maskCount < 1 || maskCount > 8) {
            QMessageBox::critical(q, QObject::tr("Parse xmltable"),
                                  QObject::tr("parse maskCount failure (out of range [1, 8] '%1' in '%2/item/radix'")
                                  .arg(emItem.attribute("maskCount"))
                                  .arg(nodePath(emItem)));
            maskCount = qBound(1, maskCount, 8);
        }
        value.setMaskCount(maskCount);
    } else if (value.radix() != 10) {
        value.setMaskCount(8);
    }
    // fillChar
    if (emItem.hasAttribute("fillChar")) {
        QString text = emItem.attribute("fillChar");
        if (text.isEmpty()) {
            value.setFillChar(QChar());
        } else {
            value.setFillChar(emItem.attribute("fillChar").at(0));
        }
    } else if (value.radix() != 10) {
        value.setFillChar(QChar('0'));
    }
    // decimals
    if (emItem.hasAttribute("decimals")) {
        value.setDecimals(emItem.attribute("decimals").toInt());
    }
    // step
    if (emItem.hasAttribute("step")) {
        value.setStep(emItem.attribute("step").toDouble());
    } else {
        value.setStep(qPow(0.1, value.decimals()));
    }
    if (value.step() == 0.) {
        value.setStep(1.);
    }
    // scale
    if (emItem.hasAttribute("scale")) {
        value.setScale(emItem.attribute("scale").toDouble());
    } else {
        value.setScale(value.step());
    }
    if (value.scale() == 0.) {
        value.setScale(1.);
    }
    // range
    if (emItem.hasAttribute("range")) {
        QString range = emItem.attribute("range");
        if (!value.range()->parse(range)) {
            QMessageBox::critical(q, QObject::tr("Parse xmltable"),
                                  QObject::tr("parse range failure ('%1' in '%2/item/range')")
                                  .arg(range).arg(nodePath(emItem)));
            return false;
        }
        // bound original
        if (!value.suffix().isEmpty()) {    //
            qreal original = value.original().toDouble();
            if (!value.inRange(original)) {
                QMessageBox::critical(q, QObject::tr("Parse xmltable"),
                                      QObject::tr("default value '%1' out of range (range: %2) in '%3/item/range'")
                                      .arg(original).arg(range).arg(nodePath(emItem)));
                return false;
            }
            value.setOriginal(original);
        }
        // without
        for (QDomElement emWithout = emItem.firstChildElement("without");
             !emWithout.isNull();
             emWithout = emWithout.nextSiblingElement("without")) {
            JNumericRange *rangeValue = new JNumericRange(&value);
            if (!rangeValue->parse(emWithout.text())) {
                QMessageBox::critical(q, QObject::tr("Parse xmltable"),
                                      QObject::tr("parse without section '%1' failiure in '%2/item/range'")
                                      .arg(emWithout.text()).arg(nodePath(emItem)));
                delete rangeValue;
                return false;
            }
            value.withouts().append(rangeValue);
        }
    } else {
        if (value.radix() == 10) {
            value.range()->setMinimum(0.);
            value.range()->setMaximum(100.);
        } else {
            value.range()->setMinimum(0.);
            value.range()->setMaximum((0x1LL << (value.maskCount() * 4)) - 1);
        }
    }
    // size
    if (emItem.hasAttribute("size")) {
        value.setSize(qBound(0, emItem.attribute("size").toInt(), 8));
    } else {
        quint64 delta = quint64(qMax(qFabs(value.range()->minimum()),
                                     qFabs(value.range()->maximum()))
                                / value.scale());
        if (delta <= 0xffULL) {
            value.setSize(1);
        } else if (delta <= 0xffffULL) {
            value.setSize(2);
        } else if (delta <= 0xfffffffULL) {
            value.setSize(4);
        } else if (delta <= 0xffffffffffffffffULL) {
            value.setSize(8);
        } else {
            value.setSize(4);   // default size: 4 bytes
        }
    }

    return true;
}

bool JXmlTablePrivate::parse(const QDomElement &emItem, JStringValue &value)
{
    if (!parse(emItem, static_cast<JItemValue &>(value))) {
        return false;
    }

    // original
    if (emItem.hasAttribute("default")) {
        value.setOriginal(emItem.attribute("default"));
    }
    // upper
    if (emItem.hasAttribute("upper")) {
        value.setUpper(QVariant(emItem.attribute("upper")).toBool());
    }
    // multiline
    if (emItem.hasAttribute("multiline")) {
        value.setMultiline(QVariant(emItem.attribute("multiline")).toBool());
    }
    // containCSS
    if (emItem.hasAttribute("containCSS")) {
        value.setContainCSS(QVariant(emItem.attribute("containCSS")).toBool());
    }
    // maxLength
    if (emItem.hasAttribute("maxLength")) {
        value.setMaxLength(QVariant(emItem.attribute("maxLength")).toInt());
    }
    // pattern
    if (emItem.hasAttribute("pattern")) {
        value.setPattern(emItem.attribute("pattern"));
    }

    return true;
}

bool JXmlTablePrivate::parse(const QDomElement &emItem, JEnumValue &value)
{
    if (!parse(emItem, static_cast<JItemValue &>(value))) {
        return false;
    }

    // original
    if (emItem.hasAttribute("default")) {
        value.setOriginal(emItem.attribute("default"));
    }
    // indexOffset
    if (emItem.hasAttribute("indexOffset")) {
        value.setIndexOffset(emItem.attribute("indexOffset").toInt());
    }
    // readOnly
    if (emItem.hasAttribute("readOnly")) {
        value.setReadOnly(QVariant(emItem.attribute("readOnly")).toBool());
    }
    // autoCompletion
    if (emItem.hasAttribute("autoCompletion")) {
        value.setAutoCompletion(QVariant(emItem.attribute("autoCompletion")).toBool());
    }
    // duplicatesEnabled
    if (emItem.hasAttribute("duplicatesEnabled")) {
        value.setDuplicatesEnabled(QVariant(emItem.attribute("duplicatesEnabled")).toBool());
    }
    // multiable
    if (emItem.hasAttribute("multiable")) {
        value.setMultiable(QVariant(emItem.attribute("multiable")).toBool());
    }
    // options
    for (QDomElement emOption = emItem.firstChildElement("option");
         !emOption.isNull();
         emOption = emOption.nextSiblingElement("option")) {
        JEnumOption *enumOption = new JEnumOption(&value);
        enumOption->setText(emOption.text());
        if (emOption.hasAttribute("encode")) {
            QString encode = emOption.attribute("encode");
            int base = 10;
            if (encode.startsWith("0x")) {
                base = 16;
            } else if (encode.startsWith("0o")) {
                base = 8;
            } else if (encode.startsWith("0b")) {
                base = 2;
            }
            enumOption->setEncode(encode.toULongLong(0, base));
        } else {
            if (value.options().isEmpty()) {
                enumOption->setEncode(value.indexOffset());
            } else {
                enumOption->setEncode(value.options().last()->encode() + 1);
            }
        }
        value.options().append(enumOption);
    }
    // size
    if (emItem.hasAttribute("size")) {
        value.setSize(qMin(4, emItem.attribute("size").toInt()));
    } else {
        value.setSize(4);   // default size: 4 bytes
    }

    return true;
}

bool JXmlTablePrivate::parse(const QDomElement &emItem, JDateTimeValue &value)
{
    if (!parse(emItem, static_cast<JItemValue &>(value))) {
        return false;
    }

    // format
    if (emItem.hasAttribute("format")) {
        value.setFormat(emItem.attribute("format"));
    }
    // original
    if (emItem.hasAttribute("default")) {
        value.setOriginal(QDateTime::fromString(emItem.attribute("default"), value.format()));
    }
    // range
    if (emItem.hasAttribute("range")) {
        QString range = emItem.attribute("range");
        //
        if (!value.range()->parse(range, value.format())) {
            qCritical() << QString("parse range failure (range: %2)").arg(range);
            return false;
        }
        //
        QDateTime original = value.original().toDateTime();
        if (!value.includes(original)) {
            qCritical() << QString("default value '%1' (range: %2) is invalid!")
                           .arg(value.toString(value.original()))
                           .arg(value.range()->toString(value.format()));
            return false;
        }
        value.setOriginal(original);
        // whitout
        for (QDomElement emWithout = emItem.firstChildElement("without");
             !emWithout.isNull();
             emWithout = emWithout.nextSiblingElement("without")) {
            JDateTimeRange *rangeValue = new JDateTimeRange(&value);
            if (!rangeValue->parse(emWithout.text(), value.format())) {
                qCritical() << QString("parse range '%1' failed")
                               .arg(emWithout.text());
                delete rangeValue;
                return false;
            }
            value.withouts().append(rangeValue);
        }

    }
    // original (auto-adjust)
    QDateTime tTemp = value.original().toDateTime();
    if (!value.includes(tTemp, true)) {
        value.setOriginal(tTemp);
    }

    return true;
}

bool JXmlTablePrivate::parse(const QDomElement &emItem, JIPv4Value &value)
{
    if (!parse(emItem, static_cast<JItemValue &>(value))) {
        return false;
    }

    // original
    if (emItem.hasAttribute("default")) {
        value.setOriginal(emItem.attribute("default"));
    }

    return true;
}

bool JXmlTablePrivate::parse(const QDomElement &emItem, JDelegateValue &value)
{
    if (!parse(emItem, static_cast<JItemValue &>(value))) {
        return false;
    }

    return true;
}

bool JXmlTablePrivate::parse(const QDomElement &emItem, JPictureDelegateValue &value)
{
    if (!parse(emItem, static_cast<JDelegateValue &>(value))) {
        return false;
    }

    // original
    if (emItem.hasAttribute("default")) {
        value.setOriginal(emItem.attribute("default"));
    }

    return true;
}

bool JXmlTablePrivate::parse(const QDomElement &emItem, JProgressDelegateValue &value)
{
    if (!parse(emItem, static_cast<JDelegateValue &>(value))) {
        return false;
    }

    // original
    if (emItem.hasAttribute("default")) {
        value.setOriginal(emItem.attribute("default").toDouble());
    }
    // minimum
    if (emItem.hasAttribute("minimum")) {
        value.setMinimum(emItem.attribute("minimum").toDouble());
    }
    // maximum
    if (emItem.hasAttribute("maximum")) {
        value.setMaximum(emItem.attribute("maximum").toDouble());
    }
    // text
    if (emItem.hasAttribute("text")) {
        value.setText(emItem.attribute("text"));
    }
    // textVisible
    if (emItem.hasAttribute("textVisible")) {
        value.setTextVisible(QVariant(emItem.attribute("textVisible")).toBool());
    }

    return true;
}

QByteArray JXmlTablePrivate::pack(int row, int column) const
{
    Q_Q(const JXmlTable);

    if (row == -1 && column == -1) {
        int rowCount = q->rowCount();
        if (rowCount == 0) {
            return QByteArray();
        }

        QByteArray buffer;
        for (int i = 0; i < rowCount; ++i) {
            QByteArray d(rowValue(i));
            quint32 size = d.size();
            buffer.append((const char *)&size, 4);
            buffer.append(d);
        }

        return buffer;
    } else if (row == -1) {
        if (column >= q->columnCount()) {
            return QByteArray();
        }

        QByteArray buffer(columnValue(column));
        quint32 size = buffer.size();
        buffer.prepend((const char *)&size, 4);
        return buffer;
    } else if (column == -1) {
        if (row >= q->rowCount()) {
            return QByteArray();
        }

        QByteArray buffer(rowValue(row));
        quint32 size = buffer.size();
        buffer.prepend((const char *)&size, 4);
        return buffer;
    } else {
        if (row >= q->rowCount() || column >= q->columnCount()) {
            return QByteArray();
        }

        return cellValue(row, column);
    }

    return QByteArray();
}

bool JXmlTablePrivate::unpack(const QByteArray &data, int row, int column)
{
    Q_Q(JXmlTable);

    if (row == -1 && column == -1) {
        if (data.size() < 3) {
            return false;   // error
        }

        QBuffer buffer;
        buffer.setData(data);
        if (!buffer.open(QIODevice::ReadOnly)) {
            return false;   // error
        }

        int row = 0;
        while (row < q->rowCount() && !buffer.atEnd()) {
            quint32 size = 0;
            if (buffer.read((char *)&size, 4) != 4) {
                break;  // error
            }

            if (size == 0) {
                break;  // empty
            }

            QByteArray d = buffer.read(size);
            if (d.size() != (int)size) {
                break;  // error
            }

            if (!setRowValue(row, d)) {
                break;  // error
            }

            ++row;
        }

        buffer.close();

        return true;
    } else if (row == -1) {
        if (column >= q->columnCount()) {
            return false;
        }

        return setColumnValue(column, data);
    } else if (column == -1) {
        if (row >= q->rowCount()) {
            return false;
        }

        return setRowValue(column, data);
    } else {
        if (row >= q->rowCount() || column >= q->columnCount()) {
            return false;
        }

        return setCellValue(row, column, data);
    }

    return false;
}

QVariant JXmlTablePrivate::unpackFromCell(const QByteArray &data, int row, int column) const
{
    JItemValue *itemValue = itemData(row, column);
    if (!itemValue) {
        return QVariant::Invalid;
    }

    return itemValue->unpack(data);
}

QByteArray JXmlTablePrivate::packFromCell(const QVariant &value, int row, int column) const
{
    JItemValue *itemValue = itemData(row, column);
    if (!itemValue) {
        return QByteArray();
    }

    return itemValue->pack(value);
}

bool JXmlTablePrivate::setHighlight(const QColor &color, int row, int column)
{
    Q_Q(JXmlTable);
    if (row == -1 && column == -1) {
        int rowCount = q->rowCount();
        int columnCount = q->columnCount();
        for (int r = 0; r < rowCount; ++r) {
            for (int c = 0; c < columnCount; ++ c) {
                setCellHighlight(color, r, c);
            }
        }
    } else if (row == -1) {
        if (column >= q->columnCount()) {
            return false;
        }

        int rowCount = q->rowCount();
        for (int r = 0; r < rowCount; ++r) {
            setCellHighlight(column, r, column);
        }
    } else if (column == -1) {
        if (row >= q->rowCount()) {
            return false;
        }

        int columnCount = q->columnCount();
        for (int c = 0; c < columnCount; ++c) {
            setCellHighlight(color, row, c);
        }
    } else if (row < q->rowCount() && column < q->columnCount()) {
        setCellHighlight(color, row, column);
    }

    return true;
}

void JXmlTablePrivate::setCellHighlight(const QColor &color, int row, int column)
{
    Q_Q(JXmlTable);
    QStandardItem *item = q->item(row, column);
    if (color.isValid()) {
        if (!item) {
            item = new QStandardItem();
            item->setEditable(false);
            item->setSelectable(false);
            item->setBackground(color);
            q->setItem(row, column, item);
        }
    } else if (item) {
        item->setBackground(QColor(0, 0, 0, 0));
    }
}

int JXmlTablePrivate::headerWidth() const
{
    return data.headerSize.width();
}

int JXmlTablePrivate::headerHeight() const
{
    return data.headerSize.height();
}

QSize JXmlTablePrivate::headerSize() const
{
    return data.headerSize;
}

QString JXmlTablePrivate::styleSheet() const
{
    return data.styleSheet;
}

bool JXmlTablePrivate::horiHeaderMovable() const
{
    return data.horiHeaderMovable;
}

bool JXmlTablePrivate::verticalHeaderVisible() const
{
    return data.verticalHeaderVisible;
}

bool JXmlTablePrivate::verticalHeaderLabel() const
{
    return data.verticalHeaderLabel;
}

bool JXmlTablePrivate::offsetEnabled() const
{
    return data.offsetEnabled;
}

bool JXmlTablePrivate::readOnly() const
{
    return data.readOnly;
}

bool JXmlTablePrivate::sync() const
{
    return data.sync;
}

void JXmlTablePrivate::setHeaderWidth(int value)
{
    if (value != data.headerSize.width()) {
        data.headerSize.setWidth(value);
        Q_EMIT headerWidthChanged(value);
    }
}

void JXmlTablePrivate::setHeaderHeight(int value)
{
    if (value != data.headerSize.height()) {
        data.headerSize.setHeight(value);
        Q_EMIT headerHeightChanged(value);
    }
}

void JXmlTablePrivate::setHeaderSize(const QSize &value)
{
    if (value != data.headerSize) {
        data.headerSize = value;
        Q_EMIT headerSizeChanged(value);
    }
}

void JXmlTablePrivate::setStyleSheet(const QString &value)
{
    if (value != data.styleSheet) {
        data.styleSheet = value;
        Q_EMIT styleSheetChanged(value);
    }
}

void JXmlTablePrivate::setHoriHeaderMovable(bool value)
{
    if (value != data.horiHeaderMovable) {
        data.horiHeaderMovable = value;
        Q_EMIT horiHeaderMovableChanged(value);
    }
}

void JXmlTablePrivate::setVerticalHeaderVisible(bool value)
{
    if (value != data.verticalHeaderVisible) {
        data.verticalHeaderVisible = value;
        Q_EMIT verticalHeaderVisibleChanged(value);
    }
}

void JXmlTablePrivate::setVerticalHeaderLabel(bool value)
{
    if (value != data.verticalHeaderLabel) {
        data.verticalHeaderLabel = value;
        Q_EMIT verticalHeaderLabelChanged(value);
    }
}

void JXmlTablePrivate::setOffsetEnabled(bool value)
{
    if (value != data.offsetEnabled) {
        data.offsetEnabled = value;
        Q_EMIT offsetEnabledChanged(value);
    }
}

void JXmlTablePrivate::setReadOnly(bool value)
{
    if (value != data.readOnly) {
        data.readOnly = value;
        Q_EMIT readOnlyChanged(value);
    }
}

void JXmlTablePrivate::setSync(bool value)
{
    if (value != data.sync) {
        data.sync = value;
        Q_EMIT syncChanged(value);
    }
}

void JXmlTablePrivate::_emit_sectionMoved(int logicalIndex, int oldVisualIndex, int newVisualIndex)
{
    Q_UNUSED(oldVisualIndex);

    if (!data.sync) {
        return;
    }

    QHeaderView *headerView = qobject_cast<QHeaderView *>(sender());
    if (!headerView) {
        return;
    }

    saveSection(logicalIndex, oldVisualIndex, newVisualIndex);
}

void JXmlTablePrivate::_emit_sectionResized(int logicalIndex, int oldSize, int newSize)
{
    Q_UNUSED(oldSize);

    if (!data.sync) {
        return;
    }

    QHeaderView *headerView = qobject_cast<QHeaderView *>(sender());
    if (!headerView) {
        return;
    }

    saveHeaderSize(logicalIndex, newSize, headerView->orientation());
}

void JXmlTablePrivate::init()
{
    Q_Q(JXmlTable);
    // styleSheet
    if (data.styleSheet.isEmpty()) {
        q->setStyleSheet("JBoolBox,QAbstractSpinBox,QLineEdit,QProgressBar,"
                         "QPlainTextEdit,QCombBox,QSpinBox{border:none;border-radius:0;background:#444;}"
                         "QLabel{padding-left:1px;background:transparent;}");
    } else {
        q->setStyleSheet(data.styleSheet);
    }
    // default properties
    q->horizontalHeader()->setStretchLastSection(true);
    q->setEditTriggers(QAbstractItemView::AllEditTriggers);
    // delegate
    q->setItemDelegate(new JXmlItemDelegate(q));
    // signals - slots
    QObject::connect(q, SIGNAL(itemChanged(QStandardItem*)),
                     q, SIGNAL(contentChanged()));
}

JItemValue *JXmlTablePrivate::itemData(int row, int column) const
{
    if (data.items.isEmpty()) {
        return 0;
    }

    Q_Q(const JXmlTable);

    JValueBase *result = 0;

    switch (data.orientations) {
    case Qt::Horizontal:
    {
        if (column >= q->columnCount()) {
            return 0;
        }

        const QMap<int, JValueBase *> &columnDatas = data.items.begin().value();
        if (!columnDatas.contains(column)) {
            return 0;
        }

        result = columnDatas[column];
        break;
    }
    case Qt::Vertical:
    {
        if (row >= q->rowCount()) {
            return 0;
        }

        const QMap<int, JValueBase *> &rowDatas = data.items.begin().value();
        if (!rowDatas.contains(row)) {
            return 0;
        }

        result = rowDatas[row];
        break;
    }
    case JCrossOrientation:
    {
        if (row >= q->rowCount() || column >= q->columnCount()) {
            return 0;
        }

        int maxIndex = 0;
        JValueBase *valueBase = data.rowKey(row, maxIndex);
        if (!valueBase) {
            return 0;
        }

        const QMap<int, JValueBase *> &columnsDatas = data.items[valueBase];
        if (!columnsDatas.contains(column)) {
            return 0;
        }

        result = columnsDatas[column];
        break;
    }
    default:
        break;
    }

    return static_cast<JItemValue *>(result);
}

QByteArray JXmlTablePrivate::cellValue(int row, int column) const
{
    Q_Q(const JXmlTable);

    QWidget *widget = q->cellWidget(row, column);
    if (widget) {
        return widget->property("jdata").toByteArray();
    } else {
        JItemValue *itemValue = itemData(row, column);
        if (itemValue) {
            return itemValue->pack(q->itemData(row, column));
        } else {
            return QByteArray(2, '0');
        }
    }
}

bool JXmlTablePrivate::setCellValue(int row, int column, const QByteArray &data)
{
    Q_Q(JXmlTable);

    QWidget *widget = q->cellWidget(row, column);
    if (widget) {
        return widget->setProperty("jdata", data);
    } else {
        JItemValue *itemValue = itemData(row, column);
        if (itemValue) {
            return q->setItemData(row, column, itemValue->unpack(data));
        } else {
            return false;
        }
    }
}

QByteArray JXmlTablePrivate::rowValue(int row) const
{
    Q_Q(const JXmlTable);

    QByteArray buffer;
    int columnCount = q->columnCount();
    for (int c = 0; c < columnCount; ++c) {
        buffer.append(cellValue(row, c));
    }

    return buffer;
}

bool JXmlTablePrivate::setRowValue(int row, const QByteArray &data)
{
    QBuffer buffer;
    buffer.setData(data);
    if (!buffer.open(QIODevice::ReadOnly)) {
        return false;
    }

    int column = 0;
    while (!buffer.atEnd()) {
        quint16 size = 0;
        if (buffer.read((char *)&size, 2) != 2) {
            break;      // error
        }

        if (size == 0) {
            ++column;
            continue;   // empty
        }

        if (buffer.atEnd()) {
            break;      // error
        }

        QByteArray d((const char *)&size, 2);
        d.append(buffer.read(size));
        if (d.size() - 2 != size) {
            break;      // error
        }

        if (!setCellValue(row, column, d)) {
            break;      // error
        }

        ++column;
    }

    buffer.close();

    return true;
}

QByteArray JXmlTablePrivate::columnValue(int column) const
{
    Q_Q(const JXmlTable);

    QByteArray buffer;
    int rowCount = q->rowCount();
    for (int r = 0; r < rowCount; ++r) {
        buffer.append(cellValue(r, column));
    }

    return buffer;
}

bool JXmlTablePrivate::setColumnValue(int column, const QByteArray &data)
{
    QBuffer buffer;
    buffer.setData(data);

    if (!buffer.open(QIODevice::ReadOnly)) {
        return false;   // error
    }

    int row = 0;
    while (!buffer.atEnd()) {
        quint16 size = 0;
        if (buffer.read((char *)&size, 2) != 2) {
            break;      // error
        }

        if (size == 0) {
            ++row;
            continue;   // emptpy
        }

        if (buffer.atEnd()) {
            break;      // error
        }

        QByteArray d((const char *)&size, 2);
        d.append(buffer.read(size));
        if (d.size() - 2 != size) {
            break;      // error
        }

        if (!setCellValue(row, column, d)) {
            break;      // error
        }

        ++column;
    }

    buffer.close();

    return true;
}

QVariant JXmlTablePrivate::itemValue(int row, int column, int role) const
{
    Q_Q(const JXmlTable);

    QWidget *widget = q->cellWidget(row, column);
    if (widget) {
        role = (role == Qt::EditRole) ? Qt::DisplayRole : role;
        if (role == Qt::DisplayRole) {
            return widget->property("jtext");
        } else {
            return widget->property("jvalue");
        }
    } else {
        JItemValue *itemValue = itemData(row, column);
        if (itemValue) {
            switch (itemValue->type()) {
            case BoolValue:
            {
                QStandardItem *item = q->item(row, column);
                if (item) {
                    return item->checkState();
                } else {
                    return QVariant::Invalid;
                }
                break;
            }
            default:
                return itemValue->jtoValue(q->itemData(row, column, role));
            }
        } else {
            return QVariant::Invalid;
        }
    }
}

bool JXmlTablePrivate::setItemValue(int row, int column, const QVariant &value, int role)
{
    Q_Q(JXmlTable);
    QWidget *widget = q->cellWidget(row, column);
    if (widget) {
        QStandardItem *item = q->item(row, column);
        if (item) {
            item->setText("");
        }

        role = (role == Qt::EditRole) ? Qt::DisplayRole : role;
        if (role == Qt::DisplayRole) {
            widget->setProperty("jtext", value);
        } else {
            widget->setProperty("jvalue", value);
        }
    } else {
        JItemValue *itemValue = itemData(row, column);
        if (!itemValue) {
            return false;
        }

        QVariant variant = itemValue->jfromValue(value, role);
        if (!q->setItemData(row, column, variant, role)) {
            return false;
        }

        if (!q->setItemData(row, column, QVariant::fromValue<void *>(itemValue), UserRoleItemValue)) {
            return false;
        }

        switch (itemValue->type()) {
        case BoolValue:
        {
            JBoolValue *boolValue = static_cast<JBoolValue *>(itemValue);
            QStandardItem *item = q->item(row, column);
            if (item) {
                JCheckBoxItem::initItem(boolValue, item);
            } else {
                q->setItem(row, column, new JCheckBoxItem(boolValue));
            }
            item->setCheckState(Qt::CheckState(value.toInt()));
            break;
        }
        case NumericValue:
        {
            JNumericValue *numericValue = static_cast<JNumericValue *>(itemValue);
            QStandardItem *item = q->item(row, column);
            if (!item) {
                item = new QStandardItem;
                q->setItem(row, column, item);
            }
            if (numericValue->radix() != 10 && value.type() != QVariant::String) {
                QString text = QString("%1")
                        .arg(value.toULongLong(), 0, numericValue->radix()).toUpper();
                if (!numericValue->fillChar().isNull()
                        && text.count() < numericValue->maskCount()) {
                    text.prepend(QString(numericValue->maskCount() - text.count(), numericValue->fillChar()));
                }
                q->setItemData(row, column, text);
            }
            break;
        }
        case StringValue:
        {
            JStringValue *stringValue = static_cast<JStringValue *>(itemValue);
            QStandardItem *item = q->item(row, column);
            if (stringValue->containCSS()) {
                if (item) {
                    item->setText("");
                }
                q->setCellWidget(row, column, new JStringLabel(stringValue, value.toString(), q));
            } else {
                if (!item) {
                    item = new QStandardItem;
                    q->setItem(row, column, item);
                }
                item->setText(value.toString());
            }
            break;
        }
        case EnumValue:
        {
            JEnumValue *enumValue = static_cast<JEnumValue *>(itemValue);
            if (enumValue->multiable()) {
                QStandardItem *item = q->item(row, column);
                if (item) {
                    QString text = variant.toString();
                    if (!text.isEmpty()) {
                        item->setToolTip("* " % variant.toString().replace('|', "\n* "));
                    }
                }
            }
            break;
        }
        case DelegateValue:
        {
            break;
        }
        default:
            break;
        }
    }

    return true;
}

QObject *JXmlTablePrivate::itemObject(int row, int column) const
{
    return itemData(row, column);
}

QVariant JXmlTablePrivate::itemProperty(int row, int column, const char *name) const
{
    QObject *object = itemObject(row, column);
    if (object) {
        return object->property(name);
    } else {
        return QVariant::Invalid;
    }
}

void JXmlTablePrivate::setItemProperty(int row, int column, const char *name, const QVariant &value)
{
    JItemValue *itemValue = itemData(row, column);
    if (!itemValue) {
        return;
    }

    itemValue->setProperty(name, value);

    Q_Q(JXmlTable);

    switch (itemValue->type()) {
    case StringValue:
    {
        JStringValue *stringValue = static_cast<JStringValue *>(itemValue);
        if (!stringValue->editable()) {
            if (QString(name) == "containCSS") {
                QStandardItem *item = q->item(row, column);
                QWidget *cellWidget = q->cellWidget(row, column);
                if (stringValue->containCSS()) {
                    if (item) {
                        item->setText("");
                    }
                    if (cellWidget) {
                        cellWidget->setProperty("jtext", value);
                    } else {
                        q->setCellWidget(row, column, new JStringLabel(stringValue, value.toString(), q));
                    }
                } else {
                    if (!item) {
                        item = new QStandardItem;
                        q->setItem(row, column, item);
                    }
                    if (cellWidget) {
                        item->setText("");
                        q->removeCellWidget(row, column);
                    } else {
                        item->setText(value.toString());
                    }
                }
            }
        }
        break;
    }
    default:
        break;
    }
}

JItemValue *JXmlTablePrivate::createItem(const QDomElement &emItem, const std::string &type)
{
    if (emItem.isNull()) {
        return 0;
    }

    switch (jvaluetype_type_of_string(type)) {
    case BoolValue:
        return parse<JBoolValue>(emItem);
    case NumericValue:
        return parse<JNumericValue>(emItem);
    case StringValue:
        return parse<JStringValue>(emItem);
    case EnumValue:
        return parse<JEnumValue>(emItem);
    case DateTimeValue:
        return parse<JDateTimeValue>(emItem);
    case IPv4Value:
        return parse<JIPv4Value>(emItem);
    case DelegateValue:
    {
        if (!emItem.hasAttribute("delegateType")) {
            return 0;
        }

        switch (jdelegatetype_type_of_string(
                    emItem.attribute("delegateType").toStdString())) {
        case PictureDelegate:
            return parse<JPictureDelegateValue>(emItem);
        case ProgressDelegate:
            return parse<JProgressDelegateValue>(emItem);
        default:
            break;
        }
        break;
    }
    default:
        break;
    }

    return 0;
}

void JXmlTablePrivate::appendItem(JItemValue *value, JValueBase *rowValue)
{
    switch (data.orientations) {
    case Qt::Horizontal:
    case Qt::Vertical:
    {
        if (data.items.isEmpty()) {
            if (value->index() == -1) {
                value->setIndex(0);
            }
            data.items[new JValueBase(this)][value->index()] = value;
        } else {
            QMap<int, JValueBase *> &columnDatas = data.items.begin().value();
            if (columnDatas.contains(value->index())) {
                JValueBase *oldValue = columnDatas.take(value->index());
                delete oldValue;
                columnDatas[value->index()] = value;    // use the newest
            } else {
                if (value->index() == -1) {
                    value->setIndex(columnDatas.keys().last() + 1);
                }
                columnDatas[value->index()] = value;
            }
        }
        break;
    }
    case JCrossOrientation:
    {
        QMap<int, JValueBase *> &columnDatas = data.items[rowValue];
        if (columnDatas.isEmpty()) {
            if (value->index() == -1) {
                value->setIndex(0);
            }
        } else if (columnDatas.contains(value->index())) {
            JValueBase *oldValue = columnDatas.take(value->index());
            delete oldValue;    // use the newest
        }
        if (value->index() == -1) {
            value->setIndex(columnDatas.keys().last() + 1);
        }
        columnDatas[value->index()] = value;
        break;
    }
    default:
        break;
    }
}

void JXmlTablePrivate::connectSignals(Qt::Orientations orientations)
{
    Q_Q(JXmlTable);
    // horizontal
    if (orientations & Qt::Horizontal) {
        connect(q->horizontalHeader(), SIGNAL(sectionMoved(int,int,int)),
                SLOT(_emit_sectionMoved(int,int,int)));
        if (data.orientations != JCrossOrientation) {
            connect(q->horizontalHeader(), SIGNAL(sectionResized(int,int,int)),
                    SLOT(_emit_sectionResized(int,int,int)));
        }
    }
    // vertical
    if (orientations & Qt::Vertical) {
        connect(q->verticalHeader(), SIGNAL(sectionMoved(int,int,int)),
                SLOT(_emit_sectionMoved(int,int,int)));
        connect(q->verticalHeader(), SIGNAL(sectionResized(int,int,int)),
                SLOT(_emit_sectionResized(int,int,int)));
    }
}

void JXmlTablePrivate::disconnectSignals(Qt::Orientations orientations)
{
    Q_Q(JXmlTable);
    // horizontal
    if (orientations & Qt::Horizontal) {
        disconnect(q->horizontalHeader(), SIGNAL(sectionMoved(int,int,int)),
                   this, SLOT(_emit_sectionMoved(int,int,int)));
        if (data.orientations != JCrossOrientation) {
            disconnect(q->horizontalHeader(), SIGNAL(sectionResized(int,int,int)),
                       this, SLOT(_emit_sectionResized(int,int,int)));
        }
    }
    // vertical
    if (orientations & Qt::Vertical) {
        disconnect(q->verticalHeader(), SIGNAL(sectionMoved(int,int,int)),
                   this, SLOT(_emit_sectionMoved(int,int,int)));
        disconnect(q->verticalHeader(), SIGNAL(sectionResized(int,int,int)),
                   this, SLOT(_emit_sectionResized(int,int,int)));
    }
}

QString JXmlTablePrivate::nodePath(const QDomNode &node)
{
    if (node.isNull()) {
        return QString::null;
    }

    return QString("%1(%2, %3)")
            .arg(node.nodeName())
            .arg(node.lineNumber())
            .arg(node.columnNumber());
}

// - class JXmlItemDelegate -

JXmlItemDelegate::JXmlItemDelegate(QObject *parent)
    : QStyledItemDelegate(parent)
    , tableView(qobject_cast<JXmlTable *>(parent))
    , tableViewPrivate(tableView->d_ptr)
{

}

JXmlItemDelegate::~JXmlItemDelegate()
{

}

QWidget *JXmlItemDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    Q_UNUSED(option);

    if (tableViewPrivate->data.readOnly) {
        return 0;
    }

    JItemValue *itemValue = tableViewPrivate->itemData(index.row(), index.column());
    if (!itemValue) {
        return 0;
    }

    if (!itemValue->enabled() || !itemValue->editable()) {
        return 0;
    }

    return itemValue->createInstance(itemValue, parent);
}

void JXmlItemDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    editor->setProperty("jvalue", index.data());
}

void JXmlItemDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    model->setData(index, editor->property("jtext"));
    model->setData(index, editor->property("jproperty"), UserRoleItemValue);

    if (editor->inherits("JMultiEnumBox")) {
        JMultiEnumBox *enumBox = qobject_cast<JMultiEnumBox *>(editor);
        model->setData(index, enumBox->toolTip(), Qt::ToolTipRole);
    }
}

void JXmlItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    JItemValue *itemValue = static_cast<JItemValue *>(index.data(UserRoleItemValue).value<void *>());
    if (itemValue) {
        switch (itemValue->type()) {
        case IPv4Value:
        {
            JIPv4Value *ipv4Value = static_cast<JIPv4Value *>(itemValue);
            drawIPv4Item(painter, option, index, ipv4Value);
            return;
        }
        case DelegateValue:
        {
            JDelegateValue *delegateValue = static_cast<JDelegateValue *>(itemValue);
            drawDelegateItem(painter, option, index, delegateValue);
            return;
        }
        default:
            break;
        }
    }

    QStyledItemDelegate::paint(painter, option, index);
}

bool JXmlItemDelegate::drawIPv4Item(QPainter *painter,
                                    const QStyleOptionViewItem &option,
                                    const QModelIndex &index,
                                    const JIPv4Value *value) const
{
    Q_UNUSED(value);

    if (option.state & QStyle::State_Selected) {
        painter->fillRect(option.rect, option.palette.highlight());
    }

    QStringList s = index.data().toString().trimmed().split('.', QString::SkipEmptyParts);
    if (s.count() < 4) {
        return false;
    }

    painter->save();
    painter->setRenderHint(QPainter::Antialiasing);
    painter->setBrush(QBrush(Qt::black));
    QRectF r(option.rect.left(), option.rect.top(),
             option.rect.width() / 4., option.rect.height() / 2.);
    qreal x = 0.;
    for (int i = 0; i < 4; ++i) {
        x = r.left() + r.width() * i;
        if (i > 0) {
            painter->drawEllipse(QPointF(x++, r.top() + r.height()), 1.2, 1.2);
        }
        painter->drawText(QRectF(x, r.top(), r.width(), option.rect.height()), s[i],
                          QTextOption(Qt::AlignCenter));
    }
    painter->restore();

    return true;
}

bool JXmlItemDelegate::drawDelegateItem(QPainter *painter,
                                        const QStyleOptionViewItem &option,
                                        const QModelIndex &index,
                                        const JDelegateValue *value) const
{
    switch (value->delegateType()) {
    case PictureDelegate:
    {
        if (option.state & QStyle::State_Selected) {
            painter->fillRect(option.rect, option.palette.highlight());
        }

        QPixmap pixmap(index.data().toString());
        if (!pixmap.isNull()) {
            QRect rect = pixmap.rect();
            rect.moveTo(option.rect.topLeft());
            rect = rect.intersected(option.rect);
            // alignment
            if (value->alignment() & Qt::AlignRight) {
                rect.translate(option.rect.right() - rect.right(), 0);
            } else if (value->alignment() & Qt::AlignBottom) {
                rect.translate(0, option.rect.bottom());
            } else if (value->alignment() & Qt::AlignHCenter) {
                rect.translate((option.rect.right() - rect.right()) / 2, 0);
            } else if (value->alignment() & Qt::AlignVCenter) {
                rect.translate(0, (option.rect.bottom() - rect.bottom()) / 2);
            }
            painter->drawPixmap(rect, pixmap);
        }
        break;
    }
    case ProgressDelegate:
    {
        const JProgressDelegateValue *progressDelegateValue =
                static_cast<const JProgressDelegateValue *>(value);
        QVariant variant = index.data();
        if (variant.type() == QVariant::String) {
            QStyledItemDelegate::paint(painter, option, index);
        } else {
            if (option.state & QStyle::State_Selected) {
                painter->fillRect(option.rect, option.palette.highlight());
            }

            QStyleOptionProgressBar optionProgressBar;
            optionProgressBar.state = QStyle::State_Enabled | QStyle::State_Horizontal;
            optionProgressBar.direction = QApplication::layoutDirection();
            optionProgressBar.fontMetrics = QApplication::fontMetrics();
            optionProgressBar.rect = option.rect;
            optionProgressBar.minimum = 0;
            optionProgressBar.maximum = 100;
            optionProgressBar.progress = (int)(index.data().toDouble() * 100 /
                                               (progressDelegateValue->maximum() - progressDelegateValue->minimum()));
            optionProgressBar.text = QString("%1%2")
                    .arg(progressDelegateValue->text())
                    .arg(optionProgressBar.progress);
            optionProgressBar.textAlignment = progressDelegateValue->alignment();
            optionProgressBar.textVisible = progressDelegateValue->textVisible();

            QProgressBar progressBar(tableView);
            QApplication::style()->drawControl(QStyle::CE_ProgressBar, &optionProgressBar, painter, &progressBar);
        }
        break;
    }
    default:
        break;
    }

    return true;
}

// - class JXmlTableInvoke -

JXmlTableInvoke::JXmlTableInvoke(JXmlTablePrivate *viewPri, QObject *parent) :
    QObject(parent),
    viewPri(viewPri),
    view(viewPri->q_ptr)
{

}

bool JXmlTableInvoke::execAdd(bool exec)
{
    if (exec) {
        return execDialog();
    } else {
        switch (viewPri->data.orientations) {
        case Qt::Horizontal:
        {
            int rowCount = view->rowCount();
            view->insertRow(rowCount == -1 ? 0 : rowCount);
            break;
        }
        case Qt::Vertical:
        {
            int columnCount = view->columnCount();
            view->insertColumn(columnCount == -1 ? 0 : columnCount);
            break;
        }
        default:
            return false;   // not supported
        }
    }

    return true;
}

bool JXmlTableInvoke::execModify(int index)
{
    switch (viewPri->data.orientations) {
    case Qt::Horizontal:
    case Qt::Vertical:
    {
#if 0
        JXmlTableEditor editor(viewPri.data());
        editor.init(index);
        if (editor.exec() != QDialog::Accepted) {
            return false;
        }
#else
        Q_UNUSED(index);
#endif
        break;
    }
    default:
        return false;   // not supported
    }

    return true;
}

bool JXmlTableInvoke::execDialog(int index)
{
    Q_UNUSED(index);
    return false;
}

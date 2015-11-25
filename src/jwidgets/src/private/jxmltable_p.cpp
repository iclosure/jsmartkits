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

    TiXmlElement *emTable = elementTable(filePath, tableName);
    if (!emTable) {
        return false;
    }

    data.filePath = filePath;

    return loadConfig(emTable);
}

bool JXmlTablePrivate::loadConfig(const TiXmlElement *emTable)
{
    data.clear();

    if (!emTable) {
        return false;
    }

    QString tableName = QString::fromUtf8(emTable->Attribute("name"));
    if (tableName.isEmpty()) {
        QMessageBox::critical(0, QObject::tr("Parse xmltable"),
                              QObject::tr("name of table cannot be empty!"));
        return false;
    }

    data.tableName = tableName;

    bool bVal = false;
    int iVal = 0;
    std::string sVal;

    // orientations
    if (emTable->QueryStringAttribute("orientations", &sVal) == TIXML_SUCCESS) {
        QStringList sOrientations =
                QString::fromStdString(sVal).trimmed().split('|', QString::SkipEmptyParts);
        QStringListIterator iter(sOrientations);
        data.orientations = (Qt::Orientation)0;
        while (iter.hasNext()) {
            const QString &sOrientation = iter.next();
            if (sOrientation == "horizontal") {
                data.orientations |= Qt::Horizontal;
            } else if (sOrientation == "vertical") {
                data.orientations |= Qt::Vertical;
            }
        }
    }
    // headerWidth
    if (emTable->QueryIntAttribute("headerWidth", &iVal) == TIXML_SUCCESS) {
        data.headerSize.setWidth(iVal);
    }
    // headerHeight
    if (emTable->QueryIntAttribute("headerHeight", &iVal) == TIXML_SUCCESS) {
        data.headerSize.setHeight(iVal);
    }
    // styleSheet
    data.styleSheet = QString::fromUtf8(emTable->Attribute("styleSheet"));
    // horiHeaderMovable
    if (emTable->QueryBoolAttribute("horiHeaderMovable", &bVal) == TIXML_SUCCESS) {
        data.horiHeaderMovable = bVal;
    }
    // verticalHeaderVisible
    if (emTable->QueryBoolAttribute("verticalHeaderVisible", &bVal) == TIXML_SUCCESS) {
        data.verticalHeaderVisible = bVal;
    }
    // verticalHeaderLabel
    if (emTable->QueryBoolAttribute("verticalHeaderLabel", &bVal) == TIXML_SUCCESS) {
        data.verticalHeaderLabel = bVal;
    }
    // offsetEnabled
    if (emTable->QueryBoolAttribute("offsetEnabled", &bVal) == TIXML_SUCCESS) {
        data.offsetEnabled = bVal;
    }
    // readOnly
    if (emTable->QueryBoolAttribute("readOnly", &bVal) == TIXML_SUCCESS) {
        data.readOnly = bVal;
    }
    // sync
    if (emTable->QueryBoolAttribute("sync", &bVal) == TIXML_SUCCESS) {
        data.sync = bVal;
    }
    // parse table
    if (!parse(emTable)) {
        return false;
    }

    return true;
}

bool JXmlTablePrivate::loadConfig(const QByteArray &data)
{
    this->data.clear();

    if (data.isEmpty()) {
        return false;
    }

    TiXmlElement emTable(data.constData());
    if (!emTable.Parse(data.constData(), 0, TIXML_ENCODING_UTF8)) {
        QMessageBox::critical(0, QObject::tr("Parse xmltable data"),
                              QObject::tr("parse data failure!"));
        return false;
    }

    return loadConfig(&emTable);
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
            q->horizontalHeader()->setMovable(data.horiHeaderMovable);
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
            q->horizontalHeader()->setMovable(data.horiHeaderMovable);
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
            q->horizontalHeader()->setMovable(data.horiHeaderMovable);
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

TiXmlElement *JXmlTablePrivate::elementTable()
{
    return elementTable(data.filePath, data.tableName);
}

TiXmlElement *JXmlTablePrivate::elementTable(const QString &filePath, const QString &tableName)
{
    Q_Q(JXmlTable);
    if (filePath.isEmpty()) {
        QMessageBox::critical(q, QObject::tr("check parameters"),
                              QObject::tr("filePath cannot be empty!"));
        return 0;
    }

    if (tableName.isEmpty()) {
        QMessageBox::critical(q, QObject::tr("check parameters"),
                              QObject::tr("tableName cannot be empty!"));
        return 0;
    }

    if (filePath.startsWith(':')) {
        QFile file(filePath);
        if (file.open(QFile::ReadOnly)) {
            if (!data.document.Parse(file.readAll().constData(), 0, TIXML_ENCODING_UTF8)) {
                QMessageBox::critical(q, QObject::tr("parse xmltable data"),
                                      QObject::tr("parse data failure!"));
                return 0;
            }
        } else {
            QMessageBox::critical(q, QObject::tr("open buffer file"),
                                  QObject::tr("open failure!"));
            return 0;
        }
    } else {
        data.document.SetValue(filePath.toStdString());
        if (!data.document.LoadFile(TIXML_ENCODING_UTF8)) {
            QMessageBox::critical(q, QObject::tr("load xmltable file"),
                                  QObject::tr("load failure!"));
            return 0;
        }
    }

    TiXmlElement *emRoot = data.document.RootElement();
    if (!emRoot) {
        QMessageBox::critical(q, QObject::tr("Parse xmltable"),
                              QObject::tr("root node is not exists in file %1!").arg(filePath));
        return 0;
    }

    // version
#if 0
    QString ver = QString::fromUtf8(emRoot->Attribute("version"));
    if (ver != version()) {
        QMessageBox::critical(0, QObject::tr("Parse xmltable"),
                              QObject::tr("the version of '%1' is invalid for %2!")
                              .arg(ver).arg(filePath));
        return 0;
    }
#endif
    TiXmlElement *emTable = emRoot->FirstChildElement("table");
    if (!emTable) {
        QMessageBox::critical(q, QObject::tr("Parse xmltable"),
                              QObject::tr("table node is not exists int file %1!").arg(filePath));
        return 0;
    } else {
        for (; emTable != 0;
             emTable = emTable->NextSiblingElement("table")) {
            QString name = QString::fromUtf8(emTable->Attribute("name"));
            if (name == tableName) {
                break;
            }
        }

        if (!emTable) {
            QMessageBox::critical(q, QObject::tr("Parse xmltable"),
                                  QObject::tr("table node %1 is not found in file %2!")
                                  .arg(tableName).arg(filePath));
            return 0;
        }
    }

    return emTable;
}

TiXmlElement *JXmlTablePrivate::elementItem(int row, int column)
{
    return elementItem(elementTable(), row, column);
}

TiXmlElement *JXmlTablePrivate::elementItem(TiXmlElement *emTable, int row, int column)
{
    if (!emTable) {
        return 0;
    }

    JItemValue *itemValue = itemData(row, column);
    if (!itemValue) {
        return 0;
    }
    return elementItem(emTable, itemValue->name());
}

TiXmlElement *JXmlTablePrivate::elementItem(const QString &name)
{
    return elementItem(elementTable(), name);
}

TiXmlElement *JXmlTablePrivate::elementItem(TiXmlElement *emTable, const QString &name)
{
    if (!emTable) {
        return 0;
    }

    for (TiXmlElement *emItem = emTable->FirstChildElement("item");
         emItem != 0;
         emItem = emItem->NextSiblingElement("item")) {
        QString _name = QString::fromUtf8(emItem->Attribute("name"));
        if (_name == name) {
            return emItem;
        }
    }

    return 0;
}

TiXmlElement *JXmlTablePrivate::elementRow(int row)
{
    TiXmlElement *emTable = elementTable();
    if (!emTable) {
        return 0;
    }

    TiXmlElement *emRow = emTable->FirstChildElement("row");
    while (!emRow && -- row >= 0) {
        emRow = emRow->NextSiblingElement("row");
    }

    return emRow;
}

TiXmlElement *JXmlTablePrivate::elementRow(TiXmlElement *emTable, int row)
{
    if (!emTable) {
        return 0;
    }

    JItemValue *itemValue = itemData(row, 0);
    if (!itemValue) {
        return 0;
    }

    return elementRow(emTable, itemValue->name());
}

TiXmlElement *JXmlTablePrivate::elementRow(TiXmlElement *emTable, const QString &name)
{
    if (!emTable) {
        return 0;
    }

    for (TiXmlElement *emRow = emTable->FirstChildElement("row");
         emRow != 0;
         emRow = emRow->NextSiblingElement("row")) {
        QString _name = QString::fromUtf8(emRow->Attribute("name"));
        if (_name == name) {
            return emRow;
        }
    }

    return 0;
}

TiXmlElement *JXmlTablePrivate::elementColumn(int column)
{
    JItemValue *itemValue = itemData(0, column);
    if (!itemValue) {
        return 0;
    }

    return elementColumn(elementTable(), itemValue->name());
}

TiXmlElement *JXmlTablePrivate::elementColumn(TiXmlElement *emTable, int column)
{
    if (!emTable) {
        return 0;
    }

    JItemValue *itemValue = itemData(0, column);
    if (!itemValue) {
        return 0;
    }

    return elementColumn(emTable, column);
}

TiXmlElement *JXmlTablePrivate::elementColumn(TiXmlElement *emTable, const QString &name)
{
    if (!emTable) {
        return 0;
    }

    TiXmlElement *emRow = emTable->FirstChildElement("row");
    if (!emRow) {
        return 0;
    }

    for (TiXmlElement *emColumn = emRow->FirstChildElement("column");
         emColumn != 0;
         emColumn = emColumn->NextSiblingElement("column")) {
        QString _name = QString::fromUtf8(emColumn->Attribute("column"));
        if (_name == name) {
            return emColumn;
        }
    }

    return 0;
}

void JXmlTablePrivate::saveHeaderSize(int logicalIndex, int newSize, Qt::Orientation orientation)
{
    TiXmlElement *emItem = 0;

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

    if (emItem) {
        emItem->SetAttribute("headerSize", newSize);
        data.document.SaveFile();
    }
}

void JXmlTablePrivate::saveSection(int logicalIndex, int oldVisualIndex, int newVisualIndex)
{
    Q_UNUSED(logicalIndex);
    Q_UNUSED(oldVisualIndex);
    Q_UNUSED(newVisualIndex);

    TiXmlElement *emTable = elementTable();
    if (!emTable) {
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
    for (TiXmlElement *emItem = emTable->FirstChildElement("item");
         emItem != 0;
         emItem = emItem->NextSiblingElement("item")) {
        emItem->SetAttribute("offset", headerView->visualIndex(index++));
    }

    data.document.SaveFile();
}

bool JXmlTablePrivate::parse(const TiXmlElement *emTable)
{
    if (!emTable) {
        return false;
    }

    std::string sVal;

    switch (data.orientations) {
    case Qt::Horizontal:
    case Qt::Vertical:
        for (const TiXmlElement *emItem = emTable->FirstChildElement("item");
             emItem != 0;
             emItem = emItem->NextSiblingElement("item")) {
            if (emItem->QueryStringAttribute("type", &sVal) != TIXML_SUCCESS) {
                continue;
            }
            JItemValue *itemValue = createItem(emItem, sVal);
            if (itemValue) {
                appendItem(itemValue, 0);
            }
        }
        break;
    case JCrossOrientation:
    {
        // rows - columns
        int iVal = 0;
        for (const TiXmlElement *emRow = emTable->FirstChildElement("row");
             emRow != 0;
             emRow = emRow->NextSiblingElement("row")) {
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
                JValueBase *oldValue = data.rowKey(rowValue->index(), iVal);
                if (!oldValue) {    // not exists
                    if (rowValue->index() == -1) {
                        rowValue->setIndex(iVal + 1);
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
            for (const TiXmlElement *emColumn = emRow->FirstChildElement("column");
                 emColumn != 0;
                 emColumn = emColumn->NextSiblingElement("column")) {
                if (emColumn->QueryStringAttribute("type", &sVal) != TIXML_SUCCESS) {
                    continue;
                }
                JItemValue *columnValue = createItem(emColumn, sVal);
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

bool JXmlTablePrivate::parse(const TiXmlElement *emItem, JValueBase &value)
{
    if (!emItem) {
        return false;
    }

    int iVal = 0;
    std::string sVal;

    // index
    if (emItem->QueryIntAttribute("index", &iVal) == TIXML_SUCCESS) {
        value.setIndex(qBound(0, iVal, 100000));
    }
    // offset
    if (emItem->QueryIntAttribute("offset", &iVal) == TIXML_SUCCESS) {
        value.setOffset(qMax(0, iVal));
    }
    // headerSize
    if (emItem->QueryIntAttribute("headerSize", &iVal) == TIXML_SUCCESS) {
        value.setHeaderSize(qBound(25, iVal, 2000));
    }
    // name
    value.setName(QString::fromUtf8(emItem->Attribute("name")));
    // filterType
    if (emItem->QueryStringAttribute("filterType", &sVal) == TIXML_SUCCESS) {
        value.setFilterType(jvaluetype_type_of_string(sVal));
    }

    return true;
}

bool JXmlTablePrivate::parse(const TiXmlElement *emItem, JItemValue &value)
{
    if (!parse(emItem, static_cast<JValueBase &>(value))) {
        return false;
    }

    bool bVal = false;
    std::string sVal;

    // editable
    if (emItem->QueryBoolAttribute("editable", &bVal) == TIXML_SUCCESS) {
        value.setEditable(bVal);
    }
    // alignment
    if (emItem->QueryStringAttribute("alignment", &sVal) == TIXML_SUCCESS) {
        QStringList sAlignments =
                QString::fromStdString(sVal).trimmed().split('|', QString::SkipEmptyParts);
        Qt::Alignment alignment = (Qt::Alignment)0;
        QStringListIterator iter(sAlignments);
        while (iter.hasNext()) {
            const QString &sAlignment = iter.next();
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
    value.setStyleSheet(QString::fromUtf8(emItem->Attribute("styleSheet")));
    // icon
    value.setIcon(QString::fromUtf8(emItem->Attribute("icon")));
    // tooltip
    value.setTooltip(QString::fromUtf8(emItem->Attribute("tooltip")));
    // placeHoldText
    value.setPlaceHoldText(QString::fromUtf8(emItem->Attribute("placeHoldText")));
    // whatsThis
    value.setWhatsThis(QString::fromUtf8(emItem->Attribute("whatsThis")));

    return true;
}

bool JXmlTablePrivate::parse(const TiXmlElement *emItem, JBoolValue &value)
{
    if (!parse(emItem, static_cast<JItemValue &>(value))) {
        return false;
    }

    bool bVal = false;
    std::string sVal;

    // original
    if (emItem->QueryStringAttribute("default", &sVal) == TIXML_SUCCESS) {
        if (sVal == "unchecked") {
            value.setOriginal(Qt::Unchecked);
        } else if (sVal == "partially") {
            value.setOriginal(Qt::PartiallyChecked);
        } else if (sVal == "checked") {
            value.setOriginal(Qt::Checked);
        }
    }
    // checkable
    if (emItem->QueryBoolAttribute("checkable", &bVal) == TIXML_SUCCESS) {
        value.setCheckable(bVal);
    }
    // text
    value.setText(QString::fromUtf8(emItem->Attribute("text")));

    return true;
}

bool JXmlTablePrivate::parse(const TiXmlElement *emItem, JNumericValue &value)
{
    if (!parse(emItem, static_cast<JItemValue &>(value))) {
        return false;
    }

    Q_Q(JXmlTable);

    int iVal = 0;
    double dVal = 0.;
    std::string sVal;

    // prefix
    value.setPrefix(QString::fromUtf8(emItem->Attribute("prefix")));
    // suffix
    value.setSuffix(QString::fromUtf8(emItem->Attribute("suffix")));
    // original
    if (value.suffix().isEmpty()) {     // is an ascii
        if (emItem->QueryStringAttribute("default", &sVal) == TIXML_SUCCESS) {
            if (sVal.empty()) {
                value.setOriginal('\0');
            } else {
                value.setOriginal(sVal.at(0));
            }
        }
    } else {
        if (emItem->QueryDoubleAttribute("default", &dVal) == TIXML_SUCCESS) {
            value.setOriginal(dVal);
        }
    }
    // radix
    if (emItem->QueryIntAttribute("radix", &iVal) == TIXML_SUCCESS) {
        if (iVal < 2 || iVal > 36) {
            QMessageBox::critical(q, QObject::tr("Parse xmltable"),
                                  QObject::tr("parse radix failure (out of range [2, 36] '%1' in '%2/item/radix'")
                                  .arg(QString::fromUtf8(emItem->Attribute("radix")))
                                  .arg(nodePath(emItem)));
            iVal = qBound(2, iVal, 36);
        }
        value.setRadix(iVal);
    }
    // maskCount
    if (emItem->QueryIntAttribute("maskCount", &iVal) == TIXML_SUCCESS) {
        if (iVal < 1 || iVal > 8) {
            QMessageBox::critical(q, QObject::tr("Parse xmltable"),
                                  QObject::tr("parse maskCount failure (out of range [1, 8] '%1' in '%2/item/radix'")
                                  .arg(QString::fromUtf8(emItem->Attribute("maskCount")))
                                  .arg(nodePath(emItem)));
            iVal = qBound(1, iVal, 8);
        }
        value.setMaskCount(iVal);
    } else if (value.radix() != 10) {
        value.setMaskCount(8);
    }
    // fillChar
    if (emItem->QueryStringAttribute("fillChar", &sVal) == TIXML_SUCCESS) {
        if (sVal.empty()) {
            value.setFillChar(QChar());
        } else {
            value.setFillChar(QString::fromUtf8(emItem->Attribute("fillChar")).at(0));
        }
    } else if (value.radix() != 10) {
        value.setFillChar(QChar('0'));
    }
    // decimals
    if (emItem->QueryIntAttribute("decimals", &iVal) == TIXML_SUCCESS) {
        value.setDecimals(iVal);
    }
    // step
    if (emItem->QueryDoubleAttribute("step", &dVal) == TIXML_SUCCESS) {
        value.setStep(dVal);
    } else {
        value.setStep(qPow(0.1, value.decimals()));
    }
    if (value.step() == 0.) {
        value.setStep(1.);
    }
    // scale
    if (emItem->QueryDoubleAttribute("scale", &dVal) == TIXML_SUCCESS) {
        value.setScale(dVal);
    } else {
        value.setScale(value.step());
    }
    if (value.scale() == 0.) {
        value.setScale(1.);
    }
    // range
    if (emItem->QueryStringAttribute("range", &sVal) == TIXML_SUCCESS) {
        QString range = QString::fromUtf8(emItem->Attribute("range"));
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
        for (const TiXmlElement *emWithout = emItem->FirstChildElement("without");
             emWithout != 0;
             emWithout = emWithout->NextSiblingElement("without")) {
            JNumericRange *rangeValue = new JNumericRange(&value);
            if (!rangeValue->parse(emWithout->GetText())) {
                QMessageBox::critical(q, QObject::tr("Parse xmltable"),
                                      QObject::tr("parse without section '%1' failiure in '%2/item/range'")
                                      .arg(emWithout->GetText()).arg(nodePath(emItem)));
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
    if (emItem->QueryIntAttribute("size", &iVal) == TIXML_SUCCESS) {
        value.setSize(qBound(0, iVal, 8));
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

bool JXmlTablePrivate::parse(const TiXmlElement *emItem, JStringValue &value)
{
    if (!parse(emItem, static_cast<JItemValue &>(value))) {
        return false;
    }

    bool bVal = false;
    int iVal = 0;

    // original
    value.setOriginal(QString::fromUtf8(emItem->Attribute("default")));
    // upper
    if (emItem->QueryBoolAttribute("upper", &bVal) == TIXML_SUCCESS) {
        value.setUpper(bVal);
    }
    // multiline
    if (emItem->QueryBoolAttribute("multiline", &bVal) == TIXML_SUCCESS) {
        value.setMultiline(bVal);
    }
    // containCSS
    if (emItem->QueryBoolAttribute("containCSS", &bVal) == TIXML_SUCCESS) {
        value.setContainCSS(bVal);
    }
    // maxLength
    if (emItem->QueryIntAttribute("maxLength", &iVal) == TIXML_SUCCESS) {
        value.setMaxLength(iVal);
    }
    // pattern
    value.setPattern(QString::fromUtf8(emItem->Attribute("pattern")));

    return true;
}

bool JXmlTablePrivate::parse(const TiXmlElement *emItem, JEnumValue &value)
{
    if (!parse(emItem, static_cast<JItemValue &>(value))) {
        return false;
    }

    bool bVal = false;
    int iVal = 0;
    std::string sVal;

    // original
    value.setOriginal(QString::fromUtf8(emItem->Attribute("default")));
    // indexOffset
    if (emItem->QueryIntAttribute("indexOffset", &iVal) == TIXML_SUCCESS) {
        value.setIndexOffset(iVal);
    }
    // autoCompletion
    if (emItem->QueryBoolAttribute("autoCompletion", &bVal) == TIXML_SUCCESS) {
        value.setAutoCompletion(bVal);
    }
    // duplicatesEnabled
    if (emItem->QueryBoolAttribute("duplicatesEnabled", &bVal) == TIXML_SUCCESS) {
        value.setDuplicatesEnabled(bVal);
    }
    // multiable
    if (emItem->QueryBoolAttribute("multiable", &bVal) == TIXML_SUCCESS) {
        value.setMultiable(bVal);
    }
    // options
    for (const TiXmlElement *emOption = emItem->FirstChildElement("option");
         emOption != 0;
         emOption = emOption->NextSiblingElement("option")) {
        JEnumOption *enumOption = new JEnumOption(&value);
        enumOption->setText(QString::fromUtf8(emOption->GetText()));
        if (emOption->QueryStringAttribute("encode", &sVal) == TIXML_SUCCESS) {
            QString encode = QString::fromUtf8(emOption->Attribute("encode"));
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
    if (emItem->QueryIntAttribute("size", &iVal) == TIXML_SUCCESS) {
        value.setSize(qMin(iVal, 4));
    } else {
        value.setSize(4);   // default size: 4 bytes
    }

    return true;
}

bool JXmlTablePrivate::parse(const TiXmlElement *emItem, JIPv4Value &value)
{
    if (!parse(emItem, static_cast<JItemValue &>(value))) {
        return false;
    }

    // original
    value.setOriginal(QString::fromUtf8(emItem->Attribute("default")));

    return true;
}

bool JXmlTablePrivate::parse(const TiXmlElement *emItem, JDelegateValue &value)
{
    if (!parse(emItem, static_cast<JItemValue &>(value))) {
        return false;
    }

    return true;
}

bool JXmlTablePrivate::parse(const TiXmlElement *emItem, JPictureDelegateValue &value)
{
    if (!parse(emItem, static_cast<JDelegateValue &>(value))) {
        return false;
    }

    // original
    value.setOriginal(QString::fromUtf8(emItem->Attribute("default")));

    return true;
}

bool JXmlTablePrivate::parse(const TiXmlElement *emItem, JProgressDelegateValue &value)
{
    if (!parse(emItem, static_cast<JDelegateValue &>(value))) {
        return false;
    }

    bool bVal = false;
    double dVal = 0.;

    // original
    if (emItem->QueryDoubleAttribute("default", &dVal) == TIXML_SUCCESS) {
        value.setOriginal(dVal);
    }
    // minimum
    if (emItem->QueryDoubleAttribute("minimum", &dVal) == TIXML_SUCCESS) {
        value.setMinimum(dVal);
    }
    // maximum
    if (emItem->QueryDoubleAttribute("maximum", &dVal) == TIXML_SUCCESS) {
        value.setMaximum(dVal);
    }
    // text
    value.setText(QString::fromUtf8(emItem->Attribute("text")));
    // textVisible
    if (emItem->QueryBoolAttribute("textVisible", &bVal) == TIXML_SUCCESS) {
        value.setTextVisible(bVal);
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
                         "QPlainTextEdit,QCombBox,QSpinBox{border:none;border-radius:0;background:white;}"
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

JItemValue *JXmlTablePrivate::createItem(const TiXmlElement *emItem, const std::string &type)
{
    if (!emItem) {
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
    case IPv4Value:
        return parse<JIPv4Value>(emItem);
    case DelegateValue:
    {
        std::string sVal;
        if (emItem->QueryStringAttribute("delegateType", &sVal) != TIXML_SUCCESS) {
            return 0;
        }

        switch (jdelegatetype_type_of_string(sVal)) {
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

QString JXmlTablePrivate::nodePath(const TiXmlNode *node)
{
    if (!node) {
        return QString::null;
    }

    return QString("%1(%2, %3)")
            .arg(node->Value())
            .arg(node->Row()).arg(node->Column());
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

            QStyleOptionProgressBarV2 optionProgressBar;
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

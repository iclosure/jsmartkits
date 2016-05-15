#include "precomp.h"
#include "jxmltable.h"
#include "private/jxmltable_p.h"

// - class JXmlTable -

JXmlTable::JXmlTable(QWidget *parent)
    : JTableView(parent)
    , d_ptr(new JXmlTablePrivate(this))
{
    Q_D(JXmlTable);
    d->init();
}

JXmlTable::JXmlTable(const QString &filePath, QWidget *parent)
    : JTableView(parent)
    , d_ptr(new JXmlTablePrivate(this))
{
    Q_D(JXmlTable);
    d->init();
    d->data.filePath = filePath;
}

JXmlTable::~JXmlTable()
{
    Q_D(JXmlTable);
    d->deleteLater();
}

bool JXmlTable::loadConfig(const QString &filePath, const QString &tableName)
{
    Q_D(JXmlTable);

    clear();

    if (!d->loadConfig(filePath, tableName)) {
        return false;
    }

    return d->updateHeaders();
}

bool JXmlTable::loadConfig(const char *text, int size)
{
    return loadConfig(QByteArray(text, size));
}

bool JXmlTable::loadConfig(const QByteArray &text)
{
    Q_D(JXmlTable);

    clear();

    if (!d->loadConfig(text)) {
        return false;
    }

    return d->updateHeaders();
}

bool JXmlTable::loadConfig(const QDomElement *emTable)
{
    Q_ASSERT(emTable != 0);
    if (!emTable || emTable->isNull()) {
        return false;
    }

    Q_D(JXmlTable);
    return d->loadConfig(*emTable);
}

QString JXmlTable::version() const
{
    Q_D(const JXmlTable);
    return d->version();
}

QString JXmlTable::config() const
{
    Q_D(const JXmlTable);
    return d->data.filePath;
}

void JXmlTable::setConfig(const QString &filePath)
{
    Q_D(JXmlTable);
    d->setConfig(filePath);
}

QString JXmlTable::tableName() const
{
    Q_D(const JXmlTable);
    return d->data.tableName;
}

bool JXmlTable::setTableName(const QString &value)
{
    Q_D(JXmlTable);
    if (value != d->data.tableName) {
        return loadConfig(d->data.filePath, value);
    }

    return true;
}

QVariant JXmlTable::itemValue(int row, int column, int role) const
{
    if (row >= rowCount() || column >= columnCount()) {
        return QVariant::Invalid;
    }

    Q_D(const JXmlTable);
    return d->itemValue(row, column, role);
}

bool JXmlTable::setItemValue(int row, int column, const QVariant &value, int role)
{
    if (row >= rowCount() || column >= columnCount()) {
        return false;
    }

    Q_D(JXmlTable);
    return d->setItemValue(row, column, value, role);
}

QObject *JXmlTable::itemObject(int row, int column) const
{
    if (row >= rowCount() || column >= columnCount()) {
        return 0;
    }

    Q_D(const JXmlTable);
    return d->itemObject(row, column);
}

QVariant JXmlTable::itemProperty(int row, int column, const char *name) const
{
    if (row >= rowCount() || column >= columnCount()) {
        return QVariant::Invalid;
    }

    Q_D(const JXmlTable);
    return d->itemProperty(row, column, name);
}

void JXmlTable::setItemProperty(int row, int column, const char *name, const QVariant &value)
{
    if (row >= rowCount() || column >= columnCount()) {
        return;
    }

    Q_D(JXmlTable);
    d->setItemProperty(row, column, name, value);
}

Qt::Orientations JXmlTable::orientations() const
{
    Q_D(const JXmlTable);
    return d->data.orientations;
}

QByteArray JXmlTable::pack(int row, int column) const
{
    Q_D(const JXmlTable);
    QByteArray data(d->data.tableName.toLatin1());
    data.append('\0');
    return data.append(d->pack(row, column));
}

bool JXmlTable::unpack(const QByteArray &data, int row, int column)
{
    if (data.isEmpty()) {
        return false;
    }

    QString tableName(data);    // truncate string by '\0'
    if (!setTableName(tableName)) {
        return false;
    }

    QByteArray newData = data;
    newData.remove(0, tableName.size() + 1);    // include '\0'
    if (newData.isEmpty()) {
        return false;
    }

    Q_D(JXmlTable);
    return d->unpack(newData, row, column);
}

QVariant JXmlTable::unpackFromItem(const QByteArray &data, int row, int column) const
{
    Q_D(const JXmlTable);
    return d->unpackFromCell(data, row, column);
}

QByteArray JXmlTable::packFromItem(const QVariant &value, int row, int column) const
{
    Q_D(const JXmlTable);
    return d->packFromCell(value, row, column);
}

bool JXmlTable::setHighlight(const QColor &color, int row, int column)
{
    Q_D(JXmlTable);
    return d->setHighlight(color, row, column);
}

void JXmlTable::removeHighlight(int row, int column)
{
    Q_D(JXmlTable);
    d->setHighlight(QColor::Invalid, row, column);
}

bool JXmlTable::isReadOnly() const
{
    Q_D(const JXmlTable);
    return d->data.readOnly;
}

QObject *JXmlTable::configObject()
{
    Q_D(JXmlTable);
    return d;
}

void JXmlTable::setReadOnly(bool value)
{
    Q_D(JXmlTable);
    if (d->data.readOnly != value) {
        d->data.readOnly = value;
        Q_EMIT readOnlyChanged(value);
    }
}

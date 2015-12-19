#ifndef JXMLTABLE_P_H
#define JXMLTABLE_P_H

#include "../jwidgets_global.h"
#include "../jxmltable.h"
#include "jxmltable_data.h"
#include <QStyledItemDelegate>
#include <QDomDocument>

// - class JXmlTableData -

class JXmlTableData
{
public:
    JXmlTableData()
        : filePath("")
        , tableName("")
        , orientations(Qt::Horizontal)
        , headerSize(-1, -1)
        , styleSheet("")
        , horiHeaderMovable(true)
        , verticalHeaderVisible(true)
        , verticalHeaderLabel(false)
        , offsetEnabled(false)
        , readOnly(false)
        , sync(false)
    {

    }

    ~JXmlTableData()
    {
        clear();
    }

    void clear(bool all = true)
    {
        if (all) {
            tableName.clear();
        }
        styleSheet.clear();
        orientations = Qt::Horizontal;
        //
        QMapIterator<JValueBase *, QMap<int, JValueBase *> > iterRow(items);
        while (iterRow.hasNext()) {
            iterRow.next();
            QMapIterator<int, JValueBase *> iterColumn(iterRow.value());
            while (iterColumn.hasNext()) {
                iterColumn.next();
                delete iterColumn.value();
            }
            delete iterRow.key();
        }
        items.clear();
    }

    JValueBase *rowKey(int row, int &maxIndex) const
    {
        maxIndex = 0;
        QMapIterator<JValueBase *, QMap<int, JValueBase *> > iter(items);
        while (iter.hasNext()) {
            iter.next();
            JValueBase *valuseBase = iter.key();
            if (valuseBase->index() == row) {
                return valuseBase;
            }
            maxIndex = qMax(maxIndex, valuseBase->index());
        }

        return 0;
    }

    QDomDocument document;
    QString filePath;
    QString tableName;
    Qt::Orientations orientations;
    QSize headerSize;
    QString styleSheet;
    bool horiHeaderMovable;
    bool verticalHeaderVisible;
    bool verticalHeaderLabel;
    bool offsetEnabled;
    bool readOnly;
    bool sync;
    QMap<JValueBase *, QMap<int, JValueBase *> > items;
};

// - class JXmlTablePrivate -

class JXmlTablePrivate : public QObject
{
    Q_OBJECT
    J_DECLARE_PUBLIC(JXmlTable)
public:
    explicit JXmlTablePrivate(JXmlTable *parent);

    inline QString version() const { return "1.0"; }

    void setConfig(const QString &filePath);
    bool loadConfig(const QString &filePath, const QString &tableName);
    bool loadConfig(const QByteArray &text);
    bool loadConfig(const QDomElement &emTable);
    bool updateHeaders();

    QDomElement elementTable();
    QDomElement elementTable(const QString &filePath, const QString &tableName);
    QDomElement elementItem(int row, int column);
    QDomElement elementItem(QDomElement &emTable, int row, int column);
    QDomElement elementItem(const QString &name);
    QDomElement elementItem(QDomElement &emTable, const QString &name);
    QDomElement elementRow(int row);
    QDomElement elementRow(QDomElement &emTable, int row);
    QDomElement elementRow(QDomElement &emTable, const QString &name);
    QDomElement elementColumn(int column);
    QDomElement elementColumn(QDomElement &emTable, int column);
    QDomElement elementColumn(QDomElement &emTable, const QString &name);

    void saveHeaderSize(int logicalIndex, int newSize, Qt::Orientation orientation);
    void saveSection(int logicalIndex, int oldVisualIndex, int newVisualIndex);

    bool parse(const QDomElement &emTable);
    bool parse(const QDomElement &emItem, JValueBase &value);
    bool parse(const QDomElement &emItem, JItemValue &value);
    bool parse(const QDomElement &emItem, JBoolValue &value);
    bool parse(const QDomElement &emItem, JNumericValue &value);
    bool parse(const QDomElement &emItem, JStringValue &value);
    bool parse(const QDomElement &emItem, JEnumValue &value);
    bool parse(const QDomElement &emItem, JIPv4Value &value);
    bool parse(const QDomElement &emItem, JDelegateValue &value);
    bool parse(const QDomElement &emItem, JPictureDelegateValue &value);
    bool parse(const QDomElement &emItem, JProgressDelegateValue &value);
    template<typename T> inline T *parse(const QDomElement &emItem);

    QByteArray pack(int row = -1, int column = -1) const;
    bool unpack(const QByteArray &data, int row = -1, int column = -1);

    QVariant unpackFromCell(const QByteArray &data, int row, int column) const;
    QByteArray packFromCell(const QVariant &value, int row, int column) const;

    bool setHighlight(const QColor &color, int row = -1, int column = -1);
    void setCellHighlight(const QColor &color, int row, int column);

public Q_SLOTS:
    void _emit_sectionMoved(int logicalIndex, int oldVisualIndex, int newVisualIndex);
    void _emit_sectionResized(int logicalIndex, int oldSize, int newSize);

private:
    void init();

    JItemValue *itemData(int row, int column) const;

    QByteArray cellValue(int row, int column) const;
    bool setCellValue(int row, int column, const QByteArray &data);

    QByteArray rowValue(int row) const;
    bool setRowValue(int row, const QByteArray &data);

    QByteArray columnValue(int column) const;
    bool setColumnValue(int column, const QByteArray &data);

    QVariant itemValue(int row, int column, int role = Qt::DisplayRole) const;
    bool setItemValue(int row, int column, const QVariant &value, int role = Qt::EditRole);

    QObject *itemObject(int row, int column) const;
    QVariant itemProperty(int row, int column, const char *name) const;
    void setItemProperty(int row, int column, const char *name, const QVariant &value);

    JItemValue *createItem(const QDomElement &emItem, const std::string &type);
    void appendItem(JItemValue *value, JValueBase *rowValue);

    void connectSignals(Qt::Orientations orientations);
    void disconnectSignals(Qt::Orientations orientations);

    static QString nodePath(const QDomNode &node);

private:
    JXmlTableData data;
    friend class JXmlItemDelegate;
};

template<typename T> Q_INLINE_TEMPLATE
T *JXmlTablePrivate::parse(const QDomElement &emItem)
{
    T *value = new T(this);
    if (parse(emItem, *value)) {
        return value;
    } else {
        delete value;
        return 0;
    }
}

// - class JXmlItemDelegate -

class JXmlItemDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    explicit JXmlItemDelegate(QObject *parent = 0);
    ~JXmlItemDelegate();

    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    void setEditorData(QWidget *editor, const QModelIndex &index) const;
    void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const;
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;

    bool drawIPv4Item(QPainter *painter, const QStyleOptionViewItem &option,
                      const QModelIndex &index, const JIPv4Value *value) const;
    bool drawDelegateItem(QPainter *painter, const QStyleOptionViewItem &option,
                          const QModelIndex &index, const JDelegateValue *value) const;

private:
    JXmlTable *tableView;
    JXmlTablePrivate *tableViewPrivate;
};

#endif // JXMLTABLE_P_H

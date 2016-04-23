#ifndef JXMLTABLE_P_H
#define JXMLTABLE_P_H

#include "../jwidgets_global.h"
#include "../jxmltable.h"
#include "jxmltable_data.h"
#include <QStyledItemDelegate>
#include <QDomDocument>

// - class JXmlTableData -

class JXmlTableInvoke;

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
        , invoke(0)
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
    JXmlTableInvoke *invoke;
    QMap<JValueBase *, QMap<int, JValueBase *> > items;
};

// - class JXmlTablePrivate -

class JXmlTablePrivate : public QObject
{
    Q_OBJECT
    J_DECLARE_PUBLIC(JXmlTable)
    Q_PROPERTY(int headerWidth READ headerWidth WRITE setHeaderWidth NOTIFY headerWidthChanged)
    Q_PROPERTY(int headerHeight READ headerHeight WRITE setHeaderHeight NOTIFY headerHeightChanged)
    Q_PROPERTY(QSize headerSize READ headerSize WRITE setHeaderSize NOTIFY headerSizeChanged)
    Q_PROPERTY(QString styleSheet READ styleSheet WRITE setStyleSheet NOTIFY styleSheetChanged)
    Q_PROPERTY(bool horiHeaderMovable READ horiHeaderMovable WRITE setHoriHeaderMovable NOTIFY horiHeaderMovableChanged)
    Q_PROPERTY(bool verticalHeaderVisible READ verticalHeaderVisible WRITE setVerticalHeaderVisible NOTIFY verticalHeaderVisibleChanged)
    Q_PROPERTY(bool verticalHeaderLabel READ verticalHeaderLabel WRITE setVerticalHeaderLabel NOTIFY verticalHeaderLabelChanged)
    Q_PROPERTY(bool offsetEnabled READ offsetEnabled WRITE setOffsetEnabled NOTIFY offsetEnabledChanged)
    Q_PROPERTY(bool readOnly READ readOnly WRITE setReadOnly NOTIFY readOnlyChanged)
    Q_PROPERTY(bool sync READ sync WRITE setSync NOTIFY syncChanged)
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
    QDomElement elementItem(QDomElement emTable, int row, int column);
    QDomElement elementItem(const QString &name);
    QDomElement elementItem(QDomElement emTable, const QString &name);
    QDomElement elementRow(int row);
    QDomElement elementRow(QDomElement emTable, int row);
    QDomElement elementRow(QDomElement emTable, const QString &name);
    QDomElement elementColumn(int column);
    QDomElement elementColumn(QDomElement emTable, int column);
    QDomElement elementColumn(QDomElement emTable, const QString &name);

    void saveHeaderSize(int logicalIndex, int newSize, Qt::Orientation orientation);
    void saveSection(int logicalIndex, int oldVisualIndex, int newVisualIndex);

    bool parse(const QDomElement &emTable);
    bool parse(const QDomElement &emItem, JValueBase &value);
    bool parse(const QDomElement &emItem, JItemValue &value);
    bool parse(const QDomElement &emItem, JBoolValue &value);
    bool parse(const QDomElement &emItem, JNumericValue &value);
    bool parse(const QDomElement &emItem, JStringValue &value);
    bool parse(const QDomElement &emItem, JEnumValue &value);
    bool parse(const QDomElement &emItem, JDateTimeValue &value);
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

    int headerWidth() const;
    int headerHeight() const;
    QSize headerSize() const;
    QString styleSheet() const;
    bool horiHeaderMovable() const;
    bool verticalHeaderVisible() const;
    bool verticalHeaderLabel() const;
    bool offsetEnabled() const;
    bool readOnly() const;
    bool sync() const;

Q_SIGNALS:
    void headerWidthChanged(int);
    void headerHeightChanged(int);
    void headerSizeChanged(const QSize &);
    void styleSheetChanged(const QString &);
    void horiHeaderMovableChanged(bool);
    void verticalHeaderVisibleChanged(bool);
    void verticalHeaderLabelChanged(bool);
    void offsetEnabledChanged(bool);
    void readOnlyChanged(bool);
    void syncChanged(bool);

public Q_SLOTS:
    void setHeaderWidth(int value);
    void setHeaderHeight(int value);
    void setHeaderSize(const QSize &value);
    void setStyleSheet(const QString &value);
    void setHoriHeaderMovable(bool value);
    void setVerticalHeaderVisible(bool value);
    void setVerticalHeaderLabel(bool value);
    void setOffsetEnabled(bool value);
    void setReadOnly(bool value);
    void setSync(bool value);

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
    friend class JXmlTableInvoke;
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

// - class JXmlTableInvoke -

class JXmlTableInvoke : public QObject
{
    Q_OBJECT
public:
    explicit JXmlTableInvoke(JXmlTablePrivate *viewPri, QObject *parent = 0);

    Q_INVOKABLE bool execAdd(bool exec = false);
    Q_INVOKABLE bool execModify(int index);

private:
    bool execDialog(int index = -1);

private:
    QScopedPointer<JXmlTablePrivate> viewPri;
    QScopedPointer<JXmlTable> view;
};

#endif // JXMLTABLE_P_H

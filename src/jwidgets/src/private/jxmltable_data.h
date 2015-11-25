#ifndef JXMLTABLE_DATA_H
#define JXMLTABLE_DATA_H

#include "../jwidgets_global.h"
#include <QObject>
#include <QDebug>

// - enum JValueType -

enum JValueType
{
    InvalidValue = -1,
    BoolValue,
    NumericValue,
    StringValue,
    EnumValue,
    IPv4Value,
    IPv6Value,
    DelegateValue,
};

extern JValueType jvaluetype_type_of_string(const std::string &string);

// - enum JDelegateType

enum JDelegateType
{
    InvalidDelegate = -1,
    PictureDelegate,
    ProgressDelegate,
};

extern JDelegateType jdelegatetype_type_of_string(const std::string &string);

// - enum JUserRole

enum JUserType
{
    UserRoleStart = Qt::UserRole + 0xffff,
    UserRoleItemValue,
    UserRoleItemType,
    UserRoleDelegateType,
    UserRoleItemData,
};

// - enum JOrientation

enum JOrientation { JCrossOrientation = 0x3 };

// - class JValueBase -

class JValueBaseData;

class JWIDGETS_EXPORT JValueBase : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int index READ index WRITE setIndex NOTIFY indexChanged)
    Q_PROPERTY(int offset READ offset WRITE setOffset NOTIFY offsetChanged)
    Q_PROPERTY(int headerSize READ headerSize WRITE setHeaderSize NOTIFY headerSizeChanged)
    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
    Q_PROPERTY(int type READ type)
    Q_PROPERTY(int filterType READ filterType)
public:
    explicit JValueBase(QObject *parent = 0);
    JValueBase(JValueType type, QObject *parent = 0);
    JValueBase(int index, QObject *parent = 0);
    virtual ~JValueBase();

    QString toString() const;

    int index() const;
    int offset() const;
    int headerSize() const;
    QString name() const;

    int type() const;
    void setType(int value);

    int filterType() const;
    void setFilterType(int value);

Q_SIGNALS:
    void indexChanged(int);
    void offsetChanged(int);
    void headerSizeChanged(int);
    void nameChanged(const QString &);

public Q_SLOTS:
    void setIndex(int value);
    void setOffset(int value);
    void setHeaderSize(int value);
    void setName(const QString &value);

private:
    Q_DISABLE_COPY(JValueBase)
    JValueBaseData *d;
};

// - class JItemValue -

class JItemValueData;

class JWIDGETS_EXPORT JItemValue : public JValueBase
{
    Q_OBJECT
    Q_PROPERTY(bool enabled READ enabled WRITE setEnabled NOTIFY enabledChanged)
    Q_PROPERTY(bool editable READ editable WRITE setEditable NOTIFY editableChanged)
    Q_PROPERTY(quint16 size READ size WRITE setSize NOTIFY sizeChanged)
    Q_PROPERTY(QVariant original READ original WRITE setOriginal NOTIFY originalChanged)
    Q_PROPERTY(Qt::Alignment alignment READ alignment WRITE setAlignment NOTIFY alignmentChanged)
    Q_PROPERTY(QString styleSheet READ styleSheet WRITE setStyleSheet NOTIFY styleSheetChanged)
    Q_PROPERTY(QString icon READ icon WRITE setIcon NOTIFY iconChanged)
    Q_PROPERTY(QString tooltip READ tooltip WRITE setTooltip NOTIFY tooltipChanged)
    Q_PROPERTY(QString placeHoldText READ placeHoldText WRITE setPlaceHoldText NOTIFY placeHoldTextChanged)
    Q_PROPERTY(QString whatsThis READ whatsThis WRITE setWhatsThis NOTIFY whatsThisChanged)
public:
    explicit JItemValue(QObject *parent = 0);
    JItemValue(JValueType type, const QVariant &original, QObject *parent = 0);
    virtual ~JItemValue();

    virtual QByteArray pack(const QVariant &value) const = 0;
    virtual QVariant unpack(const QByteArray &data) const = 0;

    virtual QVariant jfromValue(const QVariant &value, int role = Qt::EditRole) const = 0;
    virtual QVariant jtoValue(const QVariant &value, int role = Qt::DisplayRole) const = 0;

    virtual QWidget *createInstance(JItemValue *itemValue, QWidget *parent = 0) const = 0;

    bool enabled() const;
    bool editable() const;
    quint16 size() const;
    QVariant original() const;
    Qt::Alignment alignment() const;
    QString styleSheet() const;
    QString icon() const;
    QString tooltip() const;
    QString placeHoldText() const;
    QString whatsThis() const;

Q_SIGNALS:
    void enabledChanged(bool);
    void editableChanged(bool);
    void sizeChanged(quint16);
    void originalChanged(const QVariant &);
    void alignmentChanged(Qt::Alignment);
    void styleSheetChanged(const QString &);
    void iconChanged(const QString &);
    void tooltipChanged(const QString &);
    void placeHoldTextChanged(const QString &);
    void whatsThisChanged(const QString &);

public Q_SLOTS:
    void setEnabled(bool value);
    void setEditable(bool value);
    void setSize(quint16 value);
    void setOriginal(const QVariant &value);
    void setAlignment(Qt::Alignment value);
    void setStyleSheet(const QString &value);
    void setIcon(const QString &value);
    void setTooltip(const QString &value);
    void setPlaceHoldText(const QString &value);
    void setWhatsThis(const QString &value);

private:
    Q_DISABLE_COPY(JItemValue)
    JItemValueData *d;
};

// - class JBoolValue -

class JBoolValueData;

class JWIDGETS_EXPORT JBoolValue : public JItemValue
{
    Q_OBJECT
    Q_PROPERTY(bool checkable READ checkable WRITE setCheckable NOTIFY checkableChanged)
    Q_PROPERTY(QString text READ text WRITE setText NOTIFY textChanged)
public:
    explicit JBoolValue(QObject *parent = 0);
    virtual ~JBoolValue();

    QByteArray pack(const QVariant &value) const;
    QVariant unpack(const QByteArray &data) const;

    QVariant jfromValue(const QVariant &value, int role = Qt::EditRole) const;
    QVariant jtoValue(const QVariant &value, int role = Qt::DisplayRole) const;

    QWidget *createInstance(JItemValue *itemValue, QWidget *parent) const;

    bool checkable() const;
    QString text() const;

Q_SIGNALS:
    void checkableChanged(bool);
    void textChanged(const QString &);

public Q_SLOTS:
    void setCheckable(bool value);
    void setText(const QString &value);

private:
    Q_DISABLE_COPY(JBoolValue)
    JBoolValueData *d;
};

// - class JNumericRange -

class JNumericRangeData;

class JWIDGETS_EXPORT JNumericRange : public QObject
{
    Q_OBJECT
    Q_PROPERTY(qreal minimum READ minimum WRITE setMinimum NOTIFY minimumChanged)
    Q_PROPERTY(qreal maximum READ maximum WRITE setMaximum NOTIFY maximumChanged)
    Q_PROPERTY(bool minimumEdge READ minimumEdge WRITE setMinimumEdge NOTIFY minimumEdgeChanged)
    Q_PROPERTY(bool maximumEdge READ maximumEdge WRITE setMaximumEdge NOTIFY maximumEdgeChanged)
    Q_PROPERTY(QString text READ toString)
public:
    explicit JNumericRange(QObject *parent = 0);
    JNumericRange(qreal minimum, qreal maximum, bool minimumEdge, bool maximumEdge, QObject *parent = 0);
    virtual ~JNumericRange();

    bool isValid() const;
    qreal delta() const;
    QString toString() const;

    bool parse(const QString &text);

    qreal minimum() const;
    qreal maximum() const;
    bool minimumEdge() const;
    bool maximumEdge() const;

Q_SIGNALS:
    void minimumChanged(qreal);
    void maximumChanged(qreal);
    void minimumEdgeChanged(bool);
    void maximumEdgeChanged(bool);

public Q_SLOTS:
    void setMinimum(qreal value);
    void setMaximum(qreal value);
    void setMinimumEdge(bool value);
    void setMaximumEdge(bool value);

private:
    Q_DISABLE_COPY(JNumericRange)
    JNumericRangeData *d;
};

Q_INLINE_TEMPLATE
QDebug operator << (QDebug dbg, const JNumericRange &range)
{
    dbg.nospace() << range.toString() << endl;
    return dbg;
}

// - class JNumericValue -

class JNumericValueData;

class JWIDGETS_EXPORT JNumericValue : public JItemValue
{
    Q_OBJECT
    Q_PROPERTY(QString prefix READ prefix WRITE setPrefix NOTIFY prefixChanged)
    Q_PROPERTY(QString suffix READ suffix WRITE setSuffix NOTIFY suffixChanged)
    Q_PROPERTY(QChar fillChar READ fillChar WRITE setFillChar NOTIFY fillCharChanged)
    Q_PROPERTY(int radix READ radix WRITE setRadix NOTIFY radixChanged)
    Q_PROPERTY(int maskCount READ maskCount WRITE setMaskCount NOTIFY maskCountChanged)
    Q_PROPERTY(int decimals READ decimals WRITE setDecimals NOTIFY decimalsChanged)
    Q_PROPERTY(qreal step READ step WRITE setStep NOTIFY stepChanged)
    Q_PROPERTY(qreal scale READ scale WRITE setScale NOTIFY scaleChanged)
    Q_PROPERTY(QObject* range READ range)
public:
    explicit JNumericValue(QObject *parent = 0);
    virtual ~JNumericValue();

    bool isInteger() const;
    QString toString(const QVariant &value) const;
    bool inRange(qreal &value, bool increase = false) const;
    bool without(qreal &value, bool increase);

    QByteArray pack(const QVariant &value) const;
    QVariant unpack(const QByteArray &data) const;

    QVariant jfromValue(const QVariant &value, int role = Qt::EditRole) const;
    QVariant jtoValue(const QVariant &value, int role = Qt::DisplayRole) const;

    QWidget *createInstance(JItemValue *itemValue, QWidget *parent) const;

    QString prefix() const;
    QString suffix() const;
    QChar fillChar() const;
    int radix() const;
    int maskCount() const;
    int decimals() const;
    qreal step() const;
    qreal scale() const;
    JNumericRange *range() const;

    QList<JNumericRange *> &withouts() const;

    Q_INVOKABLE QObject *withoutAt(int index) const;
    Q_INVOKABLE void addWithout(qreal minimum, qreal maximum, bool minimumEdge, bool maximumEdge);
    Q_INVOKABLE void removeWithoutAt(int index);
    Q_INVOKABLE void clearWithout();

Q_SIGNALS:
    void prefixChanged(const QString &);
    void suffixChanged(const QString &);
    void fillCharChanged(const QChar &);
    void radixChanged(int);
    void maskCountChanged(int);
    void decimalsChanged(int);
    void stepChanged(qreal);
    void scaleChanged(qreal);

public Q_SLOTS:
    void setPrefix(const QString &value);
    void setSuffix(const QString &value);
    void setFillChar(const QChar &value);
    void setRadix(int value);
    void setMaskCount(int value);
    void setDecimals(int value);
    void setStep(qreal value);
    void setScale(qreal value);

private:
    Q_DISABLE_COPY(JNumericValue)
    JNumericValueData *d;
};

// - class JStringValue -

class JStringValueData;

class JWIDGETS_EXPORT JStringValue : public JItemValue
{
    Q_OBJECT
    Q_PROPERTY(bool upper READ upper WRITE setUpper NOTIFY upperChanged)
    Q_PROPERTY(bool multiline READ multiline WRITE setMultiline NOTIFY multilineChanged)
    Q_PROPERTY(bool containCSS READ containCSS WRITE setContainCSS NOTIFY containCSSChanged)
    Q_PROPERTY(int maxLength READ maxLength WRITE setMaxLength NOTIFY maxLengthChanged)
    Q_PROPERTY(QString pattern READ pattern WRITE setPattern NOTIFY patternChanged)
public:
    explicit JStringValue(QObject *parent = 0);
    virtual ~JStringValue();

    QByteArray pack(const QVariant &value) const;
    QVariant unpack(const QByteArray &data) const;

    QVariant jfromValue(const QVariant &value, int role = Qt::EditRole) const;
    QVariant jtoValue(const QVariant &value, int role = Qt::DisplayRole) const;

    QWidget *createInstance(JItemValue *itemValue, QWidget *parent) const;

    bool upper() const;
    bool multiline() const;
    bool containCSS() const;
    int maxLength() const;
    QString pattern() const;

Q_SIGNALS:
    void upperChanged(bool);
    void multilineChanged(bool);
    void containCSSChanged(bool);
    void maxLengthChanged(int);
    QString patternChanged(const QString &);

public Q_SLOTS:
    void setUpper(bool value);
    void setMultiline(bool value);
    void setContainCSS(bool value);
    void setMaxLength(int value);
    void setPattern(const QString &value);

private:
    Q_DISABLE_COPY(JStringValue)
    JStringValueData *d;
};

// - class JEnumOption -

class JEnumOptionData;

class JWIDGETS_EXPORT JEnumOption : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString text READ text WRITE setText NOTIFY textChanged)
    Q_PROPERTY(qulonglong encode READ encode WRITE setEncode NOTIFY encodeChanged)
public:
    explicit JEnumOption(QObject *parent = 0);
    JEnumOption(const QString &text, qulonglong encode, QObject *parent = 0);
    virtual ~JEnumOption();

    QString text() const;
    qulonglong encode() const;

Q_SIGNALS:
    void textChanged(const QString &);
    void encodeChanged(qulonglong);

public Q_SLOTS:
    void setText(const QString &value);
    void setEncode(qulonglong value);

private:
    Q_DISABLE_COPY(JEnumOption)
    JEnumOptionData *d;
};

// - class JEnumValue -

class JEnumValueData;

class JWIDGETS_EXPORT JEnumValue : public JItemValue
{
    Q_OBJECT
    Q_PROPERTY(int indexOffset READ indexOffset WRITE setIndexOffset NOTIFY indexOffsetChanged)
    Q_PROPERTY(bool autoCompletion READ autoCompletion WRITE setAutoCompletion NOTIFY autoCompletionChanged)
    Q_PROPERTY(bool duplicatesEnabled READ duplicatesEnabled WRITE setDuplicatesEnabled NOTIFY duplicatesEnabledChanged)
    Q_PROPERTY(bool multiable READ multiable WRITE setMultiable NOTIFY multiableChanged)
public:
    explicit JEnumValue(QObject *parent = 0);
    virtual ~JEnumValue();

    QStringList items() const;

    QString text(int index) const;
    QString text(qulonglong encode) const;

    qulonglong encode(int index) const;
    qulonglong encode(const QString &text) const;

    int index(qulonglong encode) const;
    int index(const QString &text) const;

    QByteArray pack(const QVariant &value) const;
    QVariant unpack(const QByteArray &data) const;

    QVariant jfromValue(const QVariant &value, int role = Qt::EditRole) const;
    QVariant jtoValue(const QVariant &value, int role = Qt::DisplayRole) const;

    QWidget *createInstance(JItemValue *itemValue, QWidget *parent) const;

    int indexOffset() const;
    bool autoCompletion() const;
    bool duplicatesEnabled() const;
    bool multiable() const;

    QList<JEnumOption *> &options() const;

    Q_INVOKABLE QObject *optionAt(int index) const;
    Q_INVOKABLE void insertOption(int index, const QString &text, qulonglong encode);
    Q_INVOKABLE void appendOption(const QString &text, qulonglong encode);
    Q_INVOKABLE void appendOption(const QString &text);
    Q_INVOKABLE void removeOptionAt(int index);
    Q_INVOKABLE void clearOption();

Q_SIGNALS:
    void indexOffsetChanged(int);
    void autoCompletionChanged(bool);
    void duplicatesEnabledChanged(bool);
    void multiableChanged(bool);

public Q_SLOTS:
    void setIndexOffset(int value);
    void setAutoCompletion(bool value);
    void setDuplicatesEnabled(bool value);
    void setMultiable(bool value);

private:
    Q_DISABLE_COPY(JEnumValue)
    JEnumValueData *d;
};

// - class JIPv4Value -

class JIPv4ValueData;

class  JWIDGETS_EXPORT JIPv4Value : public JItemValue
{
    Q_OBJECT
public:
    explicit JIPv4Value(QObject *parent = 0);
    virtual ~JIPv4Value();

    QByteArray pack(const QVariant &value) const;
    QVariant unpack(const QByteArray &data) const;

    QVariant jfromValue(const QVariant &value, int role = Qt::EditRole) const;
    QVariant jtoValue(const QVariant &value, int role = Qt::DisplayRole) const;

    QWidget *createInstance(JItemValue *itemValue, QWidget *parent) const;

Q_SIGNALS:

public Q_SLOTS:

private:
    Q_DISABLE_COPY(JIPv4Value)
    JIPv4ValueData *d;
};

// - class JDelegateValue -

class JDelegateValueData;

class JWIDGETS_EXPORT JDelegateValue : public JItemValue
{
    Q_OBJECT
    Q_PROPERTY(int delegateType READ delegateType WRITE setDelegateType NOTIFY delegateTypeChanged)
public:
    explicit JDelegateValue(QObject *parent = 0);
    JDelegateValue(JDelegateType delegateType, const QVariant &original, QObject *parent = 0);
    virtual ~JDelegateValue();

    QByteArray pack(const QVariant &value) const;
    QVariant unpack(const QByteArray &data) const;

    QVariant jfromValue(const QVariant &value, int role = Qt::EditRole) const;
    QVariant jtoValue(const QVariant &value, int role = Qt::DisplayRole) const;

    QWidget *createInstance(JItemValue *itemValue, QWidget *parent) const;

    int delegateType() const;

Q_SIGNALS:
    void delegateTypeChanged(int);

public Q_SLOTS:
    void setDelegateType(int value);

private:
    Q_DISABLE_COPY(JDelegateValue)
    JDelegateValueData *d;
};

// - class JPictureDelegateValue -

class JPictureDelegateValueData;

class JWIDGETS_EXPORT JPictureDelegateValue : public JDelegateValue
{
    Q_OBJECT
public:
    explicit JPictureDelegateValue(QObject *parent = 0);
    virtual ~JPictureDelegateValue();

    QByteArray pack(const QVariant &value) const;
    QVariant unpack(const QByteArray &data) const;

    QVariant jfromValue(const QVariant &value, int role = Qt::EditRole) const;
    QVariant jtoValue(const QVariant &value, int role = Qt::DisplayRole) const;

    QWidget *createInstance(JItemValue *itemValue, QWidget *parent) const;

Q_SIGNALS:

public Q_SLOTS:

private:
    Q_DISABLE_COPY(JPictureDelegateValue)
    JPictureDelegateValueData *d;
};

// - JProgressDelegateValue -

class JProgressDelegateValueData;

class JWIDGETS_EXPORT JProgressDelegateValue : public JDelegateValue
{
    Q_OBJECT
    Q_PROPERTY(qreal minimum READ minimum WRITE setMinimum NOTIFY minimumChanged)
    Q_PROPERTY(qreal maximum READ maximum WRITE setMaximum NOTIFY maximumChanged)
    Q_PROPERTY(QString text READ text WRITE setText NOTIFY textChanged)
    Q_PROPERTY(bool textVisible READ textVisible WRITE setTextVisible NOTIFY textVisibleChanged)
public:
    explicit JProgressDelegateValue(QObject *parent = 0);
    virtual ~JProgressDelegateValue();

    QByteArray pack(const QVariant &value) const;
    QVariant unpack(const QByteArray &data) const;

    QVariant jfromValue(const QVariant &value, int role = Qt::EditRole) const;
    QVariant jtoValue(const QVariant &value, int role = Qt::DisplayRole) const;

    QWidget *createInstance(JItemValue *itemValue, QWidget *parent) const;

    qreal minimum() const;
    qreal maximum() const;
    QString text() const;
    bool textVisible() const;

Q_SIGNALS:
    void minimumChanged(qreal);
    void maximumChanged(qreal);
    void textChanged(const QString &);
    void textVisibleChanged(bool);

public Q_SLOTS:
    void setMinimum(qreal value);
    void setMaximum(qreal value);
    void setText(const QString &value);
    void setTextVisible(bool value);

private:
    Q_DISABLE_COPY(JProgressDelegateValue)
    JProgressDelegateValueData *d;
};

#endif // JXMLTABLE_DATA_H

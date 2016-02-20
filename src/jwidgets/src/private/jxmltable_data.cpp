#include "precomp.h"
#include "jxmltable_data.h"
#include "jxmltable_widget.h"

#ifndef _countof
#define _countof(a) sizeof(a) / sizeof(a[0])
#endif

JValueType jvaluetype_type_of_string(const std::string &string)
{
    if (string.empty()) {
        return InvalidValue;
    }

    typedef std::map<const std::string, JValueType> map_string_type;
    static const map_string_type::value_type map_init_data[] = {
        map_string_type::value_type("bool", BoolValue),
        map_string_type::value_type("numeric", NumericValue),
        map_string_type::value_type("string", StringValue),
        map_string_type::value_type("enum", EnumValue),
        map_string_type::value_type("IPv4", IPv4Value),
        map_string_type::value_type("delegate", DelegateValue),
    };
    static const map_string_type a_map(map_init_data, map_init_data + _countof(map_init_data));

    map_string_type::const_iterator iter =  a_map.find(string);
    if (iter != a_map.end()) {
        return iter->second;
    } else {
        return InvalidValue;
    }
}

JDelegateType jdelegatetype_type_of_string(const std::string &string)
{
    if (string.empty()) {
        return InvalidDelegate;
    }

    typedef std::map<const std::string, JDelegateType> map_string_type;
    static const map_string_type::value_type map_init_data[] = {
        map_string_type::value_type("picture", PictureDelegate),
        map_string_type::value_type("progress", ProgressDelegate),
    };
    static const map_string_type a_map(map_init_data, map_init_data + _countof(map_init_data));

    map_string_type::const_iterator iter = a_map.find(string);
    if (iter != a_map.end()) {
        return iter->second;
    } else {
        return InvalidDelegate;
    }
}

// - class JValueBaseData -

class JValueBaseData
{
public:
    JValueBaseData()
        : index(-1)
        , offset(-1)
        , headerSize(-1)
        , name("")
        , type(InvalidValue)
        , filterType(InvalidValue)
    {

    }

    int index;
    int offset;
    int headerSize;
    QString name;
    int type;
    int filterType;
};

// - class JValueBase -

JValueBase::JValueBase(QObject *parent)
    : QObject(parent)
    , d(new JValueBaseData)
{

}

JValueBase::JValueBase(JValueType type, QObject *parent)
    : QObject(parent)
    , d(new JValueBaseData)
{
    d->type = type;
}

JValueBase::JValueBase(int index, QObject *parent)
    : QObject(parent)
    , d(new JValueBaseData)
{
    d->index = index;
}

JValueBase::~JValueBase()
{
    delete d;
}

QString JValueBase::toString() const
{
    return metaObject()->className();
}

int JValueBase::index() const
{
    return d->index;
}

int JValueBase::offset() const
{
    return d->offset;
}

int JValueBase::headerSize() const
{
    return d->headerSize;
}

QString JValueBase::name() const
{
    return d->name;
}

int JValueBase::type() const
{
    return d->type;
}

void JValueBase::setType(int value)
{
    if (value != d->index) {
        d->index = value;
        Q_EMIT indexChanged(value);
    }
}

int JValueBase::filterType() const
{
    return d->filterType;
}

void JValueBase::setFilterType(int value)
{
    if (value != d->filterType) {
        d->filterType = value;
    }
}

void JValueBase::setIndex(int value)
{
    if (value != d->index) {
        d->index = value;
        Q_EMIT indexChanged(value);
    }
}

void JValueBase::setOffset(int value)
{
    if (value != d->offset) {
        d->offset = value;
        Q_EMIT offsetChanged(value);
    }
}

void JValueBase::setHeaderSize(int value)
{
    if (value != d->headerSize) {
        d->headerSize = value;
        Q_EMIT headerSizeChanged(value);
    }
}

void JValueBase::setName(const QString &value)
{
    if (value != d->name) {
        d->name = value;
        Q_EMIT nameChanged(value);
    }
}

// - class JItemValueData -

class JItemValueData
{
public:
    JItemValueData()
        : enabled(true)
        , editable(true)
        , size(1)
        , original(QVariant::Invalid)
        , alignment(Qt::AlignVCenter)
        , styleSheet("")
        , tooltip("")
        , icon("")
        , placeHoldText("")
        , whatsThis("")
    {

    }

    bool enabled;
    bool editable;
    quint16 size;
    QVariant original;
    Qt::Alignment alignment;
    QString styleSheet;
    QString tooltip;
    QString icon;
    QString placeHoldText;
    QString whatsThis;
};

// - class JItemValue -

JItemValue::JItemValue(QObject *parent)
    : JValueBase(parent)
    , d(new JItemValueData)
{

}

JItemValue::JItemValue(JValueType type, const QVariant &original, QObject *parent)
    : JValueBase(type, parent)
    , d(new JItemValueData)
{
    d->original = original;
}

JItemValue::~JItemValue()
{
    delete d;
}

bool JItemValue::enabled() const
{
    return d->enabled;
}

bool JItemValue::editable() const
{
    return d->editable;
}

quint16 JItemValue::size() const
{
    return d->size;
}

QVariant JItemValue::original() const
{
    return d->original;
}

Qt::Alignment JItemValue::alignment() const
{
    return d->alignment;
}

QString JItemValue::styleSheet() const
{
    return d->styleSheet;
}

QString JItemValue::icon() const
{
    return d->icon;
}

QString JItemValue::tooltip() const
{
    return d->tooltip;
}

QString JItemValue::placeHoldText() const
{
    return d->placeHoldText;
}

QString JItemValue::whatsThis() const
{
    return d->whatsThis;
}

void JItemValue::setEnabled(bool value)
{
    if (value != d->enabled) {
        d->enabled = value;
        Q_EMIT enabledChanged(value);
    }
}

void JItemValue::setEditable(bool value)
{
    if (value != d->editable) {
        d->editable = value;
        Q_EMIT editableChanged(value);
    }
}

void JItemValue::setSize(quint16 value)
{
    if (value != d->size) {
        d->size = value;
        Q_EMIT sizeChanged(value);
    }
}

void JItemValue::setOriginal(const QVariant &value)
{
    if (value != d->original) {
        d->original = value;
        Q_EMIT originalChanged(value);
    }
}

void JItemValue::setAlignment(Qt::Alignment value)
{
    if (value != d->alignment) {
        d->alignment = value;
        Q_EMIT alignmentChanged(value);
    }
}

void JItemValue::setStyleSheet(const QString &value)
{
    if (value != d->styleSheet) {
        d->styleSheet = value;
        Q_EMIT styleSheetChanged(value);
    }
}

void JItemValue::setIcon(const QString &value)
{
    if (value != d->icon) {
        d->icon = value;
        Q_EMIT iconChanged(value);
    }
}

void JItemValue::setTooltip(const QString &value)
{
    if (value != d->tooltip) {
        d->tooltip = value;
        Q_EMIT tooltipChanged(value);
    }
}

void JItemValue::setPlaceHoldText(const QString &value)
{
    if (value != d->placeHoldText) {
        d->placeHoldText = value;
        Q_EMIT placeHoldTextChanged(value);
    }
}

void JItemValue::setWhatsThis(const QString &value)
{
    if (value != d->whatsThis) {
        d->whatsThis = value;
        Q_EMIT whatsThisChanged(value);
    }
}

// - class JBoolValueData -

class JBoolValueData
{
public:
    JBoolValueData()
        : checkable(true)
        , text("")
    {

    }

    bool checkable;
    QString text;
};

// - class JBoolValue -

JBoolValue::JBoolValue(QObject *parent)
    : JItemValue(BoolValue, Qt::Unchecked, parent)
    , d(new JBoolValueData)
{

}

JBoolValue::~JBoolValue()
{
    delete d;
}

QByteArray JBoolValue::pack(const QVariant &value) const
{
    QByteArray _buffer;
    // size
    quint16 _size = size();
    _buffer.append((const char *)&_size, 2);
    // data
    _buffer.append(QChar(value.toInt()));
    return _buffer;
}

QVariant JBoolValue::unpack(const QByteArray &data) const
{
    if (data.size() < 3 ) {
        return QVariant::Invalid;
    }

    quint16 _size = 0;
    memcpy(&_size, data, 2);
    if (_size != 1) {
        return QVariant::Invalid;
    }

    return Qt::CheckState(data.at(2));
}

QVariant JBoolValue::jfromValue(const QVariant &value, int) const
{
    return Qt::CheckState(value.toInt());
}

QVariant JBoolValue::jtoValue(const QVariant &value, int) const
{
    return Qt::CheckState(value.toInt());
}

QWidget *JBoolValue::createInstance(JItemValue *itemValue, QWidget *parent) const
{
//    return new JBoolBox(static_cast<JBoolValue *>(itemValue), parent);
    Q_UNUSED(itemValue);
    Q_UNUSED(parent);
    return 0;
}

bool JBoolValue::checkable() const
{
    return d->checkable;
}

QString JBoolValue::text() const
{
    return d->text;
}

void JBoolValue::setCheckable(bool value)
{
    if (value != d->checkable) {
        d->checkable = value;
        Q_EMIT checkableChanged(value);
    }
}

void JBoolValue::setText(const QString &value)
{
    if (value != d->text) {
        d->text = value;
        Q_EMIT textChanged(value);
    }
}

// - class JNumericRangeData -

class JNumericRangeData
{
public:
    JNumericRangeData()
        : minimum(0.)
        , maximum(1.)
        , minimumEdge(true)
        , maximumEdge(true)
    {

    }

    qreal minimum;
    qreal maximum;
    bool minimumEdge;
    bool maximumEdge;
};

// - class JNumericRange -

JNumericRange::JNumericRange(QObject *parent)
    : QObject(parent)
    , d(new JNumericRangeData)
{

}

JNumericRange::JNumericRange(qreal minimum, qreal maximum, bool minimumEdge, bool maximumEdge, QObject *parent)
    : QObject(parent)
    , d(new JNumericRangeData)
{
    d->minimum = minimum;
    d->maximum = maximum;
    d->minimumEdge = minimumEdge;
    d->maximumEdge = maximumEdge;
}

JNumericRange::~JNumericRange()
{
    delete d;
}

bool JNumericRange::isValid() const
{
    return d->minimum <= d->maximum;
}

qreal JNumericRange::delta() const
{
    return d->maximum - d->minimum;
}

QString JNumericRange::toString() const
{
    return QString("%1%2, %3%4")
            .arg(d->minimumEdge ? '[' : '(')
            .arg(d->minimum)
            .arg(d->maximum)
            .arg(d->maximumEdge ? ']' : ')');
}

bool JNumericRange::parse(const QString &text)
{
    if (text.isEmpty()) {
        return false;
    }

    static const QString patternDigit = "[+-]?((-?\\d+)(\\.\\d+)?)";
    static const QString patternAscii = "[A-Za-z]";

    QString copy = text.trimmed();
    bool isDigit = false;
    if (QRegExp("^[\\[\\(]" + patternDigit + "[ ]*,[ ]*"
                + patternDigit + "[\\]\\)]$").exactMatch(copy)) {
        isDigit = true;
    } else if (!QRegExp("^[\\[\\(]" + patternAscii + "[ ]*,[ ]*"
                       + patternAscii + "[\\]\\)$").exactMatch(copy)) {
        return false;
    }

    d->minimumEdge = ('[' == copy.at(0));
    d->maximumEdge = (']' == copy.at(copy.count() - 1));

    copy.remove(QRegExp("[ \\[\\]\\(\\)]"));

    if (isDigit) {
        QTextStream(&copy.replace(',', ' ')) >> d->minimum >> d->maximum;
    } else {
        const QChar *data = copy.remove(',').constData();
        d->minimum = data[0].toLatin1();
        d->maximum = data[1].toLatin1();
    }

    if (d->minimum > d->maximum) {
        return false;
    }

    return true;
}

qreal JNumericRange::minimum() const
{
    return d->minimum;
}

qreal JNumericRange::maximum() const
{
    return d->maximum;
}

bool JNumericRange::minimumEdge() const
{
    return d->minimumEdge;
}

bool JNumericRange::maximumEdge() const
{
    return d->maximumEdge;
}

void JNumericRange::setMinimum(qreal value)
{
    if (value != d->minimum) {
        d->minimum = value;
        Q_EMIT minimumChanged(value);
    }
}

void JNumericRange::setMaximum(qreal value)
{
    if (value != d->maximum) {
        d->maximum = value;
        Q_EMIT maximumChanged(value);
    }
}

void JNumericRange::setMinimumEdge(bool value)
{
    if (value != d->minimumEdge) {
        d->minimumEdge = value;
        Q_EMIT minimumEdgeChanged(value);
    }
}

void JNumericRange::setMaximumEdge(bool value)
{
    if (value != d->maximumEdge) {
        d->maximumEdge = value;
        Q_EMIT maximumEdgeChanged(value);
    }
}

// - class JNumericValueData -

class JNumericValueData
{
public:
    JNumericValueData()
        : prefix("")
        , suffix("")
        , fillChar()
        , radix(10)
        , maskCount(0)
        , decimals(0)
        , step(0.)
        , scale(1.)
        , range(0)
    {

    }

    QString prefix;
    QString suffix;
    QChar fillChar;
    int radix;
    int maskCount;
    int decimals;
    qreal step;
    qreal scale;
    JNumericRange *range;
    QList<JNumericRange *> withouts;
};

// - class JNumericValue -

JNumericValue::JNumericValue(QObject *parent)
    : JItemValue(NumericValue, 0, parent)
    , d(new JNumericValueData)
{
    d->range = new JNumericRange(this);
}

JNumericValue::~JNumericValue()
{
    delete d;
}

bool JNumericValue::isInteger() const
{
    return d->decimals == 0;
}

QString JNumericValue::toString(const QVariant &value) const
{
    QString str = value.toString();
    if (!d->prefix.isEmpty()) {
        return d->prefix % ' ' % str;
    }

    if (!d->suffix.isEmpty()) {
        return str % ' ' % d->suffix;
    }

    return str;
}

bool JNumericValue::inRange(qreal &value, bool increase) const
{
    if ((d->range->minimumEdge()
         ? JWIDGETS::fGreaterOrEqual(value, d->range->minimum())
         : JWIDGETS::fGreater(value, d->range->minimum()))
            && (d->range->maximumEdge()
                ? JWIDGETS::fLessOrEqual(value, d->range->maximum())
                : JWIDGETS::fLess(value, d->range->maximum()))) {
        return true;
    } else {
        if (increase) {
            value = d->range->maximumEdge()
                    ? d->range->maximum()
                    : d->range->maximum() - d->step;
        } else {
            value = d->range->minimumEdge()
                    ? d->range->minimum()
                    : d->range->minimum() + d->step;
        }
        return false;
    }
}

bool JNumericValue::without(qreal &value, bool increase)
{
    QListIterator<JNumericRange *> iter(d->withouts);
    while (iter.hasNext()) {
        const JNumericRange *without = iter.next();
        if ((without->minimumEdge()
             ? JWIDGETS::fGreaterOrEqual(value, without->minimum())
             : JWIDGETS::fGreater(value, without->minimum()))
                && (without->maximumEdge()
                    ? JWIDGETS::fLessOrEqual(value, without->maximum())
                    : JWIDGETS::fLess(value, without->maximum()))) {
            value = increase
                    ? (without->maximum() + (without->maximumEdge() ? d->step : 0))
                    : (without->minimum() + (without->minimumEdge() ? -d->step : 0));
            return true;
        }
    }

    return false;
}

QByteArray JNumericValue::pack(const QVariant &value) const
{
    QByteArray _buffer;
    // size
    quint16 _size = size();
    _buffer.append((const char *)&_size, 2);
    // data
    quint64 _v = (quint64)(value.toDouble() / d->scale);
    _buffer.append((const char *)&_v, _size);
    return _buffer;
}

QVariant JNumericValue::unpack(const QByteArray &data) const
{
    if (data.size() < 3) {
        return QVariant::Invalid;
    }

    quint16 _size = 0;
    memcpy(&_size, data, 2);
    if (_size != data.size() - 2 || _size != size()) {
        return QVariant::Invalid;
    }

    quint32 _v = 0;
    memcpy(&_v, &data.constData()[2], _size);
    return _v * d->scale;
}

QVariant JNumericValue::jfromValue(const QVariant &value, int role) const
{
    if (value.type() == QVariant::String) {
        role = (role == Qt::EditRole) ? Qt::DisplayRole : role;
        if (role == Qt::DisplayRole) {
            QString text = value.toString();
            if (d->radix != 10 && !d->fillChar.isNull() && text.count() < d->maskCount) {
                text.prepend(QString(d->maskCount - text.count(), d->fillChar));
            }
            return text;
        } else {
            return value;
        }
    } else {
        if (d->decimals  == 0) {
            if (d->maskCount < 8) {
                return value.toInt();
            } else {
                return value.toLongLong();
            }
        } else {
            return value.toDouble();
        }
    }
}

QVariant JNumericValue::jtoValue(const QVariant &value, int role) const
{
    Q_UNUSED(role);
    if (d->decimals == 0) {
        if (value.type() == QVariant::String) {
            QString text = value.toString();
            if (d->maskCount < 8) {
                return text.toInt(0, d->radix);
            } else {
                return text.toLongLong(0, d->radix);
            }
        } else {
            if (d->maskCount < 8) {
                return value.toInt();
            } else {
                return value.toLongLong();
            }
        }
    } else {
        return value.toDouble();
    }
}

QWidget *JNumericValue::createInstance(JItemValue *itemValue, QWidget *parent) const
{
    JNumericValue *numericValue = static_cast<JNumericValue *>(itemValue);
    if (numericValue->decimals() == 0) {
        if (numericValue->maskCount() < 8) {
            return new JNumericIBox(numericValue, parent);
        } else {
            return new JNumericLBox(numericValue, parent);
        }
    } else {
        return new JNumericDBox(numericValue, parent);
    }
}

QString JNumericValue::prefix() const
{
    return d->prefix;
}

QString JNumericValue::suffix() const
{
    return d->suffix;
}

QChar JNumericValue::fillChar() const
{
    return d->fillChar;
}

int JNumericValue::radix() const
{
    return d->radix;
}

int JNumericValue::maskCount() const
{
    return d->maskCount;
}

int JNumericValue::decimals() const
{
    return d->decimals;
}

qreal JNumericValue::step() const
{
    return d->step;
}

qreal JNumericValue::scale() const
{
    return d->scale;
}

JNumericRange *JNumericValue::range() const
{
    return d->range;
}

QList<JNumericRange *> &JNumericValue::withouts() const
{
    return d->withouts;
}

QObject *JNumericValue::withoutAt(int index) const
{
    if (d->withouts.isEmpty()) {
        return 0;
    }

    if (index < 0 || index >= d->withouts.count()) {
        return 0;
    }

    return d->withouts.at(index);
}

void JNumericValue::addWithout(qreal minimum, qreal maximum, bool minimumEdge, bool maximumEdge)
{
    JNumericRange *range = new JNumericRange(minimum, maximum, minimumEdge, maximumEdge);
    d->withouts.append(range);
}

void JNumericValue::removeWithoutAt(int index)
{
    if (d->withouts.isEmpty()) {
        return;
    }

    if (index < 0 || index >= d->withouts.count()) {
        return;
    }

    d->withouts.takeAt(index)->deleteLater();
}

void JNumericValue::clearWithout()
{
    QListIterator<JNumericRange *> iter(d->withouts);
    while (iter.hasNext()) {
        iter.next()->deleteLater();
    }

    d->withouts.clear();
}

void JNumericValue::setPrefix(const QString &value)
{
    if (value != d->prefix) {
        d->prefix = value;
        Q_EMIT prefixChanged(value);
    }
}

void JNumericValue::setSuffix(const QString &value)
{
    if (value != d->suffix) {
        d->suffix = value;
        Q_EMIT suffixChanged(value);
    }
}

void JNumericValue::setFillChar(const QChar &value)
{
    if (value != d->fillChar) {
        d->fillChar = value;
        Q_EMIT fillCharChanged(value);
    }
}

void JNumericValue::setRadix(int value)
{
    if (value != d->radix) {
        d->radix = value;
        Q_EMIT radixChanged(value);
    }
}

void JNumericValue::setMaskCount(int value)
{
    if (value != d->maskCount) {
        d->maskCount = value;
        Q_EMIT maskCountChanged(value);
    }
}

void JNumericValue::setDecimals(int value)
{
    if (value != d->decimals) {
        d->decimals = value;
        Q_EMIT decimalsChanged(value);
    }
}

void JNumericValue::setStep(qreal value)
{
    if (value != d->step) {
        d->step = value;
        Q_EMIT stepChanged(value);
    }
}

void JNumericValue::setScale(qreal value)
{
    if (value != d->scale) {
        d->scale = value;
        Q_EMIT scaleChanged(value);
    }
}

// - class JStringValueData -

class JStringValueData
{
public:
    JStringValueData()
        : upper(false)
        , multiline(false)
        , containCSS(false)
        , maxLength(65535)
    {

    }

    bool upper;
    bool multiline;
    bool containCSS;
    int maxLength;
    QString pattern;
};

// - class JStringValue -

JStringValue::JStringValue(QObject *parent)
    : JItemValue(StringValue, "", parent)
    , d(new JStringValueData)
{

}

JStringValue::~JStringValue()
{
    delete d;
}

QByteArray JStringValue::pack(const QVariant &value) const
{
    QByteArray _buffer(value.toString().toLatin1());
    quint16 _size = (quint16)_buffer.size();
    _buffer.prepend((const char *)&_size, 2);
    return _buffer;
}

QVariant JStringValue::unpack(const QByteArray &data) const
{
    if (data.size() < 3) {
        return QVariant::Invalid;
    }

    quint16 _size = 0;
    memcpy(&_size, data, 2);
    if (_size != data.size() - 2) {
        return QVariant::Invalid;
    }

    return QString::fromLatin1(&data.constData()[2]);
}

QVariant JStringValue::jfromValue(const QVariant &value, int) const
{
    return value;
}

QVariant JStringValue::jtoValue(const QVariant &value, int) const
{
    return value;
}

QWidget *JStringValue::createInstance(JItemValue *itemValue, QWidget *parent) const
{
    JStringValue *stringValue = static_cast<JStringValue *>(itemValue);
    if (!stringValue->editable()) {
        return new JStringLabel(stringValue, parent);
    } else {
        if (stringValue->multiline()) {
            return new JStringTEdit(stringValue, parent);
        } else {
            return new JStringLEdit(stringValue, parent);
        }
    }
}

bool JStringValue::upper() const
{
    return d->upper;
}

bool JStringValue::multiline() const
{
    return d->multiline;
}

bool JStringValue::containCSS() const
{
    return d->containCSS;
}

int JStringValue::maxLength() const
{
    return d->maxLength;
}

QString JStringValue::pattern() const
{
    return d->pattern;
}

void JStringValue::setUpper(bool value)
{
    if (value != d->upper) {
        d->upper = value;
        Q_EMIT upperChanged(value);
    }
}

void JStringValue::setMultiline(bool value)
{
    if (value != d->multiline) {
        d->multiline = value;
        Q_EMIT multilineChanged(value);
    }
}

void JStringValue::setContainCSS(bool value)
{
    if (value != d->containCSS) {
        d->containCSS = value;
        Q_EMIT containCSSChanged(value);
    }
}

void JStringValue::setMaxLength(int value)
{
    if (value != d->maxLength) {
        d->maxLength = value;
        Q_EMIT maxLengthChanged(value);
    }
}

void JStringValue::setPattern(const QString &value)
{
    if (value != d->pattern) {
        d->pattern = value;
        Q_EMIT patternChanged(value);
    }
}

// - class JEnumOptionData -

class JEnumOptionData
{
public:
    JEnumOptionData()
        : text("")
        , encode(0)
    {

    }

    QString text;
    qulonglong encode;
};

// - class JEnumOption

JEnumOption::JEnumOption(QObject *parent)
    : QObject(parent)
    , d(new JEnumOptionData)
{

}

JEnumOption::JEnumOption(const QString &text, qulonglong encode, QObject *parent)
    : QObject(parent)
    , d(new JEnumOptionData)
{
    d->text = text;
    d->encode = encode;
}

JEnumOption::~JEnumOption()
{
    delete d;
}

QString JEnumOption::text() const
{
    return d->text;
}

qulonglong JEnumOption::encode() const
{
    return d->encode;
}

void JEnumOption::setText(const QString &value)
{
    if (value != d->text) {
        d->text = value;
        Q_EMIT textChanged(value);
    }
}

void JEnumOption::setEncode(qulonglong value)
{
    if (value != d->encode) {
        d->encode = value;
        Q_EMIT encodeChanged(value);
    }
}

// - class JEnumValueData -

class JEnumValueData
{
public:
    JEnumValueData()
        : indexOffset(0)
        , autoCompletion(true)
        , duplicatesEnabled(true)
        , multiable(false)
    {

    }

    int indexOffset;
    bool autoCompletion;
    bool duplicatesEnabled;
    bool multiable;
    QList<JEnumOption *> options;
};

// - class JEnumValue -

JEnumValue::JEnumValue(QObject *parent)
    : JItemValue(EnumValue, "", parent)
    , d(new JEnumValueData)
{

}

JEnumValue::~JEnumValue()
{
    delete d;
}

QString JEnumValue::text(int index) const
{
    if (index < 0 || index >= d->options.count()) {
        return QString::null;
    }

    return d->options.at(index)->text();
}

QString JEnumValue::text(qulonglong encode) const
{
    QListIterator<JEnumOption *> iter(d->options);
    if (d->multiable) {
        QString text;
        while (iter.hasNext()) {
            const JEnumOption *option = iter.next();
            if (encode & option->encode()) {
                text.append(option->text() % '|');
            }
        }
        text.chop(1);
        return text;
    } else {
        while (iter.hasNext()) {
            const JEnumOption *option = iter.next();
            if (option->encode() == encode) {
                return option->text();
            }
        }
    }

    return QString::null;
}

qulonglong JEnumValue::encode(int index) const
{
    if (index < 0 || index >= d->options.count()) {
        return 0xfffffffffULL;
    }

    return d->options.at(index)->encode();
}

qulonglong JEnumValue::encode(const QString &text) const
{
    QListIterator<JEnumOption *> iter(d->options);
    if (d->multiable) {
        qulonglong value = 0;
        while (iter.hasNext()) {
            const JEnumOption *option = iter.next();
            if (text.contains(option->text())) {
                value |= option->encode();
            }
        }
        return value;
    } else {
        while (iter.hasNext()) {
            const JEnumOption *option = iter.next();
            if (option->text() == text) {
                return option->encode();
            }
        }
    }

    return 0;
}

int JEnumValue::index(qulonglong encode) const
{
    for (int i = 0; i < d->options.count(); ++i) {
        if (encode == d->options.at(i)->encode()) {
            return i;
        }
    }

    return -1;
}

int JEnumValue::index(const QString &text) const
{
    for (int i = 0; i < d->options.count(); ++i) {
        if (text == d->options.at(i)->text()) {
            return i;
        }
    }

    return -1;
}

QByteArray JEnumValue::pack(const QVariant &value) const
{
    QByteArray _buffer;
    // size
    quint16 _size = size();
    _buffer.append((const char *)&_size, 2);
    // data
    quint32 _encode = (quint32)value.toUInt();
    _buffer.append((const char *)&_encode, _size);
    return _buffer;
}

QVariant JEnumValue::unpack(const QByteArray &data) const
{
    if (data.size() < 3) {
        return QVariant::Invalid;
    }

    quint16 _size = 0;
    memcpy(&_size, data, 2);
    if (_size != data.size() - 2 || _size != size()) {
        return QVariant::Invalid;
    }

    quint32 _encode;
    memcpy(&_encode, &data.constData()[2], _size);
    return index(_encode);
}

QVariant JEnumValue::jfromValue(const QVariant &value, int role) const
{
    Q_UNUSED(role);
    switch (value.type()) {
    case QVariant::String:
        return value;
    case QVariant::Int:
        return text(value.toInt());
    default:
        return text(value.toULongLong());
    }
}

QVariant JEnumValue::jtoValue(const QVariant &value, int role) const
{
    switch (role) {
    case Qt::DisplayRole:
    case Qt::EditRole:
        return value;
    case Qt::UserRole:
        if (value.type() == QVariant::String) {
            return index(value.toString());
        } else {
            return index(value.toULongLong());
        }
    default:
        return encode(value.toString());
    }
}

QWidget *JEnumValue::createInstance(JItemValue *itemValue, QWidget *parent) const
{
    JEnumValue *enumValue = static_cast<JEnumValue *>(itemValue);
    if (enumValue->multiable()) {
        return new JMultiEnumBox(enumValue, parent);
    } else {
        return new JEnumBox(enumValue, parent);
    }
}

int JEnumValue::indexOffset() const
{
    return d->indexOffset;
}

bool JEnumValue::autoCompletion() const
{
    return d->autoCompletion;
}

bool JEnumValue::duplicatesEnabled() const
{
    return d->duplicatesEnabled;
}

bool JEnumValue::multiable() const
{
    return d->multiable;
}

QList<JEnumOption *> &JEnumValue::options() const
{
    return d->options;
}

QStringList JEnumValue::items() const
{
    QStringList lst;
    QListIterator<JEnumOption *> iter(d->options);
    while (iter.hasNext()) {
        lst.append(iter.next()->text());
    }

    return lst;
}

QObject *JEnumValue::optionAt(int index) const
{
    if (d->options.isEmpty()) {
        return 0;
    }

    if (index < 0 || index >= d->options.count()) {
        return 0;
    }

    return d->options.at(index);
}

void JEnumValue::insertOption(int index, const QString &text, qulonglong encode)
{
    JEnumOption *option = new JEnumOption(text, encode, this);
    d->options.insert(index, option);
}

void JEnumValue::appendOption(const QString &text, qulonglong encode)
{
    insertOption(d->options.count(), text, encode);
}

void JEnumValue::appendOption(const QString &text)
{
    if (d->options.isEmpty()) {
        return;
    } else {
        appendOption(text, d->options.last()->encode());
    }
}

void JEnumValue::removeOptionAt(int index)
{
    if (d->options.isEmpty()) {
        return;
    }

    if (index < 0 || index >= d->options.count()) {
        return;
    }

    d->options.takeAt(index)->deleteLater();
}

void JEnumValue::clearOption()
{
    QListIterator<JEnumOption *> iter(d->options);
    while (iter.hasNext()) {
        iter.next()->deleteLater();
    }

    d->options.clear();
}

void JEnumValue::setIndexOffset(int value)
{
    if (value != d->indexOffset) {
        d->indexOffset = value;
        Q_EMIT indexOffsetChanged(value);
    }
}

void JEnumValue::setAutoCompletion(bool value)
{
    if (value != d->autoCompletion) {
        d->autoCompletion = value;
        Q_EMIT autoCompletionChanged(value);
    }
}

void JEnumValue::setDuplicatesEnabled(bool value)
{
    if (value != d->duplicatesEnabled) {
        d->duplicatesEnabled = value;
        Q_EMIT duplicatesEnabledChanged(value);
    }
}

void JEnumValue::setMultiable(bool value)
{
    if (value != d->multiable) {
        d->multiable = value;
        Q_EMIT multiableChanged(value);
    }
}

// - class JIPv4ValueData -

class JIPv4ValueData
{
public:
    JIPv4ValueData()
    {

    }

};

JIPv4Value::JIPv4Value(QObject *parent)
    : JItemValue(IPv4Value, "127.0.0.1", parent)
    , d(new JIPv4ValueData)
{

}

JIPv4Value::~JIPv4Value()
{
    delete d;
}

QByteArray JIPv4Value::pack(const QVariant &value) const
{
    QByteArray _buffer(value.toString().toLatin1());
    quint16 _size = (quint16)_buffer.size();
    _buffer.prepend((const char *)&_size, 2);
    return _buffer;
}

QVariant JIPv4Value::unpack(const QByteArray &data) const
{
    if (data.size() < 3) {
        return QVariant::Invalid;
    }

    quint16 _size = 0;
    memcpy(&_size, data, 2);
    if (_size != data.size()) {
        return QVariant::Invalid;
    }

    return QString::fromLatin1(&data.constData()[2]);
}

QVariant JIPv4Value::jfromValue(const QVariant &value, int role) const
{
    Q_UNUSED(role);
    return value;
}

QVariant JIPv4Value::jtoValue(const QVariant &value, int role) const
{
    Q_UNUSED(role);
    return value;
}

QWidget *JIPv4Value::createInstance(JItemValue *itemValue, QWidget *parent) const
{
    JIPv4Value *ipv4Value = static_cast<JIPv4Value *>(itemValue);
    if (ipv4Value->editable()) {
        return new JIPv4Edit(ipv4Value, parent);
    }

    return 0;
}

// - class JDelegateValueData -

class JDelegateValueData
{
public:
    JDelegateValueData()
        : delegateType(InvalidDelegate)
    {

    }

    int delegateType;
};

// - class JDelegateValue

JDelegateValue::JDelegateValue(QObject *parent)
    : JItemValue(DelegateValue, "", parent)
    , d(new JDelegateValueData)
{

}

JDelegateValue::JDelegateValue(JDelegateType delegateType, const QVariant &original, QObject *parent)
    : JItemValue(DelegateValue, original, parent)
    , d(new JDelegateValueData)
{
    d->delegateType = delegateType;
}

JDelegateValue::~JDelegateValue()
{
    delete d;
}

QByteArray JDelegateValue::pack(const QVariant &value) const
{
    Q_UNUSED(value);
    return QByteArray();
}

QVariant JDelegateValue::unpack(const QByteArray &data) const
{
    Q_UNUSED(data);
    return QVariant::Invalid;
}

QVariant JDelegateValue::jfromValue(const QVariant &value, int role) const
{
    Q_UNUSED(role);
    return value;
}

QVariant JDelegateValue::jtoValue(const QVariant &value, int role) const
{
    Q_UNUSED(role);
    return value;
}

QWidget *JDelegateValue::createInstance(JItemValue *itemValue, QWidget *parent) const
{
    Q_UNUSED(itemValue);
    Q_UNUSED(parent);
    return 0;
}

int JDelegateValue::delegateType() const
{
    return d->delegateType;
}

void JDelegateValue::setDelegateType(int value)
{
    if (value != d->delegateType) {
        d->delegateType = value;
        Q_EMIT delegateTypeChanged(value);
    }
}

// - class JPictureDelegateValueData -

class JPictureDelegateValueData
{
public:
    JPictureDelegateValueData()
    {

    }
};

// - class JPictureDelegateValue -

JPictureDelegateValue::JPictureDelegateValue(QObject *parent)
    : JDelegateValue(PictureDelegate, "", parent)
    , d(new JPictureDelegateValueData)
{

}

JPictureDelegateValue::~JPictureDelegateValue()
{
    delete d;
}

QByteArray JPictureDelegateValue::pack(const QVariant &value) const
{
    Q_UNUSED(value);
    return QByteArray();
}

QVariant JPictureDelegateValue::unpack(const QByteArray &data) const
{
    Q_UNUSED(data);
    return QVariant::Invalid;
}

QVariant JPictureDelegateValue::jfromValue(const QVariant &value, int role) const
{
    Q_UNUSED(role);
    return value;
}

QVariant JPictureDelegateValue::jtoValue(const QVariant &value, int role) const
{
    Q_UNUSED(role);
    return value;
}

QWidget *JPictureDelegateValue::createInstance(JItemValue *itemValue, QWidget *parent) const
{
    Q_UNUSED(itemValue);
    Q_UNUSED(parent);
    return 0;
}

// - class JProgressDelegateValueData -

class JProgressDelegateValueData
{
public:
    JProgressDelegateValueData()
        : minimum(0.)
        , maximum(100.)
        , text("")
        , textVisible(true)
    {

    }

    qreal minimum;
    qreal maximum;
    QString text;
    bool textVisible;
};

// - class JProgressDelegateValue -

JProgressDelegateValue::JProgressDelegateValue(QObject *parent)
    : JDelegateValue(ProgressDelegate, 0, parent)
    , d(new JProgressDelegateValueData)
{

}

JProgressDelegateValue::~JProgressDelegateValue()
{
    delete d;
}

QByteArray JProgressDelegateValue::pack(const QVariant &value) const
{
    Q_UNUSED(value);
    return QByteArray();
}

QVariant JProgressDelegateValue::unpack(const QByteArray &data) const
{
    Q_UNUSED(data);
    return QVariant::Invalid;
}

QVariant JProgressDelegateValue::jfromValue(const QVariant &value, int role) const
{
    Q_UNUSED(role);
    return value;
}

QVariant JProgressDelegateValue::jtoValue(const QVariant &value, int role) const
{
    Q_UNUSED(role);
    return value;
}

QWidget *JProgressDelegateValue::createInstance(JItemValue *itemValue, QWidget *parent) const
{
    Q_UNUSED(itemValue);
    Q_UNUSED(parent);
    return 0;
}

qreal JProgressDelegateValue::minimum() const
{
    return d->minimum;
}

qreal JProgressDelegateValue::maximum() const
{
    return d->maximum;
}

QString JProgressDelegateValue::text() const
{
    return d->text;
}

bool JProgressDelegateValue::textVisible() const
{
    return d->textVisible;
}

void JProgressDelegateValue::setMinimum(qreal value)
{
    if (value != d->minimum) {
        d->minimum = value;
        Q_EMIT minimumChanged(value);
    }
}

void JProgressDelegateValue::setMaximum(qreal value)
{
    if (value != d->maximum) {
        d->maximum = value;
        Q_EMIT maximumChanged(value);
    }
}

void JProgressDelegateValue::setText(const QString &value)
{
    if (value != d->text) {
        d->text = value;
        Q_EMIT textChanged(value);
    }
}

void JProgressDelegateValue::setTextVisible(bool value)
{
    if (value != d->textVisible) {
        d->textVisible = value;
        Q_EMIT textVisibleChanged(value);
    }
}

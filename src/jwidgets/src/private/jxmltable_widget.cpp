#include "precomp.h"
#include "jxmltable_widget.h"
#include "../jxmltable.h"

// - class JCheckBoxItemData -

class JCheckBoxItemData
{
public:
    JBoolValue *property;
};

// - class JCheckBoxItem -

JCheckBoxItem::JCheckBoxItem(JBoolValue *property)
    : d(new JCheckBoxItemData)
{
    d->property = property;
    initItem(property, this);
}

JCheckBoxItem::~JCheckBoxItem()
{
    delete d;
}

const JBoolValue *JCheckBoxItem::property() const
{
    return d->property;
}

void JCheckBoxItem::initItem(JBoolValue *itemValue, QStandardItem *item)
{
    item->setFlags(item->flags() & ~Qt::ItemIsEditable);
    item->setEnabled(itemValue->enabled());
    item->setTextAlignment(itemValue->alignment());
    item->setText(itemValue->text());
    item->setToolTip(itemValue->tooltip());
    item->setWhatsThis(itemValue->whatsThis());
    if (!itemValue->icon().isEmpty()) {
        item->setIcon(QIcon(itemValue->icon()));
    }
    item->setCheckable(itemValue->checkable());
    item->setCheckState(Qt::CheckState(itemValue->original().toInt()));
}

// - class JBoolBoxData -

class JBoolBoxData
{
public:
    JBoolValue *property;
};

// - class JBoolBox -

JBoolBox::JBoolBox(JBoolValue *property, QWidget *parent)
    : QCheckBox(parent)
    , d(new JBoolBoxData)
{
    d->property = property;
    if (!property->styleSheet().isEmpty()) {
        setStyleSheet(property->styleSheet());
    }
    setText(property->name());
    setEnabled(property->enabled());
    setCheckable(property->checkable());
    if (!property->icon().isEmpty()) {
        setIcon(QIcon(property->icon()));
    }
    setToolTip(property->tooltip());
    setWhatsThis(property->whatsThis());

    connect(this, SIGNAL(stateChanged(int)), SLOT(_emit_stateChanged(int)));

    jsetValue(property->original());
}

JBoolBox::~JBoolBox()
{
    delete d;
}

const JBoolValue *JBoolBox::property() const
{
    return d->property;
}

QByteArray JBoolBox::jdata() const
{
    return d->property->pack(int(checkState()));
}

QVariant JBoolBox::jvalue() const
{
    return checkState();
}

QVariant JBoolBox::jtext() const
{
    return checkState();
}

QVariant JBoolBox::jproperty() const
{
    return QVariant::fromValue<void *>(d->property);
}

bool JBoolBox::jsetData(const QByteArray &data)
{
    QVariant variant = property()->unpack(data);
    if (variant.isValid()) {
        setCheckState(Qt::CheckState(variant.toInt()));
        return true;
    } else {
        return false;
    }
}

void JBoolBox::jsetValue(const QVariant &value)
{
    setCheckState(Qt::CheckState(value.toInt()));
}

void JBoolBox::_emit_stateChanged(int state)
{
    Q_UNUSED(state);
}

// - JNumericIBoxData -

class JNumericIBoxData
{
public:
    JNumericIBoxData()
        : oldValue(0)
    {

    }

    JNumericValue *property;
    int oldValue;
};

// - JNuemricIBox -

JNumericIBox::JNumericIBox(JNumericValue *property, QWidget *parent)
    : JSpinBox(parent)
    , d(new JNumericIBoxData)
{
    d->property = property;
    if (!property->styleSheet().isEmpty()) {
        setStyleSheet(property->styleSheet());
    }
    setReadOnly(!property->editable());
    setAlignment(property->alignment());
    if (!property->prefix().isEmpty()) {
        setPrefix(property->prefix() % ' ');
    }
    if (!property->suffix().isEmpty()) {
        setSuffix(' ' % property->suffix());
    }
    setToolTip(property->tooltip());
    setWhatsThis(property->whatsThis());
    setRange((int)property->range()->minimum(),
             (int)property->range()->maximum());

    connect(this, SIGNAL(valueChanged(int)), SLOT(_emit_valueChanged(int)));

    setSingleStep((int)property->step());
    setMaskCount(property->maskCount());
    setFillChar(property->fillChar());
    setRadix(property->radix());

    jsetValue(property->original());
}

JNumericIBox::~JNumericIBox()
{
    delete d;
}

const JNumericValue *JNumericIBox::property() const
{
    return d->property;
}

QByteArray JNumericIBox::jdata() const
{
    return d->property->pack(value());
}

QVariant JNumericIBox::jvalue() const
{
    return d->property->jtoValue(text(), Qt::DisplayRole);
}

QVariant JNumericIBox::jtext() const
{
    QString str = text();
    if (d->property->radix() != 10) {
        str = str.remove(prefix()).remove(suffix()).toUpper();
        if (str.count() > d->property->maskCount()) {
            str = str.mid(str.count() - d->property->maskCount());
        }
    }
    str = prefix() % str % suffix();
    return d->property->jfromValue(str, Qt::DisplayRole);
}

QVariant JNumericIBox::jproperty() const
{
    return QVariant::fromValue<void *>(d->property);
}

bool JNumericIBox::jsetData(const QByteArray &data)
{
    QVariant variant = property()->unpack(data);
    if (variant.isValid()) {
        setValue(variant.toInt());
        return true;
    } else {
        return false;
    }
}

void JNumericIBox::jsetValue(const QVariant &value)
{
    if (value.type() == QVariant::String) {
        QString text = value.toString().remove(prefix()).remove(suffix());
        lineEdit()->setText(d->property->jfromValue(text).toString());
    } else {
        lineEdit()->setText(d->property->jfromValue(value).toString());
    }
}

void JNumericIBox::_emit_valueChanged(int value)
{
    bool increase = value > d->oldValue;
    qreal v = value;
    if (d->property->inRange(v, increase)) {
        d->property->without(v, increase);
    }

    d->oldValue = (int)v;
    if (value != d->oldValue) {
        setValue(d->oldValue);
    }
}

// - class JNumericLBoxData -

class JNumericLBoxData
{
public:
    JNumericLBoxData()
        : oldValue(0.)
    {

    }

    JNumericValue *property;
    qreal oldValue;
};

// - class JNumericLBox -

JNumericLBox::JNumericLBox(JNumericValue *property, QWidget *parent)
    : JLargeSpinBox(parent)
    , d(new JNumericLBoxData)
{
    d->property = property;
    if (!property->styleSheet().isEmpty()) {
        setStyleSheet(property->styleSheet());
    }
    setReadOnly(!property->editable());
    setAlignment(property->alignment());
    if (!property->prefix().isEmpty()) {
        setPrefix(property->prefix() % ' ');
    }
    if (!property->suffix().isEmpty()) {
        setSuffix(' ' % property->suffix());
    }
    setToolTip(property->tooltip());
    setWhatsThis(property->whatsThis());
    setRange((qlonglong)property->range()->minimum(),
             (qlonglong)property->range()->maximum());

    connect(this, SIGNAL(valueChanged(qlonglong)), SLOT(_emit_valueChanged(qlonglong)));

    setSingleStep((int)property->step());
    setMaskCount(property->maskCount());
    setFillChar(property->fillChar());
    setRadix(property->radix());

    jsetValue(property->original());
}

JNumericLBox::~JNumericLBox()
{
    delete d;
}

const JNumericValue *JNumericLBox::property() const
{
    return d->property;
}

QByteArray JNumericLBox::jdata() const
{
    return d->property->pack(value());
}

QVariant JNumericLBox::jvalue() const
{
    return d->property->jtoValue(value());
}

QVariant JNumericLBox::jtext() const
{
    QString str = text();
    if (d->property->radix() != 10) {
        str = str.remove(prefix()).remove(suffix()).toUpper();
        if (str.count() > d->property->maskCount()) {
            str = str.mid(str.count() - d->property->maskCount());
        }
    }
    str = prefix() % str % suffix();
    return d->property->jfromValue(str, Qt::DisplayRole);
}

QVariant JNumericLBox::jproperty() const
{
    return QVariant::fromValue<void *>(d->property);
}

bool JNumericLBox::jsetData(const QByteArray &data)
{
    QVariant variant = d->property->unpack(data);
    if (variant.isValid()) {
        setValue(variant.toInt());
        return true;
    } else {
        return false;
    }
}

void JNumericLBox::jsetValue(const QVariant &value)
{
    if (value.type() == QVariant::String) {
        QString text = value.toString().remove(prefix()).remove(suffix());
        lineEdit()->setText(d->property->jfromValue(text).toString());
    } else {
        lineEdit()->setText(d->property->jfromValue(value).toString());
    }
}

void JNumericLBox::_emit_valueChanged(qlonglong value)
{
    bool increase = value > d->oldValue;
    qreal v = value;
    if (d->property->inRange(v, increase)) {
        d->property->without(v, increase);
    }

    d->oldValue = (qlonglong)v;
    if (value != d->oldValue) {
        setValue(d->oldValue);
    }
}

// - class JNumericDBoxData -

class JNumericDBoxData
{
public:
    JNumericDBoxData()
        : oldValue(0.)
    {

    }

    JNumericValue *property;
    qreal oldValue;
};

// - class JNumericDBox -

JNumericDBox::JNumericDBox(JNumericValue *property, QWidget *parent)
    : QDoubleSpinBox(parent)
    , d(new JNumericDBoxData)
{
    d->property = property;
    if (!property->styleSheet().isEmpty()) {
        setStyleSheet(property->styleSheet());
    }
    setReadOnly(!property->editable());
    setAlignment(property->alignment());
    if (!property->prefix().isEmpty()) {
        setPrefix(property->prefix() % ' ');
    }
    if (!property->suffix().isEmpty()) {
        setSuffix(' ' % property->suffix());
    }
    setToolTip(property->tooltip());
    setWhatsThis(property->whatsThis());
    setDecimals(property->decimals());
    setSingleStep(property->step());
    setRange(property->range()->minimum(),
              property->range()->maximum());

    connect(this, SIGNAL(valueChanged(double)), SLOT(_emit_valueChanged(double)));

    jsetValue(property->original());
}

JNumericDBox::~JNumericDBox()
{
    delete d;
}

const JNumericValue *JNumericDBox::property() const
{
    return d->property;
}

QByteArray JNumericDBox::jdata() const
{
    return d->property->pack(value());
}

QVariant JNumericDBox::jvalue() const
{
    return value();
}

QVariant JNumericDBox::jtext() const
{
    return text();
}

QVariant JNumericDBox::jproperty() const
{
    return QVariant::fromValue<void *>(d->property);
}

bool JNumericDBox::jsetData(const QByteArray &data)
{
    QVariant variant = d->property->unpack(data);
    if (variant.isValid()) {
        setValue(variant.toDouble());
        return true;
    } else {
        return false;
    }
}

void JNumericDBox::jsetValue(const QVariant &value)
{
    if (value.type() == QVariant::String) {
        setValue(valueFromText(value.toString()));
    } else {
        setValue(value.toDouble());
    }
}

void JNumericDBox::_emit_valueChanged(double value)
{
    bool increase = value > d->oldValue;
    qreal v = value;
    if (d->property->inRange(v, increase)) {
        d->property->without(v, increase);
    }

    d->oldValue = v;
    if (value != d->oldValue) {
        setValue(d->oldValue);
    }
}

// - class JStringItemData -

class JStringItemData
{
public:
    JItemValue *property;
};

// - class JStringItem -

JStringItem::JStringItem(JItemValue *property)
    : d(new JStringItemData)
{
    d->property = property;
    setEnabled(property->enabled());
    setEditable(property->editable());
    setTextAlignment(property->alignment());
    setToolTip(property->tooltip());
    setWhatsThis(property->whatsThis());
    if (!property->icon().isEmpty()) {
        setIcon(QIcon(property->icon()));
    }
    if (property->original().isValid()) {
        setText(property->jfromValue(property->original()).toString());
    }
}

JStringItem::~JStringItem()
{
    delete d;
}

const JItemValue *JStringItem::property() const
{
    return d->property;
}

// - class JStringLabelData -

class JStringLabelData
{
public:
    JStringValue *property;
};

// - JStringLabel -

JStringLabel::JStringLabel(JStringValue *property, QWidget *parent)
    : QLabel(property->original().toString(), parent)
    , d(new JStringLabelData)
{
    d->property = property;
    init();
}

JStringLabel::JStringLabel(JStringValue *property, const QString &text, QWidget *parent)
    : QLabel(text, parent)
    , d(new JStringLabelData)
{
    d->property = property;
    init();
}

JStringLabel::~JStringLabel()
{
    delete d;
}

const JStringValue *JStringLabel::property() const
{
    return d->property;
}

QByteArray JStringLabel::jdata() const
{
    return d->property->pack(text());
}

QVariant JStringLabel::jvalue() const
{
    return text();
}

QVariant JStringLabel::jtext() const
{
    return text();
}

QVariant JStringLabel::jproperty() const
{
    return QVariant::fromValue<void *>(d->property);
}

bool JStringLabel::jsetData(const QByteArray &data)
{
    QVariant variant = d->property->unpack(data);
    if (variant.isValid()) {
        setText(variant.toString());
        return true;
    } else {
        return false;
    }
}

void JStringLabel::jsetValue(const QVariant &value)
{
    setText(value.toString());
}

void JStringLabel::init()
{
    //setTextFormat(Qt::RichText);
    if (!d->property->styleSheet().isEmpty()) {
        setStyleSheet(d->property->styleSheet());
    }
    setAlignment(d->property->alignment());
    setToolTip(d->property->tooltip());
    setWhatsThis(d->property->whatsThis());

    jsetValue(d->property->original());
}

// - class JStringLEditData -

class JStringLEditData
{
public:
    JStringValue *property;
};

// - class JStringLEdit -

JStringLEdit::JStringLEdit(JStringValue *property, QWidget *parent)
    : QLineEdit(parent)
    , d(new JStringLEditData)
{
    d->property = property;
    if (!property->styleSheet().isEmpty()) {
        setStyleSheet(property->styleSheet());
    }
    setReadOnly(!property->editable());
    setAlignment(property->alignment());
    setMaxLength(property->maxLength());
    setToolTip(property->tooltip());
    setPlaceholderText(property->placeHoldText());
    setWhatsThis(property->whatsThis());
    if (!property->pattern().isEmpty()) {
        QRegExp regExp(property->pattern());
        if (regExp.isValid()) {
            setValidator(new QRegExpValidator(regExp, this));
        }
    }

    connect(this, SIGNAL(textChanged(QString)), SLOT(_emit_textChanged(QString)));

    jsetValue(property->original());
}

JStringLEdit::~JStringLEdit()
{
    delete d;
}

const JStringValue *JStringLEdit::property() const
{
    return d->property;
}

QByteArray JStringLEdit::jdata() const
{
    return d->property->pack(text());
}

QVariant JStringLEdit::jvalue() const
{
    return text();
}

QVariant JStringLEdit::jtext() const
{
    return text();
}

QVariant JStringLEdit::jproperty() const
{
    return QVariant::fromValue<void *>(d->property);
}

bool JStringLEdit::jsetData(const QByteArray &data)
{
    QVariant variant = d->property->unpack(data);
    if (variant.isValid()) {
        setText(variant.toString());
        return true;
    } else {
        return false;
    }
}

void JStringLEdit::jsetValue(const QVariant &value)
{
    setText(value.toString());
}

void JStringLEdit::_emit_textChanged(const QString &text)
{
    if (d->property->upper()) {
        setText(text.toUpper());
    }
}

// - class JStringTEditData -

class JStringTEditData
{
public:
    JStringTEditData()
        : maxLength(65535)
    {

    }

    JStringValue *property;
    int maxLength;
};

// - class JStringTEdit -

JStringTEdit::JStringTEdit(JStringValue *property, QWidget *parent)
    : QPlainTextEdit(parent)
    , d(new JStringTEditData)
{
    d->property = property;
    d->maxLength = property->maxLength();
    if (!property->styleSheet().isEmpty()) {
        setStyleSheet(property->styleSheet());
    }
    setToolTip(property->tooltip());
    setWhatsThis(property->whatsThis());
    setReadOnly(!property->editable());

    connect(this, SIGNAL(textChanged()), SLOT(_emit_textChanged()));

    jsetValue(property->original());
}

JStringTEdit::~JStringTEdit()
{
    delete d;
}

const JStringValue *JStringTEdit::property() const
{
    return d->property;
}

QByteArray JStringTEdit::jdata() const
{
    return d->property->pack(toPlainText());
}

QVariant JStringTEdit::jvalue() const
{
    return toPlainText();
}

QVariant JStringTEdit::jtext() const
{
    return toPlainText();
}

QVariant JStringTEdit::jproperty() const
{
    return QVariant::fromValue<void *>(d->property);
}

bool JStringTEdit::jsetData(const QByteArray &data)
{
    QVariant variant = d->property->unpack(data);
    if (variant.isValid()) {
        setPlainText(variant.toString());
        return true;
    } else {
        return false;
    }
}

void JStringTEdit::jsetValue(const QVariant &value)
{
    setPlainText(value.toString());
}

void JStringTEdit::_emit_textChanged()
{
    QString text = toPlainText();
    int length = text.count();
    if (length > d->maxLength) {
        QTextCursor txtCursor = textCursor();
        int pos = txtCursor.position();
        text.remove(pos - (length - d->maxLength), length - d->maxLength);
        setPlainText(text);
        txtCursor.setPosition(pos - (length - d->maxLength));
        setTextCursor(txtCursor);
    }
}

// - class JEnumBoxData -

class JEnumBoxData
{
public:
    JEnumValue *property;
};

// - JEnumBox -

JEnumBox::JEnumBox(JEnumValue *property, QWidget *parent)
    : QComboBox(parent)
    , d(new JEnumBoxData)
{
    d->property = property;
    setEditable(property->editable());
    setAutoCompletion(property->autoCompletion());
    setDuplicatesEnabled(property->duplicatesEnabled());
    setInsertPolicy(QComboBox::NoInsert);

    if (!property->styleSheet().isEmpty()) {
        setStyleSheet(property->styleSheet());
    }
    setToolTip(property->tooltip());
    setWhatsThis(property->whatsThis());
    QListIterator<JEnumOption *> iter(property->options());
    while (iter.hasNext()) {
        const JEnumOption *option = iter.next();
        addItem(option->text(), option->encode());
    }

    connect(this, SIGNAL(currentIndexChanged(int)), SLOT(_emit_currentIndexChanged(int)));

    setCurrentIndex(property->index(property->original().toString()));
}

JEnumBox::~JEnumBox()
{
    delete d;
}

const JEnumValue *JEnumBox::property() const
{
    return d->property;
}

QByteArray JEnumBox::jdata() const
{
    return d->property->pack(itemData(currentIndex()));
}

QVariant JEnumBox::jvalue() const
{
    return d->property->encode(currentIndex());
}

QVariant JEnumBox::jtext() const
{
    return currentText();
}

QVariant JEnumBox::jproperty() const
{
    return QVariant::fromValue<void *>(d->property);
}

bool JEnumBox::jsetData(const QByteArray &data)
{
    QVariant variant = d->property->unpack(data);
    if (variant.isValid()) {
        setCurrentIndex(variant.toUInt());
        return true;
    } else {
        return false;
    }
}

void JEnumBox::jsetValue(const QVariant &value)
{
    switch (value.type()) {
    case QVariant::String:
        setCurrentIndex(d->property->index(value.toString()));
        break;
    case QVariant::Int:
        setCurrentIndex(value.toInt());
    default:
        setCurrentIndex(d->property->index(value.toUInt()));
        break;
    }
}

void JEnumBox::_emit_currentIndexChanged(int index)
{
    Q_UNUSED(index);
}

// - class JMultiEnumBoxData -

class JMultiEnumBoxData
{
public:
    JEnumValue *property;
};

// - JMultiEnumBox -

JMultiEnumBox::JMultiEnumBox(JEnumValue *property, QWidget *parent)
    : JCheckComboBox(parent)
    , d(new JMultiEnumBoxData)
{
    d->property = property;
    setEditable(property->editable());
    setAutoCompletion(property->autoCompletion());
    setDuplicatesEnabled(property->duplicatesEnabled());
    setInsertPolicy(QComboBox::NoInsert);

    if (!property->styleSheet().isEmpty()) {
        setStyleSheet(property->styleSheet());
    }
    setToolTip(property->tooltip());
    setWhatsThis(property->whatsThis());
    QListIterator<JEnumOption *> iter(property->options());
    while (iter.hasNext()) {
        const JEnumOption *option = iter.next();
        addItem(option->text(), option->encode());
    }

    connect(this, SIGNAL(currentIndexChanged(QString)), SLOT(_emit_encodeChanged()));

    setEncode(property->original().toString());
}

JMultiEnumBox::~JMultiEnumBox()
{
    delete d;
}

const JEnumValue *JMultiEnumBox::property() const
{
    return d->property;
}

QByteArray JMultiEnumBox::jdata() const
{
    return d->property->pack(encode());
}

QVariant JMultiEnumBox::jvalue() const
{
    return encode();
}

QVariant JMultiEnumBox::jtext() const
{
    return currentText();
}

QVariant JMultiEnumBox::jproperty() const
{
    return QVariant::fromValue<void *>(d->property);
}

bool JMultiEnumBox::jsetData(const QByteArray &data)
{
    QVariant variant = d->property->unpack(data);
    if (variant.isValid()) {
        setEncode(variant.toULongLong());
        return true;
    } else {
        return false;
    }
}

void JMultiEnumBox::jsetValue(const QVariant &value)
{
    switch (value.type()) {
    case QVariant::String:
        setEncode(value.toString());
        break;
    default:
        setEncode(value.toULongLong());
        break;
    }
}

void JMultiEnumBox::_emit_encodeChanged()
{

}

// - class JDateTimeEditData -

class JDateTimeEditData
{
public:
    JDateTimeValue *property;
};

// - class JDateTimeEdit -

JDateTimeEdit::JDateTimeEdit(JDateTimeValue *property, QWidget *parent) :
    QDateTimeEdit(parent),
    d(new JDateTimeEditData)
{
    d->property = property;
    setReadOnly(!d->property->editable());
    if (!d->property->styleSheet().isEmpty()) {
        setStyleSheet(d->property->styleSheet());
    }
    setToolTip(d->property->tooltip());
    setWhatsThis(d->property->whatsThis());
    setDisplayFormat(d->property->format());
    setTimeSpec(d->property->timeSpec());
    setCalendarPopup(d->property->calenderPopup());
    if (d->property->range()->isValid()) {
        setDateTimeRange(d->property->range()->minimum(),
                         d->property->range()->maximum());
    }

    connect(this, SIGNAL(dateTimeChanged(QDateTime)), SLOT(_qDateTimeChanged(QDateTime)));

    jsetValue(d->property->original());
}

JDateTimeEdit::~JDateTimeEdit()
{
    delete d;
}

const JDateTimeValue *JDateTimeEdit::property() const
{
    return d->property;
}

QByteArray JDateTimeEdit::jdata() const
{
    return d->property->pack(text());
}

QVariant JDateTimeEdit::jvalue() const
{
    return text();
}

QVariant JDateTimeEdit::jproperty() const
{
    return QVariant::fromValue<void *>(d->property);
}

bool JDateTimeEdit::jsetData(const QByteArray &data)
{
    QVariant _value = d->property->unpack(data);
    if (_value.isNull()) {
        return false;
    }

    setDateTime(QDateTime::fromString(_value.toString(), d->property->format()));

    return dateTime().isValid();
}

void JDateTimeEdit::jsetValue(const QVariant &value)
{
    switch (value.type()) {
    case QVariant::String:
        setDateTime(QDateTime::fromString(value.toString(), d->property->format()));
        break;
    default:
        setDateTime(value.toDateTime());
        break;
    }
}

void JDateTimeEdit::_qDateTimeChanged(const QDateTime &value)
{
    Q_UNUSED(value);
}

// - class JIPv4EditData -

class JIPv4EditData
{
public:
    JIPv4Value *property;
};

// - class JIPv4Edit -

JIPv4Edit::JIPv4Edit(JIPv4Value *property, QWidget *parent)
    : JIPAddressEdit(parent)
    , d(new JIPv4EditData)
{
    d->property = property;
    setReadOnly(!property->editable());
    if (!property->styleSheet().isEmpty()) {
        setStyleSheet(property->styleSheet());
    }
    setToolTip(property->tooltip());
    setWhatsThis(property->whatsThis());

    connect(this, SIGNAL(textChanged(QString)), SLOT(_emit_textChanged(QString)));

    jsetValue(property->original());
}

JIPv4Edit::~JIPv4Edit()
{
    delete d;
}

const JIPv4Value *JIPv4Edit::property() const
{
    return d->property;
}

QByteArray JIPv4Edit::jdata() const
{
    return d->property->pack(text());
}

QVariant JIPv4Edit::jvalue() const
{
    return text();
}

QVariant JIPv4Edit::jtext() const
{
    return text();
}

QVariant JIPv4Edit::jproperty() const
{
    return QVariant::fromValue<void *>(d->property);
}

bool JIPv4Edit::jsetData(const QByteArray &data)
{
    QVariant variant = d->property->unpack(data);
    if (variant.isValid()) {
        setText(variant.toString());
        return true;
    } else {
        return false;
    }
}

void JIPv4Edit::jsetValue(const QVariant &value)
{
    setText(value.toString());
}

void JIPv4Edit::_emit_textChanged(const QString &text)
{
    Q_UNUSED(text);
}

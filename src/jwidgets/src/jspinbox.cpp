#include "precomp.h"
#include "jspinbox.h"
#include "private/jspinbox_p.h"

// - class JSpinBox -

JSpinBox::JSpinBox(QWidget *parent)
    : QSpinBox(parent)
    , d_ptr(new JSpinBoxPrivate(this))
{
    Q_D(JSpinBox);
    d->init();
}

JSpinBox::~JSpinBox()
{
    delete d_ptr;
}

int JSpinBox::radix() const
{
    Q_D(const JSpinBox);
    return d->radix;
}

void JSpinBox::setRadix(int value)
{
    Q_D(JSpinBox);
    if (value < 2 || value > 36) {
        qWarning("radix must between 2 and 36");
        value = qBound(2, value, 36);
    }

    if (d->radix != value) {
        d->radix = value;
        update();
    }
}

int JSpinBox::maskCount() const
{
    Q_D(const JSpinBox);
    return d->maskCount;
}

void JSpinBox::setMaskCount(int value)
{
    Q_D(JSpinBox);
    if (d->maskCount != value) {
        d->maskCount = value;
        update();
    }
}

QChar JSpinBox::fillChar() const
{
    Q_D(const JSpinBox);
    return d->fillChar;
}

void JSpinBox::setFillChar(const QChar &ch)
{
    Q_D(JSpinBox);
    if (d->fillChar != ch) {
        d->fillChar = ch;
        update();
    }
}

QValidator::State JSpinBox::validate(QString &input, int &pos) const
{
    Q_D(const JSpinBox);
    QValidator::State state;
    d->validateAndInpterpret(input, pos, state);
    return state;
}

QString JSpinBox::textFromValue(int value) const
{
    Q_D(const JSpinBox);
    if (d->radix == 10) {
        return QSpinBox::textFromValue(value);
    } else {
        return d->textFromValue(value).toUpper();
    }
}

int JSpinBox::valueFromText(const QString &text) const
{
    Q_D(const JSpinBox);
    QString copy = text;
    int pos = lineEdit()->cursorPosition();
    QValidator::State state;
    return d->validateAndInpterpret(copy, pos, state);
}

void JSpinBox::fixup(QString &str) const
{
    Q_D(const JSpinBox);
    QString copy = str;
    int pos = lineEdit()->cursorPosition();
    QValidator::State state;
    int num = d->validateAndInpterpret(copy, pos, state);
    str = prefix() % d->textFromValue(num) % suffix();
    JSBDEGBUG << str;
}

// - class JLargeSpinBox -

JLargeSpinBox::JLargeSpinBox(QWidget *parent)
    : QDoubleSpinBox(parent)
    , d_ptr(new JLargeSpinBoxPrivate(this))
{
    Q_D(JLargeSpinBox);
    d->init();
}

JLargeSpinBox::~JLargeSpinBox()
{
    delete d_ptr;
}

int JLargeSpinBox::radix() const
{
    Q_D(const JLargeSpinBox);
    return d->radix;
}

void JLargeSpinBox::setRadix(int value)
{
    Q_D(JLargeSpinBox);
    if (value < 2 || value > 36) {
        qWarning("radix must between 2 and 36");
        value = qBound(2, value, 36);
    }

    if (d->radix != value) {
        d->radix = value;
        update();
    }
}

int JLargeSpinBox::maskCount() const
{
    Q_D(const JLargeSpinBox);
    return d->maskCount;
}

void JLargeSpinBox::setMaskCount(int value)
{
    Q_D(JLargeSpinBox);
    if (d->maskCount != value) {
        d->maskCount = value;
        update();
    }
}

QChar JLargeSpinBox::fillChar() const
{
    Q_D(const JLargeSpinBox);
    return d->fillChar;
}

void JLargeSpinBox::setFillChar(const QChar &ch)
{
    Q_D(JLargeSpinBox);
    if (d->fillChar != ch) {
        d->fillChar = ch;
        update();
    }
}

qlonglong JLargeSpinBox::value() const
{
    Q_D(const JLargeSpinBox);
    return text().toULongLong(0, d->radix);
}

qlonglong JLargeSpinBox::singleStep() const
{
    return (qlonglong)QDoubleSpinBox::singleStep();
}

void JLargeSpinBox::setSingleStep(const qlonglong &value)
{
    QDoubleSpinBox::setSingleStep((double)value);
}

qlonglong JLargeSpinBox::minumum() const
{
    return (qlonglong)QDoubleSpinBox::minimum();
}

void JLargeSpinBox::setMinimum(const qlonglong &min)
{
    QDoubleSpinBox::setMinimum((double)min);
}

qlonglong JLargeSpinBox::maximum() const
{
    return (qlonglong)QDoubleSpinBox::maximum();
}

void JLargeSpinBox::setMaximum(const qlonglong &max)
{
    QDoubleSpinBox::setMaximum((double)max);
}

void JLargeSpinBox::setRange(const qlonglong &min, const qlonglong &max)
{
    QDoubleSpinBox::setRange((double)min, (double)max);
}

QValidator::State JLargeSpinBox::validate(QString &input, int &pos) const
{
    Q_D(const JLargeSpinBox);
    QValidator::State state;
    d->validateAndInterpret(input, pos, state);
    return state;
}

double JLargeSpinBox::valueFromText(const QString &text) const
{
    Q_D(const JLargeSpinBox);
    QString copy = text;
    int pos = lineEdit()->cursorPosition();
    QValidator::State state;
    return (double)d->validateAndInterpret(copy, pos, state);
}

QString JLargeSpinBox::textFromValue(double value) const
{
    Q_D(const JLargeSpinBox);
    if (d->radix == 10) {
        return QDoubleSpinBox::textFromValue((double)value);
    } else {
        return d->textFromValue(value);
    }
}

void JLargeSpinBox::fixup(QString &str) const
{
    Q_D(const JLargeSpinBox);
    QString copy = str;
    int pos = lineEdit()->cursorPosition();
    QValidator::State state;
    qlonglong num = d->validateAndInterpret(copy, pos, state);
    str = prefix() % d->textFromValue(num) % suffix();
    JSBDEGBUG << str;
}

void JLargeSpinBox::setValue(const qlonglong &value)
{
    QDoubleSpinBox::setValue((double)value);
}

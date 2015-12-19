#include "precomp.h"
#include "jspinbox_p.h"
#include "../jspinbox.h"

// - JSpinBoxPrivate -

JSpinBoxPrivate::JSpinBoxPrivate(JSpinBox *parent)
    : q_ptr(parent)
    , radix(10)
    , maskCount(0)
    , fillChar()
{

}

int JSpinBoxPrivate::validateAndInpterpret(QString &input, int &pos, QValidator::State &state) const
{
    Q_Q(const JSpinBox);
    Q_UNUSED(pos);

    QString copy = input;
    QString t = q->prefix();

    if (!t.isEmpty()) {
        copy = copy.mid(t.size());
    }

    t = q->suffix();
    if (!t.isEmpty()) {
        copy.chop(t.size());
    }

    const int minimum = q->minimum();
    const int maximum = q->maximum();
    int num = minimum;

    state = QValidator::Acceptable;

    if (copy.isEmpty() || (minimum < 0 && copy[0] == '-')) {
        state = QValidator::Intermediate;
    } else {
        bool ok = false;
        num = copy.toInt(&ok, radix);
        if (!ok) {
            state = QValidator::Invalid;
        } else if (minimum <= num && num <= maximum) {
            state = (copy == copy.toUpper()) ? QValidator::Acceptable : QValidator::Intermediate;
        } else if ((num >= 0 && num > maximum) || (num < 0 && num < minimum)) {
            state = QValidator::Invalid;
        } else {
            state = QValidator::Intermediate;
        }
    }

    return num;
}

QString JSpinBoxPrivate::textFromValue(int value) const
{
//    Q_Q(const JSpinBox);
    QString str = QString::number(value, radix).toUpper();
    if ((radix != 10)
//            && (q->maximum() <= (0x1LL << (maskCount * 4)) - 1)
            && (str.count() < maskCount)) {
        if (!fillChar.isNull()) {
            str.prepend(QString(maskCount - str.count(), fillChar));
        }
    }

    return str;
}

void JSpinBoxPrivate::init()
{

}

// - class JLargeSpinBoxPrivate -

JLargeSpinBoxPrivate::JLargeSpinBoxPrivate(JLargeSpinBox *parent)
    : q_ptr(parent)
    , radix(10)
    , maskCount(0)
    , fillChar()
{

}

qlonglong JLargeSpinBoxPrivate::validateAndInterpret(QString &input, int &pos, QValidator::State &state) const
{
    Q_Q(const JLargeSpinBox);
    Q_UNUSED(pos);

    QString copy = input;
    QString t = q->prefix();

    if (!t.isEmpty()) {
        copy = copy.mid(t.size());
    }

    t = q->suffix();
    if (!t.isEmpty()) {
        copy.chop(t.size());
    }

    const qlonglong minimum = q->minimum();
    const qlonglong maximum = q->maximum();
    qlonglong num = minimum;

    state = QValidator::Acceptable;

    if (copy.isEmpty() || (minimum < 0 && copy[0] == '-')) {
        state = QValidator::Intermediate;
    } else {
        bool ok = false;
        num = copy.toLongLong(&ok, radix);
        if (!ok) {
            state = QValidator::Invalid;
        } else if (minimum <= num && num <= maximum) {
            state = (copy == copy.toUpper()) ? QValidator::Acceptable : QValidator::Intermediate;
        } else if ((num >= 0 && num > maximum) || (num < 0 && num < minimum)) {
            state = QValidator::Invalid;
        } else {
            state = QValidator::Intermediate;
        }
    }

    return num;
}

QString JLargeSpinBoxPrivate::textFromValue(const qlonglong &value) const
{
//    Q_Q(const JSpinBox);
    QString str = QString::number(value, radix).toUpper();
    if ((radix != 10)
//            && (q->maximum() <= (0x1LL << (maskCount * 4)) - 1)
            && (str.count() < maskCount)) {
        if (!fillChar.isNull()) {
            str.prepend(QString(maskCount - str.count(), fillChar));
        }
    }

    return str;
}

void JLargeSpinBoxPrivate::_emit_valueChanged(double value)
{
    Q_Q(JLargeSpinBox);
    Q_EMIT q->valueChanged((qlonglong)value);
}

void JLargeSpinBoxPrivate::init()
{
    Q_Q(JLargeSpinBox);
    connect(q, SIGNAL(valueChanged(double)), SLOT(_emit_valueChanged(double)));
}



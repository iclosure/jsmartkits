#ifndef JSPINBOX_P_H
#define JSPINBOX_P_H

#include "../jwidgets_global.h"
#include "../jspinbox.h"

//#define JSPINBOX_QSBDEBUG
#ifdef JSPINBOX_QSBDEBUG
#define JSBDEBUG qDebug
#else
#define JSBDEGBUG if (false) qDebug()
#endif

// - class JSpinBoxPrivate -

class JSpinBoxPrivate
{
    J_DECLARE_PUBLIC(JSpinBox)
public:
    JSpinBoxPrivate(JSpinBox *parent = 0);

    int validateAndInpterpret(QString &input, int &pos, QValidator::State &state) const;

    QString textFromValue(int value) const;

private:
    void init();

private:
    int radix;
    int maskCount;
    QChar fillChar;
};

// - class JLargeSpinBpxPrivate

class JLargeSpinBoxPrivate : public QObject
{
    Q_OBJECT
    J_DECLARE_PUBLIC(JLargeSpinBox)
public:
    JLargeSpinBoxPrivate(JLargeSpinBox *parent = 0);

    qlonglong validateAndInterpret(QString &input, int &pos, QValidator::State &state) const;

    QString textFromValue(const qlonglong &value) const;

public Q_SLOTS:
    void _emit_valueChanged(double value);

private:
    void init();

private:
    int radix;
    int maskCount;
    QChar fillChar;
};

#endif // JSPINBOX_P_H

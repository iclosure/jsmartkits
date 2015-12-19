#ifndef JSPINBOX_H
#define JSPINBOX_H

#include "jwidgets_global.h"
#include <QSpinBox>

// - class JSpinBox -

class JSpinBoxPrivate;

class JWIDGETS_EXPORT JSpinBox : public QSpinBox
{
    Q_OBJECT
public:
    explicit JSpinBox(QWidget *parent = 0);
    ~JSpinBox();

    int radix() const;
    void setRadix(int value);

    int maskCount() const;
    void setMaskCount(int value);

    QChar fillChar() const;
    void setFillChar(const QChar &ch);

protected:
    virtual QValidator::State validate(QString &input, int &pos) const;
    virtual QString textFromValue(int value) const;
    virtual int valueFromText(const QString &text) const;
    virtual void fixup(QString &str) const;

private:
    Q_DISABLE_COPY(JSpinBox)
    J_DECLARE_PRIVATE(JSpinBox)
};

// - class JLargeSinBox -

class JLargeSpinBoxPrivate;

class JWIDGETS_EXPORT JLargeSpinBox : public QDoubleSpinBox
{
    Q_OBJECT
public:
    explicit JLargeSpinBox(QWidget *parent = 0);
    ~JLargeSpinBox();

    int radix() const;
    void setRadix(int value);

    int maskCount() const;
    void setMaskCount(int value);

    QChar fillChar() const;
    void setFillChar(const QChar &ch);

    qlonglong value() const;

    qlonglong singleStep() const;
    void setSingleStep(const qlonglong &value);

    qlonglong minumum() const;
    void setMinimum(const qlonglong &min);

    qlonglong maximum() const;
    void setMaximum(const qlonglong &max);

    void setRange(const qlonglong &min, const qlonglong &max);

protected:
    virtual QValidator::State validate(QString &input, int &pos) const;
    virtual double valueFromText(const QString &text) const;
    virtual QString textFromValue(double value) const;
    virtual void fixup(QString &str) const;

public Q_SLOTS:
    void setValue(const qlonglong &value);

Q_SIGNALS:
    void valueChanged(const qlonglong &);

private:
    Q_DISABLE_COPY(JLargeSpinBox)
    J_DECLARE_PRIVATE(JLargeSpinBox)
};

#endif // JSPINBOX_H

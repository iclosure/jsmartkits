#ifndef JXMLTABLE_WIDGET_H
#define JXMLTABLE_WIDGET_H

#include "../jwidgets_global.h"
#include "jxmltable_data.h"
#include <QStandardItem>
#include <QCheckBox>
#include <QLabel>
#include <QLineEdit>
#include <QPlainTextEdit>
#include <QComboBox>
#include <QDoubleSpinBox>
#include <QBuffer>
#include "../jipaddressedit.h"
#include "../jcheckcombobox.h"
#include "../jspinbox.h"

// - class JCheckBoxItem -

class JCheckBoxItemData;

class JWIDGETS_EXPORT JCheckBoxItem : public QStandardItem
{
public:
    explicit JCheckBoxItem(JBoolValue *property);
    virtual ~JCheckBoxItem();

    const JBoolValue *property() const;

    static void initItem(JBoolValue *itemValue, QStandardItem *item);

private:
    JCheckBoxItemData *d;
};

// - class JBoolBox -

class JBoolBoxData;

class JWIDGETS_EXPORT JBoolBox : public QCheckBox
{
    Q_OBJECT
    Q_PROPERTY(QByteArray jdata READ jdata WRITE jsetData)
    Q_PROPERTY(QVariant jvalue READ jvalue WRITE jsetValue)
    Q_PROPERTY(QVariant jtext READ jtext)
    Q_PROPERTY(QVariant jproperty READ jproperty)
public:
    explicit JBoolBox(JBoolValue *property, QWidget *parent = 0);
    virtual ~JBoolBox();

    const JBoolValue *property() const;
    QByteArray jdata() const;
    QVariant jvalue() const;
    QVariant jtext() const;
    QVariant jproperty() const;

public Q_SLOTS:
    bool jsetData(const QByteArray &data);
    void jsetValue(const QVariant &value);

private Q_SLOTS:
    void _emit_stateChanged(int state);

private:
    Q_DISABLE_COPY(JBoolBox)
    JBoolBoxData *d;
};

// - class JNumericIBox -

class JNumericIBoxData;

class JWIDGETS_EXPORT JNumericIBox : public JSpinBox
{
    Q_OBJECT
    Q_PROPERTY(QByteArray jdata READ jdata WRITE jsetData)
    Q_PROPERTY(QVariant jvalue READ jvalue WRITE jsetValue)
    Q_PROPERTY(QVariant jtext READ jtext WRITE jsetValue)
    Q_PROPERTY(QVariant jproperty READ jproperty)
public:
    explicit JNumericIBox(JNumericValue *property, QWidget *parent = 0);
    virtual ~JNumericIBox();

    const JNumericValue *property() const;
    QByteArray jdata() const;
    QVariant jvalue() const;
    QVariant jtext() const;
    QVariant jproperty() const;

public Q_SLOTS:
    bool jsetData(const QByteArray &data);
    void jsetValue(const QVariant &value);

private Q_SLOTS:
    void _emit_valueChanged(int value);

private:
    Q_DISABLE_COPY(JNumericIBox)
    JNumericIBoxData *d;
};

// - class JNumericLBox -

class JNumericLBoxData;

class JWIDGETS_EXPORT JNumericLBox : public JLargeSpinBox
{
    Q_OBJECT
    Q_PROPERTY(QByteArray jdata READ jdata WRITE jsetData)
    Q_PROPERTY(QVariant jvalue READ jvalue WRITE jsetValue)
    Q_PROPERTY(QVariant jtext READ jtext WRITE jsetValue)
    Q_PROPERTY(QVariant jproperty READ jproperty)
public:
    explicit JNumericLBox(JNumericValue *property, QWidget *parent = 0);
    virtual ~JNumericLBox();

    const JNumericValue *property() const;
    QByteArray jdata() const;
    QVariant jvalue() const;
    QVariant jtext() const;
    QVariant jproperty() const;

public Q_SLOTS:
    bool jsetData(const QByteArray &data);
    void jsetValue(const QVariant &value);

private Q_SLOTS:
    void _emit_valueChanged(qlonglong value);

private:
    Q_DISABLE_COPY(JNumericLBox)
    JNumericLBoxData *d;
};

// - class JNumericDBox -

class JNumericDBoxData;

class JWIDGETS_EXPORT JNumericDBox : public QDoubleSpinBox
{
    Q_OBJECT
    Q_PROPERTY(QByteArray jdata READ jdata WRITE jsetData)
    Q_PROPERTY(QVariant jvalue READ jvalue WRITE jsetValue)
    Q_PROPERTY(QVariant jtext READ jtext WRITE jsetValue)
    Q_PROPERTY(QVariant jproperty READ jproperty)
public:
    explicit JNumericDBox(JNumericValue *property, QWidget *parent = 0);
    virtual ~JNumericDBox();

    const JNumericValue *property() const;
    QByteArray jdata() const;
    QVariant jvalue() const;
    QVariant jtext() const;
    QVariant jproperty() const;

public Q_SLOTS:
    bool jsetData(const QByteArray &data);
    void jsetValue(const QVariant &value);

private Q_SLOTS:
    void _emit_valueChanged(double value);

private:
    Q_DISABLE_COPY(JNumericDBox)
    JNumericDBoxData *d;
};

// - class JStringItem -

class JStringItemData;

class JWIDGETS_EXPORT JStringItem : public QStandardItem
{
public:
    explicit JStringItem(JItemValue *property);
    virtual ~JStringItem();

    const JItemValue *property() const;

private:
    JStringItemData *d;
};

// - class JStringLabel -

class JStringLabelData;

class JStringLabel : public QLabel
{
    Q_OBJECT
    Q_PROPERTY(QByteArray jdata READ jdata WRITE jsetData)
    Q_PROPERTY(QVariant jvalue READ jvalue WRITE jsetValue)
    Q_PROPERTY(QVariant jtext READ jtext WRITE jsetValue)
    Q_PROPERTY(QVariant jproperty READ jproperty)
public:
    explicit JStringLabel(JStringValue *property, QWidget *parent = 0);
    JStringLabel(JStringValue *property, const QString &text, QWidget *parent = 0);
    virtual ~JStringLabel();

    const JStringValue *property() const;
    QByteArray jdata() const;
    QVariant jvalue() const;
    QVariant jtext() const;
    QVariant jproperty() const;

public Q_SLOTS:
    bool jsetData(const QByteArray &data);
    void jsetValue(const QVariant &value);

private:
    void init();

private:
    Q_DISABLE_COPY(JStringLabel)
    JStringLabelData *d;
};

// - class JStringLEdit -

class JStringLEditData;

class JStringLEdit : public QLineEdit
{
    Q_OBJECT
    Q_PROPERTY(QByteArray jdata READ jdata WRITE jsetData)
    Q_PROPERTY(QVariant jvalue READ jvalue WRITE jsetValue)
    Q_PROPERTY(QVariant jtext READ jtext WRITE jsetValue)
    Q_PROPERTY(QVariant jproperty READ jproperty)
public:
    explicit JStringLEdit(JStringValue *property, QWidget *parent = 0);
    virtual ~JStringLEdit();

    const JStringValue *property() const;
    QByteArray jdata() const;
    QVariant jvalue() const;
    QVariant jtext() const;
    QVariant jproperty() const;

public Q_SLOTS:
    bool jsetData(const QByteArray &data);
    void jsetValue(const QVariant &value);

private Q_SLOTS:
    void _emit_textChanged(const QString &text);

private:
    Q_DISABLE_COPY(JStringLEdit)
    JStringLEditData *d;
};

// - class JStringTEdit -

class JStringTEditData;

class JWIDGETS_EXPORT JStringTEdit : public QPlainTextEdit
{
    Q_OBJECT
    Q_PROPERTY(QByteArray jdata READ jdata WRITE jsetData)
    Q_PROPERTY(QVariant jvalue READ jvalue WRITE jsetValue)
    Q_PROPERTY(QVariant jtext READ jtext WRITE jsetValue)
    Q_PROPERTY(QVariant jproperty READ jproperty)
public:
    explicit JStringTEdit(JStringValue *property, QWidget *parent = 0);
    virtual ~JStringTEdit();

    const JStringValue *property() const;
    QByteArray jdata() const;
    QVariant jvalue() const;
    QVariant jtext() const;
    QVariant jproperty() const;

public Q_SLOTS:
    bool jsetData(const QByteArray &data);
    void jsetValue(const QVariant &value);

private Q_SLOTS:
    void _emit_textChanged();

private:
    Q_DISABLE_COPY(JStringTEdit)
    JStringTEditData *d;
};

// - class JEnumBox -

class JEnumBoxData;

class JWIDGETS_EXPORT JEnumBox : public QComboBox
{
    Q_OBJECT
    Q_PROPERTY(QByteArray jdata READ jdata WRITE jsetData)
    Q_PROPERTY(QVariant jvalue READ jvalue WRITE jsetValue)
    Q_PROPERTY(QVariant jtext READ jtext WRITE jsetValue)
    Q_PROPERTY(QVariant jproperty READ jproperty)
public:
    explicit JEnumBox(JEnumValue *property, QWidget *parent = 0);
    virtual ~JEnumBox();

    const JEnumValue *property() const;
    QByteArray jdata() const;
    QVariant jvalue() const;
    QVariant jtext() const;
    QVariant jproperty() const;

public Q_SLOTS:
    bool jsetData(const QByteArray &data);
    void jsetValue(const QVariant &value);

private Q_SLOTS:
    void _emit_currentIndexChanged(int index);

private:
    Q_DISABLE_COPY(JEnumBox)
    JEnumBoxData *d;
};

// - class JMultiEnumBox -

class JMultiEnumBoxData;

class JWIDGETS_EXPORT JMultiEnumBox : public JCheckComboBox
{
    Q_OBJECT
    Q_PROPERTY(QByteArray jdata READ jdata WRITE jsetData)
    Q_PROPERTY(QVariant jvalue READ jvalue WRITE jsetValue)
    Q_PROPERTY(QVariant jtext READ jtext WRITE jsetValue)
    Q_PROPERTY(QVariant jproperty READ jproperty)
public:
    explicit JMultiEnumBox(JEnumValue *property, QWidget *parent = 0);
    virtual ~JMultiEnumBox();

    const JEnumValue *property() const;
    QByteArray jdata() const;
    QVariant jvalue() const;
    QVariant jtext() const;
    QVariant jproperty() const;

public Q_SLOTS:
    bool jsetData(const QByteArray &data);
    void jsetValue(const QVariant &value);

private Q_SLOTS:
    void _emit_encodeChanged();

private:
    Q_DISABLE_COPY(JMultiEnumBox)
    JMultiEnumBoxData *d;
};

// - class JIPv4Edit -

class JIPv4EditData;

class JWIDGETS_EXPORT JIPv4Edit : public JIPAddressEdit
{
    Q_OBJECT
    Q_PROPERTY(QByteArray jdata READ jdata WRITE jsetData)
    Q_PROPERTY(QVariant jvalue READ jvalue WRITE jsetValue)
    Q_PROPERTY(QVariant jtext READ jtext WRITE jsetValue)
    Q_PROPERTY(QVariant jproperty READ jproperty)
public:
    explicit JIPv4Edit(JIPv4Value *property, QWidget *parent = 0);
    virtual ~JIPv4Edit();

    const JIPv4Value *property() const;
    QByteArray jdata() const;
    QVariant jvalue() const;
    QVariant jtext() const;
    QVariant jproperty() const;

public Q_SLOTS:
    bool jsetData(const QByteArray &data);
    void jsetValue(const QVariant &value);

private Q_SLOTS:
    void _emit_textChanged(const QString &text);

private:
    Q_DISABLE_COPY(JIPv4Edit)
    JIPv4EditData *d;
};

#endif // JXMLTABLE_WIDGET_H

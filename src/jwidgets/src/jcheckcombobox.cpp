#include "precomp.h"
#include "jcheckcombobox.h"

// - class JCheckComboBoxPrivate -

class JCheckComboBoxPrivate
{
    J_DECLARE_PUBLIC(JCheckComboBox)
public:
    JCheckComboBoxPrivate(JCheckComboBox *parent)
        : q_ptr(parent)
        , value(0)
        , canNotify(true)
    {

    }

    QCheckBox *checkBox(int index) const;
    QCheckBox *checkBox(QListWidgetItem *item) const;

private:
    void init();

private:
    QListWidget *listWidget;
    qulonglong value;
    bool canNotify;
};

/**
 * @brief JCheckComboBoxPrivate::checkBox
 * @param index
 * @return
 */
QCheckBox *JCheckComboBoxPrivate::checkBox(int index) const
{
    QListWidgetItem *item = listWidget->item(index);
    if (item) {
        return qobject_cast<QCheckBox *>(listWidget->itemWidget(item));
    } else {
        return 0;
    }
}

/**
 * @brief JCheckComboBoxPrivate::checkBox
 * @param item
 * @return
 */
QCheckBox *JCheckComboBoxPrivate::checkBox(QListWidgetItem *item) const
{
    return qobject_cast<QCheckBox *>(listWidget->itemWidget(item));
}

void JCheckComboBoxPrivate::init()
{
    Q_Q(JCheckComboBox);
    QLineEdit *lineEdit = new QLineEdit(q);
    q->setLineEdit(lineEdit);
    lineEdit->setReadOnly(true);
    lineEdit->installEventFilter(q);

    listWidget = new QListWidget(q);
    listWidget->setObjectName("listWidget");
    listWidget->setFrameShape(QFrame::NoFrame);
#if defined(Q_WS_WIN) || defined(Q_OS_WIN)
    listWidget->setStyle(QStyleFactory::create("windowsxp"));
#endif
    listWidget->setViewMode(QListWidget::ListMode);
    listWidget->setSelectionMode(QAbstractItemView::SingleSelection);
    listWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
    q->setModel(listWidget->model());
    q->setView(listWidget);
}

/**
 * @brief JCheckComboBox::JCheckComboBox
 * @param parent
 */
JCheckComboBox::JCheckComboBox(QWidget *parent)
    : QComboBox(parent)
    , d_ptr(new JCheckComboBoxPrivate(this))
{
    Q_D(JCheckComboBox);
    d->init();
}

/**
 * @brief JCheckComboBox::~JCheckComboBox
 */
JCheckComboBox::~JCheckComboBox()
{
    delete d_ptr;
}

/**
 * @brief JCheckComboBox::addItem
 * @param text
 */
void JCheckComboBox::addItem(const QString &text)
{
    Q_D(JCheckComboBox);
    addItem(text, (0x1ULL << d->listWidget->count()));
}

/**
 * @brief JCheckComboBox::addItem
 * @param text
 * @param encode
 */
void JCheckComboBox::addItem(const QString &text, const qulonglong &encode)
{
    Q_D(JCheckComboBox);
    QListWidgetItem *item = new QListWidgetItem(d->listWidget);
    item->setToolTip(text);

    QCheckBox *checkBox = new QCheckBox(text, d->listWidget);
    checkBox->setStyleSheet("QCheckBox{border-radius:2px;}"
                            "QCheckBox:hover{color:#e9be4b;background:#545d6b;}");
    checkBox->setProperty("encode", encode);

    d->listWidget->addItem(item);
    d->listWidget->setItemWidget(item, checkBox);

    connect(checkBox, SIGNAL(stateChanged(int)), SLOT(onStateChanged(int)));
}

/**
 * @brief JCheckComboBox::removeItem
 * @param index
 */
void JCheckComboBox::removeItem(int index)
{
    Q_D(JCheckComboBox);
    QListWidgetItem *item = d->listWidget->item(index);
    if (!item) {
        return;
    }

    QCheckBox *checkBox = d->checkBox(index);
    if (!checkBox) {
        return;
    }

    if (checkBox->checkState() != Qt::Unchecked) {
        checkBox->setCheckState(Qt::Unchecked);
    }

    delete item;
}

/**
 * @brief JCheckComboBox::removeItem
 * @param encode
 */
void JCheckComboBox::removeItem(const qulonglong &encode)
{
    Q_D(JCheckComboBox);
    int count = d->listWidget->count();
    for (int i = 0; i < count; ++i) {
        QCheckBox *checkBox = d->checkBox(i);
        if (checkBox && checkBox->property("encode").toULongLong() == encode) {
            removeItem(i);
            break;
        }
    }
}

/**
 * @brief JCheckComboBox::removeItem
 * @param text
 */
void JCheckComboBox::removeItem(const QString &text)
{
    Q_D(JCheckComboBox);
    int count = d->listWidget->count();
    for (int i = 0; i < count; ++i) {
        QCheckBox *checkBox = d->checkBox(i);
        if (checkBox && checkBox->text() == text) {
            removeItem(i);
            break;
        }
    }
}

/**
 * @brief JCheckComboBox::clearItem
 */
void JCheckComboBox::clearItem()
{
    Q_D(JCheckComboBox);
    int count = d->listWidget->count();
    while (--count) {
        QWidget *widget = d->listWidget->itemWidget(d->listWidget->item(count));
        if (widget) {
            disconnect(widget, SIGNAL(stateChanged(int)), this, SLOT(onStateChanged(int)));
        }
    }

    d->listWidget->clear();
    d->value = 0;
    lineEdit()->setText("");
    lineEdit()->setToolTip("");
}

/**
 * @brief JCheckComboBox::encode
 * @param index
 * @return
 */
qulonglong JCheckComboBox::encode(int index) const
{
    Q_D(const JCheckComboBox);
    if (index == -1) {
        return d->value;
    } else {
        return d->value & (0x1ULL << index);
    }
}

/**
 * @brief JCheckComboBox::text
 * @param index
 * @return
 */
QString JCheckComboBox::text(int index) const
{
    Q_D(const JCheckComboBox);
    if (index == -1) {
        return currentText();
    } else {
        QCheckBox *checkBox = d->checkBox(index);
        if (checkBox) {
            return checkBox->text();
        } else {
            return QString::null;
        }
    }
}

/**
 * @brief JCheckComboBox::checkedItems
 * @return
 */
QStringList JCheckComboBox::checkedItems() const
{
    Q_D(const JCheckComboBox);
    int count = d->listWidget->count();
    QStringList labels;
    for (int i = 0; i < count; ++i) {
        QCheckBox *checkBox = d->checkBox(i);
        if (checkBox && checkBox->checkState() == Qt::Checked) {
            labels << checkBox->text();
        }
    }

    return labels;
}

/**
 * @brief JCheckComboBox::setEncode
 * @param value
 */
void JCheckComboBox::setEncode(const qulonglong &value)
{
    Q_D(JCheckComboBox);
    if (value == d->value) {
        return;
    }

    d->canNotify = false;

    int count = d->listWidget->count();
    for (int i = 0; i < count; ++i) {
        QCheckBox *checkBox = d->checkBox(i);
        if (checkBox) {
            if (value & (0x1ULL << i)) {
                checkBox->setCheckState(Qt::Checked);
            } else {
                checkBox->setCheckState(Qt::Unchecked);
            }
        }
    }

    d->canNotify = true;

    Q_EMIT encodeChanged(d->value);
    Q_EMIT encodeChanged(currentText());
}

/**
 * @brief JCheckComboBox::setEncode
 * @param value
 */
void JCheckComboBox::setEncode(const QString &value)
{
    Q_D(JCheckComboBox);
    if (value == currentText()) {
        return;
    }

    d->canNotify = false;

    int count = d->listWidget->count();
    for (int i = 0; i < count; ++i) {
        QCheckBox *checkBox = d->checkBox(i);
        if (checkBox) {
            if (value.contains(checkBox->text())) {
                checkBox->setCheckState(Qt::Checked);
            } else {
                checkBox->setCheckState(Qt::Unchecked);
            }
        }
    }

    d->canNotify = true;

    Q_EMIT encodeChanged(d->value);
    Q_EMIT encodeChanged(currentText());
}

/**
 * @brief JCheckComboBox::eventFilter
 * @param o
 * @param e
 * @return
 */
bool JCheckComboBox::eventFilter(QObject *o, QEvent *e)
{
    if (o->inherits("QLineEdit")) {
        if (e->type() == QEvent::MouseButtonPress) {
            QComboBox::showPopup();
        }
    }

    return false;
}

/**
 * @brief JCheckComboBox::onStateChanged
 * @param state
 */
void JCheckComboBox::onStateChanged(int state)
{
    QCheckBox *checkBox = qobject_cast<QCheckBox *>(sender());
    if (!checkBox) {
        return;
    }

    Q_D(JCheckComboBox);

    QString text = currentText();

    qulonglong encode = checkBox->property("encode").toULongLong();
    if (state == Qt::Checked) {
        d->value |= encode;
        if (text.isEmpty()) {
            text.append(checkBox->text());
        } else {
            text.append('|' % checkBox->text());
        }
    } else {
        d->value &= ~encode;
        if (!text.isEmpty()) {
            text.remove(checkBox->text()).remove("||");
            if (text.startsWith('|')) {
                text.remove(0, 1);
            }
            if (text.endsWith('|')) {
                text.chop(1);
            }
        }
    }

    if (text.isEmpty()) {
        lineEdit()->setText("");
        setToolTip("");
    } else {
        lineEdit()->setText(text);
        text = "* " % text;
        setToolTip(text.replace('|', "\n* "));
    }

    if (d->canNotify) {
        Q_EMIT encodeChanged(d->value);
        Q_EMIT encodeChanged(currentText());
    }
}

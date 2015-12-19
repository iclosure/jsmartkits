#include "precomp.h"
#include "jipaddressedit.h"

// - class JIPAddressEditPrivate -

class JIPAddressEditPrivate : public QLineEdit
{
    J_DECLARE_PUBLIC(JIPAddressEdit)
public:
    JIPAddressEditPrivate(JIPAddressEdit *parent = 0)
        : QLineEdit(parent)
        , q_ptr(parent)
    {
        setObjectName("lineEdit");
        setFocusPolicy(Qt::NoFocus);
        setReadOnly(true);
    }

protected:
    void paintEvent(QPaintEvent *e);

private:
    void init();

private:
    QList<QSpinBox *> spins;
};

void JIPAddressEditPrivate::paintEvent(QPaintEvent *e)
{
    QLineEdit::paintEvent(e);

    QPainter painter(this);
    painter.save();
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setBrush(QBrush(Qt::black));
    qreal x = width() / 4.;
    qreal y = height() / 2.;
    for (int i = 1; i <= 3; ++ i) {
        painter.drawEllipse(QPointF(x * i, y), 1.2, 1.2);
    }
    painter.restore();
}

void JIPAddressEditPrivate::init()
{
    Q_Q(JIPAddressEdit);

    q->setFocusPolicy(Qt::NoFocus);

    QHBoxLayout *horiLayoutPrivate = new QHBoxLayout(this);
    horiLayoutPrivate->setContentsMargins(0, 0, 0, 0);
    horiLayoutPrivate->setSpacing(0);

    spins << new QSpinBox(this) << new QSpinBox(this)
          << new QSpinBox(this) << new QSpinBox(this);
    QListIterator<QSpinBox *> iter(spins);
    while (iter.hasNext()) {
        QSpinBox *spinBox = iter.next();
        spinBox->setObjectName("cellSpinBox");
        spinBox->setButtonSymbols(QAbstractSpinBox::NoButtons);
        spinBox->setAlignment(Qt::AlignCenter);
        spinBox->setFocusPolicy(Qt::WheelFocus);
        spinBox->setRange(0, 255);
        horiLayoutPrivate->addWidget(spinBox);
        if (iter.hasNext()) {
            QWidget::setTabOrder(spinBox, iter.peekNext());
        }
        spinBox->setStyleSheet("QSpinBox{border:none;background:transparent;}");
        QObject::connect(spinBox, SIGNAL(valueChanged(int)), q, SLOT(_emit_valueChanged()));
    }

    QHBoxLayout *horiLayoutMain = new QHBoxLayout(q);
    horiLayoutMain->setContentsMargins(0, 0, 0, 0);
    horiLayoutMain->setSpacing(0);
    horiLayoutMain->addWidget(this);
}

// - class JIPAddressEdit -

JIPAddressEdit::JIPAddressEdit(QWidget *parent)
    : QWidget(parent)
    , d_ptr(new JIPAddressEditPrivate(this))
{
    Q_D(JIPAddressEdit);
    d->init();
}

JIPAddressEdit::~JIPAddressEdit()
{
    delete d_ptr;
}

QString JIPAddressEdit::text() const
{
    Q_D(const JIPAddressEdit);
    QString text;
    QListIterator<QSpinBox *> iter(d->spins);
    while (iter.hasNext()) {
        QString string = iter.next()->text();
        if (string.isEmpty()) {
            text.append("0.");
        } else {
            text.append(string % '.');
        }
    }

    text.chop(1);
    return text;
}

bool JIPAddressEdit::isReadOnly() const
{
    Q_D(const JIPAddressEdit);
    return d->spins.at(0)->isReadOnly();
}

void JIPAddressEdit::setReadOnly(bool value)
{
    Q_D(JIPAddressEdit);
    QMutableListIterator<QSpinBox *> iter(d->spins);
    while (iter.hasNext()) {
        iter.next()->setReadOnly(value);
    }
}

void JIPAddressEdit::setText(const QString &text)
{
    if (QString(text).remove(' ') == this->text()) {
        return;
    }

    QRegExp regExp("((2[0-4]\\d|25[0-5]|[01]?\\d\\d?)\\.){3}(2[0-4]\\d|25[0-5]|[01]?\\d\\d?)");
    if (!regExp.exactMatch(text)) {
        return;
    }

    Q_D(JIPAddressEdit);
    QStringList texts = text.split('.', QString::SkipEmptyParts);
    QListIterator<QSpinBox *> iterSpin(d->spins);
    QStringListIterator iterText(texts);
    while (iterSpin.hasNext() && iterText.hasNext()) {
        iterSpin.next()->setValue(iterText.next().toInt());
    }

    Q_EMIT textChanged(text);
}

void JIPAddressEdit::_emit_valueChanged()
{
    Q_EMIT textChanged(text());
}

#include "precomp.h"
#include "jskinbutton.h"

JSkinButton::JSkinButton(const QString &name, QPixmap &pixmap, QWidget *parent)
    : QWidget(parent, Qt::FramelessWindowHint)
    , q_mouseEnter (false)
    , q_downloadCount (0)
{
    setFixedSize(140, 160);

    QLabel* nameLabel = new QLabel(name, this);
    QLabel* downloadLabel = new QLabel(tr("Download count: %1").arg(q_downloadCount), this);

    QLabel* iconLabel = new QLabel(this);
    iconLabel->setPixmap(pixmap);
    iconLabel->setScaledContents(true);
    iconLabel->setFixedHeight(65);

    QPushButton* selectButton = new QPushButton(tr("Use skin"), this);
    selectButton->setCursor(Qt::PointingHandCursor);
    selectButton->setContentsMargins(0, 0, 0, 0);
    selectButton->setStyleSheet("QPushButton { border-radius:3px; border:1px solid rgb(180, 190, 255); "
                                             "color:rgb(70, 70, 255); background:rgba(180, 190, 255, 150); }");
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(5);
    mainLayout->addWidget(iconLabel, 0, Qt::AlignHCenter);
    mainLayout->addWidget(nameLabel, 0, Qt::AlignHCenter);
    mainLayout->addWidget(downloadLabel, 0, Qt::AlignHCenter);
    mainLayout->addWidget(selectButton, 0, Qt::AlignHCenter);

    ///////////////////////////////////////////////////////////////////////

    connect(selectButton, SIGNAL(clicked(bool)), this, SLOT(slotThisClicked(bool)));
}

void JSkinButton::paintEvent(QPaintEvent *)
{
    QPainter painter(this);

    //! 背景刷
    QLinearGradient linear(rect().topLeft(), rect().bottomLeft());
    if ( q_mouseEnter )
    {
        linear.setColorAt(0, QColor(0, 255, 128, 100));
        linear.setColorAt(0.5, QColor(0, 255, 128, 100));
        linear.setColorAt(1, QColor(0, 255, 128, 100));
    }
    else
    {
        linear.setColorAt(0, QColor(255, 255, 0, 30));
        linear.setColorAt(0.5, QColor(255, 255, 0, 30));
        linear.setColorAt(1, QColor(255, 255, 0, 30));
    }

    painter.setBrush(linear);
    painter.setPen(QColor(0, 0, 0, 0));
    painter.drawRoundRect(0, 0, width()-1, height() - 1, 5, 5);
}

void JSkinButton::enterEvent(QEvent *)
{
    q_mouseEnter = true;
    update();
}

void JSkinButton::leaveEvent(QEvent *)
{
    q_mouseEnter = false;
    update();
}

// slot functions

// this-clicked
void JSkinButton::slotThisClicked(bool checked)
{
    Q_EMIT clicked(checked);
}

#include "precomp.h"
#include "jdialog.h"
#include "private/jdialog_p.h"
#include "jstatusbar.h"
#include "jskinbutton.h"

// - class JDialog -

JDialog::JDialog(QWidget *parent, Qt::WindowFlags f)
    : QDialog(parent, Qt::FramelessWindowHint
              | Qt::Dialog | Qt::WindowCloseButtonHint
              | Qt::WindowMinMaxButtonsHint | Qt::WindowContextHelpButtonHint
              | Qt::WindowSystemMenuHint)
{
    setAttribute(Qt::WA_TranslucentBackground);
    setMouseTracking(true);

    // defaults
    if (f == 0) {
        f = Qt::WindowMinMaxButtonsHint | Qt::WindowCloseButtonHint;
    }

    d = new JDialogPrivate(this, f | Qt::FramelessWindowHint | Qt::WindowTitleHint);

    d->makeRegions();

    QIcon icon = parent ? parent->windowIcon() : QIcon();
    setWindowIcon(icon.isNull() ? QIcon(qApp->property("#systemLogo#").toString()) : icon);

    // status bar

    // layouts
#if 0
    // move to current screen
    QDesktopWidget *desktop = QApplication::desktop();
    if (desktop) {
        QRect r = geometry();
        r.moveCenter(desktop->screenGeometry(QCursor::pos()).center());
        setGeometry(r);
    }
#endif
}

JDialog::~JDialog()
{
    delete d;
}

QWidget *JDialog::clientWidget() const
{
    return d->clientWidget;
}

void JDialog::setClientWidget(QWidget *widget)
{
    if (d->clientWidget) {
        d->vertLayoutMain->removeWidget(d->clientWidget);
        d->clientWidget->deleteLater();
    }
    d->clientWidget = widget;
    d->vertLayoutMain->insertWidget(1, d->clientWidget);
}

void JDialog::setCenter(QWidget *parent)
{
    QRect geo = geometry();
    if (parent) {
        geo.moveCenter(parent->geometry().center());
    } else if (parentWidget()) {
        geo.moveCenter(parentWidget()->geometry().center());
    } else {
        geo.moveCenter(QApplication::desktop()->screenGeometry(QCursor::pos()).center());
    }
    setGeometry(geo);
}

QLayout *JDialog::layout() const
{
    return d->clientWidget->layout();
}

void JDialog::setLayout(QLayout *layout)
{
    d->clientWidget->setLayout(layout);
}

void JDialog::setWindowTitle(const QString &text)
{
    QDialog::setWindowTitle(text);
    d->labelTitle->setText(text);
}

QString JDialog::windowTitle() const
{
    return d->labelTitle->text();
}

QIcon JDialog::windowIcon() const
{
    return QDialog::windowIcon();
}

void JDialog::setWindowIcon(const QIcon &icon)
{
    QDialog::setWindowIcon(icon);
    d->buttonIcon->setPixmap(icon);
}

void JDialog::enableShadow(bool enable)
{
    if (enable) {
        d->defShadowMargins = QMargins(8, 8, 8, 8);
        setAttribute(Qt::WA_TranslucentBackground, true);
        d->borderColor = d->clientColor;
    } else {
        d->defShadowMargins = QMargins(0, 0, 0, 0);
        setAttribute(Qt::WA_TranslucentBackground, false);
        d->borderColor = d->titleColor.second;
    }
}

QPair<QColor, QColor> JDialog::titleColor() const
{
    return d->titleColor;
}

void JDialog::setTitleColor(const QColor &top, const QColor &bottom)
{
    d->titleColor.first = top;
    d->titleColor.second = bottom;
    update();
}

QColor JDialog::borderColor() const
{
    return d->borderColor;
}

void JDialog::setBorderColor(const QColor &color)
{
    d->borderColor = color;
    update();
}

QColor JDialog::clientColor() const
{
    return d->clientColor;
}

void JDialog::setClientColor(const QColor &color)
{
    d->clientColor = color;
    update();
}

void JDialog::setWindowTitleFont(const QFont &font)
{
    d->labelTitle->setFont(font);
}

void JDialog::setTitleAlignment(Qt::Alignment alignment)
{
    d->labelTitle->setAlignment(alignment);
}

QRect JDialog::clientRect() const
{
    return d->clientWidget->rect();
}

void JDialog::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    //painter.setRenderHint(QPainter::Antialiasing);

    // center
    painter.drawPixmap(QRectF(d->shadowMargins.left(), d->shadowMargins.top(),
                              width() - d->shadowMargins.left() - d->shadowMargins.right(),
                              height() - d->shadowMargins.top() - d->shadowMargins.bottom()),
                       d->pmShadow,
                       QRectF(d->shadowMargins.left(), d->shadowMargins.top(),
                              d->pmShadow.width() - d->shadowMargins.left() - d->shadowMargins.right(),
                              d->pmShadow.height() - d->shadowMargins.top() - d->shadowMargins.bottom()));
    // shadow
    if (!isMaximized()) {
        // topLeft
        painter.drawPixmap(QPointF(0, 0),
                           d->pmShadow,
                           QRectF(0, 0, d->shadowMargins.left(), d->shadowMargins.top()));
        // top
        painter.drawPixmap(QRectF(d->shadowMargins.left(), 0,
                                  width() - d->shadowMargins.left() - d->shadowMargins.right(),
                                  d->shadowMargins.top()),
                           d->pmShadow,
                           QRectF(d->shadowMargins.left(), 0,
                                  d->pmShadow.width() - d->shadowMargins.left() - d->shadowMargins.right(),
                                  d->shadowMargins.top()));
        // topRight
        painter.drawPixmap(QRectF(width() - d->shadowMargins.right(), 0,
                                  d->shadowMargins.right(),
                                  d->shadowMargins.top()),
                           d->pmShadow,
                           QRectF(d->pmShadow.width() - d->shadowMargins.right(), 0,
                                  d->shadowMargins.right(), d->shadowMargins.top()));
        // left
        painter.drawPixmap(QRectF(0, d->shadowMargins.top(), d->shadowMargins.left(),
                                  height() - d->shadowMargins.top() - d->shadowMargins.bottom()),
                           d->pmShadow,
                           QRectF(0, d->shadowMargins.top(), d->shadowMargins.left(),
                                  d->pmShadow.height() - d->shadowMargins.top() - d->shadowMargins.bottom()));
        // right
        painter.drawPixmap(QRectF(width() - d->shadowMargins.right(), d->shadowMargins.top(),
                                  d->shadowMargins.right(),
                                  height() - d->shadowMargins.top() - d->shadowMargins.bottom()),
                           d->pmShadow,
                           QRectF(d->pmShadow.width() - d->shadowMargins.right(), d->shadowMargins.top(),
                                  d->shadowMargins.right(),
                                  d->pmShadow.height() - d->shadowMargins.top() - d->shadowMargins.bottom()));
        // bottomLeft
        painter.drawPixmap(QRectF(0, height() - d->shadowMargins.bottom(), d->shadowMargins.left(),
                                  d->shadowMargins.bottom()),
                           d->pmShadow,
                           QRectF(0, d->pmShadow.height() - d->shadowMargins.bottom(), d->shadowMargins.left(),
                                  d->shadowMargins.bottom()));
        // bottom
        painter.drawPixmap(QRectF(d->shadowMargins.left(), height() - d->shadowMargins.bottom(),
                                  width() -d->shadowMargins.left() - d->shadowMargins.right(),
                                  d->shadowMargins.bottom()),
                           d->pmShadow,
                           QRectF(d->shadowMargins.left(), d->pmShadow.height() - d->shadowMargins.bottom(),
                                  d->pmShadow.width() - d->shadowMargins.left() - d->shadowMargins.right(),
                                  d->shadowMargins.bottom()));
        // bottomRight
        painter.drawPixmap(QRectF(width() - d->shadowMargins.right(),
                                  height() - d->shadowMargins.bottom(), d->shadowMargins.right(),
                                  d->shadowMargins.bottom()),
                           d->pmShadow,
                           QRectF(d->shadowMargins.left(), d->pmShadow.height() - d->shadowMargins.bottom(),
                                  d->pmShadow.width() - d->shadowMargins.left() - d->shadowMargins.right(),
                                  d->shadowMargins.bottom()));
        // border
        painter.save();
        painter.setPen(QPen(d->borderColor, d->borderWidth, Qt::SolidLine, Qt::SquareCap, Qt::MiterJoin));
        painter.drawRect(d->shadowMargins.left() + d->borderWidth / 2,
                         d->shadowMargins.top() + d->titleHeight + d->borderWidth / 2,
                         width() - d->shadowMargins.left() - d->shadowMargins.right() - d->borderWidth,
                         height() - d->shadowMargins.top() - d->shadowMargins.bottom() - d->titleHeight - d->borderWidth);
        painter.restore();
    }
    // title
    QLinearGradient linearGradient(d->shadowMargins.left(), d->shadowMargins.top(),
                                   d->shadowMargins.left(), d->shadowMargins.top() + d->titleHeight);
    linearGradient.setColorAt(0.0, d->titleColor.first);
    linearGradient.setColorAt(1.0, d->titleColor.second);
    painter.fillRect(d->shadowMargins.left(), d->shadowMargins.top(),
                     width() - d->shadowMargins.left() - d->shadowMargins.right(),
                     d->titleHeight, linearGradient);
    // client
    painter.fillRect(d->shadowMargins.left() + d->borderWidth, d->shadowMargins.top() + d->titleHeight,
                     width() - d->shadowMargins.left() - d->shadowMargins.right() - d->borderWidth * 2,
                     height() - d->shadowMargins.top() - d->shadowMargins.bottom() - d->titleHeight - d->borderWidth,
                     d->clientColor);
}

void JDialog::resizeEvent(QResizeEvent *)
{
    d->makeRegions();
}

void JDialog::leaveEvent(QEvent *)
{
    unsetCursor();
}

void JDialog::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        d->mousePressed = true;
        d->orgPos = event->pos();
        d->rgnPressed = d->regionHit(event->pos());
        d->orgGeom = geometry();
    }
}

void JDialog::mouseMoveEvent(QMouseEvent *event)
{
    if (isMaximized()) {
        //
    } else {
        d->mouseMoveProc(event->pos());
    }
}

void JDialog::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::RightButton) {
        d->rgnPressed = d->regionHit(event->pos());
        if (d->rgnPressed == JDialogPrivate::RegionTitle) {
            d->showSystemMenu(true, event->pos());
        }
    }

    d->mousePressed = false;
    d->rgnPressed = JDialogPrivate::RegionUnknown;
    unsetCursor();
}

void JDialog::mouseDoubleClickEvent(QMouseEvent *event)
{
    if (d->flags & Qt::WindowMaximizeButtonHint) {
        if (event->button() == Qt::LeftButton) {
            if (d->regionHit(event->pos()) != JDialogPrivate::RegionUnknown) {
                d->mousePressed = false;
                d->rgnPressed = JDialogPrivate::RegionUnknown;
                d->showChanged();
            }
        }
    }
}

void JDialog::closeEvent(QCloseEvent *event)
{
    Q_UNUSED(event);
    setWindowState(Qt::WindowNoState);
}

void JDialog::show()
{
    Qt::WindowStates states = windowState();
    if (states == Qt::WindowNoState) {
        showNormal();
    } else if (states & Qt::WindowMinimized) {
        if (states & Qt::WindowMaximized) {
            showMaximized();
        } else if (states & Qt::WindowFullScreen) {
            showFullScreen();
        } else {
            QDialog::showNormal();
        }
    } else if (states & Qt::WindowMaximized) {
        showMaximized();
    } else if (states & Qt::WindowFullScreen) {
        showFullScreen();
    } else {
        QDialog::show();
    }

    QDialog::setFocus();
}

void JDialog::showMaximized()
{
    d->titleWidget->setVisible(true);
    d->showMaximized();
    QDialog::activateWindow();
}

void JDialog::showFullScreen()
{
    d->titleWidget->setVisible(false);
    d->showFullScreen();
    QDialog::activateWindow();
}

void JDialog::showNormal()
{
    d->titleWidget->setVisible(true);
    d->showNormal();
    QDialog::activateWindow();
}

#include "precomp.h"
#include "jdialog_p.h"

// - class JIconButton -

JIconButton::JIconButton(QWidget *parent)
    : QPushButton(parent)
{
    setAttribute(Qt::WA_TranslucentBackground);
}

JIconButton::JIconButton(const QString &name, QWidget *parent)
    : QPushButton(parent)
{
    setAttribute(Qt::WA_TranslucentBackground);
    setPixmap(name);
    if (q_pixmap.isNull()) {
        setFixedSize(0, 0);
    } else {
        setFixedSize(20, 20);
    }
}

void JIconButton::setPixmap(const QString &name)
{
    q_pixmap = QPixmap(name);
    if (q_pixmap.isNull()) {
        setFixedSize(0, 0);
    } else {
        setFixedSize(20, 20);
    }
}

void JIconButton::setPixmap(const QPixmap &pixmap)
{
    q_pixmap = pixmap;
    if (q_pixmap.isNull()) {
        setFixedSize(0, 0);
    } else {
        setFixedSize(20, 20);
    }
}

void JIconButton::setPixmap(const QIcon &icon)
{
    q_pixmap = QPixmap(icon.pixmap(20, 20));
    if (q_pixmap.isNull()) {
        setFixedSize(0, 0);
    } else {
        setFixedSize(20, 20);
    }
}

void JIconButton::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.drawPixmap(rect(), q_pixmap, q_pixmap.rect());
}

void JIconButton::mouseDoubleClickEvent(QMouseEvent *)
{
    Q_EMIT doubleClicked();
}

// - class JPixmapButton -

JPixmapButton::JPixmapButton(QWidget *parent)
    : QPushButton(parent)
    , q_state(Normal)
{

}

JPixmapButton::JPixmapButton(const QString &text, QWidget *parent)
    : QPushButton(text, parent)
    , q_state(Normal)
{

}

JPixmapButton::JPixmapButton(const QIcon &icon, const QString &text, QWidget *parent)
    : QPushButton(icon, text, parent)
    , q_state(Normal)
{

}

void JPixmapButton::setPixmap(const QString &name)
{
    q_pixmap.load(name);
    q_picWidth = q_pixmap.width() / 4;
    q_picHeight = q_pixmap.height();
    setFixedSize(q_picWidth, q_picHeight);
}

void JPixmapButton::setPixmap(const QPixmap &pixmap)
{
    q_pixmap = pixmap;
    q_picWidth = q_pixmap.width() / 4;
    q_picHeight = q_pixmap.height();
    setFixedSize(q_picWidth, q_picHeight);
}

void JPixmapButton::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.drawPixmap(rect(), q_pixmap, QRect(q_picWidth * q_state, 0, q_picWidth, q_picHeight));
}

void JPixmapButton::enterEvent(QEvent *)
{
    setCursor(Qt::ArrowCursor);
    q_state = Active;
    update();
}

void JPixmapButton::leaveEvent(QEvent *)
{
    q_state = Normal;
    update();
}

void JPixmapButton::mouseMoveEvent(QMouseEvent *event)
{
    if (rect().contains(event->pos())) {
        if (event->buttons() & Qt::LeftButton) {
            q_state = Selected;
        }
    } else if (q_state != Normal) {
        q_state = Normal;
    }

    update();
}

void JPixmapButton::mousePressEvent(QMouseEvent *event)
{
    if (event->buttons() & Qt::LeftButton) {
        q_state = Selected;
    }

    update();
}

void JPixmapButton::mouseReleaseEvent(QMouseEvent *)
{
    if (q_state == Selected) {
        q_state = Active;
    } else {
        q_state = Normal;
    }

    update();
}

// - class JSystemMenu -

JSystemMenu::JSystemMenu(Qt::WindowFlags f, QWidget *parent) :
    QMenu(parent)
{
    setFont(QFont("Microsoft Yahei", 10));

    if (f & Qt::WindowMaximizeButtonHint) {
        QAction *actionRestore = new QAction(QIcon(":/com.smartsoft.jsmartkits.jwidgets/image/sysmenu/restore.png"),
                                             tr("&Restore"), this);
        addAction(actionRestore);
        actionRestore->setEnabled(parent->isMaximized());
        connect(actionRestore, SIGNAL(triggered()), SLOT(showNormalParent()));
    }

    //TODO
    QAction *actionMove = new QAction(tr("&Move"), this);
    QAction *actionSize = new QAction(tr("&Size"), this);
    addAction(actionMove);
    addAction(actionSize);
    actionMove->setEnabled(false);
    actionSize->setEnabled(false);

    //
    if (f & Qt::WindowMinimizeButtonHint) {
        QAction *actionMinimize = new QAction(QIcon(":/com.smartsoft.jsmartkits.jwidgets/image/sysmenu/min.png"),
                                              tr("&Minimize"), this);
        addAction(actionMinimize);
        connect(actionMinimize, SIGNAL(triggered()), SLOT(showMinimizeParent()));
    }

    //
    if (f & Qt::WindowMaximizeButtonHint) {
        QAction *actionMaximize = new QAction(QIcon(":/com.smartsoft.jsmartkits.jwidgets/image/sysmenu/max.png"),
                                              tr("M&aximize"), this);
        addAction(actionMaximize);
        actionMaximize->setEnabled(!parent->isMaximized());
        connect(actionMaximize, SIGNAL(triggered()), SLOT(showMaximizeParent()));
    }

    //
    if (f & Qt::WindowCloseButtonHint) {
        QAction *actionClose = new QAction(QIcon(":/com.smartsoft.jsmartkits.jwidgets/image/sysmenu/close.png"),
                                           tr("Cl&ose"), this);
        addAction(actionClose);
        connect(actionClose, SIGNAL(triggered()), parent, SLOT(close()));
    }
}

void JSystemMenu::showNormalParent()
{
    reinterpret_cast<JDialog *>(parentWidget())->showNormal();
}

void JSystemMenu::showMinimizeParent()
{
    reinterpret_cast<JDialog *>(parentWidget())->showMinimized();
}

void JSystemMenu::showMaximizeParent()
{
    reinterpret_cast<JDialog *>(parentWidget())->showMaximized();
}

// - class JDialogPrivate -

JDialogPrivate::JDialogPrivate(QWidget *parent, Qt::WindowFlags f)
    : QObject(parent)
    , parentWidget(reinterpret_cast<QDialog *>(parent))
    , titleHeight(30)
    , skinName(":/com.smartsoft.jsmartkits.jwidgets/image/skin/shadow.png"), defShadowMargins(8, 8, 8, 8)
    , shadowMargins(defShadowMargins)
    , defBorderWidth(4)
    , borderWidth(defBorderWidth)
    , mousePressed(false)
    , rgnPressed(RegionUnknown)
    , flags(f)
    //
    , titleColor(QPair<QColor, QColor>("#AFAFBF", "#6D7D92"))
    , borderColor("#DDE4EF")
    , clientColor("#DDE4EF")
{
    regions.resize(RegionCount);

    pixmapRestore.load(":/com.smartsoft.jsmartkits.jwidgets/image/title_button/restore.png");
    pixmapMax.load(":/com.smartsoft.jsmartkits.jwidgets/title_button/max.png");

    // load image

    pmShadow.load(skinName);

    // title widget
    titleWidget = new QWidget(parent);
    titleWidget->setObjectName("titleWidget");
    titleWidget->setFixedHeight(titleHeight);
    titleWidget->setMouseTracking(true);
    //
    if (!(f & Qt::WindowTitleHint)) {
        titleWidget->hide();
    }

    // client widget
    clientWidget = new QWidget(parent);
    clientWidget->setObjectName("clientWidget");
    clientWidget->setCursor(Qt::ArrowCursor);

    // layouts
    vertLayoutMain = new QVBoxLayout(parent);
    vertLayoutMain->setSpacing(0);
    vertLayoutMain->setContentsMargins(shadowMargins.left() + borderWidth,
                                       shadowMargins.top()/* + borderWidth*/,
                                       shadowMargins.right() + borderWidth,
                                       shadowMargins.bottom() + borderWidth);
    vertLayoutMain->addWidget(titleWidget);
    vertLayoutMain->addWidget(clientWidget);

    // titleWidget -> children

    // layout
    horiLayoutTitle = new QHBoxLayout(titleWidget);
    horiLayoutTitle->setSpacing(0);
    horiLayoutTitle->setContentsMargins(0, 0, 0, 0);

    // icon
    buttonIcon = new JIconButton(titleWidget);
    buttonIcon->setObjectName("buttonIcon");
    buttonIcon->setMouseTracking(true);
    buttonIcon->setCheckable(true);
    connect(buttonIcon, SIGNAL(toggled(bool)), SLOT(showSystemMenu(bool)));
    connect(buttonIcon, SIGNAL(doubleClicked()), parent, SLOT(reject()));
    horiLayoutTitle->addWidget(buttonIcon);
    //horiLayoutTitle->addStretch();

    // window title
    labelTitle = new QLabel(parent->windowTitle(), parent);
    labelTitle->setObjectName("labelTitle");
    labelTitle->setMouseTracking(true);
    labelTitle->setStyleSheet("QLabel{color:white;}");
    labelTitle->setFont(QFont("Microsoft Yahei", 11));
    labelTitle->setContentsMargins(5, 0, 2, 0);
    horiLayoutTitle->addWidget(labelTitle, 0, Qt::AlignVCenter);
    //horiLayoutTitle->addStretch();
    /*
    // button: whatsthis
    buttonWhatsThis = new JPixmapButton(titleWidget);
    buttonWhatsThis->setObjectName("buttonWhatsThis");
    buttonWhatsThis->setToolTip(tr("What is this"));
    buttonWhatsThis->setPixmap(":/com.smartsoft.jsmartkits.jwidgets/image/title_button/arrow.png");  // TODO
    horiLayoutTitle->addWidget(buttonWhatsThis, 0, Qt::AlignTop);
    //connect(buttonWhatsThis, SIGNAL(clicked()), SLOT());  // TODO
*/
    // button: minimized
    buttonMinimized = new JPixmapButton(titleWidget);
    buttonMinimized->setObjectName("buttonMinimized");
    buttonMinimized->setToolTip(tr("Minimize"));
    buttonMinimized->setPixmap(":/com.smartsoft.jsmartkits.jwidgets/image/title_button/min2.png");
    if (!(f & Qt::WindowMinimizeButtonHint)) {
        buttonMinimized->hide();
    }
    horiLayoutTitle->addWidget(buttonMinimized, 0, Qt::AlignTop);
    connect(buttonMinimized, SIGNAL(clicked()), parent, SLOT(showMinimized()));

    // button: maximized
    buttonMaximized = new JPixmapButton(titleWidget);
    buttonMaximized->setObjectName("buttonMaximized");
    buttonMaximized->setToolTip(tr("Maximize"));
    buttonMaximized->setCheckable(true);
    buttonMaximized->setPixmap(pixmapMax);
    if (!(f & Qt::WindowMaximizeButtonHint)) {
        buttonMaximized->hide();
    }
    horiLayoutTitle->addWidget(buttonMaximized, 0, Qt::AlignTop);
    connect(buttonMaximized, SIGNAL(clicked()), SLOT(showChanged()));

    // button: close
    buttonClose = new JPixmapButton(titleWidget);
    buttonClose->setObjectName("buttonClose");
    buttonClose->setToolTip(tr("Close"));
    if (!(f & (Qt::WindowMinimizeButtonHint | Qt::WindowContextHelpButtonHint))
            && (f & Qt::WindowCloseButtonHint)) {
        buttonClose->setPixmap(":/com.smartsoft.jsmartkits.jwidgets/image/title_button/dlg_close.png");
    } else {
        buttonClose->setPixmap(":/com.smartsoft.jsmartkits.jwidgets/image/title_button/close.png");
    }
    horiLayoutTitle->addWidget(buttonClose, 0, Qt::AlignTop);
    if (!(f & Qt::WindowCloseButtonHint)) {
        buttonClose->hide();
    }
    connect(buttonClose, SIGNAL(clicked()), parent, SLOT(close()));
}

JDialogPrivate::~JDialogPrivate()
{

}

void JDialogPrivate::showSystemMenu(bool checked, const QPoint &pos)
{
    if (checked) {
        QPoint _pos;
        if (pos.isNull()) {
            _pos = QPoint(regions[RegionTopLeft].right(), regions[RegionTitle].bottom());
        } else {
            _pos = pos;
        }

        JSystemMenu(flags, parentWidget).exec(parentWidget->mapToGlobal(_pos));
    }
}

void JDialogPrivate::showMaximized()
{
    horiLayoutTitle->setContentsMargins(0, 0, borderWidth, 0);
    buttonMaximized->setPixmap(pixmapRestore);
    shadowMargins = QMargins();
    borderWidth = 0;
    vertLayoutMain->setContentsMargins(0, 0, 0, 0);
    parentWidget->showMaximized();
}

void JDialogPrivate::showFullScreen()
{
    showMaximized();
}

void JDialogPrivate::showNormal()
{
    horiLayoutTitle->setContentsMargins(0, 0, 0, 0);
    buttonMaximized->setPixmap(pixmapMax);
    shadowMargins = defShadowMargins;
    borderWidth = defBorderWidth;
    vertLayoutMain->setContentsMargins(shadowMargins.left() + borderWidth,
                                       shadowMargins.top()/* + borderWidth*/,
                                       shadowMargins.right() + borderWidth,
                                       shadowMargins.bottom() + borderWidth);
    parentWidget->showNormal();
}

void JDialogPrivate::showChanged()
{
    parentWidget->isMaximized() ? showNormal() : showMaximized();
}

void JDialogPrivate::showMainMenu()
{
    //
}

void JDialogPrivate::mouseMoveProc(const QPoint &p)
{
    if (mousePressed) {
        QPoint curPos = p;
        QRect curGeom = parentWidget->geometry();
        switch (rgnPressed) {
        case RegionTopLeft:
        {
            QPoint v = curPos - orgPos;
            updateGeometry(curGeom.x() + v.x(), curGeom.y() + v.y(),
                           curGeom.width() - v.x(), curGeom.height() - v.y());
            break;
        }
        case RegionTop:
        {
            int dy = curPos.y() - orgPos.y();
            updateGeometry(orgGeom.x(), curGeom.y() + dy,
                           orgGeom.width(), curGeom.height() - dy);
            break;
        }
        case RegionTopRight:
        {
            QPoint v = curPos - orgPos;
            updateGeometry(orgGeom.x(), curGeom.y() + v.y(),
                           orgGeom.width() + v.y(), curGeom.height() - v.y());
            break;
        }
        case RegionLeft:
        {
            int dx = curPos.x() - orgPos.x();
            updateGeometry(curGeom.x() + dx, curGeom.y(),
                           curGeom.width() - dx, curGeom.height());
            break;
        }
        case RegionTitle:
        {
            parentWidget->move(curGeom.topLeft() + curPos - orgPos);
            break;
        }
        case RegionRight:
        {
            int dx = curPos.x() - orgPos.x();
            updateGeometry(orgGeom.x(), orgGeom.y(),
                           orgGeom.width() + dx, orgGeom.height());
            break;
        }
        case RegionBottomLeft:
        {
            QPoint v = curPos - orgPos;
            updateGeometry(curGeom.x() + v.y(), orgGeom.y(),
                           curGeom.width() - v.y(), orgGeom.height() + v.y());
            break;
        }
        case RegionBottom:
        {
            int dy = curPos.y() - orgPos.y();
            updateGeometry(curGeom.x(), curGeom.y(),
                           orgGeom.width(), orgGeom.height() + dy);
            break;
        }
        case RegionBottomRight:
        {
            QPoint v = curPos - orgPos;
            updateGeometry(orgGeom.x(), orgGeom.y(),
                           orgGeom.width() + v.x(), orgGeom.height() + v.y());
            break;
        }
        default:
            break;
        }
    } else {
        switch (regionHit(p)) {
        case RegionTopLeft:
        case RegionBottomRight:
            parentWidget->setCursor(Qt::SizeFDiagCursor);
            break;
        case RegionTop:
        case RegionBottom:
            parentWidget->setCursor(Qt::SizeVerCursor);
            break;
        case RegionTopRight:
        case RegionBottomLeft:
            parentWidget->setCursor(Qt::SizeBDiagCursor);
            break;
        case RegionLeft:
        case RegionRight:
            parentWidget->setCursor(Qt::SizeHorCursor);
            break;
        case RegionTitle:
        default:
            parentWidget->setCursor(Qt::ArrowCursor);
            break;
        }
    }
}

void JDialogPrivate::updateGeometry(int x, int y, int w, int h)
{
    if (parentWidget->minimumWidth() <= w && w <= parentWidget->maximumWidth()) {
        parentWidget->setGeometry(x, parentWidget->pos().y(), w, parentWidget->height());
    }

    if (parentWidget->minimumHeight() <= h && h <= parentWidget->maximumHeight()) {
        parentWidget->setGeometry(parentWidget->pos().x(), y, parentWidget->width(), h);
    }
}

void JDialogPrivate::makeRegions()
{
    regions[RegionTopLeft]      = QRect(shadowMargins.left(), shadowMargins.top(), borderWidth * 2, borderWidth * 2);
    regions[RegionTop]          = QRect(shadowMargins.left() + borderWidth * 2, shadowMargins.top(),
                                        parentWidget->width() - shadowMargins.left() - shadowMargins.right() - borderWidth * 4, borderWidth);
    regions[RegionTopRight]     = QRect(parentWidget->width() - shadowMargins.right() - borderWidth * 2, shadowMargins.top(),
                                        borderWidth * 2, borderWidth * 2);
    regions[RegionLeft]         = QRect(shadowMargins.left(), shadowMargins.top() + borderWidth * 2, borderWidth,
                                        parentWidget->height() - shadowMargins.top() - shadowMargins.bottom() - borderWidth * 3);
    regions[RegionTitle]        = QRect(shadowMargins.left() + borderWidth * 2, shadowMargins.top() + borderWidth,
                                        parentWidget->width() - shadowMargins.left() - shadowMargins.right() - borderWidth * 4, titleHeight);
    regions[RegionRight]        = QRect(parentWidget->width() - shadowMargins.right() - borderWidth, shadowMargins.top() + borderWidth * 2,
                                        borderWidth, parentWidget->height() - shadowMargins.top() - shadowMargins.bottom() - borderWidth * 3);
    regions[RegionBottomLeft]   = QRect(shadowMargins.left(), parentWidget->height() - shadowMargins.bottom() - borderWidth, borderWidth, borderWidth);
    regions[RegionBottom]       = QRect(shadowMargins.left() + borderWidth, parentWidget->height() - shadowMargins.bottom() - borderWidth,
                                        parentWidget->width()- shadowMargins.left() - shadowMargins.right() - borderWidth * 2 , borderWidth);
    regions[RegionBottomRight]  = QRect(parentWidget->width() - shadowMargins.right() - borderWidth, parentWidget->height() - shadowMargins.bottom() - borderWidth,
                                        borderWidth, borderWidth);
}

JDialogPrivate::RegionType JDialogPrivate::regionHit(const QPoint &pos)
{
    for (int i = 0; i < regions.count(); ++i) {
        if (regions[i].contains(pos)) {
            return RegionType(i);
        }
    }

    return RegionUnknown;
}

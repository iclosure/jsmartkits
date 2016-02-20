#ifndef JDIALOG_P_H
#define JDIALOG_P_H

#include "../jwidgets_global.h"
#include "../jdialog.h"
#include <QPushButton>
#include <QMenu>

// - class JIconButton -

class JIconButton : public QPushButton
{
    Q_OBJECT
public:
    explicit JIconButton(QWidget *parent = 0);
    explicit JIconButton(const QString &name, QWidget *parent = 0);

    void setPixmap(const QString &name);
    void setPixmap(const QPixmap &pixmap);
    void setPixmap(const QIcon &icon);

Q_SIGNALS:
    void doubleClicked();

protected:
    void paintEvent(QPaintEvent *);
    void mouseDoubleClickEvent(QMouseEvent *);

private:
    QPixmap q_pixmap;
};

// - class JPixmapButton -

class JPixmapButton : public QPushButton
{
    Q_OBJECT
public:
    enum ButtonState {
        Normal,
        Active,
        Selected,
        Disabled,

        ButtonStateCount
    };
    Q_DECLARE_FLAGS(ButtonStates, ButtonState)

    explicit JPixmapButton(QWidget *parent = 0);
    explicit JPixmapButton(const QString &text, QWidget *parent = 0);
    JPixmapButton(const QIcon& icon, const QString &text, QWidget *parent = 0);

    void setPixmap(const QString &name);
    void setPixmap(const QPixmap &pixmap);

protected:
    void paintEvent(QPaintEvent *);
    void enterEvent(QEvent *);
    void leaveEvent(QEvent *);
    void mouseMoveEvent(QMouseEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *);

private:
    int             q_picWidth;
    int             q_picHeight;
    ButtonStates    q_state;
    QPixmap         q_pixmap;
};

// - class JSystemMenu -

class JSystemMenu : public QMenu
{
    Q_OBJECT
public:
    explicit JSystemMenu(Qt::WindowFlags f, QWidget *parent = 0);

private Q_SLOTS:
    void showNormalParent();
    void showMinimizeParent();
    void showMaximizeParent();
};

// - class JDialogPrivate -

class QHBoxLayout;
class QVBoxLayout;
class QLabel;

class JDialogPrivate : public QObject
{
    Q_OBJECT
    friend class JDialog;
public:
    explicit JDialogPrivate(QWidget *parent, Qt::WindowFlags f = 0);
    ~JDialogPrivate();

    enum RegionType {
        RegionTopLeft,
        RegionTop,
        RegionTopRight,
        RegionLeft,
        RegionTitle,
        RegionRight,
        RegionBottomLeft,
        RegionBottom,
        RegionBottomRight,
        RegionUnknown,
        RegionCount
    };

private:
    QDialog *parentWidget;
    QString skinName;
    int titleHeight;
    QMargins defShadowMargins;
    int defBorderWidth;
    QMargins shadowMargins;
    int borderWidth;
    bool mousePressed;
    RegionType rgnPressed;
    QPoint orgPos;
    QRect orgGeom;
    QVector<QRect> regions;

    QPixmap pmShadow;

    QWidget *titleWidget;
    QWidget *clientWidget;
    QVBoxLayout *vertLayoutMain;

    Qt::WindowFlags flags;

    //
    QPair<QColor/*top*/, QColor/*bottom*/> titleColor;
    QColor borderColor;
    QColor clientColor;

    // title

private Q_SLOTS:
    void showMaximized();
    void showFullScreen();
    void showNormal();

    void showChanged();
    void showSystemMenu(bool checked, const QPoint &pos = QPoint());
    void showMainMenu();

protected:
    void mouseMoveProc(const QPoint &p);
    void makeRegions();
    void updateGeometry(int x, int y, int w, int h);
    RegionType regionHit(const QPoint &pos);

    void addLayout(QLayout *layout);
    void addWidget(QWidget *widget);

private:
    QHBoxLayout *horiLayoutTitle;
    QVBoxLayout *titleLayoutMain;
    JIconButton *buttonIcon;
    QLabel *labelTitle;             // button: window title
    QMenu *menuMain;                // menu: main menu
    JPixmapButton *buttonMainMenu;  // button: main menu
    JPixmapButton *buttonSkins;     // button: change skin
    JPixmapButton *buttonWhatsThis; // button: whatsthis
    JPixmapButton *buttonMinimized; // button: minimized
    JPixmapButton *buttonMaximized; // button: maximized
    JPixmapButton *buttonClose;     // button: close

    QPixmap pixmapRestore;
    QPixmap pixmapMax;
};

#endif // JDIALOG_P_H

#include "precomp.h"
#include "jsplitter.h"

// - JSplitterHandleData -

class JSplitterHandleData
{
    friend class JSplitterHandle;
public:
    JSplitterHandleData()
        : checkable(false)
        , checked(true)
        , canceled(true)
        , hovered(false)
        , alwaysVisible(false)
        , color(221, 228, 239, 200)
    {

    }

private:
    bool checkable;
    bool checked;
    bool canceled;
    bool hovered;
    bool alwaysVisible;
    QColor color;
    QPixmap pixmap;
};

// - class JSplitterHandle -

JSplitterHandle::JSplitterHandle(Qt::Orientation o, QSplitter *parent)
    : QSplitterHandle(o, parent)
    , d(new JSplitterHandleData())
{

}

JSplitterHandle::~JSplitterHandle()
{
    delete d;
}

QColor JSplitterHandle::color() const
{
    return d->color;
}

void JSplitterHandle::setColor(const QColor &color)
{
    if (color != d->color) {
        d->color = color;
        update();
    }
}

QPixmap JSplitterHandle::pixmap() const
{
    return d->pixmap;
}

void JSplitterHandle::setPixmap(const QPixmap &pixmap)
{
    d->pixmap = pixmap;
    update();
}

bool JSplitterHandle::isCheckable() const
{
    return d->checkable;
}

void JSplitterHandle::setCheckable(bool checkable)
{
    if (checkable != d->checkable) {
        d->checkable = checkable;
        update();
    }
}

bool JSplitterHandle::isAlwaysVisible() const
{
    return d->alwaysVisible;
}

void JSplitterHandle::setAlwaysVisible(bool visible)
{
    if (visible != d->alwaysVisible) {
        d->alwaysVisible = visible;
        if (visible) {
            setVisible(true);
        }
    }
}

void JSplitterHandle::setVisible(bool visible)
{
    if (!d->alwaysVisible || splitter()->count() < 2) {
        QSplitterHandle::setVisible(visible);
    }
}

void JSplitterHandle::paintEvent(QPaintEvent *e)
{
    QPainter painter(this);
    painter.fillRect(e->rect(), d->color);
    if (d->checkable) {
        QPointF center(rect().center());
        QPainterPath path;
        switch (orientation()) {
        case Qt::Horizontal:
        {
            int radius = qMin(width(), 3);
            for (int i = 0; i < 5; ++i) {
                path.addEllipse(QPointF(center.x() + 1, center.y() + (radius * 2 + 3) * (i - 2)), radius, radius);
            }
            break;
        }
        case Qt::Vertical:
        {
            int radius = qMin(height(), 3);
            for (int i = 0; i < 5; ++i) {
                path.addEllipse(QPointF(center.x() + (radius * 2 + 3) * (i - 2), center.y() + 1), radius, radius);
            }
            break;
        }
        default:
            break;
        }

        painter.save();
        painter.setRenderHints(QPainter::Antialiasing | QPainter::HighQualityAntialiasing);
        if (d->canceled) {
            if (d->hovered) {
                painter.setPen(d->color.dark(180));
                painter.setBrush(d->color.dark(250));
            } else {
                painter.setPen(d->color.dark(50));
                painter.setBrush(d->color.dark(180));
            }
        } else {
            painter.setPen(d->color.dark(250));
            painter.setBrush(d->color.dark(500));
        }
        painter.drawPath(path);
        painter.restore();
    }
}

void JSplitterHandle::mousePressEvent(QMouseEvent *e)
{
    QSplitterHandle::mousePressEvent(e);

    if (d->checkable && d->canceled) {
        d->canceled = false;
        update();
    }
}

void JSplitterHandle::mouseReleaseEvent(QMouseEvent *e)
{
    QSplitterHandle::mouseReleaseEvent(e);

    if (d->checkable && !d->canceled) {
        d->checked = !d->checked;
        d->canceled = true;
        update();
        Q_EMIT handleClicked(d->checked);
        JSplitter *splitter = qobject_cast<JSplitter *>(QSplitterHandle::splitter());
        if (splitter) {
            int index = splitter->count();
            while (--index >= 0) {
                if (splitter->handle(index) == this) {
                    Q_EMIT splitter->handleChanged(index, d->checked);
                }
            }
        }
    }
}

void JSplitterHandle::mouseMoveEvent(QMouseEvent *e)
{
    QSplitterHandle::mouseMoveEvent(e);

    if (d->checkable && !d->canceled) {
        d->canceled = true;
        update();
    }
}

void JSplitterHandle::enterEvent(QEvent *e)
{
    QSplitterHandle::enterEvent(e);

    if (d->checkable && !d->hovered) {
        d->hovered = true;
        update();
    }
}

void JSplitterHandle::leaveEvent(QEvent *e)
{
    QSplitterHandle::leaveEvent(e);

    if (d->checkable && d->hovered) {
        d->hovered = false;
        update();
    }
}

// - class JSplitterPrivate -

class JSplitterPrivate
{
    J_DECLARE_PUBLIC(JSplitter)
public:
    JSplitterPrivate(JSplitter *parent = 0)
        : q_ptr(parent)
        , totalScale(0)
        , checkable(false)
        , alwaysVisible(false)
        , color(221, 228, 239, 200)
    {

    }

private:
    void init();

private:
    QList<double> scales;
    double totalScale;
    bool checkable;
    bool alwaysVisible;
    QColor color;
    QPixmap pixmap;
};

void JSplitterPrivate::init()
{
    Q_Q(JSplitter);
    q->setHandleWidth(6);
    q->setChildrenCollapsible(false);
    QObject::connect(q, SIGNAL(splitterMoved(int,int)),
                     q, SLOT(_emit_splitterMoved(int,int)));
}

// - class JSplitter -

JSplitter::JSplitter(QWidget *parent)
    : QSplitter(parent)
    , d_ptr(new JSplitterPrivate(this))
{
    Q_D(JSplitter);
    d->init();
}

JSplitter::JSplitter(Qt::Orientation orientation, QWidget *parent)
    : QSplitter(orientation, parent)
    , d_ptr(new JSplitterPrivate(this))
{
    Q_D(JSplitter);
    d->init();
}

JSplitter::JSplitter(const QList<double> &scales, QWidget *parent)
    : QSplitter(parent)
    , d_ptr(new JSplitterPrivate(this))
{
    Q_D(JSplitter);
    d->init();

    setScales(scales);
}

JSplitter::JSplitter(const QList<double> &scales,
                     Qt::Orientation orientation, QWidget *parent)
    : QSplitter(orientation, parent)
    , d_ptr(new JSplitterPrivate(this))
{
    Q_D(JSplitter);
    d->init();

    setScales(scales);
}

JSplitter::~JSplitter()
{
    delete d_ptr;
}

QList<double> JSplitter::scales() const
{
    Q_D(const JSplitter);
    return d->scales;
}

void JSplitter::setScales(const QList<double> &scales)
{
    Q_D(JSplitter);
    d->scales.clear();
    d->scales.append(scales);
    d->totalScale = 0.;
    QListIterator<double> iter(d->scales);
    while (iter.hasNext()) {
        d->totalScale += iter.next();
    }

    updatesSizes();
}

QColor JSplitter::handleColor(int index) const
{
    JSplitterHandle *hdl = handle(index);
    if (hdl) {
        return hdl->color();
    } else {
        return QColor::Invalid;
    }
}

void JSplitter::setHandleColor(const QColor &color, int index)
{
    Q_D(JSplitter);
    d->color = color;
    if (index == -1) {
        index = count();
        while (--index >= 0) {
            JSplitterHandle *hdl = handle(index);
            if (hdl) {
                hdl->setColor(color);
            }
        }
    } else {
        JSplitterHandle *hdl = handle(index);
        if (hdl) {
            hdl->setColor(color);
        }
    }
}

QPixmap JSplitter::handlePixmap(int index) const
{
    JSplitterHandle *hdl = handle(index);
    if (hdl) {
        return hdl->pixmap();
    } else {
        return QPixmap();
    }
}

void JSplitter::setHandlePixmap(const QPixmap &pixmap, int index)
{
    Q_D(JSplitter);
    d->pixmap = pixmap;
    if (index == -1) {
        index = count();
        while (--index >= 0) {
            JSplitterHandle *hdl = handle(index);
            if (hdl) {
                hdl->setPixmap(pixmap);
            }
        }
    } else {
        JSplitterHandle *hdl = handle(index);
        if (hdl) {
            hdl->setPixmap(pixmap);
        }
    }
}

bool JSplitter::isCheckable(int index) const
{
    JSplitterHandle *hdl = handle(index);
    if (hdl) {
        return hdl->isCheckable();
    } else {
        return false;
    }
}

void JSplitter::setCheckable(bool enable, int index)
{
    Q_D(JSplitter);
    d->checkable = enable;
    if (index == -1) {
        index = count();
        while (--index >= 0) {
            JSplitterHandle *hdl = handle(index);
            if (hdl) {
                hdl->setCheckable(enable);
            }
        }
    } else {
        JSplitterHandle *hdl = handle(index);
        if (hdl) {
            hdl->setCheckable(enable);
        }
    }
}

bool JSplitter::isAlwaysVisible(int index) const
{
    JSplitterHandle *hdl = handle(index);
    if (hdl) {
        return hdl->isAlwaysVisible();
    } else {
        return false;
    }
}

void JSplitter::setAlwaysVisible(bool visible, int index)
{
    Q_D(JSplitter);
    d->alwaysVisible = visible;
    if (index == -1) {
        index = count();
        while (--index >= 0) {
            JSplitterHandle *hdl = handle(index);
            if (hdl) {
                hdl->setAlwaysVisible(visible);
            }
        }
    } else {
        JSplitterHandle *hdl = handle(index);
        if (hdl) {
            hdl->setAlwaysVisible(visible);
        }
    }
}

JSplitterHandle *JSplitter::handle(int index) const
{
    return qobject_cast<JSplitterHandle *>(QSplitter::handle(index));
}

void JSplitter::resizeEvent(QResizeEvent *e)
{
    QSplitter::resizeEvent(e);

    updatesSizes();
}

QSplitterHandle *JSplitter::createHandle()
{
    Q_D(JSplitter);
    JSplitterHandle *handle = new JSplitterHandle(orientation(), this);
    handle->setColor(d->color);
    handle->setPixmap(d->pixmap);
    handle->setCheckable(d->checkable);
    handle->setAlwaysVisible(d->alwaysVisible);
    return handle;
}

void JSplitter::updatesSizes()
{
    Q_D(JSplitter);
    int totalSize = (orientation() == Qt::Horizontal ? width() : height());
    QList<int> sizes;
    QListIterator<double> iter(d->scales);
    while (iter.hasNext()) {
        sizes << (totalSize * iter.next() / d->totalScale);
    }

    setSizes(sizes);
}

void JSplitter::_emit_splitterMoved(int pos, int index)
{
    Q_D(JSplitter);
    Q_UNUSED(pos);

    if (index < 0 || index >= d->scales.count()) {
        return;
    }

    int totalSize = (orientation() == Qt::Horizontal ? width() : height());
    QList<int> siz = sizes();

    d->scales.replace(index - 1, d->totalScale * siz.at(index - 1) / totalSize);
    d->scales.replace(index, d->totalScale * siz.at(index) / totalSize);
}

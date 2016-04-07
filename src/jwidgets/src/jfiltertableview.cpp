#include "precomp.h"
#include "jfiltertableview.h"
#include "private/jfiltertableview_p.h"

// - class JFilterTableViewPrivate -

JFilterTableView::JFilterTableView(QWidget *parent)
    : JHeaderArea(parent)
    , d_ptr(new JFilterTableViewPrivate(this))
{
    Q_D(JFilterTableView);
    d->init();
}

JFilterTableView::~JFilterTableView()
{
    delete d_ptr;
}

JXmlTable *JFilterTableView::view()
{
    Q_D(JFilterTableView);
    return d->view;
}

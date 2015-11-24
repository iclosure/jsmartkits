#include "precomp.h"
#include "jtreeview.h"
#include "private/jtreeview_p.h"

// - class JTableView -

JTreeView::JTreeView(QWidget *parent)
    : QTreeView(parent)
    , d_ptr(new JTreeViewPrivate(this))
{

}


JTreeView::~JTreeView()
{
    d_ptr->deleteLater();
}

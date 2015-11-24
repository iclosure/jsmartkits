#include "precomp.h"
#include "jtreeview_p.h"

// - class JTreeViewPrivate -

JTreeViewPrivate::JTreeViewPrivate(JTreeView *parent)
    : QObject(parent)
    , q_ptr(parent)
{

}

#include "precomp.h"
#include "jfiltertableview_p.h"
#include "../jfiltertableview.h"

// - class JFilterTableViewPrivate -

JFilterTableViewPrivate::JFilterTableViewPrivate(JFilterTableView *parent)
    : q_ptr(parent)
{

}

void JFilterTableViewPrivate::init()
{
    Q_Q(JFilterTableView);
    q->setObjectName("JFilterTableView");

    view = new JXmlTable(q);

    q->attach(view);
}

#include "precomp.h"
#include "jfiltertableview_p.h"
#include "../jfiltertableview.h"
#include "jxmltable_data.h"

// - class JFilterTableViewPrivate -

JFilterTableViewPrivate::JFilterTableViewPrivate(JFilterTableView *parent)
    : QObject(parent)
    , q_ptr(parent)
{

}

void JFilterTableViewPrivate::init()
{
    Q_Q(JFilterTableView);

    view = new JXmlTable(q);

    connect(q, SIGNAL(attached()), SLOT(_emit_attached()));
    connect(q, SIGNAL(detached()), SLOT(_emit_detached()));

    q->attach(view);
}

void JFilterTableViewPrivate::_emit_filterChanged(int column, int type, bool visible)
{
    Q_Q(JFilterTableView);

    if (visible) {
        switch (type) {
        case InvalidValue:

            break;
        case BoolValue:

            break;
        case NumericValue:

            break;
        case StringValue:   // LineEdit
            q->setFilterItem(column, "");
            break;
        case EnumValue:     // ComboBox
            q->setFilterItem(column);
            break;
        case DelegateValue:
            break;
        default:
            break;
        }
    } else {
        q->removeFilterItem(column);
    }
}

void JFilterTableViewPrivate::_emit_attached()
{
    connect(view, SIGNAL(filterChanged(int,int,bool)),
            SLOT(_emit_filterChanged(int,int,bool)));
}

void JFilterTableViewPrivate::_emit_detached()
{
    disconnect(view, SIGNAL(filterChanged(int,int,bool)),
               this, SLOT(_emit_filterChanged(int,int,bool)));
}

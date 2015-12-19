#ifndef JTREEVIEW_P_H
#define JTREEVIEW_P_H

#include "../jwidgets_global.h"
#include "../jtreeview.h"

// - class JTreeViewPrivate -

class JTreeViewPrivate : public QObject
{
    Q_OBJECT
    J_DECLARE_PUBLIC(JTreeView)
public:
    explicit JTreeViewPrivate(JTreeView *parent);

};

#endif // JTREEVIEW_P_H

#ifndef JFILTERTABLEVIEW_P_H
#define JFILTERTABLEVIEW_P_H

#include "jwidgets_global.h"
#include "../jfiltertableview.h"

// - class JFilterTableViewPrivate -

class JFilterTableViewPrivate
{
    J_DECLARE_PUBLIC(JFilterTableView)
public:
    JFilterTableViewPrivate(JFilterTableView *parent = 0);

private:
    void init();

private:
    JXmlTable *view;
};



#endif // JFILTERTABLEVIEW_P_H

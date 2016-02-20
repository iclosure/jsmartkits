#ifndef JFILTERTABLEVIEW_P_H
#define JFILTERTABLEVIEW_P_H

#include "../jwidgets_global.h"
#include "../jfiltertableview.h"

// - class JFilterTableViewPrivate -

class JFilterTableViewPrivate : public QObject
{
    Q_OBJECT
    J_DECLARE_PUBLIC(JFilterTableView)
public:
    JFilterTableViewPrivate(JFilterTableView *parent = 0);

public Q_SLOTS:
    void _emit_filterChanged(int column, int type, bool visible);
    void _emit_attached();
    void _emit_detached();

private:
    void init();

private:
    JXmlTable *view;
};

#endif // JFILTERTABLEVIEW_P_H

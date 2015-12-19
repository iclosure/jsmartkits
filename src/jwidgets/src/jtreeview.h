#ifndef JTREEVIEW_H
#define JTREEVIEW_H

#include "jwidgets_global.h"
#include <QTreeView>

// - class JTreeView -

class JTreeViewPrivate;

class JWIDGETS_EXPORT JTreeView : public QTreeView
{
    Q_OBJECT
public:
    explicit JTreeView(QWidget *parent);
    ~JTreeView();

Q_SIGNALS:

public Q_SLOTS:

private:
    Q_DISABLE_COPY(JTreeView)
    J_DECLARE_PRIVATE(JTreeView)
};

#endif // JTREEVIEW_H

#ifndef JSTATUSBAR_H
#define JSTATUSBAR_H

#include "jwidgets_global.h"
#include <QStatusBar>

class JWIDGETS_EXPORT JStatusBar : public QStatusBar
{
    Q_OBJECT
public:
    explicit JStatusBar(QWidget *parent = 0);

private:
};

#endif // JSTATUSBAR_H

#include "precomp.h"
#include "jstatusbar.h"

JStatusBar::JStatusBar(QWidget *parent) :
    QStatusBar(parent)
{
    setFixedHeight(height());
}

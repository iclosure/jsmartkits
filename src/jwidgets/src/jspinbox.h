#ifndef JSPINBOX_H
#define JSPINBOX_H

#include "jwidgets_global.h"
#include <QSpinBox>

class JWIDGETS_EXPORT JSpinBox : public QSpinBox
{
    Q_OBJECT
public:
    explicit JSpinBox(QWidget *parent = 0);

Q_SIGNALS:

public Q_SLOTS:
};

#endif // JSPINBOX_H

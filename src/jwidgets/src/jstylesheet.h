#ifndef JSTYLESHEET_H
#define JSTYLESHEET_H

#include "jwidgets_global.h"
#include <QObject>

// - class JStyleSheet -

class JWIDGETS_EXPORT JStyleSheet
{
public:
    static JStyleSheet *instance();
    static void releaseInstance();

    QStringList keys() const;

    static QString styleSheet(const QString &name);

private:
    explicit JStyleSheet();
    ~JStyleSheet();
};

#endif // JSTYLE_H

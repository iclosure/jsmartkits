#include "precomp.h"
#include "jstylesheet.h"

// - class JStyleSheetPrivate -

class JStyleSheetPrivate
{
public:
    static JStyleSheet *_instance;
};

JStyleSheet *JStyleSheetPrivate::_instance = 0;

// - class JStyleSheet -

JStyleSheet::JStyleSheet()
{

}

JStyleSheet::~JStyleSheet()
{

}

JStyleSheet *JStyleSheet::instance()
{
    if (JStyleSheetPrivate::_instance == 0) {
        JStyleSheetPrivate::_instance = new JStyleSheet;
    }

    return JStyleSheetPrivate::_instance;
}

void JStyleSheet::releaseInstance()
{
    if (JStyleSheetPrivate::_instance != 0) {
        delete JStyleSheetPrivate::_instance;
        JStyleSheetPrivate::_instance = 0;
    }
}

QStringList JStyleSheet::keys() const
{
    QStringList l;
    l << QLatin1String("default");
    l << QLatin1String("mainwidget");
    l << QLatin1String("startPage");
    return l;
}

QString JStyleSheet::styleSheet(const QString &name)
{
    if (name == QLatin1String("default")) {
        QFile file(QLatin1String(":/com.smartsoft.jsmartkits.jwidgets/qss/stylesheet_default.qss"));
        if (!file.open(QFile::ReadOnly)) {
            return QString::null;
        }
        return file.readAll();
    } else if (name == QLatin1String("mainwidget")) {
        QFile file(QLatin1String(":/com.smartsoft.jsmartkits.jwidgets/qss/stylesheet_mainwidget.qss"));
        if (!file.open(QFile::ReadOnly)) {
            return QString::null;
        }
        return file.readAll();
    } else if (name == QLatin1String("startPage")) {
        QFile file(QLatin1String(":/com.smartsoft.jsmartkits.jwidgets/qss/stylesheet_startpage.qss"));
        if (!file.open(QFile::ReadOnly)) {
            return QString::null;
        }
        return file.readAll();
    } else if (name == QLatin1String("login")) {
        QFile file(QLatin1String(":/com.smartsoft.jsmartkits.jwidgets/qss/stylesheet_default-login.qss"));
        if (!file.open(QFile::ReadOnly)) {
            return QString::null;
        }
        return file.readAll();
    }

    return QString::null;
}

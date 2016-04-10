#include "precomp.h"
#include "jwidgets_global.h"

// - class JWidgetsCorePrivate -

class JWidgetsCorePrivate
{
public:
    static JWidgetsCore *_instance;
    QList<QTranslator *> translators;
};

JWidgetsCore *JWidgetsCorePrivate::_instance = 0;

// - class JWidgetsCore -

JWidgetsCore *JWidgetsCore::instance()
{
    if (JWidgetsCorePrivate::_instance == 0) {
        JWidgetsCorePrivate::_instance = new JWidgetsCore;
    }

    return JWidgetsCorePrivate::_instance;
}

void JWidgetsCore::releaseInstance()
{
    if (JWidgetsCorePrivate::_instance != 0) {
        delete JWidgetsCorePrivate::_instance;
        JWidgetsCorePrivate::_instance = 0;
    }
}

void JWidgetsCore::init()
{
    Q_INIT_RESOURCE(jwidget_resource);
}

bool JWidgetsCore::loadSystemLang(const QString &systemName)
{
    QString name = systemName.isEmpty() ? QLocale::system().name() : systemName;

    //
    d->translators.append(new QTranslator());
    bool result = d->translators.last()->load(
                ":/com.smartsoft.jsmartkits.jwidgets/lang/qt_" + name + ".qm");
    if (result) {
        return qApp->installTranslator(d->translators.last());
    }

    return false;
}

JWidgetsCore::JWidgetsCore() :
    d(new JWidgetsCorePrivate())
{
    init();
}

JWidgetsCore::~JWidgetsCore()
{
    Q_ASSERT(qApp);

    // remove translators
    Q_FOREACH (QTranslator *translator, d->translators) {
        qApp->removeTranslator(translator);
        delete translator;
    }

    Q_CLEANUP_RESOURCE(jwidget_resource);

    delete d;
}

// - class JAutoCursor -


JAutoCursor::JAutoCursor(Qt::CursorShape shape)
{
    QApplication::setOverrideCursor(shape);
}

JAutoCursor::~JAutoCursor()
{
    QApplication::restoreOverrideCursor();
}

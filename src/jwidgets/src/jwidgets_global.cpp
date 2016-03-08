#include "precomp.h"
#include "jwidgets_global.h"

// - class JWidgetsCorePrivate -

class JWidgetsCorePrivate
{
public:
    static JWidgetsCore *_instance;
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

JWidgetsCore::JWidgetsCore()
{
    //Q_INIT_RESOURCE(jwidget_resource);
}

JWidgetsCore::~JWidgetsCore()
{
    Q_CLEANUP_RESOURCE(jwidget_resource);
}

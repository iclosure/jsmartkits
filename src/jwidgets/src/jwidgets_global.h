#ifndef JWIDGETS_GLOBAL_H
#define JWIDGETS_GLOBAL_H

#include <qglobal.h>

// JWIDGETS_VERSION is (major << 16) + (minor << 8) + patch.

#define JWIDGETS_VERSION       0x000001
#define JWIDGETS_VERSION_STR   "0.0.1"

#if defined(Q_WS_WIN) || defined(Q_OS_WIN) || defined(Q_WS_S60)

#if defined(_MSC_VER) // MSVC Compiler
// template-class specialization 'identifier' is already instantiated
#pragma warning(disable: 4660)
#pragma warning(disable: 4819)  // warning : 936
#endif // _MSC_VER

#ifdef JWIDGETS_DLL

#if defined(JWIDGETS_MAKEDLL)
#define JWIDGETS_EXPORT  Q_DECL_EXPORT
#else
#define JWIDGETS_EXPORT  Q_DECL_IMPORT

#if defined(_DEBUG) || defined(DEBUG)
#pragma comment(lib, "com.smartsoft.jsmartkits.jwidgetsd.lib")
#else
#pragma comment(lib, "com.smartsoft.jsmartkits.jwidgets.lib")
#endif

#endif

#endif // JWIDGETS_DLL

#endif // Q_WS_WIN || Q_WS_S60

#ifndef JWIDGETS_EXPORT
#define JWIDGETS_EXPORT
#endif

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

// - namespace JWIDGETS -
#define JWIDGETS_NAMESPACE JWIDGETS
#define JWIDGETS_BEGIN_NAMESPACE namespace JWIDGETS_NAMESPACE {
#define JWIDGETS_END_NAMESPACE }
#define JWIDGETS_USE_NAMESPACE using namespace JWIDGETS_NAMESPACE;

// - private pointer
#ifndef J_DECLARE_PRIVATE
#define J_DECLARE_PRIVATE(name) \
    name ## Private *d_ptr; \
    Q_DECLARE_PRIVATE(name)
#endif

#ifndef J_DECLARE_PUBLIC
#define J_DECLARE_PUBLIC(name) \
    name *q_ptr; \
    Q_DECLARE_PUBLIC(name)
#endif

#ifndef J_DECLARE_SINGLE_INSTANCE
#define J_DECLARE_SINGLE_INSTANCE(Class) \
public: \
    static Class *instance(); \
    static void releaseInstance(); \
private: \
    static Class *_instance;
#endif

#ifndef J_IMPLEMENT_SINGLE_INSTANCE
#define J_IMPLEMENT_SINGLE_INSTANCE(Class) \
Class *Class::_instance = 0; \
\
Class *Class::instance() { \
    if (Class::_instance == 0) { \
        Class::_instance = new Class; \
    } \
    return Class::_instance; \
} \
\
void Class::releaseInstance() { \
    if (Class::_instance != 0) { \
        delete Class::_instance; \
        Class::_instance = 0; \
    } \
}
#endif

namespace JWIDGETS_NAMESPACE {
    static const double jDoubleEpsion = 1E-6;
    static const double jFDoubleEpsion = 1E-6f;

    // ==
    static inline bool jEqual(double a, double b)
    { return (a - b > -jDoubleEpsion || a - b < jDoubleEpsion) ? true : false; }
    static inline bool fEqual(float a, float b)
    { return (a - b > -jFDoubleEpsion || a - b < jFDoubleEpsion) ? true : false; }

    // >
    static inline bool fGreater(double a, double b)
    { return (a - b > jDoubleEpsion) ? true : false; }
    static inline bool fGreater(float a, float b)
    { return (a - b > jFDoubleEpsion) ? true : false; }

    // <
    static inline bool fLess(double a, double b)
    { return (a - b < -jDoubleEpsion) ? true : false; }
    static inline bool fLess(float a, float b)
    { return (a - b < -jFDoubleEpsion) ? true : false; }

    // >=
    static inline bool fGreaterOrEqual(double a, double b)
    { return ((a - b > jDoubleEpsion) ||  (a - b >= -jDoubleEpsion && a - b <= jDoubleEpsion)) ? true : false; }
    static inline bool fGreaterOrEqual(float a, float b)
    { return ((a - b > jFDoubleEpsion) ||  (a - b >= -jFDoubleEpsion && a - b <= jFDoubleEpsion)) ? true : false; }

    // <=
    static inline bool fLessOrEqual(double a, double b)
    { return ((a - b < -jDoubleEpsion) || (a - b >= -jDoubleEpsion && a - b <= jDoubleEpsion)) ? true : false; }
    static inline bool fLessOrEqual(float a, float b)
    { return ((a - b < -jFDoubleEpsion) || (a - b >= -jFDoubleEpsion && a - b <= jFDoubleEpsion)) ? true : false; }
}

// - class JWidgetsCore -

class JWidgetsCorePrivate;

class JWIDGETS_EXPORT JWidgetsCore
{
public:
    static JWidgetsCore *instance();
    static void releaseInstance();

    void init();

    bool loadSystemLang(const QString &systemName = QString());

private:
    explicit JWidgetsCore();
    ~JWidgetsCore();

private:
    JWidgetsCorePrivate *d;
};

// - class JAutoCursor -

class JWIDGETS_EXPORT JAutoCursor
{
public:
    explicit JAutoCursor(Qt::CursorShape shape);
    ~JAutoCursor();
};

#endif // JWIDGETS_GLOBAL_H

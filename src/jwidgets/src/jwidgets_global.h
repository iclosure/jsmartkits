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

#ifdef _DEGBU
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
#define J_DECLARE_PRIVATE(name) \
    name ## Private *d_ptr; \
    Q_DECLARE_PRIVATE(name)
#define J_DECLARE_PUBLIC(name) \
    name *q_ptr; \
    Q_DECLARE_PUBLIC(name)

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

#endif // JWIDGETS_GLOBAL_H

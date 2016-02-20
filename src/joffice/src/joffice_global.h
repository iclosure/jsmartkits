#ifndef JOFFICE_GLOBAL_H
#define JOFFICE_GLOBAL_H

#include <qglobal.h>

// JOFFICE_VERSION is (major << 16) + (minor << 8) + patch.

#define JOFFICE_VERSION       0x000001
#define JOFFICE_VERSION_STR   "0.0.1"

#if defined(Q_WS_WIN) || defined(Q_OS_WIN) || defined(Q_WS_S60)

#if defined(_MSC_VER) // MSVC Compiler
// template-class specialization 'identifier' is already instantiated
#pragma warning(disable: 4660)
#pragma warning(disable: 4819)  // warning : 936
#endif // _MSC_VER

#ifdef JOFFICE_DLL

#if defined(JOFFICE_MAKEDLL)
#define JOFFICE_EXPORT  Q_DECL_EXPORT
#else
#define JOFFICE_EXPORT  Q_DECL_IMPORT

#if defined(_DEBUG) || defined(DEBUG)
#pragma comment(lib, "com.smartsoft.jsmartkits.jofficed.lib")
#else
#pragma comment(lib, "com.smartsoft.jsmartkits.joffice.lib")
#endif

#endif

#endif // JOFFICE_DLL

#endif // Q_WS_WIN || Q_WS_S60

#ifndef JOFFICE_EXPORT
#define JOFFICE_EXPORT
#endif

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

// - namespace JOFFICE -
#define JOFFICE_NAMESPACE JOFFICE
#define JOFFICE_BEGIN_NAMESPACE namespace JOFFICE_NAMESPACE {
#define JOFFICE_END_NAMESPACE }
#define JOFFICE_USE_NAMESPACE using namespace JOFFICE_NAMESPACE;

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

namespace JOFFICE_NAMESPACE {

}

// - class JOfficeCore -

class JOFFICE_EXPORT JOfficeCore
{
public:
    explicit JOfficeCore();
    ~JOfficeCore();
};

#endif // JOFFICE_GLOBAL_H

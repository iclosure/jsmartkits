#ifndef INOTIFIER_H
#define INOTIFIER_H

#include "jwidgets_global.h"
#include <string>

//
#if defined(WIN32) || defined(Q_OS_WIN) || defined(Q_WS_WIN)
typedef unsigned __int64 JWPARAM;
typedef __int64 JLPARAM;
typedef __int64 JLRESULT;
#else
typedef unsigned long long JWPARAM;
typedef long long JLPARAM;
typedef long long JLRESULT;
#endif

//
class JIObserver;
typedef JLRESULT (JIObserver::*JObserverCallbackU)(unsigned int, JWPARAM, JLPARAM);
typedef JLRESULT (JIObserver::*JObserverCallbackS)(const std::string &, JWPARAM, JLPARAM);

// - class JIObserver -

class JIObserver
{
public:
    virtual ~JIObserver() {}

    //
    virtual JLRESULT jnotify(unsigned int, JWPARAM, JLPARAM) { return 0; }
    virtual JLRESULT jnotify(const std::string &, JWPARAM, JLPARAM) { return 0; }

    //
    virtual void jpushed(unsigned int) {}
    virtual void jpushed(unsigned int, JObserverCallbackU) {}
    virtual void jpushed(const std::string &) {}
    virtual void jpushed(const std::string &, JObserverCallbackS) {}
    virtual void jpopped(unsigned int = 0) {}   // when param=0, pop all id
    virtual void jpopped(const std::string &);   // when param is empty, pop all id
};

// - class INotifier -

class INotifier
{
public:
    virtual ~INotifier() {}

    //
    virtual INotifier& begin(JIObserver *observer, int delta = 0) = 0;
    virtual INotifier& end() = 0;

    template<typename O> INotifier& begin(O *observer);

    //
    virtual INotifier& push(JIObserver *observer, unsigned int id, int delta = 0) = 0;
    virtual INotifier& push(JIObserver *observer, unsigned int id, JObserverCallbackU callback, int delta = 0) = 0;
    virtual INotifier& push(JIObserver *observer, const std::string &id, int delta = 0) = 0;
    virtual INotifier& push(JIObserver *observer, const std::string &id, JObserverCallbackS callback, int delta = 0) = 0;
    virtual INotifier& push(unsigned int id) = 0;   // inotify().begin(this).push(.).push(.).end();
    virtual INotifier& push(unsigned int id, JObserverCallbackU callback) = 0;
    virtual INotifier& push(const std::string &id) = 0;
    virtual INotifier& push(const std::string &id, JObserverCallbackS callback) = 0;

    //
    template<typename O> INotifier& push(JIObserver *observer, unsigned int id,
                                       JLRESULT (O::*callback)(unsigned int, JWPARAM, JLPARAM));
    template<typename O> INotifier& push(JIObserver *observer, const std::string &id,
                                       JLRESULT (O::*callback)(const std::string &, JWPARAM, JLPARAM));
    template<typename O> INotifier& push(unsigned int id,
                                       JLRESULT (O::*callback)(unsigned int, JWPARAM, JLPARAM));
    template<typename O> INotifier& push(const std::string &id,
                                       JLRESULT (O::*callback)(const std::string &, JWPARAM, JLPARAM));

    //
    virtual INotifier& pop(JIObserver *observer) = 0;     // inotify().pop(this);    // remove all identitiy of this
    virtual INotifier& pop(JIObserver *observer, unsigned int id) = 0;
    virtual INotifier& pop(JIObserver *observer, const std::string &id) = 0;
    virtual INotifier& pop(unsigned int id) = 0;    // inotify().begin(this).pop(.).pop(.).end();
    virtual INotifier& pop(const std::string &id) = 0;

    //
    virtual void clear() = 0;

    //
    virtual JLRESULT send(unsigned int id, JWPARAM wParam = 0, JLPARAM lParam = 0) = 0;
    virtual JLRESULT send(const std::string &id, JWPARAM wParam = 0, JLPARAM lParam = 0) = 0;

    //
    virtual void post(unsigned int id, JWPARAM wParam = 0, JLPARAM lParam = 0) = 0;
    virtual void post(const std::string &id, JWPARAM wParam = 0, JLPARAM lParam = 0) = 0;
    virtual void post(unsigned int id, const std::string &msg, JLPARAM lParam = 0) = 0;
    virtual void post(const std::string &id, const std::string &msg, JLPARAM lParam = 0) = 0;
};

#if defined(_MSC_VER)
#pragma warning (push)
#pragma warning (disable : 4407)
#endif

template<typename O> inline
INotifier &INotifier::begin(O *observer)
{
    return begin(observer,
                 reinterpret_cast<JIObserver *>(observer) - static_cast<JIObserver *>(observer));
}

template<typename O> inline
INotifier &INotifier::push(JIObserver *observer, unsigned int id,
                       JLRESULT (O::*callback)(unsigned int, JWPARAM, JLPARAM))
{
    return push(observer, id, static_cast<JObserverCallbackU>(callback),
                reinterpret_cast<JIObserver *>(observer) - static_cast<JIObserver *>(observer));
}

template<typename O> inline
INotifier &INotifier::push(JIObserver *observer, const std::string &id,
                       JLRESULT (O::*callback)(const std::string &, JWPARAM, JLPARAM))
{
    return push(observer, id, static_cast<JObserverCallbackS>(callback),
                reinterpret_cast<JIObserver *>(observer) - static_cast<JIObserver *>(observer));
}

template<typename O> inline
INotifier &INotifier::push(unsigned int id,
                       JLRESULT (O::*callback)(unsigned int, JWPARAM, JLPARAM))
{
    return push(id, static_cast<JObserverCallbackU>(callback));
}

template<typename O> inline
INotifier &INotifier::push(const std::string &id,
                       JLRESULT (O::*callback)(const std::string &, JWPARAM, JLPARAM))
{
    return push(id, static_cast<JObserverCallbackS>(callback));
}

#if defined(_MSC_VER)
#pragma warning (pop)
#endif

#endif // INOTIFIER_H

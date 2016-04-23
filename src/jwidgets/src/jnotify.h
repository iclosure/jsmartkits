﻿#ifndef INOTIFY_H
#define INOTIFY_H

#include "jwidgets_global.h"
#include <string>

//
typedef unsigned __int64 JWPARAM;
typedef __int64 JLPARAM;
typedef __int64 JLRESULT;

//
class JIObserver;
typedef JLRESULT (JIObserver::*JObserverCallbackU)(unsigned int, JWPARAM, JLPARAM);
typedef JLRESULT (JIObserver::*JObserverCallbackS)(const std::string &, JWPARAM, JLPARAM);

//template<typename O> JLRESULT (O::*jcallback)(unsigned int, JWPARAM, JLPARAM);

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

// - class INotify -

class INotify
{
public:
    virtual ~INotify() {}

    //
    virtual INotify& begin(JIObserver *observer) = 0;
    virtual INotify& end() = 0;

    //
    virtual INotify& push(JIObserver *observer, unsigned int id) = 0;
    virtual INotify& push(JIObserver *observer, unsigned int id, JObserverCallbackU callback) = 0;
    virtual INotify& push(JIObserver *observer, const std::string &id) = 0;
    virtual INotify& push(JIObserver *observer, const std::string &id, JObserverCallbackS callback) = 0;
    virtual INotify& push(unsigned int id) = 0;   // inotify().begin(this).push(.).push(.).end();
    virtual INotify& push(unsigned int id, JObserverCallbackU callback) = 0;
    virtual INotify& push(const std::string &id) = 0;
    virtual INotify& push(const std::string &id, JObserverCallbackS callback) = 0;

    //
    template<typename O> INotify& push(JIObserver *observer, unsigned int id,
                                         JLRESULT (O::*callback)(unsigned int, JWPARAM, JLPARAM));
    template<typename O> INotify& push(JIObserver *observer, const std::string &id,
                                         JLRESULT (O::*callback)(const std::string &, JWPARAM, JLPARAM));
    template<typename O> INotify& push(unsigned int id,
                                         JLRESULT (O::*callback)(unsigned int, JWPARAM, JLPARAM));
    template<typename O> INotify& push(const std::string &id,
                                         JLRESULT (O::*callback)(const std::string &, JWPARAM, JLPARAM));

    //
    virtual INotify& pop(JIObserver *observer) = 0;     // inotify().pop(this);    // remove all identitiy of this
    virtual INotify& pop(JIObserver *observer, unsigned int id) = 0;
    virtual INotify& pop(JIObserver *observer, const std::string &id) = 0;
    virtual INotify& pop(unsigned int id) = 0;    // inotify().begin(this).pop(.).pop(.).end();
    virtual INotify& pop(const std::string &id) = 0;

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

public:
    //
    enum Flag
    {
        //
        AutoDeleteWParam = 0xfffffffaul,
    };
};

#pragma warning (push)
#pragma warning (disable : 4407)

template<typename O> inline
INotify &INotify::push(JIObserver *observer, unsigned int id,
                           JLRESULT (O::*callback)(unsigned int, JWPARAM, JLPARAM))
{
    return push(observer, id, static_cast<JObserverCallbackU>(callback));
}

template<typename O> inline
INotify &INotify::push(JIObserver *observer, const std::string &id,
                           JLRESULT (O::*callback)(const std::string &, JWPARAM, JLPARAM))
{
    return push(observer, id, static_cast<JObserverCallbackS>(callback));
}

template<typename O> inline
INotify &INotify::push(unsigned int id,
                           JLRESULT (O::*callback)(unsigned int, JWPARAM, JLPARAM))
{
    return push(id, static_cast<JObserverCallbackU>(callback));
}

template<typename O> inline
INotify &INotify::push(const std::string &id,
                           JLRESULT (O::*callback)(const std::string &, JWPARAM, JLPARAM))
{
    return push(id, static_cast<JObserverCallbackS>(callback));
}

#pragma warning (pop)

#endif // INOTIFY_H

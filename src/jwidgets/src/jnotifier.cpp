﻿#include "precomp.h"
#include "private/jnotifier_p.h"

// - class JNotifyPrivate -

class JNotifierPrivate
{
    J_DECLARE_PUBLIC(JNotifier)
public:
    JNotifierPrivate(JNotifier *parent) :
        q_ptr(parent),
        observer(0)
    {

    }

private:
    JIObserver *observer;
    int delta;
    jmap_obs_info_u mapObsU;
    jmap_obs_info_s mapObsS;
};

// - class JNotifier -

JNotifier::JNotifier(QObject *parent) :
    QObject(parent),
    d_ptr(new JNotifierPrivate(this))
{
    qRegisterMetaType<JNotifyMsg>();

    connect(this, SIGNAL(readyDispense(JNotifyMsg)),
            SLOT(dispense(JNotifyMsg)), Qt::QueuedConnection);
}

JNotifier::~JNotifier()
{
    Q_D(JNotifier);
    delete d;
}


INotifier &JNotifier::begin(JIObserver *observer, int delta)
{
    Q_ASSERT(observer);
    Q_D(JNotifier);
    d->observer = observer;
    d->delta = delta;
    return *this;
}

INotifier &JNotifier::end()
{
    Q_D(JNotifier);
    d->observer = 0;
    d->delta = 0;
    return *this;
}

INotifier &JNotifier::push(JIObserver *observer, unsigned int id, int delta)
{
    return push(observer, (unsigned int)id, (JObserverCallbackU)0, delta);
}

JNotifier &JNotifier::push(JIObserver *observer, unsigned int id, JObserverCallbackU callback, int delta)
{
    if (observer == 0) {
        return *this;
    }

    Q_D(JNotifier);
    jmap_obs_info_u::iterator iterObs = d->mapObsU.find(observer);
    if (iterObs != d->mapObsU.end()) {
        jlst_pair_id_cbu &pids = iterObs->second.pids;
        jlst_pair_id_cbu::iterator iterIds = pids.begin();
        for (; iterIds != pids.end(); ++iterIds) {
            jpair_id_cbu &pid = *iterIds;
            if (pid.first == id) {
                break;
            }
        }

        if (iterIds != pids.end()) {
            return *this;
        }

        pids.push_back(jpair_id_cbu(id, callback));
    } else {
        jlst_pair_id_cbu pids;
        pids.push_back(jpair_id_cbu(id, callback));
        d->mapObsU.insert(jmap_obs_info_u::value_type(observer, obs_info_u(delta, pids)));
    }

    return *this;
}

INotifier &JNotifier::push(JIObserver *observer, const std::string &id, int delta)
{
    return push(observer, (const std::string &)id, (JObserverCallbackS)0, delta);
}

INotifier &JNotifier::push(JIObserver *observer, const std::string &id, JObserverCallbackS callback, int delta)
{
    if (observer == 0) {
        return *this;
    }

    Q_D(JNotifier);
    jmap_obs_info_s::iterator iterObs = d->mapObsS.find(observer);
    if (iterObs != d->mapObsS.end()) {
        jlst_pair_id_cbs &pids = iterObs->second.pids;
        jlst_pair_id_cbs::iterator iterIds = pids.begin();
        for (; iterIds != pids.end(); ++iterIds) {
            jpair_id_cbs &pid = *iterIds;
            if (pid.first == id) {
                break;
            }
        }

        if (iterIds != pids.end()) {
            return *this;
        }

        pids.push_back(jpair_id_cbs(id, callback));
    } else {
        jlst_pair_id_cbs pids;
        pids.push_back(jpair_id_cbs(id, callback));
        d->mapObsS.insert(jmap_obs_info_s::value_type(observer, obs_info_s(delta, pids)));
    }

    return *this;
}

INotifier &JNotifier::push(unsigned int id)
{
    Q_D(JNotifier);
    return push(d->observer, (unsigned int)id, (JObserverCallbackU)0, d->delta);
}

INotifier &JNotifier::push(unsigned int id, JObserverCallbackU callback)
{
    Q_D(JNotifier);
    return push(d->observer, (unsigned int)id, (JObserverCallbackU)callback, d->delta);
}

INotifier &JNotifier::push(const std::string &id)
{
    Q_D(JNotifier);
    return push(d->observer, (const std::string &)id, (JObserverCallbackS)0, d->delta);
}

INotifier &JNotifier::push(const std::string &id, JObserverCallbackS callback)
{
    Q_D(JNotifier);
    return push(d->observer, (const std::string &)id, (JObserverCallbackS)callback, d->delta);
}

INotifier &JNotifier::pop(JIObserver *observer)
{
    if (observer == 0) {
        return *this;
    }

    Q_D(JNotifier);
    // for mapObsU
    d->mapObsU.erase(observer);
    // for mapObsS
    d->mapObsS.erase(observer);

    observer->jpopped();    // notify observer

    return *this;
}

INotifier &JNotifier::pop(JIObserver *observer, unsigned int id)
{
    Q_D(JNotifier);
    jmap_obs_info_u::iterator iterObs = d->mapObsU.find(observer);
    if (iterObs == d->mapObsU.end()) {
        return *this;
    }

    jlst_pair_id_cbu &pids = iterObs->second.pids;
    if (!pids.empty()) {
        jlst_pair_id_cbu::iterator iterIds = pids.begin();
        for (; iterIds != pids.end(); ++iterIds) {
            jpair_id_cbu &pid = *iterIds;
            if (pid.first == id) {
                pids.erase(iterIds);
                break;
            }
        }
    }

    if (pids.empty()) {
        d->mapObsU.erase(observer);
    }

    return *this;
}

INotifier &JNotifier::pop(JIObserver *observer, const std::string &id)
{
    Q_D(JNotifier);
    jmap_obs_info_s::iterator iterObs = d->mapObsS.find(observer);
    if (iterObs == d->mapObsS.end()) {
        return *this;
    }

    jlst_pair_id_cbs &pids = iterObs->second.pids;
    if (!pids.empty()) {
        jlst_pair_id_cbs::iterator iterIds = pids.begin();
        for (; iterIds != pids.end(); ++iterIds) {
            jpair_id_cbs &pid = *iterIds;
            if (pid.first == id) {
                pids.erase(iterIds);
                break;
            }
        }
    }

    if (pids.empty()) {
        d->mapObsS.erase(observer);
    }

    return *this;
}

INotifier &JNotifier::pop(unsigned int id)
{
    Q_D(JNotifier);
    return pop(d->observer, id);
}

INotifier &JNotifier::pop(const std::string &id)
{
    Q_D(JNotifier);
    return pop(d->observer, id);
}

void JNotifier::clear()
{
    Q_D(JNotifier);
    // for mapObsU
    jmap_obs_info_u::iterator iterObsU = d->mapObsU.begin();
    for (; iterObsU != d->mapObsU.end(); ++iterObsU) {
        jlst_pair_id_cbu &pids = iterObsU->second.pids;
        jlst_pair_id_cbu::iterator iterIds = pids.begin();
        for (; iterIds != pids.end(); ++iterIds) {
            iterObsU->first->jpopped(iterIds->first);
            iterIds = pids.erase(iterIds);
        }
        iterObsU = d->mapObsU.erase(iterObsU);
    }
    // for mapObsS
    jmap_obs_info_s::iterator iterObsS = d->mapObsS.begin();
    for (; iterObsS != d->mapObsS.end(); ++iterObsS) {
        jlst_pair_id_cbs &pids = iterObsS->second.pids;
        jlst_pair_id_cbs::iterator iterIds = pids.begin();
        for (; iterIds != pids.end(); ++iterIds) {
            iterObsS->first->jpopped(iterIds->first);
            iterIds = pids.erase(iterIds);
        }
        iterObsS = d->mapObsS.erase(iterObsS);
    }
}

JLRESULT JNotifier::send(unsigned int id, JWPARAM wParam, JLPARAM lParam)
{
    return dispense(id, wParam, lParam);
}

JLRESULT JNotifier::send(const std::string &id, JWPARAM wParam, JLPARAM lParam)
{
    return dispense(id, wParam, lParam);
}

void JNotifier::post(unsigned int id, JWPARAM wParam, JLPARAM lParam)
{
    QtConcurrent::run(JNotifier::postCallback, this, JNotifyMsg(id, "", wParam, lParam, ""));
}

void JNotifier::post(const std::string &id, JWPARAM wParam, JLPARAM lParam)
{
    QtConcurrent::run(JNotifier::postCallback, this, JNotifyMsg(0, id, wParam, lParam, ""));
}

void JNotifier::post(unsigned int id, const std::string &msg, JLPARAM lParam)
{
    QtConcurrent::run(JNotifier::postCallback, this, JNotifyMsg(id, "", 0, lParam, msg));
}

void JNotifier::post(const std::string &id, const std::string &msg, JLPARAM lParam)
{
    QtConcurrent::run(JNotifier::postCallback, this, JNotifyMsg(0, id, 0, lParam, msg));
}

void JNotifier::dispense(const JNotifyMsg &msg)
{
    if (msg.msg.empty()) {
        if (msg.sid.empty()) {
            dispense(msg.uid, msg.wParam, msg.lParam);
        } else {
            dispense(msg.sid, msg.wParam, msg.lParam);
        }
    } else {
        if (msg.sid.empty()) {
            dispense(msg.uid, msg.wParam, msg.lParam);
        } else {
            dispense(msg.sid, msg.wParam, msg.lParam);
        }
    }
}

JLRESULT JNotifier::dispense(unsigned int id, JWPARAM wParam, JLPARAM lParam)
{
    Q_D(JNotifier);
    JLRESULT result = 0;
    jmap_obs_info_u::const_iterator citerObs = d->mapObsU.begin();
    for (; citerObs != d->mapObsU.end(); ++citerObs) {
        JIObserver *observer = citerObs->first;
        const int delta = citerObs->second.delta;
        const jlst_pair_id_cbu &pids = citerObs->second.pids;
        jlst_pair_id_cbu::const_iterator citerIds = pids.begin();
        for (; citerIds != pids.end(); ++citerIds) {
            const jpair_id_cbu &pid = *citerIds;
            if (pid.first == id) {
                if (pid.second == 0) {
                    result = observer->jnotify(id, wParam, lParam);
                } else {
                    result = ((observer + delta)->*(pid.second))(id, wParam, lParam);
                }
            }
        }
    }

    return result;
}

JLRESULT JNotifier::dispense(const std::string &id, JWPARAM wParam, JLPARAM lParam)
{
    Q_D(JNotifier);
    JLRESULT result = 0;
    jmap_obs_info_s::const_iterator citerObs = d->mapObsS.begin();
    for (; citerObs != d->mapObsS.end(); ++citerObs) {
        JIObserver *observer = citerObs->first;
        const int delta = citerObs->second.delta;
        const jlst_pair_id_cbs &pids = citerObs->second.pids;
        jlst_pair_id_cbs::const_iterator citerIds = pids.begin();
        for (; citerIds != pids.end(); ++citerIds) {
            const jpair_id_cbs &pid = *citerIds;
            if (pid.first == id) {
                if (pid.second == 0) {
                    result = observer->jnotify(id, wParam, lParam);
                } else {
                    result = ((observer + delta)->*(pid.second))(id, wParam, lParam);
                }
            }
        }
    }

    return result;
}

void JNotifier::postCallback(JNotifier *receiver, JNotifyMsg msg)
{
    if (receiver == 0) {
        return;
    }

    Q_EMIT receiver->readyDispense(msg);
}

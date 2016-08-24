#ifndef JNOTIFIER_P_H
#define JNOTIFIER_P_H

#include "../jnotifier.h"
#include <list>
#include <vector>
#include <map>
#include <QtConcurrent/QtConcurrent>
#include <qmetatype.h>

//
typedef std::pair<unsigned int, JObserverCallbackU> jpair_id_cbu;
typedef std::pair<std::string, JObserverCallbackS> jpair_id_cbs;
typedef std::list<jpair_id_cbu> jlst_pair_id_cbu;
typedef std::list<jpair_id_cbs> jlst_pair_id_cbs;

//
struct obs_info_u {
    int delta;
    jlst_pair_id_cbu pids;

    obs_info_u() : delta(0) {}
    obs_info_u(int delta, const jlst_pair_id_cbu &pids)
        :  delta(delta), pids(pids) {}
    obs_info_u(const obs_info_u &other)
    {
        *this = other;
    }
    obs_info_u &operator =(const obs_info_u &other)
    {
        if (this == &other) {
            return *this;
        }

        delta = other.delta;
        pids = other.pids;

        return *this;
    }
};

//
struct obs_info_s {
    int delta;
    jlst_pair_id_cbs pids;

    obs_info_s() : delta(0) {}
    obs_info_s(int delta, const jlst_pair_id_cbs &pids)
        :  delta(delta), pids(pids) {}
    obs_info_s(const obs_info_s &other)
    {
        *this = other;
    }
    obs_info_s &operator =(const obs_info_s &other)
    {
        if (this == &other) {
            return *this;
        }

        delta = other.delta;
        pids = other.pids;

        return *this;
    }
};

typedef std::map<JIObserver*, obs_info_u> jmap_obs_info_u;
typedef std::map<JIObserver*, obs_info_s> jmap_obs_info_s;

// - struct JNotifyMsg -

struct JNotifyMsg
{
    unsigned int uid;
    std::string sid;
    JWPARAM wParam;
    JLPARAM lParam;
    std::string msg;

    JNotifyMsg()
    {
        uid = 0;
        wParam = 0;
        lParam = 0;
    }

    JNotifyMsg(unsigned int uid, const std::string& sid,
               JWPARAM wParam, JLPARAM lParam, const std::string& msg) :
        uid(uid), sid(sid), wParam(wParam), lParam(lParam), msg(msg)
    {

    }

    JNotifyMsg(const JNotifyMsg& other)
    {
        *this = other;
    }

    JNotifyMsg& operator =(const JNotifyMsg &other)
    {
        if (this == &other) {
            return *this;
        }

        uid = other.uid;
        sid = other.sid;
        wParam = other.wParam;
        lParam = other.lParam;
        msg = other.msg;

        return *this;
    }
};

Q_DECLARE_METATYPE(JNotifyMsg)

// - class JNotifier -

class JNotifierPrivate;

class JWIDGETS_EXPORT JNotifier : public QObject, public INotifier
{
    Q_OBJECT
public:
    explicit JNotifier(QObject *parent = 0);
    virtual ~JNotifier();

    // INotify interface
public:
    INotifier &begin(JIObserver *observer, int delta = 0);
    INotifier &end();
    INotifier &push(JIObserver *observer, unsigned int id, int delta = 0);
    JNotifier &push(JIObserver *observer, unsigned int id, JObserverCallbackU callback, int delta = 0);
    INotifier &push(JIObserver *observer, const std::string &id, int delta = 0);
    INotifier &push(JIObserver *observer, const std::string &id, JObserverCallbackS callback, int delta = 0);
    INotifier &push(unsigned int id);
    INotifier &push(unsigned int id, JObserverCallbackU callback);
    INotifier &push(const std::string &id);
    INotifier &push(const std::string &id, JObserverCallbackS callback);
    INotifier &pop(JIObserver *observer);
    INotifier &pop(JIObserver *observer, unsigned int id);
    INotifier &pop(JIObserver *observer, const std::string &id);
    INotifier &pop(unsigned int id);
    INotifier &pop(const std::string &id);
    void clear();
    JLRESULT send(unsigned int id, JWPARAM wParam, JLPARAM lParam);
    JLRESULT send(const std::string &id, JWPARAM wParam, JLPARAM lParam);
    void post(unsigned int id, JWPARAM wParam, JLPARAM lParam);
    void post(const std::string &id, JWPARAM wParam, JLPARAM lParam);
    void post(unsigned int id, const std::string &msg, JLPARAM lParam);
    void post(const std::string &id, const std::string &msg, JLPARAM lParam);

Q_SIGNALS:
    void readyDispense(const JNotifyMsg &);

protected Q_SLOTS:
    void dispense(const JNotifyMsg &msg);

private:
    JLRESULT dispense(unsigned int id, JWPARAM wParam, JLPARAM lParam);
    JLRESULT dispense(const std::string &id, JWPARAM wParam, JLPARAM lParam);

    //
    static void postCallback(JNotifier *receiver, JNotifyMsg msg);

private:
    Q_DISABLE_COPY(JNotifier)
    J_DECLARE_PRIVATE(JNotifier)
};

#endif // JNOTIFIER_P_H

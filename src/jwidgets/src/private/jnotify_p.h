#ifndef JNOTIFY_P_H
#define JNOTIFY_P_H

#include "../jnotify.h"
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

// - class JNotify -

class JNotifyPrivate;

class JWIDGETS_EXPORT JNotify : public QObject, public INotify
{
    Q_OBJECT
public:
    explicit JNotify(QObject *parent = 0);
    virtual ~JNotify();

    // INotify interface
public:
    INotify &begin(JIObserver *observer, int delta = 0);
    INotify &end();
    INotify &push(JIObserver *observer, unsigned int id, int delta = 0);
    JNotify &push(JIObserver *observer, unsigned int id, JObserverCallbackU callback, int delta = 0);
    INotify &push(JIObserver *observer, const std::string &id, int delta = 0);
    INotify &push(JIObserver *observer, const std::string &id, JObserverCallbackS callback, int delta = 0);
    INotify &push(unsigned int id);
    INotify &push(unsigned int id, JObserverCallbackU callback);
    INotify &push(const std::string &id);
    INotify &push(const std::string &id, JObserverCallbackS callback);
    INotify &pop(JIObserver *observer);
    INotify &pop(JIObserver *observer, unsigned int id);
    INotify &pop(JIObserver *observer, const std::string &id);
    INotify &pop(unsigned int id);
    INotify &pop(const std::string &id);
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
    static void postCallback(JNotify *receiver, JNotifyMsg msg);

private:
    Q_DISABLE_COPY(JNotify)
    J_DECLARE_PRIVATE(JNotify)
};

#endif // JNOTIFY_P_H

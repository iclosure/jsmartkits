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
typedef std::map<JIObserver*, jlst_pair_id_cbu> jmap_obs_lst_id_cbu;
typedef std::map<JIObserver*, jlst_pair_id_cbs> jmap_obs_lst_id_cbs;

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
    INotify &begin(JIObserver *observer);
    INotify &end();
    INotify &push(JIObserver *observer, unsigned int id);
    JNotify &push(JIObserver *observer, unsigned int id, JObserverCallbackU callback);
    INotify &push(JIObserver *observer, const std::string &id);
    INotify &push(JIObserver *observer, const std::string &id, JObserverCallbackS callback);
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

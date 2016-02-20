#include "precomp.h"
#include "jlogmanager.h"

// - class JLogManagerPrivate -

class JLogManagerPrivate
{
    J_DECLARE_PUBLIC(JLogManager)
    public:
        JLogManagerPrivate(JLogManager *parent)
      : q_ptr(parent)
    {

    }

private:
    static QString _fileName;
};

QString JLogManagerPrivate::_fileName = "";

// - class JLogManager -

J_IMPLEMENT_SINGLE_INSTANCE(JLogManager)

JLogManager::JLogManager()
    : d_ptr(new JLogManagerPrivate(this))
{
    QString pattern("%{type} | %{time} : %{message} *** \"%{file}\" (line:%{line} - function:%{function} ***");
#ifdef _MSC_VER
    pattern.append("\r\n");
#else
    pattern.append("\n");
#endif
    qSetMessagePattern(pattern);
}

JLogManager::~JLogManager()
{
    delete d_ptr;
}

void JLogManager::installMessageHandler()
{
    JLogManager::instance();

    QString path = QApplication::applicationDirPath().append("/log");
    if (!QDir(path).exists()) {
        QDir().mkdir(path);
    }
    JLogManagerPrivate::_fileName = path + QString("/%1.log").arg(QDateTime::currentDateTime()
                                                                  .toString("yyyy-MM-dd hh-mm-ss"));
    qInstallMessageHandler(messageOutput);
}

void JLogManager::messageOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    //
    static QMutex mutex;
    mutex.lock();

    QFile file(JLogManagerPrivate::_fileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Append)) {
        mutex.unlock();
        return;
    }

    QTextStream stream(&file);
    stream << qFormatLogMessage(type, context, msg);
    file.flush();
    file.close();

    mutex.unlock();
}

void JLogManager::setMessagePattern(const QString &pattern)
{
    qSetMessagePattern(pattern);
}

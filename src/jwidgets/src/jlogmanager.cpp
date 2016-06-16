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
    static QString fileName;
    static JLogManager::LogType logType;
};

QString JLogManagerPrivate::fileName = "";
JLogManager::LogType JLogManagerPrivate::logType = JLogManager::LogFile;

// - class JLogManager -

J_IMPLEMENT_SINGLE_INSTANCE(JLogManager)

JLogManager::JLogManager()
    : d_ptr(new JLogManagerPrivate(this))
{
#if !defined(Q_OS_MAC)
    QString pattern("%{time} | %{type} : %{message} *** \"%{file}"
                    "\" (line:%{line} - function:%{function} ***");
#ifdef _MSC_VER
    pattern.append("\r\n");
#else
    pattern.append("\n");
#endif
    qSetMessagePattern(pattern);
#endif
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

    JLogManagerPrivate::fileName = path + QString("/%1.log").arg(QDateTime::currentDateTime()
                                                                  .toString("yyyy-MM-dd hh-mm-ss"));
#if !defined(Q_OS_MAC)
    qInstallMessageHandler(messageOutput);
#endif
}

#if !defined(Q_OS_MAC)

void JLogManager::messageOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    //
    //static QMutex mutex;
    //mutex.lock();

    //
    if (msg.contains("libpng warning")) {
        //mutex.unlock();
        return;     // ignore
    }

    //
    switch (JLogManagerPrivate::logType) {
    case LogFile:
    {
        QFile file(JLogManagerPrivate::fileName);
        if (!file.open(QIODevice::WriteOnly
                       | QIODevice::Append
                       | QIODevice::Text)) {
            //mutex.unlock();
            return;
        }

        QTextStream stream(&file);
        stream << qFormatLogMessage(type, context, msg);
        file.flush();
        file.close();
        break;
    }
#if defined(DEBUG) || defined(_DEBUG)
    case LogConsole:
    {
        QString formatMsg = qFormatLogMessage(type, context, msg);
        switch (type) {
        case QtDebugMsg: qDebug() << formatMsg; break;
        case QtWarningMsg: qWarning() << formatMsg; break;
        case QtCriticalMsg: qCritical() << formatMsg; break;
        case QtFatalMsg: qFatal(formatMsg.toLocal8Bit()); break;
#if QT_VERSION >= 0x050600
        case QtInfoMsg: qInfo() << formatMsg; break;
#endif
        default:
            break;
        }
        break;
    }
#endif
    default:
        break;
    }

    //mutex.unlock();
}

#endif

void JLogManager::setMessagePattern(const QString &pattern)
{
#if !defined(Q_OS_MAC)
    qSetMessagePattern(pattern);
#else
    Q_UNUSED(pattern);
#endif
}

JLogManager::LogType JLogManager::logType()
{
    return JLogManagerPrivate::logType;
}

void JLogManager::setLogType(LogType logType)
{
    JLogManagerPrivate::logType = logType;
}

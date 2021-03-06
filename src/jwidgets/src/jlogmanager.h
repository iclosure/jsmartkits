﻿#ifndef JLOGMANAGER_H
#define JLOGMANAGER_H

#include "jwidgets_global.h"
#if !defined(Q_OS_MAC)
#include <QLoggingCategory>
#endif

// - class JLogManager -

class JLogManagerPrivate;

class JWIDGETS_EXPORT JLogManager
{
public:
    enum LogType { LogConsole, LogFile };
    static void installMessageHandler();
#if !defined(Q_OS_MAC)
    static void messageOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg);
#endif
    static void setMessagePattern(const QString &pattern);

    static LogType logType();
    static void setLogType(LogType logType);

private:
    JLogManager();
    ~JLogManager();

private:
    J_DECLARE_SINGLE_INSTANCE(JLogManager)
    J_DECLARE_PRIVATE(JLogManager)
};

#endif // JLOGMANAGER_H

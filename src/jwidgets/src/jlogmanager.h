﻿#ifndef JLOGMANAGER_H
#define JLOGMANAGER_H

#include "jwidgets_global.h"
#include <QLoggingCategory>

// - class JLogManager -

class JLogManagerPrivate;

class JWIDGETS_EXPORT JLogManager
{
public:
    static void installMessageHandler();
    static void messageOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg);
    static void setMessagePattern(const QString &pattern);

private:
    JLogManager();
    ~JLogManager();

private:
    J_DECLARE_SINGLE_INSTANCE(JLogManager)
    J_DECLARE_PRIVATE(JLogManager)
};

#endif // JLOGMANAGER_H

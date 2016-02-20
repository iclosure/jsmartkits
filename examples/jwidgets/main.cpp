﻿#include <QApplication>
#include <QTextCodec>
#include <QDebug>

#include "test_jxmltable.h"

int main(int argc, char *argv[])
{
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("GB2312"));
#if QT_VERSION < 0x050000
    QTextCodec::setCodecForTr(QTextCodec::codecForLocale());
    QTextCodec::setCodecForCStrings(QTextCodec::codecForLocale());
#endif

    QApplication app(argc, argv);

    TestJXmlTable testJXmlTable;
    testJXmlTable.showMaximized();

    return app.exec();
}


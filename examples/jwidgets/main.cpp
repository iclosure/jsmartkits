#include <QApplication>
#include <QTextCodec>
#include <QDebug>

#include "test_jxmltable.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
#if 0
    QTextCodec::setCodecForTr(QTextCodec::codecForName("GB2312"));
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("GB2312"));
    QTextCodec::setCodecForCStrings(QTextCodec::codecForName("GB2312"));
#endif
    TestJXmlTable testJXmlTable;
    testJXmlTable.show();

    return app.exec();
}


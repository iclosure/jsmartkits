#include <QApplication>
#include <QTextCodec>
#include <QDebug>

#include "test_jxmltable.h"
#include "test_stylesheet.h"
#include "jwidgets_inc.h"

int main(int argc, char *argv[])
{
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("GB2312"));
#if QT_VERSION < 0x050000
    QTextCodec::setCodecForTr(QTextCodec::codecForLocale());
    QTextCodec::setCodecForCStrings(QTextCodec::codecForLocale());
#endif

    QApplication app(argc, argv);

    JWidgetsCore::instance()->init();

    // stylesheet
    qApp->setStyleSheet(JStyleSheet::instance()->styleSheet("default"));
    //qApp->setStyleSheet(JStyleSheet::instance()->styleSheet("login"));

#if 1
    // test jxmltable
    TestJXmlTable testJXmlTable;
    testJXmlTable.showMaximized();
#elif 0
    // test stylesheet
    TestStyleSheet testStyleSheet;
    testStyleSheet.showMaximized();
#endif

    return app.exec();
}


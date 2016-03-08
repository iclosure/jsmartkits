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

#if 0
    // test jxmltable
    TestJXmlTable testJXmlTable;
    testJXmlTable.showMaximized();
#elif 1
    // test stylesheet
    TestStyleSheet testStyleSheet;
    testStyleSheet.showMaximized();
#endif

    return app.exec();
}


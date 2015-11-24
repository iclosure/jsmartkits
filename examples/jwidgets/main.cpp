#include <QApplication>
#include <QDebug>

#include "test_jxmltable.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    TestJXmlTable testJXmlTable;
    testJXmlTable.show();

    return app.exec();
}


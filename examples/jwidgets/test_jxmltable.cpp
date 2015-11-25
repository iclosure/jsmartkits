#include "test_jxmltable.h"
#include <QtGui>
#include "jspinbox.h"
#include "jipaddressedit.h"

// - class TestJXmlTable -

TestJXmlTable::TestJXmlTable(QWidget *parent)
    : QDialog(parent, Qt::WindowMinMaxButtonsHint)
{
    resize(800, 480);

    QHBoxLayout *horiLayoutMain = new QHBoxLayout(this);
#if 1
    q_horiSplitter = new JSplitter(this);
    q_horiSplitter->setCheckable(true);
    q_horiSplitter->setAlwaysVisible(true);
    q_horiSplitter->setScales(QList<double>() << 1 << 1);
    horiLayoutMain->addWidget(q_horiSplitter);

    q_filterTable = new JFilterTableView(this);
    q_filterTable->setTitleVisible(false);
    q_filterTable->setFilterVisible(true);
    q_horiSplitter->addWidget(q_filterTable);

    connect(q_filterTable->view(), SIGNAL(tableChanged(QString)), SLOT(setWindowTitle(QString)));
//    connect(q_horiSplitter, SIGNAL(handleChanged(int,bool)), SLOT(splitterHandleClicked(int,bool)));

    //
#if 0
    const char *buffer =
            "<table name=\"table #2\" orientations=\"horizontal|vertical\" ></table>";
    q_filterTable->view()->loadConfig(buffer, sizeof(buffer));
#else
    q_filterTable->view()->setConfig(":/jsmartkits/examples/jwidgets/other/xmltable_demo.xml");
#endif

    q_filterTable->view()->setTableName("table #0");
//    q_filterTable->view()->setTableName("table #2");

    switch (q_filterTable->view()->orientations()) {
    case Qt::Horizontal:
        q_filterTable->view()->setRowCount(10);
//        q_filterTable->view()->unpack(q_filterTable->view()->pack(0), 1, -1);
//        q_filterTable->view()->unpack(q_filterTable->view()->pack(1), 0, -1);
        break;
    case Qt::Vertical:
        q_filterTable->view()->setColumnCount(12);
//        q_filterTable->view()->unpack(q_filterTable->view()->pack(-1, 0), -1, 1);
//        q_filterTable->view()->unpack(q_filterTable->view()->pack(-1, 1), -1, 0);
        break;
    default:
        break;
    }

//    q_filterTable->view()->unpack(q_filterTable->view()->pack());
//    q_filterTable->view()->verticalHeader()->setVisible(false);

    q_areaRight = new QWidget(this);
    q_horiSplitter->addWidget(q_areaRight);

    QVBoxLayout *vertLayoutRight = new QVBoxLayout(q_areaRight);

    JLargeSpinBox *spinBox = new JLargeSpinBox(this);
    spinBox->setMinimumWidth(120);
    spinBox->setRange(0, 0xffffffff);
    spinBox->setMaskCount(8);
    spinBox->setRadix(16);
    vertLayoutRight->addWidget(spinBox);

    vertLayoutRight->addWidget(new JIPAddressEdit(this));

    QPushButton *buttonTable1 = new QPushButton("Table 1", this);
    QPushButton *buttonTable2 = new QPushButton("Table 2", this);

    vertLayoutRight->addWidget(buttonTable1);
    vertLayoutRight->addWidget(buttonTable2);

    QTableWidget *xmlTable = new QTableWidget(this);
    xmlTable->setRowCount(10);
    xmlTable->setColumnCount(10);
    //xmlTable->setItemData(1, 1, "test2");
    xmlTable->setCellWidget(1, 1, new QLabel("<font color='red'>test</font>", xmlTable));
    vertLayoutRight->addWidget(xmlTable);

    connect(buttonTable1, SIGNAL(clicked(bool)), SLOT(buttonTable1Clicked()));
    connect(buttonTable2, SIGNAL(clicked(bool)), SLOT(buttonTable2Clicked()));

    q_filterTable->view()->setHighlight("gray", -1, 4);
    q_filterTable->view()->removeHighlight();
    q_filterTable->view()->setHighlight("gray", -1, 7);

    q_filterTable->view()->setItemData(1, 1, "AFEDC");

    connect(q_horiSplitter->handle(1), SIGNAL(handleClicked(bool)), q_areaRight, SLOT(setVisible(bool)));

    //
    q_filterTable->view()->setItemValue(1, 3, "hello");
    q_filterTable->view()->setItemValue(1, 16, 0x100);

    q_filterTable->setFilterItem(5);
    q_filterTable->setFilterItem(6);
    q_filterTable->setTitleVisible(false);

    QStandardItem *item = new QStandardItem;
    item->setCheckable(true);
    item->setCheckState(Qt::Checked);
    q_filterTable->view()->setItem(1, 2, item);

    q_filterTable->view()->setItemValue(1, 18, Qt::PartiallyChecked);
    q_filterTable->view()->setItemValue(1, 19, "");
    q_filterTable->view()->setItemValue(1, 1, "192.168.1.100");

    //qDebug() << q_filterTable->view()->itemValue(1, 17);

    connect(q_filterTable->view(), SIGNAL(readOnlyChanged(bool)), SLOT(onReadOnlyChanged(bool)));

//    q_filterTable->view()->setReadOnly(true);

    QObject *object = q_filterTable->view()->itemObject(2, 4);
    if (object) {
        qDebug() << object->property("name");
        QVariant variant = object->property("range");
        QObject *object2 = 0;
        QMetaObject::invokeMethod(object, "withoutAt", Q_RETURN_ARG(QObject *, object2), Q_ARG(int, 0));
        if (object2) {
            qDebug() << object2->property("text");
        }
    }
/*
    q_filterTable->view()->setItemProperty(1, 0, "maximum", 1024);
    q_filterTable->view()->setItemValue(1, 0, 512);
    q_filterTable->view()->setItemValue(1, 1, ":/jsmartkits/examples/jwidgets/image/qtlogo.png");
    q_filterTable->view()->setItemValue(1, 2, 25);
    q_filterTable->view()->setItemValue(1, 3, "192.168.1.100");
    q_filterTable->view()->setItemValue(1, 4, 0xa4cd);*/

#else

#endif
}

void TestJXmlTable::buttonTable1Clicked()
{
    q_filterTable->view()->clearContents();
    q_filterTable->view()->setTableName("table #3");
    //q_filterTable->view()->setRowCount(100);
    q_filterTable->view()->setCellWidget(1, 0, new QLabel("<font color='red'>readOnly</font>", this));
}

void TestJXmlTable::buttonTable2Clicked()
{
    q_filterTable->view()->setTableName("table #2");
    q_filterTable->view()->setItemValue(2, 1, "test1");
    q_filterTable->view()->setHighlight("red", 2, 1);
    q_filterTable->view()->setRowCount(50);

    QVariant v = q_filterTable->view()->itemData(1, 1);
}

void TestJXmlTable::onReadOnlyChanged(bool value)
{
    qDebug() << value;
}

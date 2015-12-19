#ifndef TEST_JXMLTABLE_H
#define TEST_JXMLTABLE_H

#include <QDialog>
#include "jfiltertableview.h"
#include "jsplitter.h"

// - class TestJXmlTable -

class TestJXmlTable : public QDialog
{
    Q_OBJECT
public:
    explicit TestJXmlTable(QWidget *parent = 0);

public Q_SLOTS:
    void buttonTable1Clicked();
    void buttonTable2Clicked();
    void onReadOnlyChanged(bool value);

private:
    JFilterTableView *q_filterTable;
    JSplitter *q_horiSplitter;
    QWidget *q_areaRight;
};

#endif // TEST_JXMLTABLE_H

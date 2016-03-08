#ifndef TESTSTYLESHEET_H
#define TESTSTYLESHEET_H

#include <QDialog>

namespace Ui {
    class TestStyleSheet;
}

class TestStyleSheet : public QDialog
{
    Q_OBJECT
public:
    explicit TestStyleSheet(QWidget *parent = 0);
    ~TestStyleSheet();

Q_SIGNALS:

public Q_SLOTS:

private:
    Ui::TestStyleSheet *ui;
};

#endif // TESTSTYLESHEET_H

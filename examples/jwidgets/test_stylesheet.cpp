#include <QtGui>
#if QT_VERSION >= 0x050000
#include <QtWidgets>
#endif
#include "test_stylesheet.h"
#include "ui_test_stylesheet.h"

TestStyleSheet::TestStyleSheet(QWidget *parent) :
    QDialog(parent, Qt::WindowMinMaxButtonsHint | Qt::WindowCloseButtonHint),
    ui(new Ui::TestStyleSheet)
{
    ui->setupUi(this);

    resize(1200, 800);
}

TestStyleSheet::~TestStyleSheet()
{
    delete ui;
}

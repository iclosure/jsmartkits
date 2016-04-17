/********************************************************************************
** Form generated from reading UI file 'test_stylesheet.ui'
**
** Created by: Qt User Interface Compiler version 4.8.6
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_TEST_STYLESHEET_H
#define UI_TEST_STYLESHEET_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QHeaderView>
#include <QtGui/QLineEdit>
#include <QtGui/QTableWidget>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_TestStyleSheet
{
public:
    QLineEdit *lineEdit;
    QLineEdit *lineEdit_2;
    QTableWidget *tableWidget;

    void setupUi(QWidget *TestStyleSheet)
    {
        if (TestStyleSheet->objectName().isEmpty())
            TestStyleSheet->setObjectName(QString::fromUtf8("TestStyleSheet"));
        TestStyleSheet->resize(692, 458);
        lineEdit = new QLineEdit(TestStyleSheet);
        lineEdit->setObjectName(QString::fromUtf8("lineEdit"));
        lineEdit->setGeometry(QRect(30, 20, 141, 51));
        lineEdit_2 = new QLineEdit(TestStyleSheet);
        lineEdit_2->setObjectName(QString::fromUtf8("lineEdit_2"));
        lineEdit_2->setEnabled(false);
        lineEdit_2->setGeometry(QRect(30, 150, 113, 21));
        tableWidget = new QTableWidget(TestStyleSheet);
        tableWidget->setObjectName(QString::fromUtf8("tableWidget"));
        tableWidget->setGeometry(QRect(220, 100, 461, 321));

        retranslateUi(TestStyleSheet);

        QMetaObject::connectSlotsByName(TestStyleSheet);
    } // setupUi

    void retranslateUi(QWidget *TestStyleSheet)
    {
        TestStyleSheet->setWindowTitle(QApplication::translate("TestStyleSheet", "Form", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class TestStyleSheet: public Ui_TestStyleSheet {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_TEST_STYLESHEET_H

/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.9.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QDoubleSpinBox>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QWidget>
#include <qcustomplot.h>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralWidget;
    QCustomPlot *plot;
    QLabel *label;
    QLabel *label_2;
    QDoubleSpinBox *bx_x;
    QDoubleSpinBox *bx_y;
    QPushButton *btn_add;
    QPushButton *ptn_clear;
    QPushButton *btn_stop;
    QPushButton *btn_start;
    QMenuBar *menuBar;
    QToolBar *mainToolBar;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QStringLiteral("MainWindow"));
        MainWindow->resize(1159, 625);
        centralWidget = new QWidget(MainWindow);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        plot = new QCustomPlot(centralWidget);
        plot->setObjectName(QStringLiteral("plot"));
        plot->setGeometry(QRect(50, 30, 1071, 471));
        label = new QLabel(centralWidget);
        label->setObjectName(QStringLiteral("label"));
        label->setGeometry(QRect(350, 530, 31, 16));
        label_2 = new QLabel(centralWidget);
        label_2->setObjectName(QStringLiteral("label_2"));
        label_2->setGeometry(QRect(580, 530, 59, 14));
        bx_x = new QDoubleSpinBox(centralWidget);
        bx_x->setObjectName(QStringLiteral("bx_x"));
        bx_x->setGeometry(QRect(400, 530, 66, 23));
        bx_y = new QDoubleSpinBox(centralWidget);
        bx_y->setObjectName(QStringLiteral("bx_y"));
        bx_y->setGeometry(QRect(620, 530, 66, 23));
        btn_add = new QPushButton(centralWidget);
        btn_add->setObjectName(QStringLiteral("btn_add"));
        btn_add->setGeometry(QRect(800, 530, 80, 22));
        ptn_clear = new QPushButton(centralWidget);
        ptn_clear->setObjectName(QStringLiteral("ptn_clear"));
        ptn_clear->setGeometry(QRect(960, 530, 80, 22));
        btn_stop = new QPushButton(centralWidget);
        btn_stop->setObjectName(QStringLiteral("btn_stop"));
        btn_stop->setGeometry(QRect(210, 530, 80, 22));
        btn_start = new QPushButton(centralWidget);
        btn_start->setObjectName(QStringLiteral("btn_start"));
        btn_start->setGeometry(QRect(90, 530, 80, 22));
        MainWindow->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(MainWindow);
        menuBar->setObjectName(QStringLiteral("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 1159, 22));
        MainWindow->setMenuBar(menuBar);
        mainToolBar = new QToolBar(MainWindow);
        mainToolBar->setObjectName(QStringLiteral("mainToolBar"));
        MainWindow->addToolBar(Qt::TopToolBarArea, mainToolBar);
        statusBar = new QStatusBar(MainWindow);
        statusBar->setObjectName(QStringLiteral("statusBar"));
        MainWindow->setStatusBar(statusBar);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QApplication::translate("MainWindow", "MainWindow", Q_NULLPTR));
        label->setText(QApplication::translate("MainWindow", "x", Q_NULLPTR));
        label_2->setText(QApplication::translate("MainWindow", "y", Q_NULLPTR));
        btn_add->setText(QApplication::translate("MainWindow", "add", Q_NULLPTR));
        ptn_clear->setText(QApplication::translate("MainWindow", "clear", Q_NULLPTR));
        btn_stop->setText(QApplication::translate("MainWindow", "Stop", Q_NULLPTR));
        btn_start->setText(QApplication::translate("MainWindow", "Start", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H

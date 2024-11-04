#include <QVBoxLayout>
#include <QPushButton>
#include <QTreeWidget>
#include <QDockWidget>
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "primitiveMgrWindow.h"
#include "pdkMgrWindow.h"
#include "layoutView.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    qq::LayoutView::init({"Python"}, true);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_primitive_clicked()
{
    pr::PrimitiveMgrWindow *popupWindow = new pr::PrimitiveMgrWindow(this);
    popupWindow->show();
}


void MainWindow::on_pushButton_pdk_clicked()
{
    pdk::PdkMgrWindow *pdkMgrWindow = new pdk::PdkMgrWindow(this);
    pdkMgrWindow->show();
}


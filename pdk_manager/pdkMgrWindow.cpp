#include <QDockWidget>
#include <QIcon>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QToolBar>
#include <QAction>
#include <QToolButton>
#include <QDir>
#include "pdkMgrWindow.h"
#include "pdkDefines.h"
#include "componentCenterWidget.h"
#include "pdkLayerMgrWidget.h"
#include "drcMgrWidget.h"
#include "pdkInstanceMgr.h"

namespace pdk
{

PdkMgrWindow::PdkMgrWindow(QWidget *parent)
    : QMainWindow{parent}
{
    setup_ui();
    load_dir_files();
    set_connect();
}

PdkMgrWindow::~PdkMgrWindow()
{

}

void PdkMgrWindow::setup_ui()
{
    // 创建并设置 Dock Widget 的样式
    QString dock_widget_style = "QDockWidget {"
                                "    border: 2px solid #5F9EA0;"  // 边框颜色
                                "    border-radius: 10px;"         // 圆角半径
                                "    background-color: #E0FFFF;"   // 背景颜色
                                "}"
                                "QDockWidget::title {"
                                "    text-align: center;"          // 标题文本居中
                                "    background-color: #000000;"   // 标题栏背景色
                                "    padding: 5px;"                // 标题栏内边距
                                "    border-top-left-radius: 8px;" // 左上角圆角
                                "    border-top-right-radius: 8px;"// 右上角圆角
                                // "    border-bottom-left-radius: 8px;" // 左上角圆角
                                // "    border-bottom-right-radius: 8px;"// 右上角圆角
                                "}";
    QDockWidget* dock_pdk = new QDockWidget(tr("PDK"), this);
    QWidget *dock_pdk_widget = new QWidget(this);
    QVBoxLayout *dock_pdk_layout = new QVBoxLayout(dock_pdk_widget);

    // 创建工具栏和按钮

    // QToolBar *toolBar = new QToolBar(dock_pdk_widget);
    // QAction *iconButton = toolBar->addAction(QIcon(":/img/add.png"), "");

    QToolButton *iconButton = new QToolButton();
    iconButton->setIcon(QIcon(":/img/add.png"));

    // 添加一个空的 spacer，右对齐按钮
    QHBoxLayout *hLayout = new QHBoxLayout(dock_pdk_widget);
    hLayout->addSpacerItem(new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum));
    hLayout->addWidget(iconButton);
    dock_pdk_layout->addLayout(hLayout);
    dock_pdk->setStyleSheet(dock_widget_style);
    mp_pdk_list_widget = new QListWidget(this);
    dock_pdk_layout->addWidget(mp_pdk_list_widget);
    dock_pdk->setWidget(dock_pdk_widget);
    dock_pdk->setFixedWidth(320);
    addDockWidget(Qt::LeftDockWidgetArea, dock_pdk);

    // 创建左侧下方框（Dock Widget）
    mp_component_tree_widget = new QTreeWidget(this);
    QDockWidget* dock_component = new QDockWidget(tr(PDK_COMPONENT_NAME), this);
    dock_component->setObjectName(PDK_COMPONENT_NAME);
    dock_component->setStyleSheet(dock_widget_style);
    dock_component->setWidget(mp_component_tree_widget);
    dock_component->setFixedWidth(320);
    addDockWidget(Qt::LeftDockWidgetArea, dock_component);

    // 将第二个框放置在第一个框下面
    splitDockWidget(dock_pdk, dock_component, Qt::Vertical);

    // 创建第一个 Dock Widget
    QDockWidget* dock_layer = new QDockWidget(tr(PDK_PRI_MGR_NAME), this);
    dock_layer->setStyleSheet(dock_widget_style);
    mp_pdk_layer_mgr_widget = new PdkLayerMgrWidget(this);
    dock_layer->setWidget(mp_pdk_layer_mgr_widget);
    addDockWidget(Qt::RightDockWidgetArea, dock_layer);

    // 创建第二个 Dock Widget
    QDockWidget* dock_drc = new QDockWidget(tr(PDK_DRC_MGR_NAME), this);
    dock_drc->setStyleSheet(dock_widget_style);
    mp_drc_mgr_widget = new DrcMgrWidget(this);
    dock_drc->setWidget(mp_drc_mgr_widget);
    // dock_drc->setFixedWidth(300);
    // addDockWidget(Qt::RightDockWidgetArea, dock_drc);
    splitDockWidget(dock_layer, dock_drc, Qt::Vertical);
    // 调整大小，使两个 dock 高度各占一半
    QList<int> sizes;
    sizes << height() / 2 << height() / 2;  // 各占一半的高度
    resizeDocks({dock_layer, dock_drc}, sizes, Qt::Vertical);

    // 设置中央小部件
    mp_center = new ComponentCenterWidget(this);
    setCentralWidget(mp_center);

    // 设置主窗口标题
    setWindowTitle(tr(PDK_TITLE_WINDOW_NAME));
    setWindowIcon(QIcon(":/img/pdk_logo.png"));
    setStyleSheet("background-color: #2E2E2E; color: white;");
    // 最大化主窗口
    showMaximized();
    setWindowModality(Qt::ApplicationModal);
}

void PdkMgrWindow::load_dir_files()
{
    QString current_path = QDir::currentPath() + "/data/pdks";
    QDir current_dir(current_path);
    if (!current_dir.exists())
    {
        bool ret = current_dir.mkpath(current_path);
        if (!ret)
        {
            exit(1);
        }
    }

    // 获取当前目录下所有子目录
    QStringList dirstr_list = current_dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
    if (dirstr_list.isEmpty())
    {
        QString pdk_path = current_path + "/pdk1";
        current_dir.mkpath(pdk_path);
        dirstr_list = current_dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
    }

    for (const QString &dir_name : dirstr_list)
    {
        mp_pdk_list_widget->addItem(dir_name);
        // PdkInstanceMgr::instance()->init(current_path + "/" + dir_name);
    }

}

void PdkMgrWindow::set_connect()
{
    connect(mp_pdk_list_widget, &QListWidget::itemClicked, this, &PdkMgrWindow::load_component_tree);
}

void PdkMgrWindow::load_component_tree(QListWidgetItem *item)
{
    auto text = item->text();

    QString current_path = QDir::currentPath() + "/data/pdks" + "/" + text;
    QDir current_dir(current_path);
    if (!current_dir.exists())
    {
        exit(1);
    }

    // 获取当前目录下所有子目录
    QStringList dirstr_list = current_dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
    if (dirstr_list.isEmpty())
    {
        QString pcell1_path = current_path + "/librarys/lib1/pcell1";
        current_dir.mkpath(pcell1_path);
        // 创建 lib1 节点
        QTreeWidgetItem *lib1Item = new QTreeWidgetItem(mp_component_tree_widget);
        lib1Item->setText(0, "lib1");

        // 创建 pcell1 节点并添加到 lib1 节点下
        QTreeWidgetItem *pcell1Item = new QTreeWidgetItem(lib1Item);
        pcell1Item->setText(0, "pcell1");

        QString drc_path = current_path + "/drc";
        current_dir.mkpath(drc_path);
    }

    QString libs_path = current_path + "/librarys";
    QDir libs_dir(libs_path);
    QStringList libs_list = libs_dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
    if (!libs_list.isEmpty())
    {
        for (auto lib_name : libs_list)
        {
            // 创建 lib1 节点
            QTreeWidgetItem *lib1Item = new QTreeWidgetItem(mp_component_tree_widget);
            lib1Item->setText(0, lib_name);
            QString current_lib_path = libs_path + "/" + lib_name;
            QDir current_lib_dir(current_lib_path);
            QStringList pcells_list = current_lib_dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
            for (auto pcell_name : pcells_list)
            {
                QTreeWidgetItem *pcell1Item = new QTreeWidgetItem(lib1Item);
                pcell1Item->setText(0, pcell_name);
            }
        }

    }
    mp_component_tree_widget->expandAll();

}

void PdkMgrWindow::fill_data()
{

}


}

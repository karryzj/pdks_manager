#include <QDockWidget>
#include <QIcon>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QToolBar>
#include <QAction>
#include <QToolButton>
#include <QDir>
#include <QMenu>
#include <QInputDialog>
#include <QJsonDocument>
#include "pdkMgrWindow.h"
#include "pdkDefines.h"
#include "componentCenterWidget.h"
#include "pdkLayerMgrWidget.h"
#include "drcMgrWidget.h"
#include "pdkInstanceMgr.h"
#include "priModeWidget.h"
#include "pdkInstance.h"
#include "jsonPaser.h"
#include "gdsModeWidget.h"

namespace pdk
{

PdkMgrWindow::PdkMgrWindow(QWidget *parent)
    : QMainWindow(parent), mp_pdk_instance(new PdkInstance(""))
{
    setup_ui();
    load_dir_files();
    set_connect();
}

PdkMgrWindow::~PdkMgrWindow()
{

    set_disconnect();
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

    QToolButton *iconButton = new QToolButton();
    iconButton->setIcon(QIcon(":/img/add.png"));

    // PDK
    QHBoxLayout *hLayout = new QHBoxLayout(dock_pdk_widget);
    hLayout->addSpacerItem(new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum));
    hLayout->addWidget(iconButton);
    dock_pdk_layout->addLayout(hLayout);
    dock_pdk->setStyleSheet(dock_widget_style);
    mp_pdk_list_widget = new QListWidget(this);
    mp_pdk_list_widget->setContextMenuPolicy(Qt::CustomContextMenu);
    dock_pdk_layout->addWidget(mp_pdk_list_widget);
    dock_pdk->setWidget(dock_pdk_widget);
    dock_pdk->setFixedWidth(320);
    addDockWidget(Qt::LeftDockWidgetArea, dock_pdk);

    // 元件
    mp_component_tree_widget = new QTreeWidget(this);
    mp_component_tree_widget->setContextMenuPolicy(Qt::CustomContextMenu);
    mp_component_tree_widget->setHeaderHidden(true);
    QDockWidget* dock_component = new QDockWidget(tr(PDK_COMPONENT_NAME), this);
    dock_component->setObjectName(PDK_COMPONENT_NAME);
    dock_component->setStyleSheet(dock_widget_style);
    dock_component->setWidget(mp_component_tree_widget);
    dock_component->setFixedWidth(320);
    addDockWidget(Qt::LeftDockWidgetArea, dock_component);

    // 将第二个框放置在第一个框下面
    splitDockWidget(dock_pdk, dock_component, Qt::Vertical);
    QList<QDockWidget*> dock_list;
    dock_list << dock_pdk << dock_component;
    QList<int> dock_size;
    dock_size << 1 << 2;
    resizeDocks(dock_list, dock_size, Qt::Vertical);

    // gds图层管理
    QDockWidget* dock_layer = new QDockWidget(tr(PDK_PRI_MGR_NAME), this);
    dock_layer->setStyleSheet(dock_widget_style);
    mp_pdk_layer_mgr_widget = new PdkLayerMgrWidget(mp_pdk_instance, this);
    dock_layer->setWidget(mp_pdk_layer_mgr_widget);
    dock_layer->setFixedWidth(450);
    addDockWidget(Qt::RightDockWidgetArea, dock_layer);

    // drc管理
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
    mp_center = new ComponentCenterWidget(mp_pdk_instance);
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
    connect(mp_pdk_list_widget, &QListWidget::customContextMenuRequested, this, &PdkMgrWindow::show_list_menu);
    connect(mp_component_tree_widget, &QTreeWidget::itemClicked, this, &PdkMgrWindow::load_cell_data);
    connect(mp_component_tree_widget, &QTreeWidget::customContextMenuRequested, this, &PdkMgrWindow::show_context_menu);
}

void PdkMgrWindow::set_disconnect()
{
    disconnect(mp_pdk_list_widget, &QListWidget::itemClicked, this, &PdkMgrWindow::load_component_tree);
    disconnect(mp_pdk_list_widget, &QListWidget::customContextMenuRequested, this, &PdkMgrWindow::show_list_menu);
    disconnect(mp_component_tree_widget, &QTreeWidget::itemClicked, this, &PdkMgrWindow::load_cell_data);
    disconnect(mp_component_tree_widget, &QTreeWidget::customContextMenuRequested, this, &PdkMgrWindow::show_context_menu);
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

    mp_pdk_instance->set_pdk_path(current_path);

    mp_component_tree_widget->clear();
    // 获取当前目录下所有子目录
    QStringList dirstr_list = current_dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
    if (dirstr_list.isEmpty())
    {
        // 获取pcell
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

    // 解析layer.json
    QStringList file_list = current_dir.entryList(QDir::Files | QDir::NoDotAndDotDot);
    QString layer_json("layer.json");
    if (file_list.contains(layer_json))
    {
        mp_pdk_instance->reset_gds_layers();
        QString filePath = current_path + "/" + layer_json;
        JsonPaser parser;
        QJsonObject rootObj = parser.parse_json_root(filePath);
        parser.load_gds_layers_from_root(rootObj, mp_pdk_instance->pdk_gds_layers());
        mp_pdk_layer_mgr_widget->fill_data();
    }

    // 获取drc
    QString drc_path = current_path + "/drc";
    QDir drc_dir(drc_path);
    QStringList drc_file_list = drc_dir.entryList(QDir::Files | QDir::NoDotAndDotDot);
    if (!drc_file_list.isEmpty())
    {
        mp_drc_mgr_widget->set_drc_list(drc_file_list);
        mp_drc_mgr_widget->fill_data();
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
            if (!pcells_list.isEmpty())
            {
                for (auto pcell_name : pcells_list)
                {
                    QTreeWidgetItem *pcell1Item = new QTreeWidgetItem(lib1Item);
                    pcell1Item->setText(0, pcell_name);
                }
            }
        }
    }
    mp_component_tree_widget->expandAll();

}

void PdkMgrWindow::load_cell_data(QTreeWidgetItem *item, int column)
{
    QTreeWidgetItem *selectedItem = mp_component_tree_widget->currentItem();
    if (selectedItem)
    {
        QTreeWidgetItem *parent = selectedItem->parent();
        if (!parent)
        {
            return;
        }
        else
        {
            QString json_file_name = "data.json";
            QString py_file_name = "info.json";
            QString extra_name = "extra.json";
            QString gds_name = "data.gds";
            QString pdk_name = mp_pdk_list_widget->currentItem()->text();
            QString lib_name = parent->text(column);
            QString cell_name = item->text(column);

            QString cell_path = QDir::currentPath() + "/data/pdks/" + pdk_name + "/librarys/" + lib_name + "/" + cell_name;
            mp_pdk_instance->set_path(cell_path);
            QDir cell_dir(cell_path);
            QStringList files_list = cell_dir.entryList(QDir::Files);
            if (!files_list.isEmpty())
            {
                if (files_list.contains(extra_name))
                {
                    mp_center->get_pri_mode_widget()->set_disable_primitive_relation(false);
                    mp_center->set_switch_mode(0);
                    mp_center->set_switch_enable(false);
                    QString extra_file = cell_path + "/" + extra_name;
                    mp_pdk_instance->reset_data();
                    mp_pdk_instance->load_extra_file(extra_file);
                    if (files_list.contains(json_file_name))
                    {
                        QString pcell_file = cell_path + "/" + json_file_name;
                        mp_pdk_instance->load_pcell_file(pcell_file);
                    }
                    else if (files_list.contains(py_file_name))
                    {
                        QString py_file = cell_path + "/" + py_file_name;
                        mp_pdk_instance->load_py_json_file(py_file);
                    }
                    mp_pdk_instance->init_total_param_mgr();
                    mp_center->get_pri_mode_widget()->fillUiData();
                }
                else if (files_list.contains(json_file_name))
                {
                    mp_center->get_pri_mode_widget()->set_disable_primitive_relation(false);
                    mp_center->set_switch_mode(0);
                    mp_center->set_switch_enable(false);
                    QString pcell_file = cell_path + "/" + json_file_name;
                    mp_pdk_instance->reset_data();
                    mp_pdk_instance->load_pcell_file(pcell_file);
                    mp_pdk_instance->init_extra_param_mgr();
                    mp_pdk_instance->init_total_param_mgr();
                    mp_center->get_pri_mode_widget()->fillUiData();
                }
                else if (files_list.contains(py_file_name))
                {
                    mp_center->get_pri_mode_widget()->set_disable_primitive_relation(false);
                    mp_center->set_switch_mode(0);
                    mp_center->set_switch_enable(false);
                    QString pcell_file = cell_path + "/" + py_file_name;
                    mp_pdk_instance->reset_data();
                    mp_pdk_instance->load_py_json_file(pcell_file);
                    mp_pdk_instance->init_extra_param_mgr();
                    mp_pdk_instance->init_total_param_mgr();
                    mp_center->get_pri_mode_widget()->fillUiData();
                }
                else if (files_list.contains(gds_name))
                {
                    mp_center->get_pri_mode_widget()->set_disable_primitive_relation(false);
                    mp_center->set_switch_mode(1);
                    mp_center->set_switch_enable(false);
                    mp_center->get_gds_mode_widget()->fill_data();
                }
            }
            else
            {
                // mp_pdk_instance->reset_data();
                mp_center->get_pri_mode_widget()->clear_view();
                mp_center->get_pri_mode_widget()->set_disable_primitive_relation(true);
                mp_center->set_switch_mode(0);
                mp_center->set_switch_enable(true);
                mp_center->get_gds_mode_widget()->clear_view();
            }
        }
    }

}

void PdkMgrWindow::show_list_menu(const QPoint &pos)
{
    QListWidgetItem *item = mp_pdk_list_widget->itemAt(pos);
    if (!item)
    {
        QMenu contextMenu(tr("Context menu"), this);
        QAction actionAddCell(tr("新建PDK"), this);
        connect(&actionAddCell, &QAction::triggered, this, &PdkMgrWindow::add_pdk);
        contextMenu.addAction(&actionAddCell);
        contextMenu.exec(mp_pdk_list_widget->mapToGlobal(pos));
    }
    else
    {
        QMenu contextMenu(tr("Context menu"), this);

        QAction action_del_pdk(tr("删除PDK"), this);
        connect(&action_del_pdk, &QAction::triggered, [this, item]()
        {
            delete_pdk(item);
        });
        contextMenu.addAction(&action_del_pdk);
        contextMenu.exec(mp_pdk_list_widget->mapToGlobal(pos));

    }
}

void PdkMgrWindow::fill_data()
{

}

void PdkMgrWindow::show_context_menu(const QPoint &pos)
{
    QTreeWidgetItem *item = mp_component_tree_widget->itemAt(pos);
    if (!item)
    {
        QMenu contextMenu(tr("Context menu"), this);
        QAction actionAddCell(tr("新建元件分类"), this);
        connect(&actionAddCell, &QAction::triggered, this, &PdkMgrWindow::add_lib);
        contextMenu.addAction(&actionAddCell);
        contextMenu.exec(mp_component_tree_widget->mapToGlobal(pos));
    }
    else
    {
        if (item->parent() == nullptr)
        {
            QMenu contextMenu(tr("Context menu"), this);

            QAction action_add_file(tr("新建元件"), this);
            connect(&action_add_file, &QAction::triggered, [this, item]()
            {
                add_pcell(item);
            });
            contextMenu.addAction(&action_add_file);

            QAction action_del_cute(tr("删除元件分类"), this);
            connect(&action_del_cute, &QAction::triggered, [this, item]()
            {
                delete_lib(item);
            });
            contextMenu.addAction(&action_del_cute);
            contextMenu.exec(mp_component_tree_widget->mapToGlobal(pos));
        }
        else
        {
            QMenu contextMenu(tr("Context menu"), this);

            QAction action_del_tuyuan(tr("删除元件"), this);
            connect(&action_del_tuyuan, &QAction::triggered, [this, item]()
            {
                delete_pcell(item);
            });
            contextMenu.addAction(&action_del_tuyuan);
            contextMenu.exec(mp_component_tree_widget->mapToGlobal(pos));
        }
    }
}

void PdkMgrWindow::add_pdk()
{
    bool ok = false;
    QString text = QInputDialog::getText(mp_pdk_list_widget, tr("添加PDK"), tr("PDK名称:"), QLineEdit::Normal, "", &ok);
    if (ok && !text.isEmpty())
    {
        QString pdks_path = QDir::currentPath() + "/data/pdks";
        QDir pdks_dir(pdks_path);
        QString pdk_path = pdks_path + "/" + text;
        QDir pdk_dir(pdk_path);
        if (pdks_dir.exists() && !pdk_dir.exists() )
        {
            pdks_dir.mkpath(text);
            new QListWidgetItem(text, mp_pdk_list_widget);
        }
    }
}

void PdkMgrWindow::delete_pdk(QListWidgetItem *item)
{
    QString dirPath = QDir::currentPath() + "/data/pdks/" + item->text();
    QDir dir(dirPath);

    if (dir.exists())
    {
        dir.removeRecursively();  // 删除目录及其内容
        delete item;  // 从 QTreeWidget 中删除该项
    }
}

void PdkMgrWindow::add_lib()
{
    bool ok = false;
    QString text = QInputDialog::getText(mp_component_tree_widget, tr("添加元件分类"), tr("元件类名:"), QLineEdit::Normal, "", &ok);
    if (ok && !text.isEmpty())
    {
        QString pdk_name = mp_pdk_list_widget->selectedItems().at(0)->text();
        QString libs_path = QDir::currentPath() + "/data/pdks/" + pdk_name + "/librarys";
        QDir libs_dir(libs_path);
        QString lib_path = libs_path + "/" + text;
        QDir lib_dir(lib_path);
        if (libs_dir.exists() && !lib_dir.exists() )
        {
            libs_dir.mkpath(text);
            auto item = new QTreeWidgetItem();
            item->setText(0, text);
            mp_component_tree_widget->addTopLevelItem(item);
        }
    }
}

void PdkMgrWindow::delete_lib(QTreeWidgetItem *item)
{
    QString pdk_name = mp_pdk_list_widget->selectedItems().at(0)->text();
    QString dirPath = QDir::currentPath() + "/data/pdks/" + pdk_name +"/librarys/"+ item->text(0);
    QDir dir(dirPath);

    if (dir.exists())
    {
        dir.removeRecursively();  // 删除目录及其内容
        delete item;  // 从 QTreeWidget 中删除该项
    }
}

void PdkMgrWindow::add_pcell(QTreeWidgetItem *item)
{
    bool ok = false;
    QString text = QInputDialog::getText(mp_component_tree_widget, tr("添加元件"), tr("元件名:"), QLineEdit::Normal, "", &ok);
    if (ok && !text.isEmpty())
    {
        QString pdk_name = mp_pdk_list_widget->selectedItems().at(0)->text();
        QString lib_path = QDir::currentPath() + "/data/pdks/" + pdk_name + "/librarys/" + item->text(0);
        QDir lib_dir(lib_path);
        QString pcell_path = lib_path + "/" + text;
        QDir pcell_dir(pcell_path);
        if (lib_dir.exists() && !pcell_dir.exists() )
        {
            lib_dir.mkpath(text);
            auto pcell_item = new QTreeWidgetItem();
            pcell_item->setText(0, text);
            item->addChild(pcell_item);
        }
    }
}

void PdkMgrWindow::delete_pcell(QTreeWidgetItem *item)
{
    if (!item)
        return;
    QTreeWidgetItem *parent = item->parent();
    if (!parent)
        return;

    QString pdk_name = mp_pdk_list_widget->selectedItems().at(0)->text();
    QString dirPath = QDir::currentPath() + "/data/pdks/" + pdk_name +"/librarys/"+ parent->text(0) +"/"+ item->text(0);
    QDir dir(dirPath);

    if (dir.exists())
    {
        dir.removeRecursively();  // 删除目录及其内容
        delete item;  // 从 QTreeWidget 中删除该项
    }
}

}

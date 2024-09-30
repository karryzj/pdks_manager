#include <QVBoxLayout>
#include <QDockWidget>
#include <QHeaderView>
#include <QStatusBar>
#include <QMenu>
#include <QAction>
#include <QDir>
#include <QFileDialog>
#include <QApplication>
#include <QDesktopWidget>
#include <QScreen>
#include "layerMgr.h"
#include "layInfo.h"
#include "primitive.h"
#include "primitiveMgr.h"
#include "primitiveMgrWindow.h"
#include "primitiveWindow.h"
#include "priEditNameDialog.h"
#include "layerDefines.h"
#include "attachTreeUtils.h"
#include "pointE.h"

namespace pr
{
PrimitiveMgrWindow::PrimitiveMgrWindow(QWidget *parent)
    : QMainWindow{parent}, mp_pri_tree_widget(new QTreeWidget()), mp_label(new QLabel()), mp_layer_list(new QListWidget()),
      mp_params_table(new QTableWidget(0, 3)), mp_rules_table(new QTableWidget(0, 1)), mp_anchor_table(new QTableWidget(0, 2)),
      mp_current_pimitive(nullptr)
{
    setup_ui();
    load_dir_files();
}

void PrimitiveMgrWindow::setup_ui()
{
    // 创建并设置 Dock Widget 的样式
    QString dock_widget_style = "QDockWidget {"
                                "    border: 2px solid #5F9EA0;"  // 边框颜色
                                "    border-radius: 10px;"         // 圆角半径
                                "    background-color: #E0FFFF;"   // 背景颜色
                                "}"
                                "QDockWidget::title {"
                                "    text-align: center;"          // 标题文本居中
                                "    background-color: #5F9EA0;"   // 标题栏背景色
                                "    padding: 5px;"                // 标题栏内边距
                                "    border-top-left-radius: 8px;" // 左上角圆角
                                "    border-top-right-radius: 8px;"// 右上角圆角
                                "}";
    QDockWidget* dock_widget_left = new QDockWidget(this);
    dock_widget_left->setStyleSheet(dock_widget_style);
    dock_widget_left->setFixedWidth(400);

    mp_pri_tree_widget->setHeaderHidden(true);
    mp_pri_tree_widget->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(mp_pri_tree_widget, &QTreeWidget::customContextMenuRequested, this, &PrimitiveMgrWindow::show_context_menu);
    // 连接 QTreeWidget 的 itemSelectionChanged 信号到显示路径的槽函数
    // connect(mp_pri_tree_widget, &QTreeWidget::itemSelectionChanged, this, &PrimitiveMgrWindow::changed_primitive);
    connect(mp_pri_tree_widget, &QTreeWidget::itemClicked, this, &PrimitiveMgrWindow::load_primitive);
    connect(mp_pri_tree_widget, &QTreeWidget::itemDoubleClicked, this, &PrimitiveMgrWindow::change_name);

    dock_widget_left->setWidget(mp_pri_tree_widget);
    addDockWidget(Qt::LeftDockWidgetArea, dock_widget_left);

    // 创建主窗口的中心部件
    QLabel *centralWidget = new QLabel(this);

    // 创建状态栏
    QStatusBar* statusBar = new QStatusBar();
    setStatusBar(statusBar);

    // // 加载图像
    // QImage image;
    // // 生成缩略图
    // QImage thumbnail = image.scaled(100, 100, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    // // 将缩略图显示在 QLabel 中

    // mp_label->setPixmap(QPixmap::fromImage(thumbnail));
    QPushButton *edit_button = new QPushButton(tr("编辑"), this);
    QHBoxLayout *h1 = new QHBoxLayout();
    h1->addWidget(mp_label);
    h1->addWidget(edit_button);
    QWidget *thumbnail_widget = new QWidget();
    thumbnail_widget->setLayout(h1);

    // 创建第一个 Dock Widget
    QDockWidget* thumbnail_dock = new QDockWidget(this);
    thumbnail_dock->setStyleSheet(dock_widget_style);
    thumbnail_dock->setWidget(thumbnail_widget);
    thumbnail_dock->setFixedWidth(400);
    addDockWidget(Qt::RightDockWidgetArea, thumbnail_dock);

    // 创建第二个 Dock Widget
    QDockWidget* layers_dock = new QDockWidget(tr("使用图层"), this);
    layers_dock->setStyleSheet(dock_widget_style);
    layers_dock->setWidget(mp_layer_list);
    addDockWidget(Qt::RightDockWidgetArea, layers_dock);

    // 显示参数列表
    mp_params_table->setHorizontalHeaderLabels({tr("名称"), tr("默认值"), tr("描述")});
    mp_params_table->setSelectionBehavior(QAbstractItemView::SelectRows);
    mp_params_table->setSelectionMode(QAbstractItemView::SingleSelection);
    mp_params_table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    mp_params_table->horizontalHeader()->setStretchLastSection(true);
    mp_params_table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    mp_params_table->horizontalHeader()->setVisible(true);

    // 创建第三个 Dock Widget
    QDockWidget* params_dock = new QDockWidget(tr("参数列表"), this);
    params_dock->setStyleSheet(dock_widget_style);
    params_dock->setWidget(mp_params_table);
    addDockWidget(Qt::RightDockWidgetArea, params_dock);

    // 显示约束列表
    mp_rules_table->setHorizontalHeaderLabels({tr("参数约束名称")});
    mp_rules_table->setSelectionBehavior(QAbstractItemView::SelectRows);
    mp_rules_table->setSelectionMode(QAbstractItemView::SingleSelection);
    mp_rules_table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    mp_rules_table->horizontalHeader()->setStretchLastSection(true);
    mp_rules_table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);


    // 创建第四个 Dock Widget
    QDockWidget* rules_dock = new QDockWidget(tr("参数约束"), this);
    rules_dock->setStyleSheet(dock_widget_style);
    rules_dock->setWidget(mp_rules_table);
    addDockWidget(Qt::RightDockWidgetArea, rules_dock);

    // 显示锚点
    mp_anchor_table->setHorizontalHeaderLabels({tr("X轴坐标"), tr("Y轴坐标")});
    mp_anchor_table->setSelectionBehavior(QAbstractItemView::SelectRows);
    mp_anchor_table->setSelectionMode(QAbstractItemView::SingleSelection);
    mp_anchor_table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    mp_anchor_table->horizontalHeader()->setStretchLastSection(true);
    mp_anchor_table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    mp_anchor_table->horizontalHeader()->setVisible(true);

    // 创建第三个 Dock Widget
    QDockWidget* anchor_dock = new QDockWidget(tr("锚点"), this);
    anchor_dock->setStyleSheet(dock_widget_style);
    anchor_dock->setWidget(mp_anchor_table);
    addDockWidget(Qt::RightDockWidgetArea, anchor_dock);

    setCentralWidget(centralWidget);

    // 连接按钮点击事件到打开弹出窗口的槽函数
    connect(edit_button, &QPushButton::clicked, this, &PrimitiveMgrWindow::edit_primitive);

    // 设置主窗口的图标
    setWindowIcon(QIcon(":/img/favicon.png"));
    setWindowFlags(Qt::Window | Qt::WindowMinimizeButtonHint | Qt::WindowCloseButtonHint);

    setFixedSize(900, 900);
    // 获取屏幕的分辨率
    QRect screenGeometry = QApplication::primaryScreen()->geometry();
    int x = (screenGeometry.width() - width()) / 2;
    int y = (screenGeometry.height() - height()) / 2;
    move(x, y);

}

void PrimitiveMgrWindow::load_dir_files()
{
    QString current_path = QDir::currentPath() + "/data/primitives";
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
        QString tuyuan1_path = current_path + "/cate1/tuyuan1";
        current_dir.mkpath(tuyuan1_path);
        dirstr_list = current_dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
    }

    // 遍历目录名，寻找最大编号
    for (const QString &dir_name : dirstr_list)
    {
        // 在 QTreeWidget 中添加新单元格
        QTreeWidgetItem *new_item = new QTreeWidgetItem(mp_pri_tree_widget);
        new_item->setText(0, dir_name);
        mp_pri_tree_widget->addTopLevelItem(new_item);
        PrimitiveMgr::instance()->init(current_path + "/" + dir_name);

        // 获取当前目录下所有子目录
        QDir sub_dir(current_dir.absoluteFilePath(dir_name));
        QStringList sub_dirs_list = sub_dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
        for (const auto &sub_dir_str : sub_dirs_list)
        {
            // 将文件名作为子节点添加到目录节点
            QTreeWidgetItem *dir_item = new QTreeWidgetItem();
            dir_item->setText(0, sub_dir_str);
            new_item->addChild(dir_item);
        }
        new_item->setExpanded(true);
    }
}

void PrimitiveMgrWindow::edit_primitive()
{
    QTreeWidgetItem *selectedItem = mp_pri_tree_widget->currentItem();
    if (selectedItem)
    {
        QTreeWidgetItem *parent = selectedItem->parent();
        if (!parent)
        {
            return;
        }
        else
        {
            QString current_path_str = QDir::currentPath() + "/data/primitives/";
            QString file_name = "data.json";
            QString path = parent->text(0);
            QString sub_path = selectedItem->text(0);
            QString full_path = current_path_str + path + "/" + sub_path;
            QDir sub_dir(full_path);
            QStringList entries = sub_dir.entryList(QDir::NoDotAndDotDot | QDir::AllEntries);
            if (entries.isEmpty() || sub_dir.exists(file_name))
            {
                bool new_create = true;
                if (mp_current_pimitive)
                {
                    new_create = false;
                }

                PrimitiveWindow *popupWindow = new PrimitiveWindow(this, m_primitive_name, new_create);
                connect(popupWindow, &PrimitiveWindow::primitive_wnd_closed, this, &PrimitiveMgrWindow::refresh_item);
                // if (!new_create)
                // {
                //     popupWindow->fill_view();
                // }
                // popupWindow->fill_tree_node();
                popupWindow->show();
            }
            else
            {

            }
        }
    }
}

void PrimitiveMgrWindow::add_tuyuan(QTreeWidgetItem *item)
{
    QString current_path_str = QDir::currentPath() + "/data/primitives/" + item->text(0);
    QDir current_dir(current_path_str);
    // 获取当前目录下所有子目录
    QStringList dirs = current_dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
    PriEditNameDialog dialog(this);
    dialog.set_type_visible(true);
    if (dialog.exec() == QDialog::Accepted)
    {
        QString new_dir_name;
        int tuyuan_type = dialog.get_type();
        new_dir_name = dialog.getContent();

        if (new_dir_name.isEmpty())
        {
            QString prefix("tuyuan");
            int cute_num = find_small_dir(dirs, prefix);
            new_dir_name = QString("%1%2").arg(prefix).arg(cute_num);
        }

        // 创建新目录
        if (!current_dir.mkdir(new_dir_name))
        {
            return;
        }
        if (tuyuan_type)
        {
            // 在目录中创建空文件
            QString fileName = "python";
            QString filePath = current_path_str + "/" + new_dir_name + "/" + fileName;
            QFile file(filePath);
            if (file.open(QIODevice::WriteOnly))
            {
                file.close();  // 关闭文件
            }
            else
            {
                return;
            }
        }

        // 在 QTreeWidget 中添加新单元格
        QTreeWidgetItem *new_item = new QTreeWidgetItem();
        new_item->setText(0, new_dir_name);
        item->addChild(new_item);
        item->setExpanded(true);
    }
}

void PrimitiveMgrWindow::delete_cute(QTreeWidgetItem *item)
{
    QString dirPath = QDir::currentPath() + "/data/primitives/" + item->text(0);
    QDir dir(dirPath);

    if (dir.exists())
    {
        dir.removeRecursively();  // 删除目录及其内容
        delete item;  // 从 QTreeWidget 中删除该项
    }
}

void PrimitiveMgrWindow::delete_tuyuan(QTreeWidgetItem *item)
{
    QString dirPath = QDir::currentPath() + "/data/primitives/" + item->parent()->text(0) + "/" +  item->text(0);
    QDir dir(dirPath);

    if (dir.exists())
    {
        dir.removeRecursively();  // 删除目录及其内容
        delete item;  // 从 QTreeWidget 中删除该项
    }
}

void PrimitiveMgrWindow::show_context_menu(const QPoint &pos)
{
    QTreeWidgetItem *item = mp_pri_tree_widget->itemAt(pos);
    if (!item)
    {
        QMenu contextMenu(tr("Context menu"), this);
        QAction actionAddCell(tr("新建分类"), this);
        connect(&actionAddCell, &QAction::triggered, this, &PrimitiveMgrWindow::add_cate);
        contextMenu.addAction(&actionAddCell);
        contextMenu.exec(mp_pri_tree_widget->mapToGlobal(pos));
    }
    else
    {
        if (item->parent() == nullptr)
        {
            QMenu contextMenu(tr("Context menu"), this);

            QAction action_add_file("新建图元", this);
            connect(&action_add_file, &QAction::triggered, [this, item]()
            {
                add_tuyuan(item);
            });
            contextMenu.addAction(&action_add_file);

            QAction action_del_cute("删除分类", this);
            connect(&action_del_cute, &QAction::triggered, [this, item]()
            {
                delete_cute(item);
            });
            contextMenu.addAction(&action_del_cute);
            contextMenu.exec(mp_pri_tree_widget->mapToGlobal(pos));
        }
        else
        {
            QMenu contextMenu(tr("Context menu"), this);

            QAction action_del_tuyuan("删除图元", this);
            connect(&action_del_tuyuan, &QAction::triggered, [this, item]()
            {
                delete_tuyuan(item);
            });
            contextMenu.addAction(&action_del_tuyuan);
            contextMenu.exec(mp_pri_tree_widget->mapToGlobal(pos));
        }
    }
}

void PrimitiveMgrWindow::fill_thumbnail(const QString &img_file)
{
    QString full_path;

    full_path = img_file;
    if (!img_file.contains("/primitives"))
    {
        QString current_path = QDir::currentPath() + "/data/primitives";
        QString json_path = full_path.replace("#", "/");
        full_path = current_path + "/" + json_path;
    }

    // 加载图像
    QImage image(full_path);
    // 生成缩略图
    QImage thumbnail = image.scaled(200, 200, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    // 将缩略图显示在 QLabel 中
    mp_label->setPixmap(QPixmap::fromImage(thumbnail));
}

void PrimitiveMgrWindow::fill_layers(const QList<ly::LayerInfo*> &layers)
{
    mp_layer_list->clear();
    QStringList string_list;

    for (auto layer : layers)
    {
        if (layer->layer_name() == tr(LAYER_LOCATION_LAYER_NAME))
            continue;
        string_list.append(layer->layer_name());
    }
    mp_layer_list->addItems(string_list);
}

void PrimitiveMgrWindow::fill_params(const QList<pm::ParamDecl*> &params)
{
    mp_params_table->clearContents();
    mp_params_table->setRowCount(0);
    for (int i = 0; i < params.size(); i++)
    {
        mp_params_table->insertRow(i);
        mp_params_table->setItem(i, 0, new QTableWidgetItem(params[i]->key()));
        mp_params_table->setItem(i, 1, new QTableWidgetItem(params[i]->expression()));
        mp_params_table->setItem(i, 2, new QTableWidgetItem(params[i]->desc()));
    }
}

void PrimitiveMgrWindow::fill_rules(const QVector<pm::Rule> &rules)
{
    mp_rules_table->clearContents();
    mp_rules_table->setRowCount(0);
    for (int i = 0; i < rules.size(); i++)
    {
        mp_rules_table->insertRow(i);
        mp_rules_table->setItem(i, 0, new QTableWidgetItem(rules[i].to_str()));
    }
}

void PrimitiveMgrWindow::fill_anchors(QVector<at::AttachTreeUtils::AttachPointPosInf> &anchors)
{
    mp_anchor_table->clearContents();
    mp_anchor_table->setRowCount(0);
    for (int i = 0; i < anchors.size(); i++)
    {
        mp_anchor_table->insertRow(i);
        mp_anchor_table->setItem(i, 0, new QTableWidgetItem(anchors[i].pos.x().to_str()));
        mp_anchor_table->setItem(i, 1, new QTableWidgetItem(anchors[i].pos.y().to_str()));
    }
}

void PrimitiveMgrWindow::fill_data(Primitive *primitive)
{
    fill_thumbnail(primitive->thumbnail());

    auto layers = primitive->layer_mgr()->get_layers();
    fill_layers(layers);

    auto params = primitive->param_mgr()->params();
    fill_params(params);


    auto rules = primitive->param_mgr()->rules();
    fill_rules(rules);

    auto anchors = primitive->get_json_anchors();
    fill_anchors(anchors);

}

void PrimitiveMgrWindow::clear_data()
{
    mp_label->clear();
    mp_layer_list->clear();
    mp_params_table->clearContents();
    mp_rules_table->clearContents();
    mp_anchor_table->clearContents();
}

int PrimitiveMgrWindow::find_small_dir(const QStringList &dir_list, const QString &prefix)
{
    int smallest_number = 1;

    // 使用正则表达式匹配目录名中的编号，基于传入的前缀
    QRegularExpression regex("^" + QRegularExpression::escape(prefix) + "(\\d+)$");

    // 查找当前已经存在的最大编号
    while (true)
    {
        bool found = false;
        QString target_dir_ame = QString("%1%2").arg(prefix).arg(smallest_number);

        foreach (const QString &dir_name, dir_list)
        {
            QRegularExpressionMatch match = regex.match(dir_name);
            if (match.hasMatch() && match.captured(1).toInt() == smallest_number)
            {
                found = true;
                break;
            }
        }

        // 如果当前编号没有被占用，返回它
        if (!found)
        {
            return smallest_number;
        }
        smallest_number++;
    }
}

void PrimitiveMgrWindow::load_primitive(QTreeWidgetItem *item, int column)
{
    QString current_path_str = QDir::currentPath() + "/data/primitives/";
    QString file_name = "data.json";
    clear_data();
    if (item->childCount() == 0)
    {
        QString path = item->text(column);
        QString sub_path;
        QTreeWidgetItem *parent = item->parent();
        if (parent)
        {
            sub_path = parent->text(0) + "/" + path;
            path = parent->text(0) + "#" + path;
        }
        else
        {
            sub_path = path + "/tuyuan1";
            path = path + "#tuyuan1";
        }
        QString full_path = current_path_str + sub_path;
        QDir full_dir(full_path);
        QStringList entries = full_dir.entryList(QDir::NoDotAndDotDot | QDir::AllEntries);
        if (entries.isEmpty() || full_dir.exists(file_name))
        {
            m_primitive_name = path;
            mp_current_pimitive = PrimitiveMgr::instance()->load_primitive(path);
            if (mp_current_pimitive)
            {
                fill_data(mp_current_pimitive);
            }
        }
    }
}

void PrimitiveMgrWindow::change_name(QTreeWidgetItem *item, int column)
{
    QString current_path_str = QDir::currentPath() + "/data/primitives/";
    QDir current_dir(current_path_str);
    QString name = item->text(0);
    PriEditNameDialog dialog(name, this);
    QString new_dir_name;
    if (dialog.exec() == QDialog::Accepted)
    {
        new_dir_name = dialog.getContent();
        if (new_dir_name.isEmpty() || name == new_dir_name)
        {
            return;
        }

        QString new_full_dir_name, old_full_dir_name;
        if (item->childCount() == 0)
        {
            QString path = item->text(column);
            QTreeWidgetItem *parent = item->parent();
            if (parent)
            {
                old_full_dir_name = current_path_str + parent->text(0) + "/" + path;
                new_full_dir_name = current_path_str + parent->text(0) + "/" + new_dir_name;
                QDir parent_dir(current_path_str + parent->text(0));
                parent_dir.rename(old_full_dir_name, new_dir_name);
                PrimitiveMgr::instance()->init(current_path_str + parent->text(0));
            }
            else
            {
                QString path = item->text(column);
                old_full_dir_name = current_path_str + path;
                new_full_dir_name = current_path_str + new_dir_name;
                current_dir.rename(old_full_dir_name, new_dir_name);
                PrimitiveMgr::instance()->init(new_full_dir_name);
            }
        }
        else
        {
            QString path = item->text(column);
            old_full_dir_name = current_path_str + path;
            new_full_dir_name = current_path_str + new_dir_name;
            current_dir.rename(old_full_dir_name, new_dir_name);
            PrimitiveMgr::instance()->init(new_full_dir_name);
        }
        item->setText(column, new_dir_name);
        item->setExpanded(true);
    }
}

void PrimitiveMgrWindow::refresh_item()
{
    auto select_items = mp_pri_tree_widget->selectedItems();
    if (!select_items.isEmpty())
    {
        load_primitive(select_items.front(), 0);
    }
}

void PrimitiveMgrWindow::add_cate()
{
    QString currentDirPath = QDir::currentPath() + "/data/primitives";
    QDir currentDir(currentDirPath);
    // 获取当前目录下所有子目录
    QStringList dirs = currentDir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
    PriEditNameDialog dialog(this);
    QString new_dir_name;
    if (dialog.exec() == QDialog::Accepted)
    {
        new_dir_name = dialog.getContent();
        if (new_dir_name.isEmpty())
        {
            QString prefix("cate");
            int cute_num = find_small_dir(dirs, prefix);
            new_dir_name = QString("%1%2").arg(prefix).arg(cute_num);
        }

        // 创建新目录
        if (!currentDir.mkdir(new_dir_name))
        {
            return;
        }

        // 在 QTreeWidget 中添加新单元格
        QTreeWidgetItem *newItem = new QTreeWidgetItem(mp_pri_tree_widget);
        newItem->setText(0, new_dir_name);
        mp_pri_tree_widget->addTopLevelItem(newItem);
    }
}

}

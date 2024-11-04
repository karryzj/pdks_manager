#include "primitiveWindow.h"
#include <QToolBar>
#include <QStatusBar>
#include <QDockWidget>
#include <QTreeWidget>
#include <QListWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QGraphicsTextItem>
#include <QDialog>
#include <QComboBox>
#include <QLineEdit>
#include <QDir>
#include <attachTreeRootNode.h>
#include <PriShapeAction.h>
#include <PriShapeDialog.h>
#include <QDebug>
#include <QMouseEvent>
#include <QCoreApplication>
#include <ShapePointGraphicsItem.h>
#include <QTransform>
#include <PriGraphicsScene.h>
#include <QInputDialog>
#include "PriMouseFollower.h"
#include "PriTreeWidget.h"
#include "layInfo.h"
#include "priGraphicsView.h"
#include "layWidget.h"
#include "priTreeWidgetMenu.h"
#include "primitive.h"
#include "primitiveDefines.h"
#include "shapeDefines.h"
#include "shapeFactory.h"
#include "shapeDecl.h"
#include <PriDockWidget.h>
#include <PriGridResolutionDialog.h>
#include <PriImportDialog.h>
#include <PriRounedCornerDialog.h>
#include <PriRulerDialog.h>
#include <PriUtils.h>
#include <QDragEnterEvent>
#include <QGraphicsSceneContextMenuEvent>
#include <QShortcut>
#include <ShapeDrawGraphicsItem.h>
#include <qmessagebox.h>
#include "paramEditWidget.h"
#include "paramRuleWidget.h"
#include "priAnchorWidget.h"
#include "priCoordinateWidget.h"
#include "primitiveMgr.h"
#include "primitive.h"
#include "attachTreeUtils.h"
#include "layerDefines.h"
#include "layerMgr.h"
#include "viewport.h"
#include "attachTreeNodeMgr.h"
#include "priAttachTreeNodeMgr.h"
#include "attachTreeUtils.h"
#include "ruler.h"
#include "qcursor.h"
#include "commandManager.h"
// #include "redoService.h"
#include "common_defines.h"
#include "scopeTimer.h"
#include "configManager.h"
#include "priTreeWidgetItem.h"
#include "addCommand.h"
#include "removeCommand.h"

using namespace pr;
using namespace at;
using namespace sp;

PrimitiveWindow::PrimitiveWindow(QWidget* parent, QString pri_name, bool new_create) : QMainWindow(parent),
    mp_graphics_scene(new PriGraphicsScene(this)), mp_graphics_view(new PriGraphicsView(mp_graphics_scene, this))
{
    SCOPE_TIMER("PrimitiveWindow构造用时");

    //scene指针传给utils
    at::AttachTreeUtils::set_scene(mp_graphics_scene);

    // 启用抗锯齿
    mp_graphics_view->setRenderHint(QPainter::Antialiasing, false);

    // 设置视图的渲染提示
    mp_graphics_view->setRenderHint(QPainter::SmoothPixmapTransform, true);

    // 调整视图的变换模式
    mp_graphics_view->setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
    mp_graphics_view->setResizeAnchor(QGraphicsView::AnchorViewCenter);

    if(new_create)
    {
        mp_pri = pr::PrimitiveMgr::instance()->new_primitive(mp_graphics_view, pri_name);
    }
    else
    {
        mp_pri = pr::PrimitiveMgr::instance()->load_primitive(mp_graphics_view, pri_name);
    }
    mp_graphics_view->set_primitive(mp_pri);

    mp_param_mgr = mp_pri->param_mgr();
    mp_graphics_scene->init();

    // cm::RedoService::instance()->mgr(new db::Manager());

    QToolBar* general_tool_bar = addToolBar(PRI_GERNERAL_TOOL_BAR_NAME);
    general_tool_bar->setObjectName(PRI_GERNERAL_TOOL_BAR_NAME);
    // 设置工具栏图标大小
    general_tool_bar->setIconSize(QSize(32, 32));
    // 设置工具按钮样式，使文字显示在图标下方
    general_tool_bar->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);

    QToolBar* draw_tool_bar = addToolBar(PRI_DRAW_TOOL_BAR_NAME);
    draw_tool_bar->setObjectName(PRI_DRAW_TOOL_BAR_NAME);
    // 设置工具栏图标大小
    draw_tool_bar->setIconSize(QSize(32, 32));
    // 设置工具按钮样式，使文字显示在图标下方
    draw_tool_bar->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);

    QToolBar* align_tool_bar = addToolBar( PRI_ALIGN_TOOL_BAR_NAME);
    align_tool_bar->setObjectName(PRI_ALIGN_TOOL_BAR_NAME);
    // 设置工具栏图标大小
    align_tool_bar->setIconSize(QSize(32, 32));
    // 设置工具按钮样式，使文字显示在图标下方
    align_tool_bar->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);

    QToolBar* function_tool_bar = addToolBar( PRI_FUNCTION_TOOL_BAR_NAME);
    function_tool_bar->setObjectName(PRI_FUNCTION_TOOL_BAR_NAME);
    // 设置工具栏图标大小
    function_tool_bar->setIconSize(QSize(32, 32));
    // 设置工具按钮样式，使文字显示在图标下方
    function_tool_bar->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);

    // 创建状态栏
    QStatusBar* statusBar = new QStatusBar(this);
    setStatusBar(statusBar);
    // 创建用于显示坐标的标签
    mp_mouse_pos_label = new QLabel(tr("坐标："), this);
    statusBar->addPermanentWidget(mp_mouse_pos_label);

    setStyleSheet("QMainWindow::separator {"
                  "background: gray;"
                  "width: 1px;"
                  "height: 1px;"
                  "}");
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
                                // "    border-bottom-left-radius: 8px;" // 左上角圆角
                                // "    border-bottom-right-radius: 8px;"// 右上角圆角
                                "}";
    mp_dock_widget_top = new PriDockWidget(tr("图元树型结构"), this);
    mp_dock_widget_top->setStyleSheet(dock_widget_style);
    mp_pri_tree_widget = new pr::PriTreeWidget(mp_pri->at_root(), this);
    mp_dock_widget_top->setWidget(mp_pri_tree_widget);
    addDockWidget(Qt::LeftDockWidgetArea, mp_dock_widget_top);

    // 创建左侧下方框（Dock Widget）
    mp_layer_widget = new ly::LayerWidget(mp_pri->layer_mgr(), this);
    mp_dock_widget_bottom = new PriDockWidget(tr(PRI_LAYER_DOCK_WIDGET_NAME), this);
    mp_dock_widget_bottom->setObjectName(PRI_LAYER_DOCK_WIDGET_LIST_NAME);
    mp_dock_widget_bottom->setStyleSheet(dock_widget_style);
    mp_dock_widget_bottom->setWidget(mp_layer_widget);
    addDockWidget(Qt::LeftDockWidgetArea, mp_dock_widget_bottom);

    // 将第二个框放置在第一个框下面
    splitDockWidget(mp_dock_widget_top, mp_dock_widget_bottom, Qt::Vertical);

    // 创建第一个 Dock Widget
    mp_param_list_dock = new PriDockWidget(tr("参数列表"), this);
    mp_param_list_dock->setStyleSheet(dock_widget_style);
    mp_param_edit_widget = new pm::ParamEditWidget(mp_param_mgr);
    mp_param_list_dock->setWidget(mp_param_edit_widget);
    addDockWidget(Qt::RightDockWidgetArea, mp_param_list_dock);

    // 创建第二个 Dock Widget
    mp_param_constraints_dock = new PriDockWidget(tr("参数约束"), this);
    mp_param_constraints_dock->setStyleSheet(dock_widget_style);
    mp_param_rule_widget = new pm::ParamRuleWidget(mp_param_mgr);
    mp_param_constraints_dock->setWidget(mp_param_rule_widget);
    addDockWidget(Qt::RightDockWidgetArea, mp_param_constraints_dock);

    // 创建第三个 Dock Widget
    mp_coordinate_table = new pr::PrimitiveCoordinateUi(mp_pri->at_root());
    mp_coord_anchor_dock = new PriDockWidget(tr("坐标点"), this);
    mp_coord_anchor_dock->setStyleSheet(dock_widget_style);
    mp_coord_anchor_dock->setWidget(mp_coordinate_table);
    addDockWidget(Qt::RightDockWidgetArea, mp_coord_anchor_dock);

    // 创建第四个 Dock Widget
    mp_anchor_table = new pr::PrimitiveAnchorUi(mp_pri, nullptr);
    mp_anchor_dock = new PriDockWidget(tr("锚点"), this);
    mp_anchor_dock->setStyleSheet(dock_widget_style);
    mp_anchor_dock->setWidget(mp_anchor_table);
    addDockWidget(Qt::RightDockWidgetArea, mp_anchor_dock);

    // 设置中央小部件
    setCentralWidget(mp_graphics_view);

    // 设置主窗口标题
    QString title = QString("图元编辑-%1").arg(pri_name);
    setWindowTitle(tr(title.toStdString().c_str()));

    init_general_actions();

    init_draw_shape_actions();  // 这一步必须在DockWidget都搞完了再进行

    // 初始化工具栏上的对齐命令
    init_align_actions();  // 这一步必须在DockWidget都搞完了再进行

    init_function_actions();

    init_keyboard_shortcuts();

    // 最大化主窗口
    showMaximized();
    setWindowModality(Qt::ApplicationModal);
    setWindowFlags(Qt::Window | Qt::WindowCloseButtonHint);

    QString root_path = QDir::currentPath() + "/data/primitives";
    QString pri_path = pri_name.replace("#", "/");
    m_current_path = root_path + "/" + pri_path;

    set_connects();

    mp_pri->at_root()->tree_node_mgr()->update();

    emit mp_pri->signal_add_new_tree();
    if (!new_create)
    {
        fill_tree_node();
    }

    mp_graphics_scene->addItem(&m_preview_polygen);
}

PrimitiveWindow::~PrimitiveWindow()
{
    mp_graphics_scene->removeItem(&m_preview_polygen);
    clear_cache();
    remove_connects();

    mp_pri->at_root()->remove_all();
    SAFE_DELETE(mp_graphics_view);
    SAFE_DELETE(mp_graphics_scene);
    SAFE_DELETE(mp_mouse_pos_label);
    SAFE_DELETE(mp_ruler_dialog);
    SAFE_DELETE(mp_pri_tree_widget);
    SAFE_DELETE(mp_layer_widget);
    SAFE_DELETE(mp_param_edit_widget);
    SAFE_DELETE(mp_param_rule_widget);
    SAFE_DELETE(mp_coordinate_table);
    SAFE_DELETE(mp_anchor_table);
}

void PrimitiveWindow::fill_tree_node()
{
    mp_pri_tree_widget->update_tree();
    mp_anchor_table->update_table();
    mp_coordinate_table->update_table();
}

void PrimitiveWindow::fill_view()
{
    mp_pri->at_root()->set_new_graphics_view(mp_graphics_view);
}

void PrimitiveWindow::connect_new_tree_node(at::AttachTreeNode* new_tree_node)
{
    if(!new_tree_node->connect_with_mw())
    {
        // 线的重编辑
        sp::ShapeDrawGraphicsItem* item = new_tree_node->shape_item();
        auto succeed = connect(item, &sp::ShapeDrawGraphicsItem::item_double_clicked, this, &PrimitiveWindow::edit_draw_shape_dialog);
        Q_ASSERT(succeed);
        // 点的拖拽
        for(sp::ShapePointGraphicsItem * point_item : new_tree_node->point_items())
        {
            // 绑定point和tree node
            succeed = connect(point_item, &sp::ShapePointGraphicsItem::send_dragged_rectangle_info, this, &PrimitiveWindow::add_dragged_rectangle_info);
            Q_ASSERT(succeed);
        }
        new_tree_node->set_connect_with_mw(true);
    }
}

pr::Primitive *pr::PrimitiveWindow::current_primitive() const
{
    return mp_pri;
}

void PrimitiveWindow::fill_data()
{
    fill_tree_node();
    fill_view();
}

void PrimitiveWindow::update_mouse_position(const QPointF& scenePos)
{
    // 更新状态栏显示鼠标位置信息
    QPointF mapped_point = mp_pri->tree_node_mgr()->viewport()->map_from_scene(scenePos);
    QString statusMsg = QString("鼠标位置：(%1, %2)").arg(mapped_point.x()).arg(mapped_point.y());
    mp_mouse_pos_label->setText(statusMsg);
}

void PrimitiveWindow::adjust_mouse_position(const QPointF &scenePos)
{
    if ( false == m_is_recording || false == m_is_shift_pressed)
    {
        return;
    }

    QPointF last_point;
    if(!m_chosen_coord_points.isEmpty())
    {
        last_point = m_chosen_coord_points.last().first;
    }
    else
    {
        QPointF attach_point_in_scene = (ShapePointGraphicsItem::cachedItem ? ShapePointGraphicsItem::cachedItem : mp_pri->at_root()->origin_point())->pos();
        last_point = mp_pri->tree_node_mgr()->viewport()->map_from_scene(attach_point_in_scene);
    }
    QPointF true_current_scene_pos = current_scene_pos();

    double delat_x = abs(last_point.x() - true_current_scene_pos.x());
    double delat_y = abs(last_point.y() - true_current_scene_pos.y());

    if(delat_x < delat_y)
    {
        true_current_scene_pos.setX(last_point.x());
    }
    else
    {
        true_current_scene_pos.setY(last_point.y());
    }

    QPointF target_pos_scence = mp_pri->tree_node_mgr()->viewport()->map_to_scene(true_current_scene_pos);;

    // 设置全局鼠标位置
    // 将场景坐标转换为视图坐标
    QPoint viewPos = mp_graphics_view->mapFromScene(target_pos_scence);
    viewPos.rx() += 1;   // HINT@leixunyong。为了解决无限调用的问题。
    viewPos.ry() += 1;
    qDebug() << "viewPos QCursor set " << viewPos;
    // 将视图坐标转换为全局坐标
    QPoint globalPos = mp_graphics_view->mapToGlobal(viewPos);

    // 设置全局鼠标位置
    QCursor::setPos(globalPos);
}

void PrimitiveWindow::resizeEvent(QResizeEvent* event)
{
    match_viewport();
}

void PrimitiveWindow::keyPressEvent(QKeyEvent *event)
{
    // cm::RedoService::instance()->keyPressEvent(event);

    if(event->key() == Qt::Key_F)
    {
        fit_view();
    }

    if(event->key() == Qt::Key_R)
    {
        restore_view();
    }

    if (event->key() == Qt::Key_Shift)
    {
        m_is_shift_pressed = true;
    }

    if(event->key() == Qt::Key_Return)
    {
        if(m_is_recording)
        {
            newly_draw_shape_dialog();
            setCursor(Qt::ArrowCursor);
        }
    }
    QMainWindow::keyPressEvent(event);
}

void PrimitiveWindow::keyReleaseEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Shift)
    {
        m_is_shift_pressed = false;
    }
    QMainWindow::keyReleaseEvent(event);
}

void PrimitiveWindow::paintEvent(QPaintEvent *event)
{
    QMainWindow::paintEvent(event);
    if(m_is_recording)
    {
        QPainter painter(this);
        painter.setPen(Qt::red);
        for (const auto &info : m_chosen_coord_points)
        {
            QPointF point_in_scence = mp_pri->tree_node_mgr()->viewport()->map_to_scene(info.first);
            info.second->set_new_position(point_in_scence);
            PriUtils::modify_preview_polygen_by_points(m_chosen_coord_points, mp_pri, m_preview_polygen);
        }
    }
}

void PrimitiveWindow::closeEvent(QCloseEvent *event)
{
    // 显示确认对话框
    QMessageBox::StandardButton resBtn = QMessageBox::question(this, "Question",
                                         tr("Are you sure you want to close the application?\n"),
                                         QMessageBox::No | QMessageBox::Yes,
                                         QMessageBox::Yes);
    if (resBtn != QMessageBox::Yes)
    {
        // 如果用户选择 "No"，则忽略关闭事件
        event->ignore();
        return;
    }
    sp::ShapePointGraphicsItem::cachedItem = nullptr;
    clear_cache();
    remove_connects();

    SAFE_DELETE(mp_mouse_pos_label);
    SAFE_DELETE(mp_ruler_dialog);
    SAFE_DELETE(mp_pri_tree_widget);
    SAFE_DELETE(mp_layer_widget);
    SAFE_DELETE(mp_param_edit_widget);
    SAFE_DELETE(mp_param_rule_widget);
    SAFE_DELETE(mp_coordinate_table);
    SAFE_DELETE(mp_anchor_table);
    emit primitive_wnd_closed();
    QMainWindow::closeEvent(event);
}

void PrimitiveWindow::init_general_actions()
{
    QAction* undo_action = cmd::CommandManager::instance()->undo_stack().createUndoAction(this, tr(PRI_UNDO_ACTION_NAME));
    undo_action->setIcon(QIcon(":/img/lay_undo.png"));
    undo_action->setShortcut(QKeySequence::Undo);
    QAction* redo_action = cmd::CommandManager::instance()->undo_stack().createRedoAction(this, tr(PRI_REDO_ACTION_NAME));
    redo_action->setIcon(QIcon(":/img/lay_redo.png"));
    redo_action->setShortcut(QKeySequence::Redo);
    QAction* import_action = new QAction(QIcon(":/img/lay_import.png"), tr(PRI_IMPORT_ACTION_NAME), this);
    QAction* save_action = new QAction(QIcon(":/img/lay_save.png"), tr(PRI_SAVE_ACTION_NAME), this);
    save_action->setShortcut(QKeySequence("Ctrl+S"));

    auto general_tool_bar = findChild<QToolBar*>(PRI_GERNERAL_TOOL_BAR_NAME);

    general_tool_bar->addAction(undo_action);
    general_tool_bar->addAction(redo_action);
    general_tool_bar->addAction(import_action);
    general_tool_bar->addAction(save_action);
    connect(import_action, &QAction::triggered, this, &PrimitiveWindow::load_json);
    connect(save_action, &QAction::triggered, this, &PrimitiveWindow::save);
    connect(undo_action, &QAction::triggered, this, &PrimitiveWindow::undo);
    connect(redo_action, &QAction::triggered, this, &PrimitiveWindow::redo);

}

void PrimitiveWindow::init_draw_shape_actions(void)
{
    // 获取单例实例
    sp::ShapeFactory* shapeIns = sp::ShapeFactory::instance();
    QMap<QString, sp::ShapeDecl*> shapeDecl = shapeIns->get_shapes_decl();

    // 初始化工具栏
    QToolBar* tool_bar = findChild<QToolBar*>(PRI_DRAW_TOOL_BAR_NAME);
    Q_ASSERT(tool_bar);

    // 获得左下角的Layer Widget
    QDockWidget* dock_widget_bottom = findChild<QDockWidget*>(PRI_LAYER_DOCK_WIDGET_LIST_NAME);
    ly::LayerWidget* layer_widget = dynamic_cast<ly::LayerWidget*>(dock_widget_bottom->widget());
    Q_ASSERT(layer_widget);
    // 遍历所有形状
    for (auto it = shapeDecl.begin(); it != shapeDecl.end(); ++it)
    {
        auto& shape_decl = it.value();

        // 获取形状名称
        auto shape_name = shape_decl->name();

        // 获取形状资源路径
        auto shape_icon_name =  shape_decl->icon();

        //将Shape加入到Action
        auto drawShapeAction = new PriShapeAction(QIcon(":/img/" + QString(shape_icon_name)), tr(shape_name.toStdString().c_str()), this,
            layer_widget);
        tool_bar->addAction(drawShapeAction);

        connect(drawShapeAction, & QAction::triggered, this, & PrimitiveWindow::newly_draw_shape_dialog);

    }
    tool_bar->addSeparator();
}

void PrimitiveWindow::init_align_actions()
{
    // 初始化工具栏
    QToolBar* align_tool_bar = findChild<QToolBar*>(PRI_ALIGN_TOOL_BAR_NAME);
    Q_ASSERT(align_tool_bar);

    bool succeed = false;
    QString icon_path;

    auto left_align_action = align_tool_bar->addAction(PRI_LEFT_ALIGN_ACTION_NAME);
    icon_path = ":/img/" + QString(PRI_LEFT_ALIGN_ACTION_NAME) + ".png";
    QIcon icon_left(icon_path);
    left_align_action->setIcon(icon_left);
    succeed = connect(left_align_action, & QAction::triggered, this, &PrimitiveWindow::align_points);
    Q_ASSERT(succeed);

    auto right_align_action = align_tool_bar->addAction(PRI_RIGHT_ALIGN_ACTION_NAME);
    icon_path = ":/img/" + QString(PRI_RIGHT_ALIGN_ACTION_NAME) + ".png";
    QIcon icon_right(icon_path);
    right_align_action->setIcon(icon_right);
    succeed = connect(right_align_action, & QAction::triggered, this, &PrimitiveWindow::align_points);
    Q_ASSERT(succeed);

    auto top_align_action = align_tool_bar->addAction(PRI_TOP_ALIGN_ACTION_NAME);
    icon_path = ":/img/" + QString(PRI_TOP_ALIGN_ACTION_NAME) + ".png";
    QIcon icon_top(icon_path);
    top_align_action->setIcon(icon_top);
    succeed = connect(top_align_action, & QAction::triggered, this, &PrimitiveWindow::align_points);
    Q_ASSERT(succeed);

    auto bottom_align_action = align_tool_bar->addAction(PRI_BOTTOM_ALIGN_ACTION_NAME);
    icon_path = ":/img/" + QString(PRI_BOTTOM_ALIGN_ACTION_NAME) + ".png";
    QIcon icon_bottom(icon_path);
    bottom_align_action->setIcon(icon_bottom);
    succeed = connect(bottom_align_action, & QAction::triggered, this, &PrimitiveWindow::align_points);
    Q_ASSERT(succeed);
}

void PrimitiveWindow::init_function_actions()
{
    // 初始化工具栏
    QToolBar* function_tool_bar = findChild<QToolBar*>(PRI_FUNCTION_TOOL_BAR_NAME);
    Q_ASSERT(function_tool_bar);

    bool succeed = false;
    QString icon_path;

    // 测距功能
    auto ruler_action = function_tool_bar->addAction(PRI_RULER_ACTION_NAME);
    icon_path = ":/img/" + QString(PRI_RULER_ACTION_NAME) + ".png";
    QIcon icon_ruler(icon_path);
    ruler_action->setIcon(icon_ruler);
    succeed = connect(ruler_action, & QAction::triggered, this, &PrimitiveWindow::rule_2_points_distance);
    Q_ASSERT(succeed);

    auto grid_resolution_action = function_tool_bar->addAction(PRI_GRID_RESOLUTION_ACTION_NAME);
    icon_path = ":/img/" + QString(PRI_GRID_RESOLUTION_ACTION_NAME) + ".png";
    QIcon icon_grid_resolution(icon_path);
    grid_resolution_action->setIcon(icon_grid_resolution);
    succeed = connect(grid_resolution_action, & QAction::triggered, this, &PrimitiveWindow::setup_current_grid_resolution);
    Q_ASSERT(succeed);
    {
        QString resolution = QString::number(PRI_GRID_RESOLUTION_DEFAULT_VALUE);
        grid_resolution_action->setObjectName(PRI_GRID_RESOLUTION_ACTION_NAME);
        grid_resolution_action->setText(resolution);
        cm::ConfigManager::instance()->register_new_config(CM_LOGIC_GRID_RESOLUTION_KEY, PRI_GRID_RESOLUTION_DEFAULT_VALUE);
    }

    // 弧长设置
    auto arc_len_action = function_tool_bar->addAction(PRI_ARC_LEN_ACTION_NAME);
    icon_path = ":/img/" + QString(PRI_RULER_ACTION_NAME) + ".png";  // HINT@leixunyong。这里的弧长图标有问题。
    QIcon icon_arc(icon_path);
    arc_len_action->setIcon(icon_arc);
    succeed = connect(arc_len_action, & QAction::triggered, this, &PrimitiveWindow::set_arc_length);
    Q_ASSERT(succeed);
    {
        cm::ConfigManager::instance()->register_new_config(CM_ARC_LEN_KEY, CM_ARC_LEN_DEFAULT_VALUE);
    }

    // 圆角设置
    auto rounded_corner_action = function_tool_bar->addAction("rounded corner");
    icon_path = ":/img/" + QString(PRI_ROUNDED_CORNER_ACTION_NAME) + ".png";
    QIcon icon_rounded_corner(icon_path);
    rounded_corner_action->setIcon(icon_rounded_corner);
    succeed = connect(rounded_corner_action, & QAction::triggered, this, &PrimitiveWindow::set_rounded_corner_params);
    Q_ASSERT(succeed);
    {
        cm::ConfigManager::instance()->register_new_config(CM_INNER_CORNER_RADIUS_KEY, CM_INNER_CORNER_RADIUS_DEFAULT_VALUE);
        cm::ConfigManager::instance()->register_new_config(CM_OUTER_CORNER_RADIUS_KEY, CM_OUTER_CORNER_RADIUS_DEFAULT_VALUE);
        cm::ConfigManager::instance()->register_new_config(CM_OUTER_CORNER_DIRECTION_KEY, CM_OUTER_CORNER_DIRECTION_DEFAULT_VALUE);
    }
}

void pr::PrimitiveWindow::init_keyboard_shortcuts()
{
    bool succeed = false;

    QShortcut *short_cut_ctrl_c = new QShortcut(QKeySequence("Ctrl+C"), this);
    succeed = connect(short_cut_ctrl_c, &QShortcut::activated, this->mp_pri_tree_widget->tree_widge_menu(), &PriTreeWidgetMenu::record_copied_tree_node_info);
    Q_ASSERT(succeed);

    QShortcut *short_cut_ctrl_x = new QShortcut(QKeySequence("Ctrl+X"), this);
    succeed = connect(short_cut_ctrl_x, &QShortcut::activated, this->mp_pri_tree_widget->tree_widge_menu(), &PriTreeWidgetMenu::record_cut_tree_node_info);
    Q_ASSERT(succeed);

    QShortcut *short_cut_ctrl_v = new QShortcut(QKeySequence("Ctrl+V"), this);
    succeed = connect(short_cut_ctrl_v, &QShortcut::activated, this->mp_pri_tree_widget->tree_widge_menu(), &PriTreeWidgetMenu::paste_cut_or_copied_tree_node_info);
    Q_ASSERT(succeed);

    QShortcut *short_cut_ctrl_i = new QShortcut(QKeySequence(Qt::Key_I), this);
    succeed = connect(short_cut_ctrl_i, &QShortcut::activated, this, &PrimitiveWindow::add_inner_rounded_corner_for_shape);
    Q_ASSERT(succeed);

    QShortcut *short_cut_ctrl_o = new QShortcut(QKeySequence(Qt::Key_O), this);
    succeed = connect(short_cut_ctrl_o, &QShortcut::activated, this, &PrimitiveWindow::add_outer_rounded_corner_for_shape);
    Q_ASSERT(succeed);

    // HINT@leixunyong。后续快捷键都可以继续往下添加。
}

void PrimitiveWindow::set_connects()
{
    if(false == m_has_connects)
    {
        bool succeed = false;
        succeed = connect(this, &PrimitiveWindow::align_operation, mp_graphics_view, &PriGraphicsView::align_point_items);
        Q_ASSERT(succeed);

        succeed = connect(mp_graphics_view, & PriGraphicsView::mouseMoved, this, & PrimitiveWindow::update_mouse_position);
        Q_ASSERT(succeed);
        succeed = connect(mp_graphics_view, & PriGraphicsView::mouseMoved, this, & PrimitiveWindow::adjust_mouse_position);
        Q_ASSERT(succeed);
        succeed = connect(mp_graphics_view, & PriGraphicsView::mouse_left_button_press, this, & PrimitiveWindow::record_mouse_left_button_click_point);
        Q_ASSERT(succeed);
        succeed = connect(mp_graphics_view, & PriGraphicsView::key_escape_press, this, & PrimitiveWindow::cancel_current_operations);
        Q_ASSERT(succeed);
        succeed = connect(mp_graphics_view, & PriGraphicsView::double_click_mouse_left_button, this, & PrimitiveWindow::finish_pickup_polygen_point);
        Q_ASSERT(succeed);
        succeed = connect(mp_graphics_view, & PriGraphicsView::mouse_right_button_press, mp_pri_tree_widget, & PriTreeWidget::process_tree_widget_context_menu);
        Q_ASSERT(succeed);
        succeed = connect(mp_graphics_view, & PriGraphicsView::mouse_left_button_press_item, mp_pri_tree_widget->tree_widge_menu(), & PriTreeWidgetMenu::set_pickup_tree_widget_item);
        Q_ASSERT(succeed);

        // 连接scene删除treenode到锚点列表的槽函数
        succeed = connect(mp_graphics_scene, & pr::PriGraphicsScene::before_delete_tree_node, mp_anchor_table,
                          & pr::PrimitiveAnchorUi::on_delete_node);
        Q_ASSERT(succeed);
        // 连接scene删除treenode到坐标点列表的槽函数
        succeed = connect(mp_graphics_scene, & pr::PriGraphicsScene::before_delete_tree_node, mp_coordinate_table,
                          & pr::PrimitiveCoordinateUi::onDeleteNode);
        Q_ASSERT(succeed);
        succeed = connect(mp_graphics_scene, &PriGraphicsScene::after_delete_tree_node, this, &PrimitiveWindow::after_delete_tree_node);
        Q_ASSERT(succeed);
        // 连接sence中选择变化时，发送给图元树的信号，用于反选图元树
        succeed = connect(mp_graphics_scene, &PriGraphicsScene::selectionChanged, this, &PrimitiveWindow::on_scene_selection_changed);
        Q_ASSERT(succeed);

        succeed = connect(mp_pri, &Primitive::signal_add_new_tree, this, &PrimitiveWindow::connect_tree_node_with_mw);
        Q_ASSERT(succeed);

        succeed = connect(mp_pri->at_root()->origin_point(), &sp::ShapePointGraphicsItem::send_dragged_rectangle_info, this, &PrimitiveWindow::add_dragged_rectangle_info);
        Q_ASSERT(succeed);

        // 连接图元树右键选项到锚点表的槽函数
        succeed = connect(mp_pri_tree_widget->tree_widge_menu(), & pr::PriTreeWidgetMenu::signal_anchor_action_triggered, mp_anchor_table,
                          & pr::PrimitiveAnchorUi::add_anchor_to_table);
        Q_ASSERT(succeed);
        // 连接图元树右键选项到坐标点表的槽函数
        succeed = connect(mp_pri_tree_widget->tree_widge_menu(), & pr::PriTreeWidgetMenu::signal_coord_action_triggered, mp_coordinate_table,
                          & pr::PrimitiveCoordinateUi::addCoordToTable);
        Q_ASSERT(succeed);

        succeed = connect(this, &PrimitiveWindow::scene_selection_changed_to_tree, mp_pri_tree_widget, &PriTreeWidget::onSceneSelectionChanged);
        Q_ASSERT(succeed);

        succeed = connect(mp_param_edit_widget, &pm::ParamEditWidget::param_changed, this, &PrimitiveWindow::update_current_primitive);
        Q_ASSERT(succeed);

        // auto conn_ret = connect(mp_layer_widget, &ly::LayerWidget::layer_changed, this, &PrimitiveWindow::update_current_primitive);
        succeed = connect(mp_layer_widget, SIGNAL(layer_changed()), this, SLOT(update_current_primitive()));
        Q_ASSERT(succeed);

        succeed = connect(mp_layer_widget, &ly::LayerWidget::delete_layer_info, this, &PrimitiveWindow::delete_tree_nodes_with_layer_info);
        Q_ASSERT(succeed);

        // view changed
        succeed = connect(mp_pri->tree_node_mgr()->viewport(), &cm::Viewport::view_changed, this, &PrimitiveWindow::on_view_changed);
        Q_ASSERT(succeed);


        succeed = connect(mp_pri_tree_widget, & pr::PriTreeWidget::before_delete_tree_node, mp_graphics_scene,
                          & pr::PriGraphicsScene::before_delete_tree_node);
        Q_ASSERT(succeed);
        succeed = connect(mp_pri_tree_widget, & pr::PriTreeWidget::after_delete_tree_node, mp_graphics_scene,
                          & pr::PriGraphicsScene::after_delete_tree_node);
        Q_ASSERT(succeed);

        succeed = connect(mp_graphics_scene, & pr::PriGraphicsScene ::before_delete_tree_node, mp_pri_tree_widget->tree_widge_menu(),
                          & pr::PriTreeWidgetMenu::on_before_tree_node);
        Q_ASSERT(succeed);

        succeed = connect(mp_dock_widget_top, &PriDockWidget::dock_widget_resize, this, &PrimitiveWindow::on_dock_widget_resize);
        Q_ASSERT(succeed);

        succeed = connect(mp_dock_widget_bottom, &PriDockWidget::dock_widget_resize, this, &PrimitiveWindow::on_dock_widget_resize);
        Q_ASSERT(succeed);

        succeed = connect(mp_param_list_dock, &PriDockWidget::dock_widget_resize, this, &PrimitiveWindow::on_dock_widget_resize);
        Q_ASSERT(succeed);

        succeed = connect(mp_param_constraints_dock, &PriDockWidget::dock_widget_resize, this, &PrimitiveWindow::on_dock_widget_resize);
        Q_ASSERT(succeed);

        succeed = connect(mp_coord_anchor_dock, &PriDockWidget::dock_widget_resize, this, &PrimitiveWindow::on_dock_widget_resize);
        Q_ASSERT(succeed);

        succeed = connect(mp_anchor_dock, &PriDockWidget::dock_widget_resize, this, &PrimitiveWindow::on_dock_widget_resize);
        Q_ASSERT(succeed);

        m_has_connects = true;
    }
}

void PrimitiveWindow::remove_connects()
{
    if(true  == m_has_connects )
    {
        bool succeed = false;
        succeed = disconnect(this, &PrimitiveWindow::align_operation, mp_graphics_view, &PriGraphicsView::align_point_items);
        Q_ASSERT(succeed);

        succeed = disconnect(mp_graphics_view, & PriGraphicsView::mouseMoved, this, & PrimitiveWindow::update_mouse_position);
        Q_ASSERT(succeed);
        succeed = disconnect(mp_graphics_view, & PriGraphicsView::mouseMoved, this, & PrimitiveWindow::adjust_mouse_position);
        Q_ASSERT(succeed);
        succeed = disconnect(mp_graphics_view, & PriGraphicsView::mouse_left_button_press, this, & PrimitiveWindow::record_mouse_left_button_click_point);
        Q_ASSERT(succeed);
        succeed = disconnect(mp_graphics_view, & PriGraphicsView::key_escape_press, this, & PrimitiveWindow::cancel_current_operations);
        Q_ASSERT(succeed);
        succeed = disconnect(mp_graphics_view, & PriGraphicsView::double_click_mouse_left_button, this, & PrimitiveWindow::finish_pickup_polygen_point);
        Q_ASSERT(succeed);
        succeed = disconnect(mp_graphics_view, & PriGraphicsView::mouse_right_button_press, mp_pri_tree_widget, & PriTreeWidget::process_tree_widget_context_menu);
        Q_ASSERT(succeed);
        succeed = disconnect(mp_graphics_view, & PriGraphicsView::mouse_left_button_press_item, mp_pri_tree_widget->tree_widge_menu(), & PriTreeWidgetMenu::set_pickup_tree_widget_item);
        Q_ASSERT(succeed);

        // 连接scene删除treenode到锚点列表的槽函数
        succeed = disconnect(mp_graphics_scene, & pr::PriGraphicsScene::before_delete_tree_node, mp_anchor_table,
                             & pr::PrimitiveAnchorUi::on_delete_node);
        Q_ASSERT(succeed);
        // 连接scene删除treenode到坐标点列表的槽函数
        succeed = disconnect(mp_graphics_scene, & pr::PriGraphicsScene::before_delete_tree_node, mp_coordinate_table,
                             & pr::PrimitiveCoordinateUi::onDeleteNode);
        Q_ASSERT(succeed);
        succeed = disconnect(mp_graphics_scene, &PriGraphicsScene::after_delete_tree_node, this, &PrimitiveWindow::after_delete_tree_node);
        Q_ASSERT(succeed);
        // 连接sence中选择变化时，发送给图元树的信号，用于反选图元树
        succeed = disconnect(mp_graphics_scene, &PriGraphicsScene::selectionChanged, this, &PrimitiveWindow::on_scene_selection_changed);
        Q_ASSERT(succeed);

        succeed = disconnect(mp_pri, &Primitive::signal_add_new_tree, this, &PrimitiveWindow::connect_tree_node_with_mw);
        Q_ASSERT(succeed);

        succeed = disconnect(mp_pri->at_root()->origin_point(), &sp::ShapePointGraphicsItem::send_dragged_rectangle_info, this, &PrimitiveWindow::add_dragged_rectangle_info);
        Q_ASSERT(succeed);

        // 连接图元树右键选项到锚点表的槽函数
        succeed = disconnect(mp_pri_tree_widget->tree_widge_menu(), & pr::PriTreeWidgetMenu::signal_anchor_action_triggered, mp_anchor_table,
                             & pr::PrimitiveAnchorUi::add_anchor_to_table);
        Q_ASSERT(succeed);
        // 连接图元树右键选项到坐标点表的槽函数
        succeed = disconnect(mp_pri_tree_widget->tree_widge_menu(), & pr::PriTreeWidgetMenu::signal_coord_action_triggered, mp_coordinate_table,
                             & pr::PrimitiveCoordinateUi::addCoordToTable);
        Q_ASSERT(succeed);

        succeed = disconnect(this, &PrimitiveWindow::scene_selection_changed_to_tree, mp_pri_tree_widget, &PriTreeWidget::onSceneSelectionChanged);
        Q_ASSERT(succeed);

        succeed = disconnect(mp_param_edit_widget, &pm::ParamEditWidget::param_changed, this, &PrimitiveWindow::update_current_primitive);
        Q_ASSERT(succeed);

        // auto conn_ret = connect(mp_layer_widget, &ly::LayerWidget::layer_changed, this, &PrimitiveWindow::update_current_primitive);
        succeed = disconnect(mp_layer_widget, SIGNAL(layer_changed()), this, SLOT(update_current_primitive()));
        Q_ASSERT(succeed);

        succeed = disconnect(mp_layer_widget, &ly::LayerWidget::delete_layer_info, this, &PrimitiveWindow::delete_tree_nodes_with_layer_info);
        Q_ASSERT(succeed);

        // view changed
        succeed = disconnect(mp_pri->tree_node_mgr()->viewport(), &cm::Viewport::view_changed, this, &PrimitiveWindow::on_view_changed);
        Q_ASSERT(succeed);

        succeed = disconnect(mp_pri_tree_widget, & pr::PriTreeWidget::before_delete_tree_node, mp_graphics_scene,
                             & pr::PriGraphicsScene::before_delete_tree_node);
        Q_ASSERT(succeed);
        succeed = disconnect(mp_pri_tree_widget, & pr::PriTreeWidget::after_delete_tree_node, mp_graphics_scene,
                             & pr::PriGraphicsScene::after_delete_tree_node);

        succeed = disconnect(mp_graphics_scene, & pr::PriGraphicsScene ::before_delete_tree_node, mp_pri_tree_widget->tree_widge_menu(),
                             & pr::PriTreeWidgetMenu::on_before_tree_node);
        Q_ASSERT(succeed);

        succeed = disconnect(mp_dock_widget_top, &PriDockWidget::dock_widget_resize, this, &PrimitiveWindow::on_dock_widget_resize);
        Q_ASSERT(succeed);

        succeed = disconnect(mp_dock_widget_bottom, &PriDockWidget::dock_widget_resize, this, &PrimitiveWindow::on_dock_widget_resize);
        Q_ASSERT(succeed);

        succeed = disconnect(mp_param_list_dock, &PriDockWidget::dock_widget_resize, this, &PrimitiveWindow::on_dock_widget_resize);
        Q_ASSERT(succeed);

        succeed = disconnect(mp_param_constraints_dock, &PriDockWidget::dock_widget_resize, this, &PrimitiveWindow::on_dock_widget_resize);
        Q_ASSERT(succeed);

        succeed = disconnect(mp_coord_anchor_dock, &PriDockWidget::dock_widget_resize, this, &PrimitiveWindow::on_dock_widget_resize);
        Q_ASSERT(succeed);

        succeed = disconnect(mp_anchor_dock, &PriDockWidget::dock_widget_resize, this, &PrimitiveWindow::on_dock_widget_resize);
        Q_ASSERT(succeed);

        m_has_connects = false;
    }
}

at::AttachTreeNode* PrimitiveWindow::add_new_tree_node(const QString &shape_name,
        const QVector<pm::ParamDecl> &params,
        at::NodeType node_type,
        at::NodeDirection node_direction,
        at::NodeBooleanSubtractType node_boolean_subtract_type,
        ly::LayerInfo *layer_info)
{
    at::AttachTreeBaseNode* parent_node = nullptr;
    int parent_attach_point_idx = -1;
    if (sp::ShapePointGraphicsItem::cachedItem == nullptr || sp::ShapePointGraphicsItem::cachedItem == mp_pri->at_root()->origin_point())
    {
        parent_node = mp_pri->at_root();
        parent_attach_point_idx = 0;  // 只有一个点，肯定索引为0
    }
    else
    {
        auto tree_node = dynamic_cast<AttachTreeNode*>(at::AttachTreeUtils::attach_tree_node_point_item_in(sp::ShapePointGraphicsItem::cachedItem, mp_pri->at_root()));
        if(nullptr != tree_node)
        {
            parent_attach_point_idx = at::AttachTreeUtils::point_item_index_in_tree_node(tree_node,
                                      sp::ShapePointGraphicsItem::cachedItem);
            parent_node = tree_node;
        }
        else
        {
            parent_node = mp_pri->at_root();
            parent_attach_point_idx = 0;  // 只有一个点，肯定索引为0
        }

    }

    // 新增节点
    at::AttachTreeNode* new_tree_node = parent_node->add_child(parent_attach_point_idx,
                                        shape_name,
                                        params,
                                        node_type,
                                        node_direction,
                                        node_boolean_subtract_type,
                                        layer_info);
    emit mp_pri->signal_add_new_tree();
    new_tree_node->update();
    mp_pri->tree_node_mgr()->update();
    return new_tree_node;
}

void PrimitiveWindow::newly_draw_shape_dialog()
{
    // 获取当前点击的Action
    QAction* action = qobject_cast<QAction*>(sender());
    if(nullptr == action)
    {
        action = m_cache_action;
    }
    Q_ASSERT(action);

    sp::ShapeFactory* shape_ins = sp::ShapeFactory::instance();
    QMap<QString, sp::ShapeDecl*> shape_decls = shape_ins->get_shapes_decl();

    auto itor = shape_decls.find(action->text());
    Q_ASSERT(itor != shape_decls.end());
    if(itor.value()->is_varargs())
    {
        if(itor.key() == SHAPE_POLYGEN)
        {
            if(m_is_recording == false)
            {
                clear_cache();
                m_is_recording = true;  // HINT@leixunyong。这里的做法是，第一次进入的时候设置进入准备状态
                m_cache_action = action;
                setCursor(Qt::CrossCursor);
                return;
            }
            else
            {
                QVector<QPair<QString, QVariant>> point_info;
                for(auto idx = 0; idx < m_chosen_coord_points.size(); idx++)
                {
                    QPointF last_point(0, 0);
                    if(idx > 0)
                    {
                        last_point = m_chosen_coord_points[idx - 1].first ;
                    }

                    const QPointF& true_point = m_chosen_coord_points[idx].first;

                    QString point_x_name = "x"  + QString::number(idx + 1)  ;
                    QPointF local_point = mp_pri->tree_node_mgr()->viewport()->map_from_scene(mp_polygen_cache_point->pos());
                    QVariant point_x_variant = static_cast<double>(true_point.x() - local_point.x());

                    point_info.append(QPair<QString, QVariant> {point_x_name, point_x_variant});

                    QString point_y_name = "y"  + QString::number(idx + 1)  ;
                    QVariant point_y_variant = static_cast<double>(true_point.y() - local_point.y());
                    point_info.append(QPair<QString, QVariant> {point_y_name, point_y_variant});
                }
                itor.value()->set_variational_param_list(point_info);
                m_is_recording = false;
            }
        }
    }

    // 预览完了就清空
    if(action->objectName() == SHAPE_POLYGEN)
    {
        qDebug() << "polygen points number is " << m_chosen_coord_points.size();
        for(auto idx = 0; idx < m_chosen_coord_points.size(); idx++)
        {
            auto point_item = m_chosen_coord_points[idx].second;
            Q_ASSERT(point_item->scene() == mp_graphics_scene);
            point_item->setVisible(false);
            point_item->setSelected(false);
            mp_graphics_scene->removeItem(point_item);
        }
        m_chosen_coord_points.clear();
        m_preview_polygen.setPolygon(QPolygonF{});
    }
    PriShapeDialog * dialog = dynamic_cast<PriShapeAction * >(action)->ui_form();
    bool succeed = connect(dialog, &PriShapeDialog::add_new_param_to_param_mgr, mp_param_edit_widget, &pm::ParamEditWidget::on_add_new_expressison);
    Q_ASSERT(succeed);

    dialog->init_ui_in_create_shape();
    int exec_case = dialog->exec();
    if (exec_case == QDialog::DialogCode::Accepted)
    {
        auto shape_node_type = dialog->shape_node_type();
        auto shape_node_direction = dialog->shape_node_direction();
        auto shape_node_boolean_subtract_type = dialog->shape_node_boolean_subtract_type();
        auto shape_layer_info_name = dialog->shape_layer_info_name();
        ly::LayerInfo* shape_layer_info = mp_pri->layer_mgr()->get_layer_info_by_name(
                                              shape_layer_info_name);  // 找到图层信息
        auto param_list = dialog->shape_param_list();

        auto tree_node = add_new_tree_node(action->objectName(), param_list, shape_node_type, shape_node_direction, shape_node_boolean_subtract_type, shape_layer_info);
        dialog->accept();
        mp_pri->tree_node_mgr()->update();
        cmd::CommandManager::instance()->push(new cmd::AddCommand(tree_node));
    }
    else if (exec_case == QDialog::DialogCode::Rejected)
    {
        dialog->reject();
    }

    succeed = disconnect(dialog, &PriShapeDialog::add_new_param_to_param_mgr, mp_param_edit_widget, &pm::ParamEditWidget::on_add_new_expressison);
    Q_ASSERT(succeed);
}

void PrimitiveWindow::edit_draw_shape_dialog(sp::ShapeDrawGraphicsItem* item)
{
    QString shape_name = item->shape_name();
    // 使用正则表达式匹配末尾的数字
    QRegularExpression re("\\d+$");
    shape_name.remove(re);
    QAction* action = findChild<QAction*>(shape_name);
    Q_ASSERT(action);

    PriShapeDialog* dialog = dynamic_cast<PriShapeAction*>(action)->ui_form();
    bool succeed = connect(dialog, &PriShapeDialog::add_new_param_to_param_mgr, mp_param_edit_widget, &pm::ParamEditWidget::on_add_new_expressison);
    Q_ASSERT(succeed);
    dialog->init_ui_in_edit_shape(item);
    int exec_case = dialog->exec();
    if (exec_case == QDialog::DialogCode::Accepted)
    {
        // cm::RedoService::instance()->transaction();

        auto shape_node_type = dialog->shape_node_type();
        auto shape_node_direction = dialog->shape_node_direction();
        auto shape_node_boolean_subtract_type = dialog->shape_node_boolean_subtract_type();
        auto shape_layer_info_name = dialog->shape_layer_info_name();
        ly::LayerInfo* shape_layer_info = mp_pri->layer_mgr()->get_layer_info_by_name(
                                              shape_layer_info_name);  // 找到图层信息
        auto param_list = dialog->shape_param_list();

        at::AttachTreeNode* tree_node = at::AttachTreeUtils::attach_tree_node_shape_item_in(item, mp_pri->at_root());
        if (tree_node->node_type() != shape_node_type)   // 判断一下再设置是为了减少无端的性能损耗，下同
        {
            tree_node->set_node_type(shape_node_type);
        }

        if (tree_node->node_direction() != shape_node_direction)
        {
            tree_node->set_node_direction(shape_node_direction);
        }

        if (tree_node->node_boolean_subtract_type() != shape_node_boolean_subtract_type)
        {
            tree_node->set_node_boolean_subtract_type(shape_node_boolean_subtract_type);
        }

        if (tree_node->layer_info() != shape_layer_info)
        {
            tree_node->set_layer_info(shape_layer_info);
        }
        tree_node->set_params(param_list);

        dialog->accept();
        mp_pri->tree_node_mgr()->update();
        // cm::RedoService::instance()->commit();
    }
    else if (exec_case == QDialog::DialogCode::Rejected)
    {
        dialog->reject();
    }
    succeed = disconnect(dialog, &PriShapeDialog::add_new_param_to_param_mgr, mp_param_edit_widget, &pm::ParamEditWidget::on_add_new_expressison);
    Q_ASSERT(succeed);
}

void PrimitiveWindow::save_json()
{
    mp_anchor_table->save_anchors_to_primitive();
    auto save_case = mp_pri->save();
    if(save_case == 0)
    {
        QMessageBox::information(this, "message", "Save successful!");
    }
    else
    {
        QMessageBox::warning(this, "message", "Save failed!");
    }
}

void PrimitiveWindow::fit_view()
{
    auto viewport = mp_pri->tree_node_mgr()->viewport();

    QRectF bouding_rect = mp_pri->at_root()->bounding_rect();
    QRectF lb_rect = viewport->map_from_scene(bouding_rect);

    double max = std::max({std::abs(lb_rect.left()), std::abs(lb_rect.right()), std::abs(lb_rect.top()), std::abs(lb_rect.bottom())});
    // keep 0.1 ratior space
    max *= 1.1;
    viewport->fit(QRectF(-max, max, max * 2, max * 2));
}

void PrimitiveWindow::restore_view()
{
    auto viewport = mp_pri->tree_node_mgr()->viewport();
    viewport->restore();
}

void PrimitiveWindow::on_view_changed()
{
    mp_pri->tree_node_mgr()->update();
    mp_graphics_scene->set_ruler(mp_pri->dbu(), mp_pri->tree_node_mgr()->viewport());
}

void PrimitiveWindow::match_viewport()
{
    // 设置scene rect
    QRect view_rect = mp_graphics_view->geometry();
    if( view_rect.width() > view_rect.height())
    {
        mp_graphics_view->scene()->setSceneRect(0, 0, view_rect.width(), view_rect.height());

        // 设置viewport
        mp_pri->tree_node_mgr()->viewport()->set_size(view_rect.width(), view_rect.height());
    }
}

void PrimitiveWindow::save_img()
{
    // 渲染场景内容到 QImage
    auto visible = mp_pri->layer_mgr()->get_layers().at(0)->visible();
    mp_pri->layer_mgr()->get_layers().at(0)->set_visible(false);
    mp_graphics_scene->toggle_ruler();

    auto scene_size = mp_graphics_scene->sceneRect().size().toSize();
    auto items = mp_graphics_scene->items();
    QVector<bool> items_status;
    for (auto &item : items)
    {
        items_status.append(item->isSelected());
        item->setSelected(false);
    }

    QImage image(scene_size, QImage::Format_ARGB32);
    image.fill(Qt::transparent);  // 如果需要透明背景
    QPainter painter(&image);
    mp_graphics_scene->render(&painter);

    // 保存为 PNG 文件
    QString file_name = m_current_path + "/" + "thumbnail.png";
    image.save(file_name);

    // 恢复状态
    mp_pri->layer_mgr()->get_layers().at(0)->set_visible(visible);
    for (auto i = 0; i < items.size(); i++)
    {
        items[i]->setSelected(items_status[i]);
    }
    mp_graphics_scene->toggle_ruler();
}

void PrimitiveWindow::save()
{
    QRectF box_cache = mp_pri->tree_node_mgr()->viewport()->box();
    fit_view();
    save_json();
    save_img();
    mp_pri->tree_node_mgr()->viewport()->set_box(box_cache);
}

void PrimitiveWindow::load_json()
{
    PriImportDialog dialog(mp_pri);
    auto return_value = dialog.exec();
    if(return_value == QDialog::Accepted)
    {
        emit mp_pri->signal_add_new_tree();

        mp_layer_widget->update_all_items();
        mp_param_edit_widget->update_all_items();
        mp_param_rule_widget->update_all_items();
        mp_coordinate_table->update_table();
        mp_anchor_table->update_table();
    }
}

void PrimitiveWindow::after_delete_tree_node()
{
    mp_graphics_view->drag_box()->set_tree_node(nullptr);
    mp_pri_tree_widget->update_tree();
}

void PrimitiveWindow::on_scene_selection_changed()
{
    // scene选择发生改变，把scene发送到tree widget
    emit scene_selection_changed_to_tree(mp_graphics_scene);
}

void PrimitiveWindow::add_dragged_rectangle_info(sp::ShapePointGraphicsItem *point_item, QPointF end_point)
{
    // 先纠正起点和终点的位置
    auto start_point = mp_pri->tree_node_mgr() ->viewport()->map_from_scene(point_item->pos());

    // 移动到最近的格点
    end_point = PriUtils::closest_grid_point(end_point, mp_pri->at_root());

    // 确定node type
    NodeType node_type = NodeType::LOCATION;
    // 确定node direction
    NodeDirection node_direction = NodeDirection::NONE;
    if(end_point.x() < start_point.x()  && end_point.y() > start_point.y())
    {
        node_direction = NodeDirection::TOP_LEFT;
    }
    else if(end_point.x() < start_point.x()  && end_point.y() < start_point.y())
    {
        node_direction = NodeDirection::BOTTOM_LEFT;
    }
    else if(end_point.x() > start_point.x()  && end_point.y() < start_point.y())
    {
        node_direction = NodeDirection::BOTTOM_RIGHT;
    }
    else if(end_point.x() > start_point.x()  && end_point.y() > start_point.y())
    {
        node_direction = NodeDirection::TOP_RIGHT;
    }
    else
    {
        return;  // 如果绘制在一根线上就不要增加新的节点了。如果后续有需求修改就再改下逻辑。
    }

    // 确定图层信息
    ly::LayerInfo* layer_info = mp_pri->layer_mgr()->get_layer_info_by_name(LAYER_LOCATION_LAYER_NAME);

    // 确定参数
    sp::ShapeFactory* shapeIns = sp::ShapeFactory::instance();
    QMap<QString, sp::ShapeDecl*> shapeDecl = shapeIns->get_shapes_decl();

    auto itor = shapeDecl.find(SHAPE_RECTANGLE);
    Q_ASSERT(itor != shapeDecl.end());

    auto& shape_decl = itor.value();

    // 获取形状名称
    QString current_shape_name = shape_decl->name();
    // 获取形状资源路径
    auto param_list = shape_decl->param_list();

    QVector<pm::ParamDecl> params;
    for(auto param : param_list)
    {
        const auto& param_name = param.first;
        QVariant param_var = param.second;

        pm::ParamDecl new_param("", 0);
        new_param.set_key(param_name);
        if(SHAPE_RECTANGLE_WIDTH == param_name)
        {
            new_param.set_value(abs(start_point.x() - end_point.x()));
            new_param. set_expression(QString::number(abs(start_point.x() - end_point.x())));
        }
        else if(SHAPE_RECTANGLE_HEIGHT == param_name)
        {
            new_param.set_value(abs(start_point.y() - end_point.y()));
            new_param.set_expression(QString::number(abs(start_point.y() - end_point.y())));
        }
        else if(SHAPE_ROTATE == param_name)
        {
            new_param.set_value(0);
            new_param.set_expression(QString::number(0));
        }
        else
        {
            Q_ASSERT(false);  // HINT@leixunyong。走到这里来的时候说明有参数没有适配。
        }
        params.push_back(new_param);
    }
    add_new_tree_node(SHAPE_RECTANGLE, params, node_type, node_direction, NodeBooleanSubtractType::NONE, layer_info);
}

void PrimitiveWindow::update_current_primitive()
{
    mp_pri->tree_node_mgr()->update();
}

void PrimitiveWindow::delete_tree_nodes_with_layer_info(ly::LayerInfo *layer_info)
{
    if(nullptr == layer_info)
    {
        return;
    }

    auto root_node = mp_pri->at_root();
    if(root_node->children().size() != 1)
    {
        return;
    }

    QVector<AttachTreeNode*> ready_check_tree_nodes = root_node->children()[0];

    while(!ready_check_tree_nodes.empty())
    {
        bool find_tree_node = false;
        QVector<AttachTreeNode*> next_check_tree_nodes;
        for(auto tree_node : ready_check_tree_nodes)
        {
            auto this_tree_node_layer = tree_node->layer_info() ;
            if(this_tree_node_layer->layer_name() == layer_info->layer_name())
            {
                ready_check_tree_nodes.removeOne(tree_node);
                tree_node->parent_node()->remove_child(tree_node);
                delete tree_node;
                find_tree_node = true;
                break;
            }
            else
            {
                const auto& children =  tree_node->children();
                for(auto itor = children.begin(); itor != children.end(); itor++)
                {
                    const auto& tree_nodes = itor.value();
                    next_check_tree_nodes.append(tree_nodes);
                }
            }
        }

        if(!find_tree_node)
        {
            qSwap( next_check_tree_nodes, ready_check_tree_nodes);
        }
    }

    mp_pri_tree_widget->update_tree();
    return;
}

void PrimitiveWindow::record_mouse_left_button_click_point(const QPoint &pos)
{
    // HINT@leixunyong。pos是留给后面使用的。
    if(m_is_recording)
    {
        // 调整场景坐标
        QPointF logic_pos = current_scene_pos();
        QPointF adjusted_logic_pos = PriUtils::closest_grid_point(logic_pos, mp_pri->at_root(), true);

        ShapePointGraphicsItem* point_item = new ShapePointGraphicsItem(mp_pri->tree_node_mgr()->viewport()->map_to_scene(adjusted_logic_pos), nullptr, 0);
        mp_graphics_scene->addItem(point_item);
        QPointF curent_pos = adjusted_logic_pos;   // HINT@leixunyong。这个坐标不一定准确，因此需要更正
        if(m_is_shift_pressed)
        {
            QPointF last_point;
            if(!m_chosen_coord_points.isEmpty())
            {
                last_point = m_chosen_coord_points.last().first;
            }
            else
            {
                QPointF attach_point_in_scene = (ShapePointGraphicsItem::cachedItem ? ShapePointGraphicsItem::cachedItem : mp_pri->at_root() ->origin_point())->pos();
                last_point = mp_pri->tree_node_mgr()->viewport()->map_from_scene(attach_point_in_scene);
            }

            double delat_x = abs(last_point.x() - curent_pos.x());
            double delat_y = abs(last_point.y() - curent_pos.y());

            delat_x < delat_y ? curent_pos.setX(last_point.x()) : curent_pos.setY(last_point.y());
        }

        ShapePointGraphicsItem::cachedItem != nullptr ? mp_polygen_cache_point = ShapePointGraphicsItem::cachedItem :
            mp_polygen_cache_point = mp_pri->at_root()->origin_point();

        QPair<QPointF, sp::ShapePointGraphicsItem*> new_value = {curent_pos, point_item};
        m_chosen_coord_points.push_back(new_value);
        PriUtils::modify_preview_polygen_by_points(m_chosen_coord_points, mp_pri, m_preview_polygen);
    }
    update();
}

void PrimitiveWindow::align_points()
{
    // 获取当前点击的Action
    QAction* action = qobject_cast<QAction*>(sender());
    emit align_operation(action->text());
}

void PrimitiveWindow::rule_2_points_distance()
{
    if(mp_ruler_dialog)
    {
        delete mp_ruler_dialog;
    }

    mp_ruler_dialog = new PriRulerDialog(mp_pri, mp_graphics_view);
}

void PrimitiveWindow::setup_current_grid_resolution()
{
    QToolBar* function_tool_bar = findChild<QToolBar*>(PRI_FUNCTION_TOOL_BAR_NAME);
    Q_ASSERT(function_tool_bar);

    auto grid_resolution_action = function_tool_bar->findChild<QAction*> (PRI_GRID_RESOLUTION_ACTION_NAME);
    Q_ASSERT(grid_resolution_action);

    PriGridResolutionDialog dialog(grid_resolution_action);
    dialog.exec();
}

void PrimitiveWindow::cancel_current_operations()
{
    // 取消多边形 action
    if(m_is_recording == true)
    {
        m_is_recording = false;
        if(m_is_shift_pressed == true)
        {
            m_is_shift_pressed = false;
        }

        while(!m_chosen_coord_points.isEmpty())
        {
            auto pair = m_chosen_coord_points[0];
            mp_graphics_scene->removeItem(pair.second);
            m_chosen_coord_points.remove(0);
            m_preview_polygen.setPolygon(QPolygonF{});
        }

        setCursor(Qt::ArrowCursor);
    }
    // 取消测距action
    SAFE_DELETE(mp_ruler_dialog);
}

void PrimitiveWindow::finish_pickup_polygen_point()
{
    if(m_is_recording)
    {
        setCursor(Qt::ArrowCursor);
        newly_draw_shape_dialog();
    }
}

void PrimitiveWindow::connect_tree_node_with_mw()
{
    //TODO@leixunyong 临时代码
    std::function<void(at::AttachTreeNode*)> func;
    func = [&] (at::AttachTreeNode* tree_node)
    {
        connect_new_tree_node(tree_node);
        for(auto itor = tree_node->children().begin(); itor != tree_node->children().end(); itor++)
        {
            const auto& tree_nodes = itor.value();
            for(auto tree_node : tree_nodes)
            {
                func(tree_node);
            }
        }
        return;
    };

    if (mp_pri->at_root()->children().size() > 0)
    {
        const auto tree_nodes = mp_pri->at_root()->children().begin().value();
        for(auto tree_node : tree_nodes)
        {
            func(tree_node);
        }
        // 更新
        mp_pri_tree_widget->update_tree();
    }
}

void PrimitiveWindow::redo()
{
    cmd::CommandManager::instance()->redo();
}

void PrimitiveWindow::undo()
{
    cmd::CommandManager::instance()->undo();
}

void PrimitiveWindow::set_arc_length()
{
    bool ok = false;
    qreal arc_len = cm::ConfigManager::instance()->query(CM_ARC_LEN_KEY).toDouble();
    QString input =  QString::number(arc_len);

    QString text = QInputDialog::getText(nullptr, tr("设置弧长"), tr("弧长(um):"), QLineEdit::Normal, input, &ok);
    if (ok && !text.remove(' ').isEmpty())
    {
        qreal arc_len = text.toDouble(&ok);
        if(false == ok || arc_len < 0.001)
        {
            QMessageBox::warning(nullptr, "Warning", "your input is invalid!");
        }
        else
        {
            cm::ConfigManager::instance()->setup_value(CM_ARC_LEN_KEY, arc_len);
            mp_pri->at_root()->update();
            mp_pri->at_root()->tree_node_mgr()->update();
        }
    }
}

void PrimitiveWindow::set_rounded_corner_params()
{
    PriRounedCornerDialog dialog(mp_pri);
    dialog.exec();
}

void PrimitiveWindow::add_inner_rounded_corner_for_shape()
{
    auto widget_item = this->mp_pri_tree_widget->tree_widge_menu()->get_current_pickup_tree_widget_item();
    if(nullptr == widget_item)
    {
        return;
    }

    auto graphics_item = widget_item->graphics_item();
    if(nullptr == graphics_item)
    {
        return;
    }

    auto point_item = dynamic_cast<sp::ShapePointGraphicsItem*>(graphics_item);
    AttachTreeNode* tree_node = nullptr;
    if(point_item)
    {
        AttachTreeBaseNode* node = at::AttachTreeUtils::attach_tree_node_point_item_in(point_item, mp_pri->at_root());
        if(nullptr == node || node == mp_pri->at_root())
        {
            return;
        }

        tree_node = dynamic_cast<AttachTreeNode*>(node);
        if(nullptr == tree_node)
        {
            return;
        }

        int idx = AttachTreeUtils::point_item_index_in_tree_node(tree_node, point_item);
        const auto& shape_name = tree_node->shape_name();
        if( (shape_name != SHAPE_RECTANGLE) && (shape_name != SHAPE_TRIANGLE))
        {
            return;
        }

        if( shape_name == SHAPE_TRIANGLE && idx != 1)
        {
            return;
        }


        AttachTreeNode * delete_rectangle_node = AttachTreeUtils::create_inner_rounded_corner(point_item, mp_pri->at_root(), mp_pri->layer_mgr());

        AttachTreeUtils::rotate_rounded_corner(tree_node, idx, delete_rectangle_node);
    }
    else
    {
        auto shape_item = dynamic_cast<sp::ShapeDrawGraphicsItem*>(graphics_item);
        if(nullptr == shape_item)
        {
            return;
        }

        tree_node = AttachTreeUtils::attach_tree_node_shape_item_in(shape_item, mp_pri->at_root());
        if(nullptr == tree_node || tree_node->shape_name() != SHAPE_RECTANGLE)
        {
            return;
        }

        auto point_items = tree_node->point_items();
        for(auto idx = 0; idx < point_items.size(); idx++)
        {
            point_item = point_items[idx];

            AttachTreeNode * delete_rectangle_node = AttachTreeUtils::create_inner_rounded_corner(point_item, mp_pri->at_root(), mp_pri->layer_mgr());

            AttachTreeUtils::rotate_rounded_corner(tree_node, idx, delete_rectangle_node);
        }
    }

    emit mp_pri->signal_add_new_tree();

    tree_node->update();
    mp_pri->at_root()->tree_node_mgr()->update();
}

void PrimitiveWindow::add_outer_rounded_corner_for_shape()
{
    auto widget_item = this->mp_pri_tree_widget->tree_widge_menu()->get_current_pickup_tree_widget_item();
    if(nullptr == widget_item)
    {
        return;
    }

    auto graphics_item = widget_item->graphics_item();
    if(nullptr == graphics_item)
    {
        return;
    }

    auto point_item = dynamic_cast<sp::ShapePointGraphicsItem*>(graphics_item);
    AttachTreeNode* tree_node = nullptr;
    if(nullptr == point_item)
    {
        return;
    }

    AttachTreeBaseNode* node = at::AttachTreeUtils::attach_tree_node_point_item_in(point_item, mp_pri->at_root());
    if(nullptr == node || node == mp_pri->at_root())
    {
        return;
    }

    tree_node = dynamic_cast<AttachTreeNode*>(node);
    if(nullptr == tree_node)
    {
        return;
    }

    if(tree_node->node_type() != NodeType::ADD)
    {
        return;
    }

    int idx = AttachTreeUtils::point_item_index_in_tree_node(tree_node, point_item);
    const auto& shape_name = tree_node->shape_name();
    if( (shape_name != SHAPE_RECTANGLE) && (shape_name != SHAPE_TRIANGLE))
    {
        return;
    }

    if( shape_name == SHAPE_TRIANGLE && idx != 1)
    {
        return;
    }

    AttachTreeNode * add_rectangle_node = AttachTreeUtils::create_outer_rounded_corner(point_item, mp_pri->at_root(), mp_pri->layer_mgr());

    AttachTreeUtils::rotate_rounded_corner(tree_node, idx, add_rectangle_node);

    emit mp_pri->signal_add_new_tree();

    tree_node->update();
    mp_pri->at_root()->tree_node_mgr()->update();
}

void pr::PrimitiveWindow::on_dock_widget_resize()
{
    match_viewport();
}

QPointF PrimitiveWindow::current_scene_pos() const
{
    // 将视图中的坐标转换为场景坐标
    QString point_str = mp_mouse_pos_label->text();
    // 定义正则表达式模式，匹配两个浮点数
    QRegularExpression regex(R"(\(([-+]?\d*\.?\d+),\s*([-+]?\d*\.?\d+)\))");
    QRegularExpressionMatch match = regex.match(point_str);

    if (match.hasMatch())
    {
        bool ok1, ok2;
        double x = match.captured(1).toDouble(&ok1);
        double y = match.captured(2).toDouble(&ok2);
        return QPointF(x, y);
    }
    else
    {
        Q_ASSERT(false);
    }
}

void PrimitiveWindow::clear_cache()
{
    for(auto idx = 0; idx < m_chosen_coord_points.size(); idx++)
    {
        auto point_item = m_chosen_coord_points[idx].second;
        Q_ASSERT(point_item->scene() == mp_graphics_scene);
        mp_graphics_scene->removeItem(point_item);
        SAFE_DELETE(point_item);
    }
    m_chosen_coord_points.clear();
    m_preview_polygen.setPolygon(QPolygonF{});
}

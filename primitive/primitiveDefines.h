#ifndef PRIMITIVEDEFINES_H
#define PRIMITIVEDEFINES_H

// 定义图元绘制面板上的公用选项

// 工具栏
#define PRI_GERNERAL_TOOL_BAR_NAME                                 "通用工具栏"
// 绘图工具栏
#define PRI_DRAW_TOOL_BAR_NAME                                     "绘图工具栏"

#define PRI_ALIGN_TOOL_BAR_NAME                                    "对齐工具栏"

#define PRI_FUNCTION_TOOL_BAR_NAME                                 "功能工具栏"

// 以下是图形的面板控件名的宏定义
#define PRI_WINDOW_TITLE_PREFIX                                    "create"                        // 设置图形参数。

#define PRI_TYPE                                                   "type"                           // 类型
#define PRI_TYPE_TYPEOPTION                                        "type type option"               // 类型对应的下拉框名
#define PRI_TYPE_LOCATION                                          "location"                       // 定位
#define PRI_TYPE_ADD                                               "add"                            // 增加
#define PRI_TYPE_DELETE                                            "delete"                         // 删除

#define PRI_DIRECTION                                              "direction"                      // 方向
#define PRI_DIRECTION_TYPEOPTION                                   "direction type option"          // 类型对应的下拉框名
#define PRI_DIRECTION_TOP_LEFT                                     "top left"                       // 左上
#define PRI_DIRECTION_BOTTOM_LEFT                                  "bottom left"                    // 左下
#define PRI_DIRECTION_TOP_RIGHT                                    "top right"                      // 右上
#define PRI_DIRECTION_BOTTOM_RIGHT                                 "bottom right"                   // 右下

#define PRI_LAYER                                                  "layer"                          // 图层
#define PRI_LAYER_TYPEOPTION                                       "layer type option"              // 类型对应的下拉框名
#define PRI_LAYER_NONE                                             "none"                           // 无

#define PRI_OK                                                     "OK"                             // 确定
#define PRI_CANCEL                                                 "cancel"                         // 取消

#define PRI_DIALOG_WIDTH_SIZE                                      400
#define PRI_DIALOG_HEIGHT_SIZE                                     600

// Layer widget中的名称
#define PRI_LAYER_DOCK_WIDGET_NAME                                 "layer option"                   // 使用图层。左下角图层选项的字符串
#define PRI_LAYER_DOCK_WIDGET_LIST_NAME                            "layerinfolist"                  // 左下图层选项的list名

#define PRI_ROOT_NODE_NAME                                         "root node"                      // 根节点的名字

// PriGraphicsScene的名字
#define PRI_GRAPHICS_SCENE                                         "primitive graphics scene"       // PriGraphicsScene的名字

// 在图形区域展开右键菜单
#define PRI_RIGHT_CLICK_MENU_NAME                                  "context menu"                   // 右键菜单的名字
#define PRI_RIGHT_CLICK_DIALOG_WIDTH                               200                              // 右键菜单的宽度
#define PRI_RIGHT_CLICK_DIALOG_HEIGHT                              100                              // 右键菜单的高度

#define PRI_RIGHT_CLICK_MENU_PICKUP_SHAPE                          "pickup shape"                   // 右键菜单中选择图形的选项
#define PRI_RIGHT_CLICK_MENU_PICKUP_POINT                          "pickup point"                   // 右键菜单中选择点的选项

// TreeNode树节点展开的一些控件名
#define PRI_TREE_WIDGET_MENU_CUT_ACTION_NAME                       "cut"
#define PRI_TREE_WIDGET_MENU_COPY_ACTION_NAME                      "copy"
#define PRI_TREE_WIDGET_MENU_PASTE_ACTION_NAME                     "paste"

#define PRI_TREE_WIDGET_MENU_ANCHOR_POINT_ACTION_NAME              "set as anchor point"
#define PRI_TREE_WIDGET_MENU_COORD_POINT_ACTION_NAME               "set as coordinate point"
#define PRI_TREE_WIDGET_MENU_ROTATE_ACTION_NAME                    "rotate"

#define PRI_TREE_WIDGET_MENU_ATTRIBUTE_ACTION_NAME                 "attribute"

#define PRI_TREE_WIDGET_MENU_SHAPE_INFO_ACTION_NAME                "shape info"

// undo redo import save
#define PRI_UNDO_ACTION_NAME                                       "undo"
#define PRI_REDO_ACTION_NAME                                       "redo"
#define PRI_IMPORT_ACTION_NAME                                     "import"
#define PRI_SAVE_ACTION_NAME                                       "save"

// import menu sub menu
#define PRI_IMPORT_DIALOG_NAME                                     "import file"
#define PRI_IMPORT_DIALOG_WIDTH                                    300
#define PRI_IMPORT_DIALOG_HEIGHT                                   200

#define PRI_IMPORT_DIALOG_CATEGORY_NAME                            "category"
#define PRI_IMPORT_DIALOG_PRIMITIVE_FOLDER_NAME                    "primitive folder"

#define PRI_LEFT_ALIGN_ACTION_NAME                                 "left_align"
#define PRI_RIGHT_ALIGN_ACTION_NAME                                "right_align"
#define PRI_TOP_ALIGN_ACTION_NAME                                  "top_align"
#define PRI_BOTTOM_ALIGN_ACTION_NAME                               "bottom_align"

#define PRI_RULER_ACTION_NAME                                      "ruler"
#define PRI_RULER_DISTANCE_NAME                                    "ruler distance"
#define PRI_DIRECTION_LABEL_NAME                                   "direction"
#define PRI_EXPRESSION_LABEL_NAME                                  "expression"
#define PRI_VALUE_LABEL_NAME                                       "value"
#define PRI_X_DISTANCE_LABEL_NAME                                  "Δx"
#define PRI_X_DISTANCE_EXPRSSION_LINE_EDIT_NAME                    "Δx_expression"
#define PRI_X_DISTANCE_VALUE_LINE_EDIT_NAME                        "Δx_value"
#define PRI_Y_DISTANCE_LABEL_NAME                                  "Δy"
#define PRI_Y_DISTANCE_EXPRSSION_LINE_EDIT_NAME                    "Δy_expression"
#define PRI_Y_DISTANCE_VALUE_LINE_EDIT_NAME                        "Δy_value"
#define PRI_DISTANCE_LINE_EDIT_NAME                                "distance"
#define PRI_DISTANCE_EXPRESSION_LINE_EDIT_NAME                     "distance_expression"
#define PRI_DISTANCE_VALUE_LINE_EDIT_NAME                          "distance_value"
// others...
#endif // PRIMITIVEDEFINES_H

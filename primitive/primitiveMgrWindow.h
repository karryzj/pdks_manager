#ifndef PRIMITIVEMGRWINDOW_H
#define PRIMITIVEMGRWINDOW_H

#include <QMainWindow>
#include <QTreeWidget>
#include <QLabel>
#include <QPushButton>
#include <QListWidget>
#include <QTableWidget>
#include <QVector>
#include "attachTreeUtils.h"
#include "priCommon.h"

namespace ly
{
class LayerInfo;
}
namespace pm
{
class ParamDecl;
class Rule;
class PointE;
}
namespace pr
{
class Primitive;

class PRI_PUBLIC PrimitiveMgrWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit PrimitiveMgrWindow(QWidget *parent = nullptr);

private:
    QTreeWidget *mp_pri_tree_widget;
    QLabel      *mp_label;
    QPushButton *mp_edit_button;
    QListWidget *mp_layer_list;
    QTableWidget *mp_params_table;
    QTableWidget *mp_rules_table;
    QTableWidget *mp_anchor_table;
    Primitive *mp_current_pimitive;
    QString      m_primitive_name;

    void setup_ui();
    void load_dir_files();
    void fill_data(Primitive *primitive);
    void fill_thumbnail(const QString &img_file);
    void fill_layers(const QList<ly::LayerInfo*> &layers);
    void fill_params(const QList<pm::ParamDecl*> &params);
    void fill_rules(const QVector<pm::Rule> &rules);
    int find_small_dir(const QStringList &dir_list, const QString &prefix);
    void fill_anchors(QVector<at::AttachTreeUtils::AttachPointPosInf> &anchors);
    void clear_data();
private slots:
    void edit_primitive();
    void show_context_menu(const QPoint &pos);
    void add_cate();
    void add_tuyuan(QTreeWidgetItem *item);
    void delete_cute(QTreeWidgetItem *item);
    void delete_tuyuan(QTreeWidgetItem *item);
    void load_primitive(QTreeWidgetItem *item, int column);
    void change_name(QTreeWidgetItem *item, int column);
    void refresh_item();
};

}
#endif // PRIMITIVEMGRWINDOW_H

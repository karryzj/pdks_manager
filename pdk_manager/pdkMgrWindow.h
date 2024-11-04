#ifndef PDKMGRWINDOW_H
#define PDKMGRWINDOW_H

#include <QMainWindow>
#include <QObject>
#include <QListWidget>
#include <QTreeWidget>
#include <QTableView>
#include <QListView>
#include <QWidget>
#include "pdkCommon.h"

namespace pdk
{

class ComponentCenterWidget;
class DrcMgrWidget;
class PdkLayerMgrWidget;
class PdkInstance;

class PDK_PUBLIC PdkMgrWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit PdkMgrWindow(QWidget *parent = nullptr);
    ~PdkMgrWindow();

private:
    void setup_ui();
    void fill_data();
    void load_dir_files();
    void set_connect();
    void set_disconnect();

    PdkInstance         *mp_pdk_instance;
    PdkLayerMgrWidget   *mp_pdk_layer_mgr_widget;
    DrcMgrWidget        *mp_drc_mgr_widget;
    ComponentCenterWidget     *mp_center;
    QListWidget         *mp_pdk_list_widget;
    QTreeWidget         *mp_component_tree_widget;
    // QListView   *mp_drc_list_view;

private slots:
    void load_component_tree(QListWidgetItem *item);
    void load_cell_data(QTreeWidgetItem *item, int column);
    void show_list_menu(const QPoint &pos);
    void show_context_menu(const QPoint &pos);
    void add_pdk();
    void delete_pdk(QListWidgetItem *item);
    void add_lib();
    void delete_lib(QTreeWidgetItem *item);
    void add_pcell(QTreeWidgetItem *item);
    void delete_pcell(QTreeWidgetItem *item);
};

}

#endif // PDKMGRWINDOW_H

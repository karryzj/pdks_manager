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


    PdkLayerMgrWidget *mp_pdk_layer_mgr_widget;
    DrcMgrWidget    *mp_drc_mgr_widget;
    ComponentCenterWidget     *mp_center;
    QListWidget *mp_pdk_list_widget;
    QTreeWidget *mp_component_tree_widget;
    // QTableView  *mp_layer_table_view;
    // QListView   *mp_drc_list_view;

private slots:
    void load_component_tree(QListWidgetItem *item);
};

}

#endif // PDKMGRWINDOW_H

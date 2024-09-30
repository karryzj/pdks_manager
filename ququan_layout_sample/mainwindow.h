#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "gsiInterpreter.h"
#include "layDispatcher.h"
#include "dbCell.h"
#include "dbLayer.h"
#include "qtextformat.h"
#include "layBusy.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
class QPlainTextEdit;
class QTextCharFormat;
class QTreeWidgetItem;
QT_END_NAMESPACE


namespace qq
{
class DialogEditor;
}

namespace lay{
class DispatcherDelegate;
class Dispatcher;
class LayoutView;
class Layout;
class MacroEditorPage;
class MacroController;
}

namespace lym
{
class MacroCollection;
}

namespace db{
class Manager;
}


class MainWindow
    : public QMainWindow, public lay::BusyMode
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void init_python_IDE();
    static lay::LayoutView* get_view();
    static MainWindow* get_window();
    void update_cells();
    void update_libs();
    void update_layer_list();
    bool is_busy () const override;
    void enter_busy_mode (bool bm) override;


protected:
    void showEvent(QShowEvent *event) override;


private slots:
    void on_pb_add_clicked();

    void on_pb_load_clicked();

    void on_pb_save_clicked();

    void on_tw_cells_itemDoubleClicked(QTreeWidgetItem *item, int column);

    void on_pb_ide_clicked();

private:
    void addPlygon(unsigned int layer_idx);

    //void update_cells();
    void update_cell_children(db::Cell *parent, QTreeWidgetItem *parentWidget);

    void init_page_editor();

private:
    Ui::MainWindow *ui;
    lay::LayoutView *view;
    qq::DialogEditor *mp_dialog;
    bool m_busy;

};
#endif // MAINWINDOW_H

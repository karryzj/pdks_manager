#ifndef PRIPYTHONWINDOW_H
#define PRIPYTHONWINDOW_H

#include <QMainWindow>

#include "gsiInterpreter.h"
#include "layDispatcher.h"
#include "dbCell.h"
#include "dbLayer.h"
#include "qtextformat.h"
#include "layBusy.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class PriPythonWindow;
}
class QPlainTextEdit;
class QTextCharFormat;
class QTreeWidgetItem;
QT_END_NAMESPACE


namespace qq
{
class LayoutView;
class PageEditor;
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

namespace pr
{

class PriPythonWindow : public QMainWindow, public lay::BusyMode
{
    Q_OBJECT
public:
    PriPythonWindow(QWidget *parent,  const QString &py_path);
    ~PriPythonWindow();

    static lay::LayoutView* get_view();
    static PriPythonWindow* get_window();
    void update_layer_list();
    bool is_busy () const override;
    void enter_busy_mode (bool bm) override;

protected:
    void showEvent(QShowEvent *event) override;
    void closeEvent(QCloseEvent *event) override;

private:
    void init_page_editor();

private:
    qq::LayoutView *mp_qq_view;
    qq::PageEditor *mp_page_editor;
    Ui::PriPythonWindow *ui;
    lay::LayoutView *view;
    QString m_py_path;
    bool m_busy;

};

}

#endif // PRIPYTHONWINDOW_H

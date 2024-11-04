

#include <QPushButton>
#include <QHBoxLayout>
#include <QPlainTextEdit>
#include <QTreeWidgetItem>
#include <QDialog>

#include "priPythonWindow.h"
#include "common_defines.h"
#include "tlExceptions.h"
#include "tlProgress.h"
#include "tlScriptError.h"
#include "ui_priPythonWindow.h"

#include "layLayoutView.h"
#include "dbManager.h"
#include "layDispatcher.h"
#include "laybasicConfig.h"
#include "dbInit.h"
#include "layInit.h"
#include "dbLayout.h"
#include "dbLibrary.h"
#include "dbPCellDeclaration.h"
#include "pya.h"
#include "layMacroController.h"
#include "layMacroEditorPage.h"
#include "lymMacroCollection.h"
#include "dbLibraryManager.h"
#include "laySystemPaths.h"
#include "tlFileUtils.h"
#include "editor.h"
#include "libForceLink.h"
#include "layoutView.h"
#include "iconsForceLink.h"
#include "pmForceLink.h"
#include "dbWriter.h"
#include "commonForceLink.h"

using namespace pr;

static lay::LayoutView *mp_view=0;
static PriPythonWindow *mp_mainwindow=0;


static bool import_gds(QString input_gds, QString output_gds, QString cell_name, QString png_file, int width=500,int height=500)
{
    try
    {
        //加载gds，并将top cell的名称修改为cell_name，此处的cell_name应填元件的名称
        lay::LayoutView* layout_view = new lay::LayoutView((db::Manager*)0,true,0);
        unsigned int cv_index = layout_view->load_layout(input_gds.toStdString().c_str(),true);
        db::Layout *layout = &(layout_view->cellview(cv_index)->layout());
        db::Cell *top = &layout->cell (*(layout->begin_top_down ()));
        top->set_name(cell_name.toStdString().c_str());

        //设置样式
        layout_view->config_set("background-color", "#ffffff");
        layout_view->config_set("grid-visible", "false");
        layout_view->add_missing_layers();
        layout_view->max_hier();
        layout_view->zoom_fit();

        //保存缩略图
        layout_view->save_image(png_file.toStdString().c_str(),width,height);

        //保存gds
        db::SaveLayoutOptions options;
        options.clear_cells ();
        options.add_cell (top->cell_index ());
        options.set_format_from_filename (output_gds.toStdString().c_str());

        db::Writer writer (options);
        tl::OutputStream stream (output_gds.toStdString().c_str());
        writer.write (*layout, stream);
    }
    catch(...)
    {
        return false;
    }

    return true;
}


PriPythonWindow::PriPythonWindow(QWidget *parent, const QString &py_path)
    : QMainWindow(parent)
    , lay::BusyMode()
    , ui(new Ui::PriPythonWindow)
    , m_py_path(py_path)
    , mp_qq_view(nullptr)
    , mp_page_editor(nullptr)
    , m_busy(false)

{
    // 最大化主窗口
    showMaximized();
    setWindowModality(Qt::ApplicationModal);
    setWindowFlags(Qt::Window | Qt::WindowCloseButtonHint);


    ui->setupUi(this);
    mp_mainwindow=this;

    mp_qq_view = new qq::LayoutView(ui->fm_canvas);
    view = mp_qq_view->internal();
    mp_view = view;
    QString qs=tr("from ququan.library_helper import add_shape_to_cell,register_pdk_librarys\n"
                    "register_pdk_librarys('pdk1',True)\n");
    //qq::sp_interpreter->eval_string(qs.toStdString().c_str());
    //pya::PythonInterpreter::instance()->eval_string(qs.toStdString().c_str());

    /** UI **/
    init_page_editor();
    mp_qq_view->add_layout();
    update_layer_list();

    //import_gds(QString("d:/t1.gds"),QString("d:/t11.gds"),QString("xxtop"),QString("d:/t11.png"));
}


void PriPythonWindow::update_layer_list()
{
    this->ui->lw_layers->clear();
    db::Layout &layout = view->active_cellview()->layout();
    for(db::Layout::layer_iterator iterator = layout.begin_layers(); iterator != layout.end_layers(); ++iterator)
    {
        const db::LayerProperties *properties =  (*iterator).second;
        char *a = new char[20];
        sprintf(a, "layer %d/%d", properties->layer, properties->datatype);
        this->ui->lw_layers->addItem(a);

        // 通过python脚本调用 layout.layer(10,1)时，会自动新建一个图层，将新图层更新到view中
        /*std::vector <unsigned int> nl;
        int l=(*iterator).first;
        nl.push_back (l);
        view->add_new_layers(nl,view->active_cellview_index());*/
    }

    //view->transaction (tl::to_string (tr ("New layer")));
    //view->update_content ();
    //view->commit ();
}

bool PriPythonWindow::is_busy() const
{
    return m_busy;
}

void PriPythonWindow::enter_busy_mode(bool bm)
{
    m_busy = bm;
}


void PriPythonWindow::init_page_editor()
{
    mp_page_editor = new qq::PageEditor(ui->fm_edit_page, m_py_path.toStdString());
}



PriPythonWindow::~PriPythonWindow()
{

}


void PriPythonWindow::showEvent(QShowEvent *event)
{
    //update_libs();
}

void PriPythonWindow::closeEvent(QCloseEvent *event)
{
    SAFE_DELETE(mp_qq_view);    //internal view 同步释放

    SAFE_DELETE(mp_page_editor);

    mp_view = nullptr;
    mp_mainwindow = nullptr;
}




lay::LayoutView* PriPythonWindow::get_view()
{
    return mp_view;
}

PriPythonWindow* PriPythonWindow::get_window()
{
    return mp_mainwindow;
}


namespace gsi
{
    extern LAYBASIC_PUBLIC gsi::Class<lay::LayoutViewBase> decl_LayoutViewBase;

    static gsi::Class<lay::LayoutView> decl_MyView (decl_LayoutViewBase,"lay", "MyView",
       gsi::method ("view", &PriPythonWindow::get_view,"@brief test\n")+
       gsi::method ("mywindow", &PriPythonWindow::get_window,"@brief test\n"),
       "@brief A box class with floating-point coordinates\n"
       );

    static void refresh(PriPythonWindow* win)
    {
        win->update_layer_list();
    }

    gsi::Class<PriPythonWindow> decl_MyWindow("lay","MyMainWindow",
        gsi::method_ext ("refresh", &refresh,"@brief test\n"),
      "@brief A box class with floating-point coordinates\n"
        );
}








#include <QPushButton>
#include <QHBoxLayout>
#include <QPlainTextEdit>
#include <QTreeWidgetItem>
#include <QDialog>

#include "mainwindow.h"
#include "tlExceptions.h"
#include "tlProgress.h"
#include "tlScriptError.h"
#include "ui_mainwindow.h"

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

static lay::LayoutView *mp_view=0;
static MainWindow *mp_mainwindow=0;


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


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , lay::BusyMode()
    , ui(new Ui::MainWindow)
    , mp_dialog(nullptr)
    , m_busy(false)

{
    ui->setupUi(this);
    mp_mainwindow=this;

    qq::LayoutView::init({"Python"}, true);

    qq::LayoutView *qq_view = new qq::LayoutView(ui->fm_canvas);
    view = qq_view->internal();
    mp_view = view;
    QString qs=tr("from ququan.library_helper import add_shape_to_cell,register_pdk_librarys\n"
                    "register_pdk_librarys('pdk1',True)\n");
    //qq::sp_interpreter->eval_string(qs.toStdString().c_str());
    //pya::PythonInterpreter::instance()->eval_string(qs.toStdString().c_str());

    /** UI **/
    init_page_editor();
    qq_view->add_layout();
    update_layer_list();
    update_cells();
    update_libs();

    //import_gds(QString("d:/t1.gds"),QString("d:/t11.gds"),QString("xxtop"),QString("d:/t11.png"));
}


void MainWindow::addPlygon(unsigned int layer_idx)
{
    std::vector <db::Point> points;
    points.reserve (8);


    points.push_back (db::Point (278, 382));
    points.push_back (db::Point (67, 389));
    points.push_back (db::Point (72, 499));
    points.push_back (db::Point (147, 586));
    points.push_back (db::Point (341, 640));
    points.push_back (db::Point (346, 640));
    points.push_back (db::Point (565, 438));
    points.push_back (db::Point (278, 382));

    //  Produce the shape
    db::SimplePolygon poly;
    poly.assign_hull (points.begin (), points.end ());

    view->active_cellview().cell()->shapes(layer_idx).insert(poly);

}

void MainWindow::update_layer_list()
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

void MainWindow::update_cells()
{
    this->ui->tw_cells->clear();

    std::list<lay::CellView> cellViews = view->cellview_list();

    db::Layout &layout = view->active_cellview()->layout();


    for( db::Layout::top_down_iterator i = layout.begin_top_down(); i != layout.end_top_cells(); ++i)
    {
        db::cell_index_type idx = *i;
        QTreeWidgetItem *itm = new QTreeWidgetItem();
        db::Cell &cell = view->active_cellview()->layout().cell(idx);
        itm->setData(0, Qt::UserRole, QVariant(idx));
        itm->setText(0,  (std::to_string(idx) + " " + cell.get_display_name()).c_str());
        this->ui->tw_cells->addTopLevelItem(itm);

        update_cell_children(&cell, itm);
    }

}

bool MainWindow::is_busy() const
{
    return m_busy;
}

void MainWindow::enter_busy_mode(bool bm)
{
    m_busy = bm;
}



void MainWindow::update_cell_children(db::Cell *parent, QTreeWidgetItem *parentWidget)
{
    if(!parent || !parentWidget)
    {
        return;
    }
    for(db::Cell::child_cell_iterator i = parent->begin_child_cells(); ! i.at_end(); ++i)
    {
        db::cell_index_type idx = *i;
        QTreeWidgetItem *itm = new QTreeWidgetItem(parentWidget);
        db::Cell &cell = view->active_cellview()->layout().cell(idx);
        itm->setData(0, Qt::UserRole, QVariant(idx));
        itm->setText(0,  (std::to_string(idx) + " " + cell.get_display_name()).c_str());
        update_cell_children(&cell, itm);
    }
}

//choose a cell, as a top cell. Then change the view.
void MainWindow::on_tw_cells_itemDoubleClicked(QTreeWidgetItem *item, int column)
{
    lay::LayoutViewBase::cell_path_type path;
    while (item) {
        path.push_back (item->data(0, Qt::UserRole).toInt());
        item = item->parent ();
    }

    if (! path.empty ()) {
        std::reverse (path.begin (), path.end ());
    }

    view->select_cell_fit(path, view->active_cellview_index());
    //设置到最大
    view->max_hier();
    // or use
    //view->set_hier_levels(xxx)

}


void MainWindow::update_libs()
{
    this->ui->tw_libs->clear();
    for (db::LibraryManager::iterator i = db::LibraryManager::instance ().begin (); i != db::LibraryManager::instance ().end (); ++i) {
        db::Library *lib = db::LibraryManager::instance ().lib (i->second);
        QTreeWidgetItem *itm = new QTreeWidgetItem();
        itm->setText(0, lib->get_name().c_str());
        for(db::Layout::pcell_iterator j = lib->layout().begin_pcells(); j != lib->layout().end_pcells(); ++j)
        {
            QTreeWidgetItem *child = new QTreeWidgetItem(itm);
            child->setText(0, j->first.c_str());
        }

        //增加直接通过read gds得到的cell
        auto layout=&(lib->layout());
        db::Layout::top_down_const_iterator top = layout->begin_top_down ();
        while (top != layout->end_top_down ()) {

            if (layout->cell (*top).is_top ()) {
                if ( ! layout->cell (*top).is_proxy ()) {
                    QTreeWidgetItem *child = new QTreeWidgetItem(itm);
                    child->setText(0, layout->cell (*top).get_display_name().c_str());
                }
            }

            ++top;

        }

        this->ui->tw_libs->addTopLevelItem(itm);
    }


}

void MainWindow::init_page_editor()
{
    qq::PageEditor *page_editor = new qq::PageEditor(ui->fm_edit_page);
}



MainWindow::~MainWindow()
{
    delete view;

    delete ui;
}


void MainWindow::showEvent(QShowEvent *event)
{
    //update_libs();
}



void MainWindow::on_pb_add_clicked()
{
    //添加plygon
    addPlygon(1);

    update_layer_list();
}


void MainWindow::on_pb_load_clicked()
{
    std::string gdsPath = "C:/Users/yanlin/Desktop/tmp/test2.gds";

    qq::LayoutView::current()->load_layout(gdsPath);

    update_layer_list();
    update_cells();
}


void MainWindow::on_pb_save_clicked()
{

}



lay::LayoutView* MainWindow::get_view()
{
    return mp_view;
}

MainWindow* MainWindow::get_window()
{
    return mp_mainwindow;
}



void MainWindow::on_pb_ide_clicked()
{
    if( !mp_dialog)
    {
        mp_dialog = new qq::DialogEditor();
    }
    mp_dialog->show();
}



namespace gsi
{
    extern LAYBASIC_PUBLIC gsi::Class<lay::LayoutViewBase> decl_LayoutViewBase;

    static gsi::Class<lay::LayoutView> decl_MyView (decl_LayoutViewBase,"lay", "MyView",
       gsi::method ("view", &MainWindow::get_view,"@brief test\n")+
       gsi::method ("mywindow", &MainWindow::get_window,"@brief test\n"),
       "@brief A box class with floating-point coordinates\n"
       );

    static void refresh(MainWindow* win)
    {
        win->update_cells();
        win->update_libs();
        win->update_layer_list();
    }

    gsi::Class<MainWindow> decl_MyWindow("lay","MyMainWindow",
        gsi::method_ext ("refresh", &refresh,"@brief test\n"),
      "@brief A box class with floating-point coordinates\n"
        );
}



/*
# python ide sample:
import sys
import pya
import klayout
import klayout.lay

#print(sys.version)
print(klayout.lay.MyView.view())
view=klayout.lay.MyView.view()

layout=klayout.lay.MyView.view().cellview(view.active_cellview_index()).layout()
top=layout.top_cells()[0]
print(top)

# create a CIRCLE from library
lib_basic=pya.Library.library_by_name("Basic")
params={"actual_radius":12.0,"layer":layout.get_info(layout.layer(2, 0)), "npoints":101}
cell=layout.create_cell("CIRCLE","Basic",params)
top.insert(pya.CellInstArray(cell.cell_index(), pya.Trans()))


# create a polygon shape
polys=pya.DPolygon([pya.DPoint(1,1),pya.DPoint(1,10),pya.DPoint(10,30)])
view.cellview(view.active_cellview_index()).cell.shapes(layout.layer(1, 0)).insert(polys)
view.max_hier()

klayout.lay.MyView.mywindow().refresh()
 * */





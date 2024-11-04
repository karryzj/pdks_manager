#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QFileDialog>
#include "gdsModeWidget.h"
#include "pdkInstance.h"

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

namespace pdk
{
GdsModeWidget::GdsModeWidget(PdkInstance *pdk_instance, QWidget *parent)
    : QWidget(parent), mp_pdk_instance(pdk_instance)
{
    setupUi();
}

GdsModeWidget::~GdsModeWidget()
{

}

void GdsModeWidget::fill_data()
{
    QString export_gds_name = mp_pdk_instance->path() + "/data.gds";
    QString png_name = mp_pdk_instance->path() + "/thumbnail.png";

    QPixmap pixmap(png_name);
    mp_thumbnail_label->setPixmap(pixmap.scaled(mp_thumbnail_label->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
    mp_file_name_label->setText(QFileInfo(export_gds_name).fileName());
    mp_open_gds_button->setEnabled(false);
}

void GdsModeWidget::clear_view()
{
    mp_open_gds_button->setEnabled(true);
    mp_thumbnail_label->clear();
    mp_file_name_label->clear();
}

void GdsModeWidget::setupUi()
{
    // 创建表头（Header）
    QWidget *header = new QWidget(this);
    QHBoxLayout *headerLayout = new QHBoxLayout(header);

    mp_open_gds_button = new QPushButton(this);
    mp_open_gds_button->setIcon(QIcon(":/img/add.png"));
    connect(mp_open_gds_button, &QPushButton::clicked, this, &GdsModeWidget::open_file_dialog);
    // header->setStyleSheet("QWidget {background-color: #1E1E1E; color: white;}");

    // 设置缩略图标签
    mp_thumbnail_label = new QLabel();
    mp_thumbnail_label->setFixedSize(380, 280);
    mp_thumbnail_label->setAlignment(Qt::AlignCenter);
    mp_thumbnail_label->setStyleSheet("border: 1px solid black");
    // 调整布局，右对齐按钮
    headerLayout->addStretch();
    headerLayout->addWidget(mp_open_gds_button);
    headerLayout->addStretch();
    headerLayout->addWidget(mp_thumbnail_label);
    header->setLayout(headerLayout);

    // 设置文件名标签
    mp_file_name_label = new QLabel();
    mp_file_name_label->setAlignment(Qt::AlignCenter);

    QVBoxLayout *layout = new QVBoxLayout();
    layout->addWidget(header);
    layout->addWidget(mp_file_name_label);

    setLayout(layout);
}

void GdsModeWidget::open_file_dialog()
{
    QString filePath = QFileDialog::getOpenFileName(this, "Open File", "", "GDS (*.gds *.gdsii);;All Files (*)");
    if (!filePath.isEmpty())
    {
        QString export_gds_name = mp_pdk_instance->path() + "/data.gds";
        QString png_name = mp_pdk_instance->path() + "/thumbnail.png";

        QString path = mp_pdk_instance->path();
        QFileInfo fileInfo(path);
        QString cell_name = fileInfo.fileName();

        import_gds(filePath, export_gds_name, cell_name, png_name);
        QPixmap pixmap(png_name);
        mp_thumbnail_label->setPixmap(pixmap.scaled(mp_thumbnail_label->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
        mp_file_name_label->setText(QFileInfo(export_gds_name).fileName());
    }
}

}

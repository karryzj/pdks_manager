#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTableView>
#include <QListView>
#include <QLabel>
#include <QDir>
#include <QDebug>
#include "priModeWidget.h"
#include "priSelectDelegate.h"
#include "pdkDefines.h"
#include "layerSelectWidget.h"
#include "pdkParamTechItemDelegate.h"
#include "pdkParamTechRuleWidget.h"
#include "techRuleWidget.h"
#include "primitiveInfoWidget.h"
#include "pdkInstance.h"
#include "layerDefines.h"
#include "layerMgr.h"
#include "layInfo.h"
#include "gdsLayerInfo.h"

namespace pdk
{
PriModeWidget::PriModeWidget(PdkInstance *pdk_instance, QWidget *parent)
    : QWidget(parent), mp_pdk_instance(pdk_instance)
{
    setupUi();
    fillData();
}

PriModeWidget::~PriModeWidget()
{

}

void PriModeWidget::set_disable_primitive_relation(bool enable)
{
    mp_pri_lib_combo_box->setEnabled(enable);
    mp_pri_name_combo_box->setEnabled(enable);
}

void PriModeWidget::setupUi()
{
    QHBoxLayout *all_layout = new QHBoxLayout();

    // 左侧布局
    QVBoxLayout *left_layout = new QVBoxLayout();
    QWidget *left_top_widget = new QWidget();
    QHBoxLayout *left_top_layout = new QHBoxLayout();
    QHBoxLayout *left_bottom_layout = new QHBoxLayout();
    QVBoxLayout *left_bottom_right_layout = new QVBoxLayout();

    // 左上选择图元
    QLabel *label = new QLabel(tr(PDK_SELECT_PRI_NAME));
    label->setText(tr(PDK_SELECT_PRI_NAME));
    label->setFixedHeight(50);
    label->setAlignment(Qt::AlignRight | Qt::AlignVCenter); // 设置右对齐和垂直居中对齐
    mp_pri_lib_combo_box = new QComboBox();
    // 创建一个默认的 QStandardItem
    QStandardItemModel *lib_model = qobject_cast<QStandardItemModel *>(mp_pri_lib_combo_box->model());
    QStandardItem *lib_item = new QStandardItem(tr("图元分类"));
    lib_item->setSelectable(false); // 设置为不可选择
    lib_model->insertRow(0, lib_item);
    mp_pri_lib_combo_box->setStyleSheet("QComboBox { height: 50px; }"); // 使用样式表设置高度
    connect(mp_pri_lib_combo_box, &QComboBox::currentTextChanged, this, &PriModeWidget::fillPriName);

    // mp_pri_lib_combo_box->setPlaceholderText("text");
    mp_pri_name_combo_box = new QComboBox();
    QStandardItemModel *name_model = qobject_cast<QStandardItemModel *>(mp_pri_name_combo_box->model());
    QStandardItem *name_item = new QStandardItem(tr("图元名称"));
    name_item->setSelectable(false); // 设置为不可选择
    name_model->insertRow(0, name_item);
    mp_pri_name_combo_box->setStyleSheet("QComboBox { height: 50px; }"); // 使用样式表设置高度
    connect(mp_pri_name_combo_box, QOverload<int>::of(&QComboBox::activated), this, &PriModeWidget::fillCellData);
    // left_top_layout->addStretch();
    left_top_layout->addWidget(label);
    left_top_layout->addWidget(mp_pri_lib_combo_box);
    left_top_layout->addWidget(mp_pri_name_combo_box);
    // left_top_layout->addStretch();
    left_top_widget->setLayout(left_top_layout);
    left_top_widget->setFixedHeight(80);

    mp_layer_select_widget = new LayerSelectWidget(mp_pdk_instance);
    left_bottom_layout->addWidget(mp_layer_select_widget);

    mp_pdk_param_tech_rule_widget = new PdkParamTechRuleWidget(mp_pdk_instance);
    mp_tech_rule_widget = new TechRuleWidget(mp_pdk_instance);

    left_bottom_right_layout->addWidget(mp_pdk_param_tech_rule_widget);
    left_bottom_right_layout->addWidget(mp_tech_rule_widget);
    left_bottom_layout->addLayout(left_bottom_right_layout);

    left_layout->addWidget(left_top_widget);
    left_layout->addLayout(left_bottom_layout);

    // 右侧布局
    QVBoxLayout *right_layout = new QVBoxLayout();
    mp_pri_info_widget = new PrimitiveInfoWidget(mp_pdk_instance);
    right_layout->addWidget(mp_pri_info_widget);

    all_layout->addLayout(left_layout);
    all_layout->addLayout(right_layout);
    // all_layout->addWidget(tableView);
    setLayout(all_layout);
    // setStyleSheet("background-color: #2E2E2E; color: white;");
}

void PriModeWidget::fillData()
{
    QString current_path = QDir::currentPath() + "/data/primitives";
    QDir current_dir(current_path);
    if (!current_dir.exists())
    {
        exit(1);
    }

    // 获取当前目录下所有子目录
    mp_pri_lib_combo_box->clear();
    QStringList dirstr_list = current_dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
    if (!dirstr_list.isEmpty())
    {
        mp_pri_lib_combo_box->addItems(dirstr_list);
        // mp_pri_lib_combo_box->setCurrentText(dirstr_list.at(0));
    }
    int idx = mp_pri_lib_combo_box->findText(mp_pdk_instance->cell_relation().first);
    mp_pri_lib_combo_box->setCurrentIndex(idx);
}

void PriModeWidget::fillUiData()
{
    QStringList layer_list;

    if (mp_pdk_instance->gds_layer_info().size() > 0)
    {
        auto layers = mp_pdk_instance->gds_layer_info().keys();
        for (auto layer : layers)
        {
            layer_list.append(layer);
        }
    }
    else
    {
        auto layer_infos = mp_pdk_instance->layer_mgr()->get_layers();
        for (auto layer : layer_infos)
        {
            if (layer->layer_name() == tr(LAYER_LOCATION_LAYER_NAME))
                continue;
            layer_list.append(layer->layer_name());
        }
    }

    QStringList gds_list;
    auto gds_layers = mp_pdk_instance->pdk_gds_layers();
    foreach (auto gds_layer, gds_layers)
    {
        gds_list.append(gds_layer->layer_name);
    }
    fillData();
    mp_layer_select_widget->set_layer_list(layer_list);
    mp_layer_select_widget->set_gds_list(gds_list);
    mp_layer_select_widget->FillData();
    mp_pdk_param_tech_rule_widget->fillData();
    mp_tech_rule_widget->fill_data();
    mp_pri_info_widget->fill_data();
}

void PriModeWidget::clear_view()
{
    // mp_pri_lib_combo_box->clear();
    // mp_pri_name_combo_box->clear();
    mp_layer_select_widget->clear_view();
    mp_pdk_param_tech_rule_widget->clear_view();
    mp_tech_rule_widget->clear_view();
    mp_pri_info_widget->clear_view();
}

bool PriModeWidget::removeDir(const QString &dirPath)
{
    QDir dir(dirPath);
    if (!dir.exists())
    {
        return true;
    }

    QFileInfoList dir_file_list = dir.entryInfoList(QDir::NoDotAndDotDot | QDir::AllEntries);
    for (QFileInfo info : dir_file_list)
    {
        if (info.isDir())
        {
            if (!removeDir(info.absoluteFilePath()))
            {
                return false;
            }
        }
        else
        {
            if (!QFile::remove(info.absoluteFilePath()))
            {
                return false;
            }
        }
    }
    return dir.rmdir(dirPath);
}

bool PriModeWidget::copyDirContents(const QString &srcPath, const QString &dstPath)
{
    QDir srcDir(srcPath);
    if (!srcDir.exists())
    {
        qWarning() << "Source directory does not exist:" << srcPath;
        return false;
    }

    QDir dstDir(dstPath);
    if (dstDir.exists())
    {
        removeDir(dstPath);
    }
    if (!dstDir.mkpath(dstPath))
    {
        qWarning() << "Failed to create destination directory:" << dstPath;
        return false;
    }

    foreach (QString fileName, srcDir.entryList(QDir::Files))
    {
        QString srcFilePath = srcDir.filePath(fileName);
        QString dstFilePath = dstDir.filePath(fileName);

        if (QFile::exists(dstFilePath))
        {
            if (!QFile::remove(dstFilePath))
            {
                qWarning() << "Failed to remove existing file:" << dstFilePath;
                return false;
            }
        }

        if (!QFile::copy(srcFilePath, dstFilePath))
        {
            qWarning() << "Failed to copy file:" << srcFilePath << "to" << dstFilePath;
            return false;
        }
    }

    foreach (QString dirName, srcDir.entryList(QDir::Dirs | QDir::NoDotAndDotDot))
    {
        QString srcDirPath = srcDir.filePath(dirName);
        QString dstDirPath = dstDir.filePath(dirName);
        if (!copyDirContents(srcDirPath, dstDirPath))
        {
            return false;
        }
    }

    return true;
}

void PriModeWidget::fillPriName(const QString &text)
{
    QString current_path = QDir::currentPath() + "/data/primitives" + "/" + text;
    QDir current_dir(current_path);
    if (!current_dir.exists())
    {
        exit(1);
    }

    // 获取当前目录下所有子目录
    QStringList dirstr_list = current_dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
    if (!dirstr_list.isEmpty())
    {
        mp_pri_name_combo_box->clear();
        mp_pri_name_combo_box->addItem(tr("图元名称"));
        mp_pri_name_combo_box->setItemData(0, 0, Qt::UserRole - 1);
        mp_pri_name_combo_box->addItems(dirstr_list);
        // mp_pri_name_combo_box->setCurrentText(dirstr_list.at(0));
    }

    int idx = mp_pri_name_combo_box->findText(mp_pdk_instance->cell_relation().second);
    mp_pri_name_combo_box->setCurrentIndex(idx);
}

// void PriModeWidget::fillCellData(const QString &text)
// {
//     if (text.isEmpty())
//         return;
//     QString pri_lib = mp_pri_lib_combo_box->currentText();
//     QString current_path = QDir::currentPath() + "/data/primitives" + "/" + pri_lib + "/" + text;
//     QString dst_path = mp_pdk_instance->path();

//     bool ret = copyDirContents(current_path, dst_path);
//     if (ret)
//     {
//         QString pcell_file = dst_path + "/data.json";
//         mp_pdk_instance->load_pcell_file(pcell_file);
//     }
// }

void PriModeWidget::fillCellData(int idx)
{
    if (idx == 0)
        return;
    QString pri_lib = mp_pri_lib_combo_box->currentText();
    QString pri_name = mp_pri_name_combo_box->itemText(idx);
    QString current_path = QDir::currentPath() + "/data/primitives" + "/" + pri_lib + "/" + pri_name;
    QString dst_path = mp_pdk_instance->path();
    mp_pdk_instance->set_cell_relation(pri_lib, pri_name);

    bool ret = copyDirContents(current_path, dst_path);
    if (ret)
    {
        mp_pdk_instance->reset_data();
        QString pri_json = "data.json";
        QString py_json = "info.json";
        QDir dst_dir(dst_path);
        QStringList files_list = dst_dir.entryList(QDir::Files | QDir::NoDotAndDotDot);
        if (files_list.contains(pri_json))
        {
            QString pcell_file = dst_path + "/" + pri_json;
            mp_pdk_instance->load_pcell_file(pcell_file);
            mp_pdk_instance->init_extra_param_mgr();
            mp_pdk_instance->init_total_param_mgr();
            fillUiData();
        }
        else if (files_list.contains(py_json))
        {
            QString pcell_file = dst_path + "/" + py_json;
            mp_pdk_instance->load_py_json_file(pcell_file);
            mp_pdk_instance->init_extra_param_mgr();
            mp_pdk_instance->init_total_param_mgr();
            fillUiData();
        }

    }
}

}

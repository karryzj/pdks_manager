#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTableView>
#include <QListView>
#include <QLabel>
#include "priModeWidget.h"
#include "priSelectDelegate.h"
#include "pdkDefines.h"
#include "layerSelectWidget.h"
#include "pdkParamTechItemDelegate.h"
#include "pdkParamTechRuleWidget.h"
#include "techRuleWidget.h"
#include "primitiveInfoWidget.h"

namespace pdk
{
PriModeWidget::PriModeWidget(QWidget *parent)
    : QWidget{parent}
{
    setupUi();
}

PriModeWidget::~PriModeWidget()
{

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
    mp_pri_lib_combo_box = new QComboBox();
    mp_pri_name_combo_box = new QComboBox();
    mp_pri_save_button = new QToolButton();
    mp_pri_save_button->setText(tr(PDK_SAVE_PRI_NAME));
    mp_pri_save_button->setIcon(QIcon(":/img/pdk_save.png"));
    mp_pri_save_button->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    mp_export_gds_button = new QToolButton();
    mp_export_gds_button->setText(tr(PDK_EXPORT_GDS_NAME));
    mp_export_gds_button->setIcon(QIcon(":/img/export_gds.png"));
    mp_export_gds_button->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);

    left_top_layout->addWidget(label);
    left_top_layout->addWidget(mp_pri_lib_combo_box);
    left_top_layout->addWidget(mp_pri_name_combo_box);
    left_top_layout->addWidget(mp_pri_save_button);
    left_top_layout->addWidget(mp_export_gds_button);
    left_top_widget->setLayout(left_top_layout);
    left_top_widget->setFixedHeight(50);

    QStringList layer_list;
    layer_list << "Layer1" << "Layer2" << "Layer3" << "Layer4";

    QStringList gds_list;
    gds_list << "M1" << "M2" << "M3" << "M4";

    mp_layer_select_widget = new LayerSelectWidget(layer_list, gds_list);
    left_bottom_layout->addWidget(mp_layer_select_widget);

    mp_pdk_param_tech_rule_widget = new PdkParamTechRuleWidget();
    mp_tech_rule_widget = new TechRuleWidget();

    left_bottom_right_layout->addWidget(mp_pdk_param_tech_rule_widget);
    left_bottom_right_layout->addWidget(mp_tech_rule_widget);
    left_bottom_layout->addLayout(left_bottom_right_layout);

    left_layout->addWidget(left_top_widget);
    left_layout->addLayout(left_bottom_layout);

    // 右侧布局
    QVBoxLayout *right_layout = new QVBoxLayout();
    PrimitiveInfoWidget *pri_info_widget = new PrimitiveInfoWidget();
    right_layout->addWidget(pri_info_widget);

    all_layout->addLayout(left_layout);
    all_layout->addLayout(right_layout);
    // all_layout->addWidget(tableView);
    setLayout(all_layout);
    // setStyleSheet("background-color: #2E2E2E; color: white;");
}

}

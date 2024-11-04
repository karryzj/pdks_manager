#include <QVBoxLayout>
#include <QLabel>
#include <QDir>
#include <QFileDialog>
#include "pdkDefines.h"
#include "pdkInstance.h"
#include "priModeWidget.h"
#include "gdsModeWidget.h"
#include "componentCenterWidget.h"
#include "PythonMethod.h"

namespace pdk
{
ComponentCenterWidget::ComponentCenterWidget(PdkInstance *mp_pdk_instance, QWidget *parent)
    : QWidget(parent), mp_pdk_instance(mp_pdk_instance), m_isOn(false)
{
    setupUi();
}

void ComponentCenterWidget::setupUi()
{
    QVBoxLayout *layout = new QVBoxLayout();

    QHBoxLayout *hlayout = new QHBoxLayout();

    QLabel *pri_label = new QLabel(tr(PDK_PRI_PYTHON_NAME));
    QLabel *gds_label = new QLabel(tr(PDK_GDS_FILE_NAME));
    // 添加开关按钮
    mp_switchButton = new QToolButton();
    mp_switchButton->setIcon(QIcon(":/img/switch_pri.png"));
    mp_switchButton->setIconSize(QSize(40, 20));  // 设置图标大小
    mp_switchButton->setCheckable(true);  // 使按钮可切换
    mp_switchButton->setChecked(m_isOn);  // 初始状态
    connect(mp_switchButton, &QToolButton::clicked, this, &ComponentCenterWidget::toggleMode);

    mp_pri_save_button = new QToolButton();
    mp_pri_save_button->setText(tr(PDK_SAVE_PRI_NAME));
    mp_pri_save_button->setIcon(QIcon(":/img/pdk_save.png"));
    mp_pri_save_button->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    connect(mp_pri_save_button, &QToolButton::clicked, this, &ComponentCenterWidget::savePrimitive);
    mp_export_gds_button = new QToolButton();
    mp_export_gds_button->setText(tr(PDK_EXPORT_GDS_NAME));
    mp_export_gds_button->setIcon(QIcon(":/img/export_gds.png"));
    mp_export_gds_button->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    connect(mp_export_gds_button, &QToolButton::clicked, this, &ComponentCenterWidget::exportGds);

    hlayout->addStretch();
    hlayout->addWidget(pri_label);
    hlayout->addWidget(mp_switchButton);
    hlayout->addWidget(gds_label);
    hlayout->addStretch();
    hlayout->addWidget(mp_pri_save_button);
    hlayout->addWidget(mp_export_gds_button);
    // hlayout->addStretch();

    // 创建 QStackedWidget 来管理不同模式
    mp_stackedWidget = new QStackedWidget(this);

    // 添加模式1
    mp_pri_mode_widget = new PriModeWidget(mp_pdk_instance);
    // QVBoxLayout *mode1Layout = new QVBoxLayout(mp_pri_mode_widget);
    // mode1Layout->addWidget(new QLabel("", mp_pri_mode_widget));
    mp_stackedWidget->addWidget(mp_pri_mode_widget);

    // 添加模式2
    mp_gds_mode_widget = new GdsModeWidget(mp_pdk_instance);
    // QVBoxLayout *mode2Layout = new QVBoxLayout(mp_gds_mode_widget);
    // mode2Layout->addWidget(new QLabel("", mp_gds_mode_widget));
    mp_stackedWidget->addWidget(mp_gds_mode_widget);


    // 将组件添加到布局
    layout->addLayout(hlayout);
    layout->addWidget(mp_stackedWidget);
    // 设置控件间距为10个像素
    layout->setSpacing(10);
    setLayout(layout);

}

void ComponentCenterWidget::set_switch_enable(bool enable)
{
    mp_switchButton->setEnabled(enable);
}

void ComponentCenterWidget::set_switch_mode(int mode)
{
    if (mode)
    {
        mp_switchButton->setChecked(true);
        mp_stackedWidget->setCurrentIndex(1);  // 切换到模式2
        mp_switchButton->setIcon(QIcon(":/img/switch_gds.png"));
    }
    else
    {
        mp_switchButton->setChecked(false);
        mp_stackedWidget->setCurrentIndex(0);  // 切换到模式1
        mp_switchButton->setIcon(QIcon(":/img/switch_pri.png"));
    }
}

void ComponentCenterWidget::fill_data()
{

}

PriModeWidget *ComponentCenterWidget::get_pri_mode_widget()
{
    return mp_pri_mode_widget;
}

GdsModeWidget *ComponentCenterWidget::get_gds_mode_widget()
{
    return mp_gds_mode_widget;
}

void ComponentCenterWidget::toggleMode()
{
    // 切换模式
    m_isOn = mp_switchButton->isChecked();
    // m_isOn = !m_isOn;
    if (m_isOn)
    {
        set_switch_mode(1);
        // mp_stackedWidget->setCurrentIndex(1);  // 切换到模式2
        // mp_switchButton->setIcon(QIcon(":/img/switch_gds.png"));
    }
    else
    {
        set_switch_mode(0);
        // mp_stackedWidget->setCurrentIndex(0);  // 切换到模式1
        // mp_switchButton->setIcon(QIcon(":/img/switch_pri.png"));
    }
}

void ComponentCenterWidget::savePrimitive()
{
    JsonPaser parser(mp_pdk_instance);
    QString extra_json = mp_pdk_instance->path() + "/extra.json";
    parser.save_extra_to_file(extra_json);

}

void ComponentCenterWidget::exportGds()
{
    // 使用 QFileInfo 获取路径信息
    QFileInfo pdk_fileInfo(mp_pdk_instance->pdk_path());
    // 获取最后的目录名
    QString pdk = pdk_fileInfo.fileName();

    // 使用 QFileInfo 获取路径信息
    QFileInfo cell_fileInfo(mp_pdk_instance->path());
    // 获取最后的目录名
    QString cell = cell_fileInfo.fileName();

    // 使用 QDir 获取路径信息
    QDir dir(mp_pdk_instance->path());
    // 获取路径的所有部分
    QStringList pathParts;
    if (dir.absolutePath().contains('/'))
    {
        pathParts = dir.absolutePath().split('/', QString::SkipEmptyParts);
    }
    else
    {
        pathParts = dir.absolutePath().split('\\', QString::SkipEmptyParts);
    }

    // 获取倒数第二个目录名
    QString lib;
    if (pathParts.size() > 1)
    {
        lib = pathParts.at(pathParts.size() - 2);
    }

    QString gds_file = QFileDialog::getSaveFileName(this, "Save File", "", "GDS Files (*.gds *gdsii);;All Files (*)");
    if (!gds_file.isEmpty())
    {
        cm::PythonMethod::instance()->export_pcell_to_gds(pdk,lib,cell,gds_file);
    }

}

}

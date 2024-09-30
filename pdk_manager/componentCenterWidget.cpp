#include <QVBoxLayout>
#include <QLabel>
#include "pdkDefines.h"
#include "priModeWidget.h"
#include "gdsModeWidget.h"
#include "componentCenterWidget.h"

namespace pdk
{
ComponentCenterWidget::ComponentCenterWidget(QWidget *parent)
    : QWidget(parent), m_isOn(false)
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

    hlayout->addStretch();
    hlayout->addWidget(pri_label);
    hlayout->addWidget(mp_switchButton);
    hlayout->addWidget(gds_label);
    hlayout->addStretch();

    // 创建 QStackedWidget 来管理不同模式
    mp_stackedWidget = new QStackedWidget(this);

    // 添加模式1
    mp_pri_mode_widget = new PriModeWidget();
    // QVBoxLayout *mode1Layout = new QVBoxLayout(mp_pri_mode_widget);
    // mode1Layout->addWidget(new QLabel("", mp_pri_mode_widget));
    mp_stackedWidget->addWidget(mp_pri_mode_widget);

    // 添加模式2
    mp_gds_mode_widget = new GdsModeWidget();
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

void ComponentCenterWidget::toggleMode()
{
    // 切换模式
    m_isOn = !m_isOn;
    if (m_isOn)
    {
        mp_stackedWidget->setCurrentIndex(1);  // 切换到模式2
        mp_switchButton->setIcon(QIcon(":/img/switch_gds.png"));
    }
    else
    {
        mp_stackedWidget->setCurrentIndex(0);  // 切换到模式1
        mp_switchButton->setIcon(QIcon(":/img/switch_pri.png"));
    }
}

}

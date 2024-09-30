#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include "drcMgrWidget.h"
#include "drcMgrItemWidget.h"
#include "pdkDefines.h"

namespace pdk
{
DrcMgrWidget::DrcMgrWidget(QWidget *parent)
    : QWidget(parent), mp_list_widget(new QListWidget())
{
    setup_ui();
    fill_data();
}

void DrcMgrWidget::setup_ui()
{
    QVBoxLayout *layout = new QVBoxLayout();

    // 创建表头（Header）
    QWidget *header = new QWidget(this);
    QHBoxLayout *headerLayout = new QHBoxLayout(header);

    // 在表头添加标签和按钮
    QLabel *headerLabel = new QLabel(tr(PDK_DRC_LIST_NAME), header);
    headerLayout->addWidget(headerLabel);

    // 在表头右边添加按钮
    mp_add_button = new QPushButton();
    mp_add_button->setIcon(QIcon(":/img/add.png"));

    // 调整布局，右对齐按钮
    headerLayout->addStretch();  // 使用 stretch 将按钮右对齐
    headerLayout->addWidget(mp_add_button);
    layout->addWidget(header);

    layout->addWidget(mp_list_widget);
    setLayout(layout);
}

void DrcMgrWidget::fill_data()
{
    // 添加数据到模型
    for (int i = 0; i < 10; ++i)
    {
        // 创建 QListWidgetItem
        QListWidgetItem *item = new QListWidgetItem(mp_list_widget);

        // 创建 CustomWidget 作为列表项的显示内容
        DrcMgrItemWidget *customWidget = new DrcMgrItemWidget();

        // 将 CustomWidget 设置为 QListWidgetItem 的显示内容
        item->setSizeHint(customWidget->sizeHint());  // 调整大小以适应内容
        mp_list_widget->setItemWidget(item, customWidget);

    }

}

}

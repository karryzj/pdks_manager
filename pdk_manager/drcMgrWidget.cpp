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
    connect(mp_add_button, &QPushButton::clicked, this, &DrcMgrWidget::add_drc_item);

    header->setStyleSheet("QWidget {background-color: #1E1E1E; color: white;}");
    // 调整布局，右对齐按钮
    headerLayout->addStretch();  // 使用 stretch 将按钮右对齐
    headerLayout->addWidget(mp_add_button);
    layout->addWidget(header);

    layout->addWidget(mp_list_widget);
    setLayout(layout);
}

QStringList DrcMgrWidget::drc_list() const
{
    return m_drc_list;
}

void DrcMgrWidget::set_drc_list(const QStringList &drc_list)
{
    m_drc_list = drc_list;
}

void DrcMgrWidget::fill_data()
{
    // 添加数据到模型
    mp_list_widget->clear();
    for (int i = 0; i < m_drc_list.size(); ++i)
    {
        // 创建 QListWidgetItem
        QListWidgetItem *item = new QListWidgetItem(mp_list_widget);

        // 创建 CustomWidget 作为列表项的显示内容
        DrcMgrItemWidget *customWidget = new DrcMgrItemWidget(m_drc_list.at(i));
        connect(customWidget, &DrcMgrItemWidget::delete_drc_item, [this, item](DrcMgrItemWidget *widget)
        {
            int row = mp_list_widget->row(item);
            delete mp_list_widget->takeItem(row);
        });

        // 将 CustomWidget 设置为 QListWidgetItem 的显示内容
        item->setSizeHint(customWidget->sizeHint());  // 调整大小以适应内容
        mp_list_widget->setItemWidget(item, customWidget);

    }

}

void DrcMgrWidget::add_drc_list(const QString &drc)
{
    m_drc_list.append(drc);
}

void DrcMgrWidget::add_drc_item()
{
    // 创建 QListWidgetItem
    QListWidgetItem *item = new QListWidgetItem(mp_list_widget);

    DrcMgrItemWidget *customWidget = new DrcMgrItemWidget("");
    connect(customWidget, &DrcMgrItemWidget::delete_drc_item, [this, item](DrcMgrItemWidget *widget)
    {
        int row = mp_list_widget->row(item);
        delete mp_list_widget->takeItem(row);
    });

    // 将 CustomWidget 设置为 QListWidgetItem 的显示内容
    item->setSizeHint(customWidget->sizeHint());  // 调整大小以适应内容
    mp_list_widget->addItem(item);
    mp_list_widget->setItemWidget(item, customWidget);
    update();
}

}

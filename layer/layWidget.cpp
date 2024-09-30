#include <QToolBar>
#include <QDockWidget>
#include <QTreeWidget>
#include <QListWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QRandomGenerator>
#include "layerMgr.h"
#include "layListItemWidget.h"
#include "layInfo.h"
#include "layWidget.h"
#include "layerDefines.h"

namespace ly
{

LayerWidget::LayerWidget(ly::LayerMgr *layer_mgr, QWidget *parent)
    : QWidget(parent)
    , mp_layer_mgr(layer_mgr), m_layers(mp_layer_mgr->get_layers())
{
    QVBoxLayout *layout = new QVBoxLayout();
    mp_list_widget = new QListWidget();

    update_all_items();

    QHBoxLayout *button_bottom = new QHBoxLayout();
    mp_new_layer_button = new QPushButton(QIcon(":/img/lay_new.png"), tr("新增"));
    mp_del_layer_button = new QPushButton(QIcon(":/img/lay_del.png"), tr("删除"));
    // 设置按钮的圆角矩形样式
    QString button_style = "QPushButton {"
                           "border: 2px solid #5F9EA0;"  // 边框颜色
                           "border-radius: 10px;"         // 圆角半径
                           "background-color: #ADD8E6;"   // 背景颜色
                           "padding: 5px 15px;"           // 内边距
                           "}";

    mp_new_layer_button->setStyleSheet(button_style);
    mp_new_layer_button->setEnabled(true);
    mp_del_layer_button->setStyleSheet(button_style);
    mp_del_layer_button->setEnabled(false);
    button_bottom->addWidget(mp_new_layer_button);
    button_bottom->addWidget(mp_del_layer_button);
    layout->addWidget(mp_list_widget);
    layout->addLayout(button_bottom);
    setLayout(layout);

    connect(mp_new_layer_button, &QPushButton::clicked, this, &LayerWidget::addItems);
    connect(mp_del_layer_button, &QPushButton::clicked, this, &LayerWidget::removeItem);
    connect(mp_list_widget, &QListWidget::itemSelectionChanged, this, &LayerWidget::updateRemoveButtonState);

    updateButtonState();
}

int LayerWidget::find_small_layer(const QList<LayerInfo*> &layers, const QString &prefix)
{
    QStringList layer_list;
    // 使用正则表达式匹配目录名中的编号，基于传入的前缀
    QRegularExpression regex("^" + QRegularExpression::escape(prefix) + "(\\d+)$");

    for (const auto layer : layers)
    {
        QRegularExpressionMatch layer_match = regex.match(layer->layer_name());
        if (layer_match.hasMatch())
            layer_list.append(layer->layer_name());
    }

    int smallest_number = 1;

    // 查找当前已经存在的最大编号
    while (true)
    {
        bool found = false;
        QString target_layer_ame = QString("%1%2").arg(prefix).arg(smallest_number);

        foreach (const QString &dir_name, layer_list)
        {
            QRegularExpressionMatch match = regex.match(dir_name);
            if (match.hasMatch() && match.captured(1).toInt() == smallest_number)
            {
                found = true;
                break;
            }
        }

        // 如果当前编号没有被占用，返回它
        if (!found)
        {
            return smallest_number;
        }
        smallest_number++;
    }
}

void LayerWidget::addItems()
{
    int red, green, blue;
    LayerInfo *layer_info = new LayerInfo();
    int layer_num = find_small_layer(m_layers, "Layer");
    QString layer_name = QString("Layer%1").arg(layer_num);
    layer_info->set_layer_name(layer_name);
    layer_info->set_border_line(LayerInfo::Border_Style::SolidLine);
    if (mp_list_widget->count() > 10)
    {
        red = QRandomGenerator::global()->bounded(200);
        green = QRandomGenerator::global()->bounded(200);
        blue = QRandomGenerator::global()->bounded(200);
        layer_info->set_border_color(QColor(red, green, blue));
        layer_info->set_fill_color(QColor(red, green, blue));
    }
    else
    {
        layer_info->set_border_color(QColor(LayerInfo::s_layer_color[layer_num % LayerInfo::s_layer_color.size()]));
        layer_info->set_fill_color(QColor(LayerInfo::s_layer_color[layer_num % LayerInfo::s_layer_color.size()]));
    }

    layer_info->set_fill_line((layer_num % 2) ? LayerInfo::Fill_Style::RightDiagonal : LayerInfo::Fill_Style::LeftDiagonal);

    // mp_layer_mgr->set_layer_info_by_name(layer_info);
    m_layers.append(layer_info);

    LayListItemWidget *listItemWidget = new LayListItemWidget(*layer_info, layer_name);
    QListWidgetItem *item = new QListWidgetItem(mp_list_widget);
    // item->setText(layer_name);
    item->setSizeHint(listItemWidget->sizeHint() * 1.5);
    mp_list_widget->setItemWidget(item, listItemWidget);

    updateButtonState();
}

void LayerWidget::removeItem()
{
    QList<QListWidgetItem*> selectedItems = mp_list_widget->selectedItems();
    if (!selectedItems.isEmpty())
    {
        QListWidgetItem *item = selectedItems.first();
        LayListItemWidget *listItemWidget = dynamic_cast<LayListItemWidget *>(mp_list_widget->itemWidget(item));
        if (listItemWidget)
        {
            auto layer_info = listItemWidget->get_current_layerinfo();
            if (layer_info.layer_name().isEmpty() || layer_info.layer_name() == tr(LAYER_LOCATION_LAYER_NAME))
                return;
            auto itor = std::find_if(m_layers.begin(), m_layers.end(), [&](auto info)
            {
                return layer_info.layer_name() == info->layer_name();
            });
            if(itor != m_layers.end())
            {
                m_layers.removeOne(*itor);
            }
            // mp_layer_mgr->remove_layer_info(layer_info);
            // item->setHidden(true);
            mp_list_widget->takeItem(mp_list_widget->row(item));
            delete item;
            // todo 发送删除了对应层信息的信号
        }
    }
    updateButtonState();
}

void LayerWidget::updateRemoveButtonState()
{
    mp_del_layer_button->setEnabled(!mp_list_widget->selectedItems().isEmpty());
}

void LayerWidget::onItemStyleChanged(const LayerInfo &style)
{
    auto it = std::find_if(m_layers.begin(), m_layers.end(), [&](const LayerInfo *info)
    {
        return style.layer_name() == info->layer_name();
    });

    if (it != m_layers.end())
    {
        **it = style;
        emit layer_changed();
    }
}

void LayerWidget::onItemStateChanged(const QString &layer, int state)
{
    auto it = std::find_if(m_layers.begin(), m_layers.end(), [&](const LayerInfo *info)
    {
        return layer == info->layer_name();
    });

    if (it != m_layers.end())
    {
        (*it)->set_visible(state);
        emit layer_changed();
    }
}

void LayerWidget::update_all_items()
{
    mp_list_widget->clear();
    for (int i = 0; i < m_layers.size(); ++i)
    {
        QString item_name;
        if (i == 0)
        {
            item_name = tr(LAYER_LOCATION_LAYER_NAME);
        }
        else
        {
            item_name = QString("Layer%1").arg(i);
        }
        QListWidgetItem *item = new QListWidgetItem(mp_list_widget);
        LayListItemWidget *listItemWidget = new LayListItemWidget(*m_layers[i], item_name);
        item->setSizeHint(listItemWidget->sizeHint() * 1.5);
        mp_list_widget->setItemWidget(item, listItemWidget);
        connect(listItemWidget, &LayListItemWidget::styleChanged, this, &LayerWidget::onItemStyleChanged);
        connect(listItemWidget, &LayListItemWidget::layerStateChanged, this, &LayerWidget::onItemStateChanged);
    }
}

void LayerWidget::updateButtonState()
{
    mp_del_layer_button->setEnabled(!mp_list_widget->selectedItems().isEmpty());
}

LayerWidget::~LayerWidget()
{
    delete mp_list_widget;
    delete mp_new_layer_button;
    delete mp_del_layer_button;
}

const QListWidget *LayerWidget::list_widget() const
{
    return mp_list_widget;
}

const QList<LayerInfo *> &LayerWidget::layer_infos() const
{
    return m_layers;
}
}

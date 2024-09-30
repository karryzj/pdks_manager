#include "priLayerInfoComboBox.h"
#include "layWidget.h"
#include "primitiveDefines.h"

#include <layInfo.h>
#include "layerDefines.h"

using namespace pr;

PriLayerInfoComboBox::PriLayerInfoComboBox(ly::LayerWidget* layer_widget, QWidget *parent)
    : QComboBox(parent)
    , mp_layer_widget(layer_widget)
{

}

PriLayerInfoComboBox::~PriLayerInfoComboBox()
{

}

void pr::PriLayerInfoComboBox::display_only_vaild_layer_info_item()
{
    // 先移除全部选项
    clear();
    // 图层信息
    const auto& layer_infos = mp_layer_widget->layer_infos();
    for(auto itor = layer_infos.begin() + 1; itor != layer_infos.end(); itor++)
    {
        ly::LayerInfo * layer_info = (*itor);
        const QString& item_name = layer_info->layer_name();
        addItem(item_name);
    }

    setEnabled(true);
    update();
}

void pr::PriLayerInfoComboBox::display_only_location_layer_info_item()
{
    // 先移除全部选项
    clear();

    const auto& layer_infos = mp_layer_widget->layer_infos();
    for(auto itor = layer_infos.begin(); itor != layer_infos.end(); itor++)  // 这么遍历看起来有点蠢，但是实际上是检查了是否有location这个图层。
    {
        ly::LayerInfo * layer_info = (*itor);
        const QString& item_name = layer_info->layer_name();
        if(item_name == tr(LAYER_LOCATION_LAYER_NAME))
        {
            addItem(item_name);
            setEnabled(false);
            update();
            return;
        }
    }
    Q_ASSERT(false);  // HINT@leixunyong没有找到location的图层，说明有问题。
}

ly::LayerInfo *pr::PriLayerInfoComboBox::default_layer_info() const
{
    const auto& layer_infos = mp_layer_widget->layer_infos();
    if(layer_infos.isEmpty())
    {
        return nullptr;
    }
    else
    {
        return layer_infos[0];
    }
}

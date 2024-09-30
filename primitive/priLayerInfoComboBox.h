#ifndef PRILAYERINFOCOMBOBOX_H
#define PRILAYERINFOCOMBOBOX_H

#include <QComboBox>

namespace ly
{
class LayerWidget;
class LayerInfo;
}

namespace pr
{
class PriLayerInfoComboBox final: public QComboBox
{
public:
    PriLayerInfoComboBox(ly::LayerWidget* layer_widget, QWidget *parent);
    ~PriLayerInfoComboBox() override;

    void display_only_vaild_layer_info_item();
    void display_only_location_layer_info_item();

    ly::LayerInfo* default_layer_info() const;

private:
    ly::LayerWidget* mp_layer_widget;
};

}


#endif // PRILAYERINFOCOMBOBOX_H

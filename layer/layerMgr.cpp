#include <QToolBar>
#include <QDockWidget>
#include <QTreeWidget>
#include <QListWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QPainter>
#include "layerMgr.h"
#include "layerUtils.h"
#include "layInfo.h"
#include "layerDefines.h"

namespace ly
{

LayerMgr::LayerMgr()
{
    if (m_layers.isEmpty())
    {
        LayerInfo* location_layer = new LayerInfo;
        QString layer_name = QObject::tr(LAYER_LOCATION_LAYER_NAME);
        location_layer->set_layer_name(layer_name);
        location_layer->set_border_line(LayerInfo::Border_Style::SolidLine);
        location_layer->set_border_color(QColor(Qt::black));
        location_layer->set_fill_line(LayerInfo::Fill_Style::NotFill);
        location_layer->set_fill_color(QColor(Qt::white));
        m_layers.append(location_layer);

        LayerInfo* layer_info = new LayerInfo;
        layer_info->set_layer_name("Layer1");
        layer_info->set_border_line(LayerInfo::Border_Style::SolidLine);
        layer_info->set_border_color(QColor(Qt::black));
        layer_info->set_fill_line(LayerInfo::Fill_Style::LeftDiagonal);
        layer_info->set_fill_color(QColor(Qt::red));
        m_layers.append(layer_info);
    }
}

LayerMgr::~LayerMgr()
{
    for (auto &layer : m_layers)
    {
        m_layers.removeAll(layer);
        delete layer;
    }
}

LayerInfo* LayerMgr::get_layer_info_by_name(const QString &lay_name) const
{
    auto it = std::find_if(m_layers.begin(), m_layers.end(), [&](const LayerInfo *info)
    {
        return lay_name == info->layer_name();
    });
    if (it != m_layers.end())
        return *it;
    else
    {
        return nullptr;
    }
}

bool LayerMgr::find_layer_info_by_name(const QString &lay_name)
{
    auto it = std::find_if(m_layers.begin(), m_layers.end(), [&](const LayerInfo *info)
    {
        return lay_name == info->layer_name();
    });
    if (it != m_layers.end())
        return true;
    else
        return false;
}

void LayerMgr::set_layers(LayerInfo *lay_info)
{
    auto itor = std::find_if(m_layers.begin(), m_layers.end(), [&](auto info)
    {
        return lay_info->layer_name() == info->layer_name();
    });
    if(itor != m_layers.end())
    {
        **itor = *lay_info;
    }
    else
    {
        m_layers.append(lay_info);
    }
}

QList<LayerInfo*>& LayerMgr::get_layers()
{
    return m_layers;
}

void LayerMgr::getPainter(QPainter &painter, const LayerInfo &lay_info)
{
    ly::layerUtils::setup_painter_by_layer_info(&lay_info, painter);
}

void LayerMgr::remove_layer_info(const LayerInfo &lay_info)
{
    auto itor = std::find_if(m_layers.begin(), m_layers.end(), [&](auto info)
    {
        return lay_info.layer_name() == info->layer_name();
    });
    if(itor != m_layers.end())
    {
        m_layers.removeOne(*itor);
        delete *itor;
    }
}

void LayerMgr::reset()
{
    for (auto &layer : m_layers)
    {
        delete layer;
    }
    m_layers.clear();
    if (m_layers.isEmpty())
    {
        LayerInfo* location_layer = new LayerInfo;
        QString layer_name = QObject::tr(LAYER_LOCATION_LAYER_NAME);
        location_layer->set_layer_name(layer_name);
        location_layer->set_border_line(LayerInfo::Border_Style::SolidLine);
        location_layer->set_border_color(QColor(Qt::black));
        location_layer->set_fill_line(LayerInfo::Fill_Style::NotFill);
        location_layer->set_fill_color(QColor(Qt::white));
        m_layers.append(location_layer);

        LayerInfo* layer_info = new LayerInfo;
        layer_info->set_layer_name("Layer1");
        layer_info->set_border_line(LayerInfo::Border_Style::SolidLine);
        layer_info->set_border_color(QColor(Qt::black));
        layer_info->set_fill_line(LayerInfo::Fill_Style::LeftDiagonal);
        layer_info->set_fill_color(QColor(Qt::red));
        m_layers.append(layer_info);
    }
}


}

#ifndef PRIMITIVEPCELL_H
#define PRIMITIVEPCELL_H

#include "dbEdgeProcessor.h"
#include "dbPCellDeclaration.h"
#include "paramMgr.h"
#include "pointE.h"
#include "qjsonobject.h"
#include "pdkCommon.h"

namespace pdk
{

struct PolyInfo
{
    int node_id;
    int node_type;
    QString layer;
    db::DPolygon poly;
    PolyInfo(){};
    PolyInfo(int id, int type, QString layer,db::DPolygon poly) : node_id(id), node_type(type),layer(layer), poly(poly) {};
};

class PDK_PUBLIC PrimitivePcellAnchor
{
public:
    PrimitivePcellAnchor(QString x,QString y,QString rotate);
private:
    double m_x;
    double m_y;
    double m_rotate;
    QString m_str_x;
    QString m_str_y;
    QString m_str_rotate;
};

class PDK_PUBLIC PrimitivePcell: public db::PCellDeclaration
{
public:
    PrimitivePcell(QString data_file);
    PrimitivePcell(QString pdk, QString lib, QString cell);
    ~PrimitivePcell();
    void load_json();

    /**
   *  @brief This PCell can be created from a shape
   */
    virtual bool can_create_from_shape (const db::Layout &layout, const db::Shape &shape, unsigned int layer) const;

    /**
   *  @brief Get the parameters from a shape
   */
    //virtual db::pcell_parameters_type parameters_from_shape (const db::Layout &layout, const db::Shape &shape, unsigned int layer) const;

    /**
   *  @brief Get the instance transformation from a shape
   */
    virtual db::Trans transformation_from_shape (const db::Layout &layout, const db::Shape &shape, unsigned int layer) const;

    /**
   *  @brief Get the layer declarations
   */
    virtual std::vector<db::PCellLayerDeclaration> get_layer_declarations (const db::pcell_parameters_type &parameters) const;

    /**
   *  @brief Coerces the parameters (in particular updates the computed ones)
   */
    virtual void coerce_parameters (const db::Layout &layout, db::pcell_parameters_type &parameters) const;

    // add by yl
    virtual bool
    check_param (const tl::Variant &value, const db::PCellParameterDeclaration &decl) const;

    /**
   *  @brief Produces the layout
   */
    virtual void produce (const db::Layout &layout, const std::vector<unsigned int> &layer_ids, const db::pcell_parameters_type &parameters, db::Cell &cell) const;

    /**
   *  @brief Get the display name for a PCell with the given parameters
   */
    virtual std::string get_display_name (const db::pcell_parameters_type &) const;

    /**
   *  @brief Get the parameter declarations
   */
    virtual std::vector<db::PCellParameterDeclaration> get_parameter_declarations () const;

    //
    bool add_param(QString key, QString value, QString desc);
    bool add_rule(QString rule);
    QString get_anchor_str(int idx) const;
    bool add_anchor(pm::PointE p,QString rotate);
    bool set_param_default(QString key,QString val);
    bool set_param_defaults(std::map<QString, QString> &params);
    double get_value(std::map<QString,QString> map, QString key) const;
    db::DPolygon get_poly_by_node(QJsonObject &node) const;
    std::vector<PolyInfo> get_node_polys(QJsonObject node,std::map<int,QJsonObject> &node_map,std::map<int,std::vector<db::DPoint>> &node_attach_points_map) const;
    std::map<QString,std::vector<db::Polygon>> format_polys(std::vector<PolyInfo> poly_infos,double dbu,std::map<int,QJsonObject> node_map) const;
    std::pair<int,int> get_l(std::map<QString,std::pair<int,int>> l,QString n) const;

private:
    QJsonObject get_node_first_add_parent(QJsonObject node,std::map<int,QJsonObject> node_map) const;
    std::vector<int> get_all_add_childs(QJsonObject node) const;
    std::vector<int> get_del_ids(QJsonObject node,std::map<int,QJsonObject> node_map) const;
    db::Point get_coord_anchor(double odd,pm::PointE anchor) const;
    unsigned int get_layer_idx(QString name) const;

private:
    QString m_path="";
    QString m_data_file="";
    QString m_extra_file="";
    pm::ParamMgr* m_param_mgr;
    QJsonObject m_root;

    std::map<QString,std::pair<int,int>> m_layers;
    //std::map<QString, tl::Variant> m_params;
    std::vector<pm::Rule> m_ruls;
    std::vector<std::pair<pm::PointE,QString>> m_anchors;
    pm::PointE m_coord_anchor;

    double m_point_distance=1;
    QJsonObject m_node;

    //std::map<int,QJsonObject> m_node_map;
    //std::map<int,std::vector<db::DPoint>> m_node_attach_points_map;

};

}

#endif // PRIMITIVEPCELL_H

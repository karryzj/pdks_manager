#include "primitivePcell.h"
#include "common_defines.h"
#include "priFiler.h"
#include "qjsonarray.h"
#include "qjsondocument.h"
#include "crypto.h"
#include "shapeCurve.h"
#include "configManager.h"

namespace pdk
{

PrimitivePcell::PrimitivePcell(QString pdk, QString lib, QString cell)
{
    m_path = "data/pdks/"+pdk+"/librarys/"+cell;
    m_data_file = m_path+"data.json";
    m_extra_file = m_path+"extra.json";
    load_json();
}

PrimitivePcell::PrimitivePcell(QString data_file)
{
    m_data_file = data_file;
    load_json();
}

PrimitivePcell::~PrimitivePcell()
{
    if(m_param_mgr) delete m_param_mgr;
}

void PrimitivePcell::load_json()
{
    QJsonDocument doc = cm::Crypto::decrypt_json_doc(m_data_file);
    if (!doc.isObject())
    {
        qDebug() << "JSON document is not an object.";
        return;
    }

    m_param_mgr = new pm::ParamMgr();

    QJsonObject root_object = doc.object();
    m_root = root_object;
    QJsonObject primitive_object = root_object["primitive"].toObject();

    //加载图层
    QJsonArray layers_array = primitive_object["layers"].toArray();
    int layer_idx=1;
    for (int i = 0; i < layers_array.size(); ++i)
    {
        QJsonObject layer_object = layers_array[i].toObject();
        auto layer_name = layer_object["layer_name"].toString();
        // 这里需要额外进行查重判断。如果已经有同名的图层就不进行加载。
        if (m_layers.find(layer_name) != m_layers.end() || layer_name=="Location")
        {
            continue;
        }
        m_layers.insert(std::pair<QString,std::pair<int,int>>(layer_name,std::pair<int,int>(layer_idx,0)));
        layer_idx+=1;
    }

    QJsonObject param_rules = primitive_object["param_rules"].toObject();
    //加载参数
    QJsonArray param_array = param_rules["params"].toArray();
    for (const QJsonValue &json_obj : param_array)
    {
        auto param_json_obj=json_obj.toObject();
        QString desc =  param_json_obj["desc"].toString();
        QString param_key =  param_json_obj["key"].toString();
        QString param_value = param_json_obj["value"].toString();
        if(desc.length()==0)
        {
            desc = param_key;
        }
        desc = "params_mgr\t"+desc;

        add_param(param_key, param_value, desc);

    }

    //加载rules
    QJsonArray rule_array = param_rules["rules"].toArray();
    for (const QJsonValue &json_obj : rule_array)
    {
        add_rule(json_obj.toString());
    }

    //加载锚点
    QJsonArray anchors_array = primitive_object["anchors"].toArray();
    for (int i = 0; i < anchors_array.size(); ++i)
    {
        QJsonObject anchor_obj = anchors_array[i].toObject();
        auto p = pm::PointE(anchor_obj["x"].toString(), anchor_obj["y"].toString());
        auto rotate = anchor_obj["rotate_angle"].toString();
        add_anchor(p,rotate);
    }

    QJsonObject coord_anchor_obj = primitive_object["coord_anchor"].toObject();
    auto x = coord_anchor_obj["x"].toString();
    auto y = coord_anchor_obj["y"].toString();

    m_coord_anchor = pm::PointE(x, y);

    m_node = primitive_object["node"].toObject();
    m_point_distance = primitive_object["arc"].toObject()["length"].toDouble();
}

bool PrimitivePcell::add_param(QString key, QString value, QString desc)
{
    pm::ParamDecl* param = nullptr;

    param = new pm::ParamDecl(key, value, desc);
    m_param_mgr->add_param(param);
    return true;
}

bool PrimitivePcell::add_rule(QString rule)
{
    auto ruleObj = pm::Rule(rule);
    m_ruls.push_back(ruleObj);

    QString name = "rule"+QString::number(m_ruls.size());
    QString desc = "rules\t"+name;

    add_param(name,rule,desc);
    return true;
}

QString PrimitivePcell::get_anchor_str(int idx) const
{
    if(idx >= m_anchors.size())
    {
        return "";
    }

    std::pair<pm::PointE,QString> anchor=m_anchors[idx];
    pm::PointE p=anchor.first;
    QString s=QString::number(m_param_mgr->parse(p.x().to_str()));
    s += "," +QString::number(m_param_mgr->parse(p.y().to_str()));
    s += "," +QString::number(m_param_mgr->parse(anchor.second));

    return s;
}

bool PrimitivePcell::add_anchor(pm::PointE p,QString rotate)
{
    m_anchors.push_back(std::pair<pm::PointE,QString>(p,rotate));

    QString name = "anchor"+QString::number(m_anchors.size());
    QString desc = "anchors\t"+name;
    QString val = get_anchor_str(m_anchors.size()-1);

    add_param(name,val,desc);

    return true;
}


bool PrimitivePcell::set_param_default(QString key,QString val)
{
    pm::ParamDecl *param=m_param_mgr->find_param(key);
    if(!param)
    {
        return false;
    }

    param->set_expression(val);
    return true;
}

bool PrimitivePcell::set_param_defaults(std::map<QString, QString> &params)
{
    bool is_valid=true;
    bool tmp;
    for (std::map<QString, QString>::const_iterator it = params.begin(); it != params.end(); ++it) {
        tmp = set_param_default(it->first,it->second);
        if(!tmp)
        {
            is_valid=false;
        }
    }

    return is_valid;
}



std::map<QString,QString> get_params_map(QJsonArray params)
{
    std::map<QString,QString> map;
    for(int i=0;i<params.size();i++)
    {
        QString key=params[i].toObject()["key"].toString();
        QString value=params[i].toObject()["value"].toString();
        map.insert(std::pair<QString,QString>(key,value));
    }

    return map;
}

//获取矩形
std::vector<db::DPoint> get_rectangle(double width,double height)
{
    std::vector<db::DPoint> points;
    points.push_back(db::DPoint(0,height));
    points.push_back(db::DPoint(0,0));
    points.push_back(db::DPoint(width,0));
    points.push_back(db::DPoint(width,height));

    return points;
}

//获取直角三角形
std::vector<db::DPoint> get_triangle(double width,double height)
{
    std::vector<db::DPoint> points;
    points.push_back(db::DPoint(0,height));
    points.push_back(db::DPoint(0,0));
    points.push_back(db::DPoint(width,0));

    return points;
}

//四边形
std::vector<db::DPoint> get_quadrangle(double width,double height,double x1,double x2,double x3,double x4)
{

    std::vector<db::DPoint> points;
    points.push_back(db::DPoint(x2,height));
    points.push_back(db::DPoint(0,x1));
    points.push_back(db::DPoint(width-x4,0));
    points.push_back(db::DPoint(width,height-x3));

    return points;
}

std::vector<db::DPoint> get_arc_point(db::DPoint point,double r, double point_distance, double angle0=0, double angle1=360)
{
    std::vector<db::DPoint> points;

    int n=int(ceil(r*(angle1-angle0)*M_PI/180/point_distance)+2);
    std::vector<double> angles;
    for (int i=0;i<n;i++)
    {
        angles.push_back(angle0+1.0*i/(n-1)*(angle1-angle0));
    }

    for (int i=0;i<n;i++)
    {
        points.push_back(db::DPoint(point.x()+r * cos(angles[i]*M_PI/180),point.y()+r*sin(angles[i]*M_PI/180)));
    }
    return points;
}

//圆
std::vector<db::DPoint> get_circle(db::DPoint point,double r,double point_distance)
{
    return get_arc_point(point,r,point_distance);
}

std::vector<db::DPoint> get_sector(db::DPoint point,double rin,double rout,double angle0,double angle1,double point_distance)
{
    std::vector<db::DPoint> points;

    if(angle1<angle0)
    {
        angle1=angle1+360;
    }
    if(angle1-angle0>=360)
    {
        angle0=0;
        angle1=360;
    }

    if(rin!=0)
    {
        std::vector<db::DPoint> points_in=get_arc_point(point,rin,point_distance,angle0,angle1);
        std::vector<db::DPoint> points_out=get_arc_point(point,rout,point_distance,angle0,angle1);

        std::reverse(points_out.begin(), points_out.end());
        points_in.insert(points_in.end(),points_out.begin(),points_out.end());

        return points_in;
    }
    else
    {
        points = get_arc_point(point,rout,point_distance,angle0,angle1);
        if(angle1-angle0<360)
        {
            points.push_back(point);
        }
        return points;
    }
}

std::vector<db::DPoint> get_ellipse_point(db::DPoint point,double rx,double ry, double point_distance, double angle0=0, double angle1=360)
{
    std::vector<db::DPoint> points;
    double r = std::max(rx,ry);
    int n=int(ceil(r*(angle1-angle0)*M_PI/180/point_distance)+2);

    std::vector<double> angles;
    for (int i=0;i<n;i++)
    {
        angles.push_back(angle0+1.0*i/(n-1)*(angle1-angle0));
    }

    for (int i=0;i<n;i++)
    {
        points.push_back(db::DPoint(point.x()+rx * cos(angles[i]*M_PI/180),point.y()+ry*sin(angles[i]*M_PI/180)));
    }
    return points;
}

std::vector<db::DPoint> get_ellipse(db::DPoint point,double rx,double ry,double angle0,double angle1,double point_distance)
{
    std::vector<db::DPoint> points;

    if(angle1<angle0)
    {
        angle1=angle1+360;
    }
    if(angle1-angle0>=360)
    {
        angle0=0;
        angle1=360;
    }

    points = get_ellipse_point(point,rx,ry,point_distance,angle0,angle1);
    if(angle1-angle0<360)
    {
        points.push_back(point);
    }
    return points;
}

void mirr_points(std::vector<db::DPoint> &points,bool mirrx,bool mirry)
{
    for(int i=0;i<points.size();i++)
    {
        if(mirrx)
        {
            points[i].set_x(-points[i].x());
        }

        if(mirry)
        {
            points[i].set_y(-points[i].y());
        }
    }
}

//获取
double PrimitivePcell:: get_value(std::map<QString,QString> map, QString key) const
{
    if(map.find(key)!=map.end())
    {
        QString val = map.at(key);
        return m_param_mgr->parse(val);
    }

    return 0;
}

db::DPolygon PrimitivePcell::get_poly_by_node(QJsonObject &node) const
{
    db::DPolygon poly;
    std::vector<db::DPoint> points;
    QString shape_name = node["shape_name"].toString();
    at::NodeDirection direction = static_cast<at::NodeDirection>(node["node_direction"].toInt());
    QJsonArray params=node["params"].toArray();
    auto params_map =get_params_map(params);

    if(shape_name=="rectangle")
    {
        double width = get_value(params_map,"width");
        double height = get_value(params_map,"height");
        points=get_rectangle(width,height);
    }
    else if(shape_name=="quadrangle")
    {
        double width = get_value(params_map,"width");
        double height = get_value(params_map,"height");
        double x1 = get_value(params_map,"parameter one");
        double x2 = get_value(params_map,"parameter two");
        double x3 = get_value(params_map,"parameter three");
        double x4 = get_value(params_map,"parameter four");
        points=get_quadrangle(width,height,x1,x2,x3,x4);
    }
    else if(shape_name=="triangle")
    {
        double width = get_value(params_map,"width");
        double height = get_value(params_map,"height");
        points=get_triangle(width,height);
    }
    else if(shape_name=="circle")
    {
        double r = get_value(params_map,"radius");
        db::DPoint p(r,r);
        points=get_circle(p,r,m_point_distance);
    }
    else if(shape_name=="sector")
    {
        double rin = get_value(params_map,"inside radius");
        double rout = get_value(params_map,"outside radius");
        double angle0 = get_value(params_map,"start angle");
        double angle1 = get_value(params_map,"end angle");
        db::DPoint p(rout,rout);
        points=get_sector(p,rin,rout,angle0,angle1,m_point_distance);
    }
    else if(shape_name=="ellipse")
    {
        double rx = get_value(params_map,"semi major axis");
        double ry = get_value(params_map,"semi minor axis");
        double angle0 = get_value(params_map,"start angle");
        double angle1 = get_value(params_map,"end angle");
        db::DPoint p(rx,ry);
        points=get_ellipse(p,rx,ry,angle0,angle1,m_point_distance);
    }
    else if(shape_name=="polygen")
    {
        int xy_num = int(floor((params.size()-1)/2));
        for(int i=0;i<xy_num;i++)
        {
            double x = get_value(params_map,"x"+QString::number(i+1));
            double y = get_value(params_map,"y"+QString::number(i+1));
            points.push_back(db::DPoint(x,y));
        }
        points.push_back(db::DPoint(0,0));
    }
    else if(shape_name=="curve")        //曲线
    {
        //直接调用pr和sp模块的解析曲线生成点集
        // TODO 通过设置CM_ARC_LEN_KEY的方式，多线程或者和图元软件同时打开时会有问题，后面修改为不调用pr和sp模块
        bool hasARC = cm::ConfigManager::instance()->has_key(CM_ARC_LEN_KEY);
        qreal arc_len;
        if(hasARC)
        {
            arc_len=cm::ConfigManager::instance()->query(CM_ARC_LEN_KEY).toDouble();
        }
        cm::ConfigManager::instance()->setup_value(CM_ARC_LEN_KEY, m_point_distance);
        pr::PriFiler filter(nullptr);
        QVector<pm::ParamDecl> param_decls;
        filter.load_tree_child_node_params(param_decls,node);
        sp::ShapeCurve sc(m_param_mgr,param_decls,nullptr);
        sc.update_variables();
        QVector<QPointF> pointset=sc.point_set();

        if(hasARC)
        {
            cm::ConfigManager::instance()->setup_value(CM_ARC_LEN_KEY, arc_len);    //避免同时打开影响到其他，还原回去
        }


        for(int i=0;i<pointset.size();i++)
        {
            points.push_back(db::DPoint(pointset[i].x(),pointset[i].y()));
        }
    }
    else
    {
        std::wcout << "unsupport shape type" << std::endl;
    }

    if(direction==at::NodeDirection::TOP_LEFT)
    {
        mirr_points(points,true,false);
    }
    else if(direction==at::NodeDirection::BOTTOM_LEFT)
    {
        mirr_points(points,true,true);
    }
    else if(direction==at::NodeDirection::BOTTOM_RIGHT)
    {
        mirr_points(points,false,true);
    }

    poly.assign_hull(points.begin(),points.end());

    //rotate
    double rotate = get_value(params_map,"rotate");
    if(rotate)
    {
        db::DCplxTrans tr(1, rotate, false, db::DVector(0,0));
        poly=poly.transform(tr);
    }

    return poly;
}

std::vector<PolyInfo> PrimitivePcell::get_node_polys(QJsonObject node,std::map<int,QJsonObject> &node_map,std::map<int,std::vector<db::DPoint>> &node_attach_points_map) const
{
    std::vector<PolyInfo> polys;

    int id=node["id"].toInt();
    node_map[id] = node;

    db::DPoint ori_p(0,0);

    if(id==0)   //root
    {
        std::vector<db::DPoint> attachs;
        auto at_points=node["attach_points"].toArray();
        db::DPoint p(0,0);
        attachs.push_back(p);
        node_attach_points_map[0] = attachs;
    }
    else        //非root
    {
        QJsonObject parent=node_map[node["parent_node_id"].toInt()];
        int parent_id=parent["id"].toInt();
        ori_p=node_attach_points_map[parent_id][node["parent_attach_point_idx"].toInt()];


        //1.更新自己的附着点坐标
        std::vector<db::DPoint> attachs;
        auto at_points=node["attach_points"].toArray();
        for(int i=0;i<at_points.size();i++)
        {
            db::DPoint p;
            p.set_x(m_param_mgr->parse(at_points[i].toObject()["x"].toString()) + ori_p.x());
            p.set_y(m_param_mgr->parse(at_points[i].toObject()["y"].toString())+ ori_p.y());
            attachs.push_back(p);
        }
        node_attach_points_map[id] = attachs;

        //2.将shape加入返回值
        at::NodeType node_type = static_cast<at::NodeType>(node["node_type"].toInt());
        if(node_type==at::NodeType::ADD || node_type==at::NodeType::DELETE)
        {
            db::DPolygon poly = get_poly_by_node(node);
            //从相对坐标移动到绝对坐标
            db::DCplxTrans tr(1, 0, false, db::DVector(ori_p.x(),ori_p.y()));
            poly=poly.transform(tr);

            QString layer=node["layer"].toString();

            polys.push_back(PolyInfo(id,node["node_type"].toInt(),layer,poly));
        }
    }

    //3.遍历子节点子节点
    auto childrens=node["children"].toArray();
    for(int i=0;i<childrens.size();i++)
    {
        auto poly_i = get_node_polys(childrens[i].toObject(),node_map,node_attach_points_map);
        polys.insert(polys.end(),poly_i.begin(),poly_i.end());
    }

    return polys;
}

//获取第一个为ADD的父亲节点
QJsonObject PrimitivePcell::get_node_first_add_parent(QJsonObject node,std::map<int,QJsonObject> node_map) const
{
    QJsonObject parent;
    while(true)
    {
        int id = node["parent_node_id"].toInt();
        parent=node_map[id];
        auto parent_id=parent["id"].toInt();
        at::NodeType node_type = static_cast<at::NodeType>(parent["node_type"].toInt());
        if (parent_id==0 || node_type == at::NodeType::ADD)
            break;
    }
    return parent;
}

//获取包含自己以及自己所有子孙的add类型的节点
std::vector<int> PrimitivePcell::get_all_add_childs(QJsonObject node) const
{
    std::vector<int> ids;

    at::NodeType node_type = static_cast<at::NodeType>(node["node_type"].toInt());
    if (node_type == at::NodeType::ADD)
    {
        ids.push_back(node["id"].toInt());
    }

    //遍历子节点
    auto childrens=node["children"].toArray();
    for(int i=0;i<childrens.size();i++)
    {
        auto ids_i = get_all_add_childs(childrens[i].toObject());
        ids.insert(ids.end(),ids_i.begin(),ids_i.end());
    }
    return ids;
}

std::vector<int> PrimitivePcell::get_del_ids(QJsonObject node,std::map<int,QJsonObject> node_map) const
{
    std::vector<int> ids;
    at::NodeBooleanSubtractType type = static_cast<at::NodeBooleanSubtractType>(node["node_boolean_subtract_type"].toInt());
    if(type==at::NodeBooleanSubtractType::ONLY_WITH_PARENT_NODE)
    {
        auto parent = get_node_first_add_parent(node,node_map);
        auto id=parent["id"].toInt();
        if(id !=0)
        {
            ids.push_back(id);
        }
    }
    else if(type==at::NodeBooleanSubtractType::WITH_PARENT_AND_BROTHER_NODE)
    {
        auto parent = get_node_first_add_parent(node,node_map);
        auto id=parent["id"].toInt();
        if(id !=0)
        {
            ids.push_back(id);

            //获取父亲的直属第一层add子节点
            auto childrens=parent["children"].toArray();
            for(int i=0;i<childrens.size();i++)
            {
                at::NodeType node_type = static_cast<at::NodeType>(childrens[i].toObject()["node_type"].toInt());
                if (node_type == at::NodeType::ADD)
                {
                    ids.push_back(childrens[i].toObject()["id"].toInt());
                }
            }
        }
    }
    else if(type==at::NodeBooleanSubtractType::WITH_PARENT_AND_CHILD_NODE)
    {
        auto parent = get_node_first_add_parent(node,node_map);
        auto id=parent["id"].toInt();
        if(id !=0)
        {
            ids=get_all_add_childs(parent);
        }
    }

    return ids;
}

db::Point PrimitivePcell::get_coord_anchor(double odd,pm::PointE anchor) const
{
    db::Point coord_anchor;
    coord_anchor.set_x(m_param_mgr->parse(anchor.x().to_str()) * odd);
    coord_anchor.set_y(m_param_mgr->parse(anchor.y().to_str()) * odd);

    return coord_anchor;
}

std::map<QString,std::vector<db::Polygon>> PrimitivePcell::format_polys(std::vector<PolyInfo> poly_infos,double dbu,std::map<int,QJsonObject> node_map) const
{
    std::map<QString,std::vector<db::Polygon>> layer_polys;

    //区分添加和删除
    std::map<int,PolyInfo> maps;
    std::map<int,std::vector<int>> del_ids;
    std::vector<int> add_ids;
    double odd = 1/dbu;

    for(size_t i=0;i<poly_infos.size();i++)
    {
        auto id=poly_infos[i].node_id;
        maps[poly_infos[i].node_id] = poly_infos[i];

        at::NodeType node_type = static_cast<at::NodeType>(poly_infos[i].node_type);

        if(node_type==at::NodeType::DELETE)
        {
            auto dels=get_del_ids(node_map[id],node_map);
            for(size_t j=0;j<dels.size();j++)
            {
                if(del_ids.find(dels[j]) == del_ids.end())
                {
                    std::vector<int> vv;
                    del_ids[dels[j]] = vv;
                }
                del_ids[dels[j]].push_back(id);
            }
        }
        else if(node_type==at::NodeType::ADD)
        {
            add_ids.push_back(id);
        }
    }

    db::EdgeProcessor edge_processor;
    //以删除图形id为key，找出所有需要和该删除操作的add图形
    for(size_t i=0;i<add_ids.size();i++)
    {
        int id=add_ids[i];
        auto info=maps[id];
        QString layer=info.layer;

        //需要将DPolygon乘以1/dbu
        if(layer_polys.find(layer)==layer_polys.end())
        {
            std::vector<db::Polygon> lp;
            layer_polys[layer]=lp;
        }

        if(del_ids.find(id)==del_ids.end()) //没有需要删除的操作
        {
            layer_polys[layer].push_back(db::Polygon(info.poly*odd));
        }
        else
        {
            std::vector<db::Polygon> out;
            std::vector<db::Polygon> a;
            a.push_back(db::Polygon(info.poly*odd));
            std::vector<db::Polygon> b;
            for(size_t j=0;j<del_ids[id].size();j++)
            {
                b.push_back(db::Polygon(maps[del_ids[id][j]].poly*odd));
            }
            edge_processor.boolean(a, b, out, db::BooleanOp::ANotB);
            for(size_t j=0;j<out.size();j++)
            {
                layer_polys[layer].push_back(out[j]);
            }
        }
    }

    db::Point coord_anchor=get_coord_anchor(dbu,m_coord_anchor);
    //coord_anchor.set_x(m_param_mgr->parse(m_coord_anchor.x().to_str()) * odd);
    //coord_anchor.set_y(m_param_mgr->parse(m_coord_anchor.y().to_str()) * odd);
    db::CplxTrans tr(1, 0, false, db::DVector(coord_anchor.x(),coord_anchor.y()));
    bool need_tr = !(coord_anchor.x()==0 && coord_anchor.y()==0);

    //
    std::map<QString,std::vector<db::Polygon>> layer_dpolys;
    for(std::map<QString,std::vector<db::Polygon>>::const_iterator it=layer_polys.begin();it!=layer_polys.end();++it)
    {
        QString layer = it->first;
        std::vector<db::Polygon> polys = it->second;
        std::vector<db::Polygon> out;

        edge_processor.simple_merge(polys,out,false,false);

        std::vector<db::Polygon> dpolys;
        for(size_t i=0;i<out.size();i++)
        {
            db::Polygon p=out[i];
            if(need_tr)
            {
                p=p.transform(tr);
            }

            dpolys.push_back(p);
        }
        layer_dpolys[layer]=dpolys;
    }

    return layer_dpolys;
}

bool
PrimitivePcell::can_create_from_shape (const db::Layout & /*layout*/, const db::Shape &shape, unsigned int /*layer*/) const
{
    return (shape.is_polygon () || shape.is_box () || shape.is_path ());
}

db::Trans
PrimitivePcell::transformation_from_shape (const db::Layout & /*layout*/, const db::Shape &shape, unsigned int /*layer*/) const
{
    //  use the displacement to define the center of the circle
    return db::Trans (shape.bbox ().center () - db::Point ());
}


std::vector<db::PCellLayerDeclaration>
PrimitivePcell::get_layer_declarations (const db::pcell_parameters_type &parameters) const
{
    std::vector<db::PCellLayerDeclaration> layers;

    for(std::map<QString,std::pair<int,int>>::const_iterator it=m_layers.begin();it!=m_layers.end();++it)
    {
        auto l=it->first;
        auto dn = it->second;
        db::LayerProperties lp(dn.first,dn.second);
        layers.push_back (lp);
    }
    return layers;
}

void
PrimitivePcell::coerce_parameters (const db::Layout & /*layout*/, db::pcell_parameters_type &parameters) const
{
    auto params = m_param_mgr->params();
    if (parameters.size () < params.size()) {
        return;
    }

    int anchor_idx=0;

    for(int i=0;i<parameters.size();i++)
    {
        auto param = params[i];
        //如果是rule，不处理
        if(param->desc().startsWith("rules\t"))
        {
            continue;
        }

        //如果是anchor，更新显示
        if(param->desc().startsWith("anchors\t"))
        {
            parameters[i] = get_anchor_str(anchor_idx);
            anchor_idx+=1;
            continue;
        }


        if(param->is_expression(m_param_mgr))
        {
            parameters[i] = param->to_double();
        }
        else
        {
            double val=parameters[i].to_double();
            param->set_value(val);
            param->set_expression(QString::number(val));
        }
    }

}

bool PrimitivePcell::check_param(const tl::Variant &value, const db::PCellParameterDeclaration &decl) const
{
    QString desc =QString::fromStdString(decl.get_description());
    if(desc.startsWith("rules\t") || desc.startsWith("anchors\t"))
    {
        return true;
    }
    auto param = m_param_mgr->find_param(QString::fromStdString(decl.get_name()));
    if(!param)
    {
        return false;
    }

    if(param->is_expression(m_param_mgr))
    {
        return true;
    }

    pm::ParamDecl *param_decl = new pm::ParamDecl(QString::fromStdString(decl.get_name()),value.to_double(),"");
    bool isvalid= m_param_mgr->isValid(param_decl);

    delete param_decl;

    return isvalid;
}

std::pair<int,int> PrimitivePcell::get_l(std::map<QString,std::pair<int,int>> l,QString n) const
{
    return l[n];
}



void
PrimitivePcell::produce (const db::Layout &layout, const std::vector<unsigned int> &layer_ids, const db::pcell_parameters_type &parameters, db::Cell &cell) const
{
    /*if (parameters.size () < p_total || layer_ids.size () < 1) {
        return;
    }

    double r = parameters [p_radius].to_double () / layout.dbu ();
    int n = std::max (3, parameters [p_npoints].to_int ());

    std::vector <db::Point> points;
    points.reserve (n);

    //  Produce an outer circle approximation. This
    //  one looks slightly better in the case of few points.
    double rr = r / cos (M_PI / n);
    double da = 2.0 * M_PI / n;
    for (int i = 0; i < n; ++i) {
        double a = (i + 0.5) * da;
        points.push_back (db::Point (db::coord_traits<db::Coord>::rounded (-rr * cos (a)), db::coord_traits<db::Coord>::rounded (rr * sin (a))));
    }

    //  Produce the shape
    db::SimplePolygon poly;
    poly.assign_hull (points.begin (), points.end ());
    cell.shapes (layer_ids [p_layer]).insert (poly);*/

    /*std::vector <db::Point> points;
    points.push_back(db::Point(1,1));
    points.push_back(db::Point(1,10));
    points.push_back(db::Point(10,10));
    db::SimplePolygon poly;
    poly.assign_hull (points.begin (), points.end ());*/

    std::map<int,QJsonObject> node_map;
    std::map<int,std::vector<db::DPoint>> node_attach_points_map;
    auto np = get_node_polys(m_node,node_map,node_attach_points_map);
    std::map<QString,std::vector<db::Polygon>> layer_polys=format_polys(np,layout.dbu(),node_map);
    for(std::map<QString,std::vector<db::Polygon>>::const_iterator it=layer_polys.begin();it!=layer_polys.end();++it)
    {
        QString layer_str=it->first;

        auto l=get_l(m_layers,layer_str);
        auto lay=cell.layout();
        int layer=lay->get_layer_maybe(db::LayerProperties (l.first, l.second));//lay.get_layer(db::LayerProperties (l.first, l.second));
        if(layer==-1)
        {
            continue;
        }
        unsigned int ulayer = (unsigned int)layer;
        auto polys = it->second;

        for(size_t i=0;i<polys.size();i++)
        {
            cell.shapes (ulayer).insert (polys[i]);//db::Polygon(polys[i])
        }
    }

}

std::string
PrimitivePcell::get_display_name (const db::pcell_parameters_type &parameters) const
{
    return "ququan_shape";
}

std::vector<db::PCellParameterDeclaration>
PrimitivePcell::get_parameter_declarations () const
{
    std::vector<db::PCellParameterDeclaration> parameters;

    QList<pm::ParamDecl*> params = m_param_mgr->params();
    for(int i=0;i<params.size();i++)
    {
        auto param=params[i];
        parameters.push_back (db::PCellParameterDeclaration (param->key().toStdString()));
        parameters.back ().set_description (tl::to_string (param->desc().toStdString()));
        //是表达式，转为string
        if(param->is_expression(m_param_mgr))
        {
            parameters.back ().set_type (db::PCellParameterDeclaration::t_string);
            parameters.back ().set_default (param->to_str());
        }
        else
        {
            //是数字转为double
            parameters.back ().set_type (db::PCellParameterDeclaration::t_double);
            parameters.back ().set_default (param->to_double());
        }

        if(param->desc().startsWith("rules\t") || param->desc().startsWith("anchors\t"))
        {
            parameters.back ().set_readonly(true);
        }

    }

    return parameters;
}

}



#include "crypto.h"
#include "dbPoint.h"
#include "gsiDecl.h"
#include "paramDecl.h"
#include "paramMgr.h"
#include "ParamUtils.h"


namespace gsi
{
//解密json
Class<cm::Crypto> decl_Crypto ("param", "Crypto",
    gsi::method ("decode", &cm::Crypto::decrypt_json_doc_to_string, gsi::arg ("input_file"),gsi::arg("password","ququan123456"),
                "@decode(input_file)\n"
                "\n"
                "This method was added in version 0.29.\n"
                )+
    gsi::method ("encode", &cm::Crypto::encrypt_file, gsi::arg ("input_file"), gsi::arg ("output_file"),gsi::arg("password","ququan123456"),
                "@encode(input_file,output_file)\n"
                "\n"
                "This method was added in version 0.29.\n"
                ) ,
    "@brief This namespace provides a collection of utility functions\n"
    "\n"
    "This class has been introduced in version 0.29."
    );

// ParamDecl
static pm::ParamDecl *new_p (QString key, QVariant val, QString desc)
{
    return new pm::ParamDecl(key,val,desc);
}

static double get_ParamDecl_value(pm::ParamDecl *p)
{
    double val= p->value();
    return val;
}

Class<pm::ParamDecl> decl_ParamDecl ("param", "ParamDecl",
    constructor ("new", &new_p,gsi::arg ("key"),gsi::arg ("val"),gsi::arg ("desc"),
                    "@brief new pm::ParamDecl.\n"
                    ) +
    gsi::method_ext ("value", &get_ParamDecl_value,
                    "@brief value().\n"
                    )+
    gsi::method ("==", &pm::ParamDecl::operator==, gsi::arg ("b"),
                    "@brief Compares two layer info objects\n"
                    "@return True, if both are equal\n"
                    "\n"
                    "This method was added in version 0.18.\n"
                    ) ,
    "@brief This namespace provides a collection of utility functions\n"
    "\n"
    "This class has been introduced in version 0.29."
);




// ParamMgr
static pm::ParamMgr *new_v ()
{
    return new pm::ParamMgr();
}

static void add_param(pm::ParamMgr *p,QString key, QVariant val, QString desc)
{
    pm::ParamDecl *pd= new pm::ParamDecl(key,val,desc);
    p->add_param(pd);
}

static void update_param(pm::ParamMgr *p,QString key, QVariant val, QString desc)
{
    pm::ParamDecl *pd= new pm::ParamDecl(key,val,desc);
    p->update_param(pd);
}

static bool exist_param(pm::ParamMgr *p,QString key)
{
    return p->find_param(key) != nullptr;
}

static pm::ParamDecl* find_param(pm::ParamMgr *p,QString key)
{
    return p->find_param(key);
}

static void del_param(pm::ParamMgr *p,QString key)
{
    p->del_param(key);
}

static bool add_rule(pm::ParamMgr *p,QString rule)
{
    return p->add_rule(pm::Rule(rule));
}

static void del_rule(pm::ParamMgr *p,QString rule)
{
    return p->del_rule(rule);
}

static double parse(pm::ParamMgr *p,QString exp)
{
    return p->parse(exp);
}

static void refresh_value(pm::ParamMgr *p)
{
    return p->refresh_params_value();
}

static bool isValid_rule(pm::ParamMgr *p,QString rule)
{
    return p->isValid(pm::Rule(rule));
}

static bool isValid(pm::ParamMgr *p)
{
    return p->isValid();
}

static bool isValid_pvalue(pm::ParamMgr *p,QString key, QVariant val)
{
    pm::ParamDecl *pd= new pm::ParamDecl(key,val,"");
    return p->isValid(pd);
}

QVector<QString> split_string(const QString &text, const QString& delimiter)
{
    QVector<QString> lines;
    QStringList parts = text.split(delimiter);
    for(auto str : parts)
    {
        lines.append(str);
    }
    return lines;
}

static std::vector<db::DPoint> gen_curve_points(pm::ParamMgr *p,QString equations,double distance)
{
    QVector<QString> sub_strs = split_string(equations, "\n");
    QVector<pm::ParamDecl> params;
    for(const auto& sub_str : sub_strs)
    {
        pm::ParamDecl param_equation("", sub_str);
        params.push_back(param_equation);
    }

    auto points = pm::ParamUtils::gen_curve_points(p, params,distance);
    std::vector<db::DPoint> dpoints;
    for(auto p:points)
    {
        dpoints.push_back(db::DPoint(p.x(),p.y()));
    }
    return dpoints;
}



Class<pm::ParamMgr> decl_paramMgr ("param", "ParamMgr",
    constructor ("new", &new_v,
                        "@brief new pm::ParamMgr.\n"
                        ) +
    gsi::method_ext ("add_param", &add_param, gsi::arg ("key"),gsi::arg ("val"),gsi::arg ("desc"),
                        "@brief add_param(key,val,desc).\n"
                       )+
    gsi::method_ext ("update_param", &update_param, gsi::arg ("key"),gsi::arg ("val"),gsi::arg ("desc"),
                       "@brief update_param(key,val,desc).\n"
                       )+
    gsi::method_ext ("del_param", &del_param, gsi::arg ("key"),
                        "@brief del_param(key).\n"
                       )+
    gsi::method_ext ("exist_param", &exist_param, gsi::arg ("key"),
                       "@brief exist_param(key).\n"
                       )+
    gsi::method_ext ("find_param", &find_param, gsi::arg ("key"),
                        "@brief find_param(key).\n"
                        )+
    gsi::method_ext ("add_rule", &add_rule, gsi::arg ("rule"),
                        "@brief add_rule(rule).\n"
                       )+
    gsi::method_ext ("del_rule", &del_rule, gsi::arg ("rule"),
                        "@brief del_rule(rule).\n"
                       )+
    gsi::method_ext ("refresh_value", &refresh_value,
                        "@brief refresh_value(exp).\n"
                       )+
    gsi::method_ext ("parse", &parse, gsi::arg ("exp"),
                       "@brief parse(exp).\n"
                       )+
    gsi::method_ext ("gen_curve_points", &gen_curve_points, gsi::arg ("equations"),gsi::arg ("distance"),
                       "@brief gen_curve_points(equations,distance).\n"
                       )+
    gsi::method_ext ("isValid", &isValid_rule, gsi::arg ("exp"),
                        "@brief isValid(exp).\n"
                       )+
    gsi::method_ext ("isValid", &isValid_pvalue, gsi::arg ("p"),gsi::arg ("value"),
                       "@brief isValid(exp).\n"
                       )+
    gsi::method_ext ("isValid", &isValid,
                       "@brief isValid().\n"
                       ),
    "@brief This namespace provides a collection of utility functions\n"
    "\n"
    "This class has been introduced in version 0.29."
    );

}

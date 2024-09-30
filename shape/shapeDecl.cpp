#include "shapeDecl.h"
#include "shapeFactoryBase.h"

namespace sp
{
ShapeDecl::ShapeDecl(QString name, QString icon,
                     const QVector<QPair<QString, QVariant>>& param_list,
                     ShapeFactoryBase* shape_factory,
                     bool is_varargs)
    : QObject()
    , m_icon(icon)
    , m_name(name)
    , m_fixed_param_list(param_list)
    , mp_shape_factory(shape_factory)
    , m_is_varargs(is_varargs)
{
}

ShapeDecl::ShapeDecl(const ShapeDecl &decl)
    : QObject()
    , m_icon(decl.m_icon)
    , m_name(decl.m_name)
    , m_fixed_param_list(decl.m_fixed_param_list)
    , mp_shape_factory(decl.mp_shape_factory)

{
}

ShapeDecl::~ShapeDecl()
{
    delete(mp_shape_factory);
}

QString ShapeDecl::name() const
{
    return m_name;
}

QString ShapeDecl::icon() const
{
    return m_icon;
}

QVector<QPair<QString, QVariant>> ShapeDecl::param_list() const
{
    auto total_param_list = m_fixed_param_list;
    for(const auto& param : m_variational_param_list)
    {
        total_param_list.push_back(param);
    }
    return total_param_list;
}

bool ShapeDecl::is_varargs() const
{
    return m_is_varargs;
}

void ShapeDecl::set_variational_param_list(const QVector<QPair<QString, QVariant> > &param_list)
{
    m_variational_param_list = param_list;
}

ShapeFactoryBase *ShapeDecl::factory() const
{
    return mp_shape_factory;
}
}

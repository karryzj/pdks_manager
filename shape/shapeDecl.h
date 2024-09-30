#ifndef SHAPEDECL_H
#define SHAPEDECL_H

#endif // SHAPEDECL_H

#include <QObject>
#include <qvariant.h>
#include "spCommon.h"

namespace sp
{

class ShapeFactoryBase;
class ShapeFactory;

class SP_PUBLIC ShapeDecl final: QObject
{
    Q_OBJECT
public:
    friend class ShapeFactory;

    ShapeDecl(QString name, QString icon, const QVector<QPair<QString, QVariant>>& fixed_param_list, ShapeFactoryBase* shape_factory, bool is_varargs = false);
    ShapeDecl(const ShapeDecl &decl);
    ~ShapeDecl() override;

    QString name() const;
    QString icon() const;
    QVector<QPair<QString, QVariant>> param_list() const;

    bool is_varargs() const;

    void set_variational_param_list(const QVector<QPair<QString, QVariant>>& param_list);

private:
    ShapeFactoryBase *factory() const;

private:
    QString m_icon;
    QString m_name;
    QVector<QPair<QString, QVariant>> m_fixed_param_list;
    QVector<QPair<QString, QVariant>> m_variational_param_list;
    ShapeFactoryBase* mp_shape_factory;
    bool m_is_varargs;

};

}

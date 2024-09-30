#ifndef SHAPEFACTORY_H
#define SHAPEFACTORY_H

#endif // SHAPEFACTORY_H


#include <QObject>
#include <qmap.h>

#include "shapeFactoryBase.h"


namespace pm
{

class ParamMgr;

}


namespace sp
{

class ShapeDecl;


// 此类为单例
class SP_PUBLIC ShapeFactory final : public ShapeFactoryBase
{
    Q_OBJECT
private:
    ShapeFactory();
    ~ShapeFactory() override;

public:
    static ShapeFactory* instance();  // 在软件关闭时，自动析构此单例。

    //shape register
    void register_shape_decl(ShapeDecl *decl);

    //get shape declaration
    QMap<QString, ShapeDecl*> get_shapes_decl() const;

    //create shape
    ShapeBase *create_shape(const QString &shape_name, pm::ParamMgr *param_mgr, const QVector<pm::ParamDecl> &params, ShapePointGraphicsItem* parent_attach_point) const override;
private:
    QMap<QString, ShapeDecl*> m_shapes_decl;
};
}

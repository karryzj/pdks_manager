#ifndef SHAPEFACTORYBASE_H
#define SHAPEFACTORYBASE_H



#include <QObject>
#include "spCommon.h"

class QDialog;
namespace pm
{

class ParamDecl;
class ParamMgr;

}


namespace sp
{

class ShapeBase;

class ShapePointGraphicsItem;
class SP_PUBLIC ShapeFactoryBase : public QObject
{
public:
    Q_OBJECT
public:
    ShapeFactoryBase();
    ~ShapeFactoryBase() override;

public:
    virtual ShapeBase *create_shape(const QString& shape_name, pm::ParamMgr *param_mgr, const QVector<pm::ParamDecl> &params, ShapePointGraphicsItem* parent_attach_point) const = 0;
};

}


#endif // SHAPEFACTORYBASE_H

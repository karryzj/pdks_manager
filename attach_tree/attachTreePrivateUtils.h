#ifndef ATTACHTREEPRIVATEUTILS_H
#define ATTACHTREEPRIVATEUTILS_H

#include "attachTreeDefines.h"
#include <qstring.h>


namespace pm
{
class ParamDecl;
}

namespace ly
{
class LayerInfo;
}

namespace at
{
// HINT@leixunyong。此类不对外导出，仅在本模块使用。
class AttachTreeNode;
class AttachTreeBaseNode;
class AttachTreePrivateUtils final
{
public:
    AttachTreePrivateUtils() = delete;
    static AttachTreeNode* add_new_child(AttachTreeBaseNode* parent,
                                         int parent_attach_point_idx,
                                         const QString& shape_name,
                                         const QVector<pm::ParamDecl> & params,
                                         NodeType node_type,
                                         NodeDirection node_direction,
                                         ly::LayerInfo* layer);
};
}



#endif // ATTACHTREEPRIVATEUTILS_H

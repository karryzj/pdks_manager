#ifndef PRIMIRRORDIALOG_H
#define PRIMIRRORDIALOG_H

#include "attachTreeDefines.h"
#include <QDialog>


class QComboBox;
namespace at
{
class AttachTreeNode;
}

namespace pr
{
class PriMirrorDialog final: public QDialog
{
public:
    PriMirrorDialog(at::AttachTreeNode* tree_node);
    ~PriMirrorDialog() override;

private:
    void init();
    static at::NodeDirection mirror_result_by_x(at::NodeDirection old_direction);
    static at::NodeDirection mirror_result_by_y(at::NodeDirection old_direction);
private slots:
    virtual void accept() override;

private:
    at::AttachTreeNode* mp_tree_node;
    QComboBox* mp_mirror_axis_combo_box;
};
}



#endif // PRIMIRRORDIALOG_H

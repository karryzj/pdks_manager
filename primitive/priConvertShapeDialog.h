#ifndef PRICONVERTSHAPEDIALOG_H
#define PRICONVERTSHAPEDIALOG_H

#include <QDialog>
#include <qmap.h>


class QComboBox;
namespace  at
{
class AttachTreeNode;
};

namespace pr
{
class priConvertShapeDialog final : public QDialog
{
    Q_OBJECT
public:
    priConvertShapeDialog(at::AttachTreeNode* tree_node);
    ~priConvertShapeDialog() override;

private:
    void init();

signals:
    void tree_widget_update();

private slots:
    virtual void accept() override;

private:
    at::AttachTreeNode* mp_tree_node;
    QComboBox* mp_convert_shape_combo_box;
    QMap<QString, QString> m_convert_shape_lib;
};
}



#endif // PRICONVERTSHAPEDIALOG_H

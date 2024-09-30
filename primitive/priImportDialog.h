#ifndef PRIIMPORTDIALOG_H
#define PRIIMPORTDIALOG_H

#include <qdialog.h>
#include <qmap.h>
#include <qvector.h>


class QComboBox;
namespace at
{
class AttachTreeBaseNode;
class AttachTreeNode;
}

namespace pr
{
class Primitive;
class PriImportDialog final : public QDialog
{
    Q_OBJECT
    friend class PrimitiveWindow;
private:
    PriImportDialog(Primitive* pri);
    ~PriImportDialog() override;
private:
    void init();
    void init_primitive_list();

private slots:
    void update_primitive_folders_selection(int index);
    void add_primitive_to_selected_tree_node();
private:
    Primitive* mp_pri;
    QComboBox* mp_category_combo_box = nullptr;
    QComboBox* mp_primitive_folder_combo_box = nullptr;
    QMap<QString, QVector<QString>> m_category_to_primitive_folders;
};
}



#endif // PRIIMPORTDIALOG_H

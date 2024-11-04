#ifndef PRIROUNEDCORNERDIALOG_H
#define PRIROUNEDCORNERDIALOG_H

#include <QDialog>


class QLineEdit;

class QComboBox;
namespace pr
{
class Primitive;
class PriRounedCornerDialog final : public QDialog
{
    Q_OBJECT
public:
    PriRounedCornerDialog(Primitive* pri);
    ~PriRounedCornerDialog();

private:
    void init();
    void deinit();

private slots:
    void accept_modifies();

private:
    Primitive* mp_pri;

    QLineEdit* mp_inner_corner_radius_line_edit;
    QLineEdit* mp_outer_corner_radius_line_edit;
    QComboBox* mp_direction_combo_box;
};

}


#endif // PRIROUNEDCORNERDIALOG_H

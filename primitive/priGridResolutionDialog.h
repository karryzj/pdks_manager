#ifndef PRIGRIDRESOLUTIONDIALOG_H
#define PRIGRIDRESOLUTIONDIALOG_H

#include <QDialog>

class QLineEdit;
namespace pr
{

class PriGridResolutionDialog final : public QDialog
{
    Q_OBJECT
public:
    PriGridResolutionDialog(QAction* res_action);
    ~PriGridResolutionDialog() override;

private:
    void init();

private slots:
    void update_resolution();

private:
    QAction* mp_res_action;
    QLineEdit *mp_line_edit;
};

}


#endif // PRIGRIDRESOLUTIONDIALOG_H

#ifndef PRIROTATECFGDIALOG_H
#define PRIROTATECFGDIALOG_H

#include <QDialog>

namespace Ui
{
class PriRotateCfgDialog;
}

struct RotateCfg
{
    QString degrees;
    int by_to;
    int child_self;
};

class PriRotateCfgDialog : public QDialog
{
    Q_OBJECT

public:
    explicit PriRotateCfgDialog(QWidget *parent = nullptr);
    ~PriRotateCfgDialog();

    RotateCfg get_result();
    RotateCfg get_old_result();

private slots:
    void on_buttonBox_accepted();

    void on_buttonBox_rejected();

private:
    Ui::PriRotateCfgDialog *ui;
    RotateCfg m_rotate_cfg;
    RotateCfg m_old_rotate_cfg;
};

#endif // PRIROTATECFGDIALOG_H

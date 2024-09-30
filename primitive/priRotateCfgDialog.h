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

private:
    Ui::PriRotateCfgDialog *ui;
};

#endif // PRIROTATECFGDIALOG_H

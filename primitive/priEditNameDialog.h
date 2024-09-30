#ifndef PRIEDITNAMEDIALOG_H
#define PRIEDITNAMEDIALOG_H

#include <QDialog>

namespace Ui
{
class PriEditNameDialog;
}

class PriEditNameDialog : public QDialog
{
    Q_OBJECT

public:
    explicit PriEditNameDialog(QWidget *parent = nullptr);
    PriEditNameDialog(const QString &name, QWidget *parent = nullptr);
    ~PriEditNameDialog();

    QString getContent() const;
    int get_type() const;
    void set_type_visible(bool is_visible);

private:
    Ui::PriEditNameDialog *ui;
};

#endif // PRIEDITNAMEDIALOG_H

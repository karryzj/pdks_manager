#ifndef PRIPARAMCHECKBOX_H
#define PRIPARAMCHECKBOX_H

#include <QCheckBox>

namespace pr
{
class PriShapeDialog;
class PriParamCheckBox final: public QCheckBox
{
    Q_OBJECT
public:
    explicit PriParamCheckBox(const QString& param_name, PriShapeDialog* dialog);
    ~PriParamCheckBox() override;

public:
    const QString& param_name() const;

signals:
    void change_extracted_param_status(bool checked, const QString& param_name);

private slots:
    void on_state_changed(int state);

private:
    QString m_param_name;
};
}


#endif // PRIPARAMCHECKBOX_H

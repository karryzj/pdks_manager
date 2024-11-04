#include "priParamCheckBox.h"
#include <PriShapeDialog.h>

using namespace pr;

PriParamCheckBox::PriParamCheckBox(const QString& param_name, PriShapeDialog* dialog)
    : QCheckBox(dialog)
    , m_param_name(param_name)
{
    connect(this, &QCheckBox::stateChanged, this, &PriParamCheckBox::on_state_changed);
    setObjectName(param_name);
}

PriParamCheckBox::~PriParamCheckBox()
{

}

const QString &PriParamCheckBox::param_name() const
{
    return m_param_name;
}

void PriParamCheckBox::on_state_changed(int state)
{
    bool checked = false;
    if (state == Qt::Checked)
    {
        checked = true;
    }
    emit change_extracted_param_status(checked, m_param_name);
}

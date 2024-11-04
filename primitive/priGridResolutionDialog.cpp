#include "priGridResolutionDialog.h"
#include "common_defines.h"
#include "qaction.h"
#include "qlineedit.h"

#include <QDoubleValidator>
#include <qboxlayout.h>
#include <qdialogbuttonbox.h>
#include <qlabel.h>
#include <qmessagebox.h>

#include "configManager.h"
using namespace pr;

PriGridResolutionDialog::PriGridResolutionDialog(QAction* res_action)
    : QDialog()
    , mp_res_action(res_action)
{
    init();
}

PriGridResolutionDialog::~PriGridResolutionDialog()
{

}

void PriGridResolutionDialog::init()
{
    // 创建布局
    QVBoxLayout *layout = new QVBoxLayout(this);

    // 创建输入框并设置只能输入数字
    mp_line_edit = new QLineEdit(this);
    QDoubleValidator *validator = new QDoubleValidator(0.01, std::numeric_limits<double>::max(), 3, this);
    validator->setNotation(QDoubleValidator::StandardNotation);
    mp_line_edit->setValidator(validator);
    layout->addWidget(mp_line_edit);

    // 创建按钮框
    QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    layout->addWidget(buttonBox);

    // 连接按钮信号到槽
    connect(buttonBox, &QDialogButtonBox::accepted, this, &PriGridResolutionDialog::update_resolution);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
}

void pr::PriGridResolutionDialog::update_resolution()
{
    // 更新配置的值
    Q_ASSERT(mp_line_edit);
    QString res_str = mp_line_edit->text();
    qreal res = res_str.toDouble();
    if(res < 0.001)
    {
        QMessageBox::warning(this, "Warning", "Your entered grid resolution is less than 0.001, please re-enter.");
        return;
    }

    accept();

    cm::ConfigManager::instance()->setup_value(CM_LOGIC_GRID_RESOLUTION_KEY, res);

    // 更新action的名字
    mp_res_action->setText(res_str);
    return;
}

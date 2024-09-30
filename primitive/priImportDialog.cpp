#include "priImportDialog.h"
#include "ShapePointGraphicsItem.h"
#include "attachTreeRootNode.h"
#include "primitive.h"
#include "primitiveDefines.h"

#include <PriFiler.h>
#include <qboxlayout.h>
#include <qcombobox.h>
#include <qfiledialog.h>
#include <qjsondocument.h>
#include <qjsonobject.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qpushbutton.h>
#include <QDebug>
#include "crypto.h"
using namespace pr;

PriImportDialog::PriImportDialog(Primitive* pri)
    : QDialog()
    , mp_pri(pri)
{
    init();
}

PriImportDialog::~PriImportDialog()
{

}

void pr::PriImportDialog::init()
{
    init_primitive_list();

    this->setWindowTitle(tr(PRI_IMPORT_DIALOG_NAME));  // 设置面板名
    QVBoxLayout* main_layout = new QVBoxLayout(this);

    // 指定图元的一级目录
    QHBoxLayout* category_layout = new QHBoxLayout(this);

    QLabel* category_label = new QLabel(tr(PRI_IMPORT_DIALOG_CATEGORY_NAME));
    mp_category_combo_box = new QComboBox(this);
    mp_category_combo_box->setObjectName(tr(PRI_IMPORT_DIALOG_CATEGORY_NAME));
    for (auto itor = m_category_to_primitive_folders.constBegin(); itor != m_category_to_primitive_folders.constEnd(); ++itor)
    {
        const QString &category_name = itor.key();
        mp_category_combo_box->addItem(category_name);
    }
    category_layout->addWidget(category_label);
    category_layout->addWidget(mp_category_combo_box);
    main_layout->addLayout(category_layout);

    // 指定图元的二级目录

    QHBoxLayout* primitive_folder_layout = new QHBoxLayout(this);

    QLabel* primitive_folder_label = new QLabel(tr(PRI_IMPORT_DIALOG_PRIMITIVE_FOLDER_NAME));
    mp_primitive_folder_combo_box = new QComboBox(this);
    mp_primitive_folder_combo_box->setObjectName(tr(PRI_IMPORT_DIALOG_PRIMITIVE_FOLDER_NAME));
    if(!m_category_to_primitive_folders.isEmpty())
    {
        const QVector<QString>& primitive_folders = m_category_to_primitive_folders.begin().value();
        for (auto itor = primitive_folders.constBegin(); itor != primitive_folders.constEnd(); ++itor)
        {
            const QString &primitive_folder = *itor;
            mp_primitive_folder_combo_box->addItem(primitive_folder);
        }
    }

    primitive_folder_layout->addWidget(primitive_folder_label);
    primitive_folder_layout->addWidget(mp_primitive_folder_combo_box);
    main_layout->addLayout(primitive_folder_layout);

    // 设置一级目录和二级目录的联动
    connect(mp_category_combo_box, QOverload<int>::of( & QComboBox::currentIndexChanged),
            this, & PriImportDialog::update_primitive_folders_selection);

    // 分隔线
    QFrame* line = new QFrame(this);
    line->setFrameShape(QFrame::HLine);
    line->setFrameShadow(QFrame::Sunken);
    main_layout->addWidget(line);

    // 右下角增加 确定和取消按钮
    QHBoxLayout* button_layout = new QHBoxLayout();

    QPushButton* ok_button = new QPushButton(tr(PRI_OK));
    connect(ok_button, & QPushButton::clicked, this, & QDialog::accept);
    ok_button->setObjectName(QObject::tr(PRI_OK));
    connect(ok_button, &QPushButton::clicked, this, &PriImportDialog::add_primitive_to_selected_tree_node);

    QPushButton* cancel_button = new QPushButton(QObject::tr(PRI_CANCEL));
    connect(cancel_button, & QPushButton::clicked, this, & QDialog::close);
    cancel_button->setObjectName(tr(PRI_CANCEL));
    connect(cancel_button, &QPushButton::clicked, this, &QPushButton::close);

    button_layout->addWidget(ok_button);
    button_layout->addWidget(cancel_button);

    main_layout->addLayout(button_layout);

    this->setFixedSize(PRI_IMPORT_DIALOG_WIDTH, PRI_IMPORT_DIALOG_HEIGHT);
}

void pr::PriImportDialog::init_primitive_list()
{
    const QString current_path = QDir::currentPath() + "/data/primitives";
    QDir dir(current_path);

    Q_ASSERT(dir.exists() && dir.isReadable());  // 检查路径是否存在
    QStringList sub_directories = dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);

    auto cates = mp_pri->primitive_path();
    for(const QString &dir_name : sub_directories)
    {
        QString sub_current_path = current_path + "/" + dir_name;
        QDir sub_dir(sub_current_path);
        Q_ASSERT(sub_dir.exists() && sub_dir.isReadable());  // 检查路径是否存在

        QStringList json_folders = sub_dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
        QVector<QString> json_folder_names;
        for(const QString& json_folder_name : json_folders)  // 这里看起来非常蠢
        {
            if(cates.first == dir_name && cates.second == json_folder_name)
            {
                continue;
            }
            json_folder_names.push_back(json_folder_name);
        }
        m_category_to_primitive_folders.insert(dir_name, json_folder_names);
    }
}

void pr::PriImportDialog::add_primitive_to_selected_tree_node()
{
    if(mp_category_combo_box->count() == 0 || mp_category_combo_box->currentText().isEmpty())
    {
        return;
    }

    if(mp_primitive_folder_combo_box->count() == 0 || mp_primitive_folder_combo_box->currentText().isEmpty())
    {
        return;
    }

    const QString current_path = QDir::currentPath() + "/data/primitives";
    QString primitive_json_path = current_path + "/" + mp_category_combo_box->currentText() + "/" +
                                  mp_primitive_folder_combo_box->currentText() + "/data.json";  //  HINT@leixunyong。这样的硬编码实在是太丑陋了。

    QJsonDocument doc = cm::Crypto::decrypt_json_doc(primitive_json_path);  // HINT@leixunyong,这里需要输入密码，但是我没有。
    if (!doc.isObject())
    {
        qDebug() << "JSON document is not an object.";
        return;
    }

    // QJsonObject jsonObject = doc.object();
    // QFile json_file(primitive_json_path);
    // if (!json_file.open(QIODevice::ReadOnly | QIODevice::Text))
    // {
    //     QString error_info = json_file.errorString();
    //     return;
    // }

    // QByteArray file_data = json_file.readAll();
    // json_file.close();

    QJsonDocument& json_doc = doc;
    Q_ASSERT(!json_doc.isNull() && json_doc.isObject());

    QJsonObject json_obj = json_doc.object();

    PriFiler filer(mp_pri);
    auto root_point_item = sp::ShapePointGraphicsItem::cachedItem;
    if(nullptr == root_point_item)
    {
        root_point_item = mp_pri->at_root()->origin_point();
    }

    filer.load_primitive_from_file(json_obj, root_point_item);
    accept();
}

void PriImportDialog::update_primitive_folders_selection(int)
{
    mp_primitive_folder_combo_box->clear();
    auto itor = m_category_to_primitive_folders.find(mp_category_combo_box->currentText());
    Q_ASSERT(m_category_to_primitive_folders.end() != itor);

    const auto& primitive_folders = itor.value();
    for(const auto& primitive_folder : primitive_folders)
    {
        mp_primitive_folder_combo_box->addItem(primitive_folder);
    }
}

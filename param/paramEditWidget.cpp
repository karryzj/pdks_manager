#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QTableWidgetItem>
#include <QMessageBox>
#include "paramMgr.h"
#include "paramDecl.h"
#include "paramEditWidget.h"
#include "paramEditDialog.h"
#include "expression.h"
#include "muParser.h"
#include "rule.h"

namespace pm
{
ParamEditWidget::ParamEditWidget(ParamMgr *param_mgr, QWidget *parent)
    : QWidget{parent}, mp_table_widget(new QTableWidget(0, 3)), mp_param_mgr(param_mgr)
{
    QVBoxLayout *layout = new QVBoxLayout();
    mp_table_widget->setHorizontalHeaderLabels({tr("名称"), tr("默认值"), tr("描述")});
    mp_table_widget->setSelectionBehavior(QAbstractItemView::SelectRows);
    mp_table_widget->setSelectionMode(QAbstractItemView::SingleSelection);
    mp_table_widget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    mp_table_widget->horizontalHeader()->setStretchLastSection(true);
    mp_table_widget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    QHBoxLayout *button_bottom = new QHBoxLayout();
    mp_new_button = new QPushButton(QIcon(":/img/lay_new.png"), tr("新增"));
    mp_del_button = new QPushButton(QIcon(":/img/lay_del.png"), tr("删除"));
    // 设置按钮的圆角矩形样式
    QString button_style = "QPushButton {"
                           "border: 2px solid #5F9EA0;"  // 边框颜色
                           "border-radius: 10px;"         // 圆角半径
                           "background-color: #ADD8E6;"   // 背景颜色
                           "padding: 5px 15px;"           // 内边距
                           "}";

    mp_new_button->setStyleSheet(button_style);
    mp_new_button->setEnabled(true);
    mp_del_button->setStyleSheet(button_style);
    // mp_del_button->setEnabled(false);
    button_bottom->addWidget(mp_new_button);
    button_bottom->addWidget(mp_del_button);
    layout->addWidget(mp_table_widget);
    layout->addLayout(button_bottom);
    setLayout(layout);

    update_all_items();

    connect(mp_new_button, &QPushButton::clicked, this, &ParamEditWidget::addItem);
    connect(mp_del_button, &QPushButton::clicked, this, &ParamEditWidget::deleteItem);
    connect(mp_table_widget, &QTableWidget::cellDoubleClicked, this, &ParamEditWidget::onCellDoubleClicked);
}

void ParamEditWidget::addItem()
{
    ParamEditDialog dialog(this);
    if (dialog.exec() == QDialog::Accepted)
    {
        QStringList contents = dialog.getContent();
        auto name = contents.first();
        auto expr_str = contents.at(1);
        QString desc = "";
        if (contents.size() > 2)
        {
            desc = contents.last();
        }
        // HITN@leixunyong。如果同名参数已经存在，弹窗警告并退出。
        auto found_param = mp_param_mgr->find_param(name);
        if(found_param)
        {
            // 创建并显示警告弹窗
            QMessageBox::warning(this, "Warning", "The parameter with the same name is added to the parameter list!");
            return;
        }

        ParamDecl *param_decl = new ParamDecl(name, expr_str, desc);
        mp_param_mgr->add_param(param_decl);

        int rowCount = mp_table_widget->rowCount();
        mp_table_widget->insertRow(rowCount);
        // mp_table_widget->setItem(rowCount, 0, new QTableWidgetItem(QString::number(rowCount + 1)));
        mp_table_widget->setItem(rowCount, 0, new QTableWidgetItem(name));
        mp_table_widget->setItem(rowCount, 1, new QTableWidgetItem(expr_str));
        if (contents.size() > 2)
        {
            mp_table_widget->setItem(rowCount, 2, new QTableWidgetItem(desc));
        }
    }
}

bool ParamEditWidget::has_variable(mu::Parser &parser, const QString &expr, const QString &name)
{
    parser.SetExpr(expr.toStdWString());
    auto variables = parser.GetUsedVar();
    if (variables.find(name.toStdWString()) != variables.end())
    {
        return true;
    }
    else
    {
        return false;
    }
}

void ParamEditWidget::deleteItem()
{
    int currentRow = mp_table_widget->currentRow();
    if (currentRow != -1)
    {
        QTableWidgetItem *item = mp_table_widget->item(currentRow, 0);
        if (item)
        {
            mu::Parser parser;
            QString name = item->text();
            auto it = std::find_if(mp_param_mgr->params().begin(), mp_param_mgr->params().end(), [&](const auto param)
            {
                return param->key() == name;
            });
            if (it != mp_param_mgr->params().end())
            {
                // for (auto new_it = mp_param_mgr->params().begin(); new_it != it; ++new_it)
                // {
                //     parser.DefineConst((*new_it)->key().toStdWString(), (*new_it)->value());
                // }
                // parser.DefineConst((*it)->key().toStdWString(), (*it)->value());

                for (++it; it != mp_param_mgr->params().end(); ++it)
                {
                    auto is_found = has_variable(parser, (*it)->expression(), name);
                    if (is_found)
                        return;
                }
            }

            mp_param_mgr->del_param(item->text());
        }
        mp_table_widget->removeRow(currentRow);

    }
}


void ParamEditWidget::onCellDoubleClicked(int row, int column)
{
    // 获取当前数据
    QStringList currentData;
    QTableWidgetItem *item1 = mp_table_widget->item(row, 0);
    QTableWidgetItem *item2 = mp_table_widget->item(row, 1);
    QTableWidgetItem *item3 = mp_table_widget->item(row, 2);
    if (item3)
    {
        // mp_param_mgr->add_param(new ParamDecl(item1->text(), item2->text().toDouble(), item3->text()));
        currentData << item1->text() << item2->text() << item3->text();
    }
    else
    {
        // mp_param_mgr->add_param(new ParamDecl(item1->text(), item2->text().toDouble()));
        currentData << item1->text() << item2->text();
    }

    // 弹出对话框
    ParamEditDialog dialog(currentData, this);
    if (dialog.exec() == QDialog::Accepted)
    {
        // 获取修改后的数据
        QStringList newData = dialog.getContent();
        // 更新 QTableWidget 的数据
        if (currentData != newData)
        {
            auto old_params = mp_param_mgr->clone_params();
            QString new_name = newData.at(0);
            QString new_value = newData.at(1);
            QString new_desc = "";
            if (newData.size() > 2)
            {
                new_desc = newData.at(2);
            }
            ParamDecl tmp_param(new_name, new_value, new_desc);
            mp_param_mgr->update_param(&tmp_param);

            mp_param_mgr->refresh_params_value();
            auto is_update_ok = mp_param_mgr->isValid();
            if (!is_update_ok)
            {
                mp_param_mgr->params().clear();
                mp_param_mgr->params() = std::move(old_params);
            }
            else
            {
                old_params.clear();
                item2->setText(new_value);
                if (newData.size() > 2)
                {
                    item3->setText(new_desc);
                }
                emit mp_param_mgr->param_changed(new_name);
                emit param_changed();
            }
        }
    }
}

void ParamEditWidget::on_add_new_expressison(const QString &name, const QString &expr_str, const QString &desc)
{
    ParamDecl *param_decl = new ParamDecl(name, expr_str, desc);
    mp_param_mgr->add_param(param_decl);

    int rowCount = mp_table_widget->rowCount();
    mp_table_widget->insertRow(rowCount);
    mp_table_widget->setItem(rowCount, 0, new QTableWidgetItem(name));
    mp_table_widget->setItem(rowCount, 1, new QTableWidgetItem(expr_str));
    mp_table_widget->setItem(rowCount, 2, new QTableWidgetItem(desc));
}

ParamEditWidget::~ParamEditWidget()
{
    delete mp_table_widget;
}

void ParamEditWidget::update_all_items()
{
    mp_table_widget->clear();
    mp_table_widget->setRowCount(0);
    for (auto i = 0; i < mp_param_mgr->params().size(); i++)
    {
        int rowCount = i;
        mp_table_widget->insertRow(rowCount);
        QString name = mp_param_mgr->params().at(i)->key();
        auto expr_str = mp_param_mgr->params().at(i)->expression();
        auto desc = mp_param_mgr->params().at(i)->desc();
        mp_table_widget->setItem(rowCount, 0, new QTableWidgetItem(name));
        mp_table_widget->setItem(rowCount, 1, new QTableWidgetItem(expr_str));
        if (!desc.isEmpty())
        {
            mp_table_widget->setItem(rowCount, 2, new QTableWidgetItem(desc));
        }
        else
        {
            mp_table_widget->setItem(rowCount, 2, new QTableWidgetItem());
        }
    }
}

}

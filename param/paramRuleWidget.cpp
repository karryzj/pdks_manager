#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QMessageBox>
#include "paramRuleWidget.h"
#include "paramRuleDialog.h"
#include "paramMgr.h"
#include "paramDecl.h"
#include "rule.h"
#include <QDebug>
#include "muParser.h"

namespace pm
{
ParamRuleWidget::ParamRuleWidget(ParamMgr *param_mgr, QWidget *parent)
    : QWidget{parent}, mp_table_widget(new QTableWidget(0, 1)), mp_param_mgr(param_mgr)
{
    QVBoxLayout *layout = new QVBoxLayout();
    mp_table_widget->setHorizontalHeaderLabels({tr("约束")});
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

    connect(mp_new_button, &QPushButton::clicked, this, &ParamRuleWidget::addItem);
    connect(mp_del_button, &QPushButton::clicked, this, &ParamRuleWidget::deleteItem);
    connect(mp_table_widget, &QTableWidget::cellDoubleClicked, this, &ParamRuleWidget::onCellDoubleClicked);
}

void ParamRuleWidget::addItem()
{
    ParamRuleDialog dialog(this);
    if (dialog.exec() == QDialog::Accepted)
    {
        QString contents = dialog.getContent().trimmed();
        if (!mp_param_mgr->rules().isEmpty())
        {
            const auto it = std::find_if(mp_param_mgr->rules().cbegin(), mp_param_mgr->rules().cend(), [&](const Rule& rule)
            {
                return contents == rule.to_str();
            });
            if(it != mp_param_mgr->rules().end())
            {
                QMessageBox::warning(this, "Warning", "Rule existed!");
                return;
            }
        }

        auto ret = mp_param_mgr->add_rule(Rule(contents));
        if (!ret)
            return;
        int rowCount = mp_table_widget->rowCount();
        mp_table_widget->insertRow(rowCount);
        mp_table_widget->setItem(rowCount, 0, new QTableWidgetItem(contents));
    }
}

void ParamRuleWidget::deleteItem()
{
    int currentRow = mp_table_widget->currentRow();
    if (currentRow != -1)
    {
        QTableWidgetItem *item = mp_table_widget->item(currentRow, 0);
        if (item)
        {
            mp_param_mgr->del_rule(item->text());
        }
        mp_table_widget->removeRow(currentRow);

        // 重新排列序号
        // for (int i = 0; i < mp_table_widget->rowCount(); ++i)
        // {
        //     mp_table_widget->setItem(i, 0, new QTableWidgetItem(QString::number(i + 1)));
        // }
    }
}

void ParamRuleWidget::onCellDoubleClicked(int row, int column)
{
    // 获取当前数据
    QString currentData;
    QTableWidgetItem *item = mp_table_widget->item(row, 0);
    if (item)
    {
        currentData = item->text();
        mp_param_mgr->add_rule(Rule(currentData));
    }

    // 弹出对话框
    ParamRuleDialog dialog(currentData, this);
    if (dialog.exec() == QDialog::Accepted)
    {
        // 获取修改后的数据
        QString newData = dialog.getContent();
        // 更新 QTableWidget 的数据
        if (currentData != newData)
        {
            mp_param_mgr->del_rule(currentData);
            mp_param_mgr->add_rule(Rule(newData));
            item->setText(newData);
        }
    }
}

ParamRuleWidget::~ParamRuleWidget()
{
    delete mp_table_widget;
}

void ParamRuleWidget::update_all_items()
{
    mp_table_widget->clear();
    for (auto i = 0; i < mp_param_mgr->rules().size(); i++)
    {
        int rowCount = i;
        mp_table_widget->insertRow(rowCount);
        QString name = mp_param_mgr->rules().at(i).to_str();
        mp_table_widget->setItem(rowCount, 0, new QTableWidgetItem(name));
    }
}

}

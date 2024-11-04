#include <algorithm>
#include <iostream>
#include <QMessageBox>
#include "paramMgr.h"
#include "paramDecl.h"
#include "qdebug.h"
#include "rule.h"
#include "muParser.h"

namespace pm
{

ParamMgr::ParamMgr() : mp_parser(new mu::Parser())
{

}

ParamMgr::~ParamMgr()
{
    delete mp_parser;
}

void ParamMgr::refresh_params_value()
{
    mu::Parser param_parser;
    for (auto it = m_params.begin(); it != m_params.end(); ++it)
    {
        if (it == m_params.begin())
        {
            param_parser.SetExpr((*it)->expression().toStdWString());
            auto new_ret = param_parser.Eval();
            (*it)->set_value(new_ret);
            param_parser.DefineConst((*it)->key().toStdWString(), (*it)->value());
        }
        else
        {
            try
            {
                param_parser.SetExpr((*it)->expression().toStdWString());
                auto new_ret = param_parser.Eval();
                (*it)->set_value(new_ret);
                param_parser.DefineConst((*it)->key().toStdWString(), (*it)->value());
            }
            catch(mu::Parser::exception_type &e)
            {
                std::wcout << "Error: " << e.GetMsg() << std::endl;
                return;
            }
        }
    }
}

void ParamMgr::add_param(ParamDecl *param_decl)
{
    // HINT@leixunyong。需要查重。
    auto key = param_decl->key();
    auto found_param = find_param(key);
    if(nullptr != found_param)
    {
        delete param_decl;
        qDebug() << "add " << key << " parameter" << "twice";
        return;   // HINT@leixunyong。如果有一样的参数，就不进行处理。
    }

    double val = calc_expr(param_decl->expression());
    param_decl->set_value(val);
    m_params.append(param_decl);
}

void ParamMgr::del_param(ParamDecl *param)
{
    del_param(param->key());
}

void ParamMgr::del_param(const QString &key)
{
    auto itor = std::find_if(m_params.begin(), m_params.end(), [&](auto param)
    {
        return param->key() == key;
    });
    if(itor != m_params.end())
    {
        m_params.removeOne(*itor);
    }
}

void ParamMgr::update_param(ParamDecl *param)
{
    auto itor = std::find_if(m_params.begin(), m_params.end(), [&](auto item)
    {
        return param->key() == item->key();
    });
    if(itor != m_params.end())
    {
        (*itor)->set_expression(param->expression());
        (*itor)->set_value(param->value());
        (*itor)->set_desc(param->desc());
    }
}

QList<ParamDecl *> ParamMgr::clone_params()
{
    QList<ParamDecl *> params_list;
    for (auto param : m_params)
    {
        if (param)
        {
            params_list.append(clone_param(param));
        }
    }
    return params_list;
}

ParamDecl *ParamMgr::clone_param(ParamDecl *param)
{
    return (new ParamDecl(*param));
}

QList<ParamDecl*> &ParamMgr::params()
{
    return m_params;
}

ParamDecl *ParamMgr::find_param(const QString &param_name)
{
    for(auto param : m_params)
    {
        if(param->key() == param_name)
        {
            return param;
        }
    }
    return nullptr;
}

bool ParamMgr::add_rule(const Rule &rule)
{
    auto ret = parse_rule(rule);
    if (ret == 0)
    {
        // QMessageBox::warning(NULL, "Error", "Rule Invalid");
        return false;
    }

    m_rules.push_back(rule);
    return true;
}

void ParamMgr::del_rule(const Rule &rule)
{
    del_rule(rule.to_str());
}

void ParamMgr::del_rule(const QString &rule_str)
{
    auto it = std::find_if(m_rules.begin(), m_rules.end(), [&](const Rule& rule)
    {
        return rule_str == rule.to_str();
    });
    if(it != m_rules.end())
    {
        m_rules.erase(it);
    }
}

QVector<Rule>& ParamMgr::rules()
{
    return m_rules;
}

bool ParamMgr::isValid()
{
    mu::Parser parser;
    for (const auto &val : m_params)
    {
        auto name = val->key().toStdWString();
        parser.DefineConst(name, val->value());
    }

    for (auto item : m_rules)
    {
        try
        {
            parser.SetExpr(item.to_str().toStdWString());
            auto ret = parser.Eval();
            if (Expression::is_bool(item.to_str()))
            {
                if (ret == 0)
                {
                    // QMessageBox::warning(NULL, "Error", "Rule Invalid");
                    return false;
                }
            }
            else
            {
                return false;
            }
        }
        catch(mu::Parser::exception_type &e)
        {
            std::wcout << "Error: " << e.GetMsg() << std::endl;
            return false;
        }
    }

    return true;
}

bool ParamMgr::isValid(const Rule &rule)
{
    auto ret = parse_rule(rule);
    if (ret == 0)
    {
        // QMessageBox::warning(NULL, "Error", "Rule Invalid");
        return false;
    }
    return true;
}

bool ParamMgr::isValid(ParamDecl *param_decl)
{
    mu::Parser param_parser;
    mu::Parser rule_parser;
    for (const auto &param : m_params)
    {
        auto name = param->key().toStdWString();
        param_parser.DefineConst(name, param->value());
        rule_parser.DefineConst(name, param->value());
    }

    auto new_name = param_decl->key().toStdWString();
    auto new_expr = param_decl->expression();
    bool ret_ok = false;
    auto ret_val = new_expr.toDouble(&ret_ok);
    if (!ret_ok)
    {
        try
        {
            param_parser.SetExpr(new_expr.toStdWString());
            auto new_ret = param_parser.Eval();
            param_decl->set_value(new_ret);
        }
        catch(mu::Parser::exception_type &e)
        {
            std::wcout << "Error: " << e.GetMsg() << std::endl;
            return false;
        }
    }
    double new_value = param_decl->value();
    rule_parser.DefineConst(new_name, new_value);

    //更新相关参数 add by supeng
    for (auto it = m_params.begin(); it != m_params.end(); ++it)
    {
        if((*it)->key().toStdWString() == new_name)
        {
            continue;
        }
        if (it == m_params.begin())
        {
            rule_parser.SetExpr((*it)->expression().toStdWString());
            auto new_ret = rule_parser.Eval();
            rule_parser.DefineConst((*it)->key().toStdWString(), new_ret);
        }
        else
        {
            try
            {
                rule_parser.SetExpr((*it)->expression().toStdWString());
                auto new_ret = rule_parser.Eval();
                rule_parser.DefineConst((*it)->key().toStdWString(), new_ret);
            }
            catch(mu::Parser::exception_type &e)
            {
                std::wcout << "Error: " << e.GetMsg() << std::endl;
                return false;
            }
        }
    }

    for (auto item : m_rules)
    {
        try
        {
            rule_parser.SetExpr(item.to_str().toStdWString());
            auto ret = rule_parser.Eval();
            if (ret == 0)
            {
                // QMessageBox::warning(NULL, "Error", "Rule Invalid");
                return false;
            }
        }
        catch(mu::Parser::exception_type &e)
        {
            std::wcout << "Error: " << e.GetMsg() << std::endl;
            return false;
        }
    }
    return true;
}

bool ParamMgr::parse_rule(const Rule &rule)
{
    mu::Parser parser;
    for (const auto &val : m_params)
    {
        auto name = val->key().toStdWString();
        parser.DefineConst(name, val->value());
    }

    double ret;
    try
    {
        parser.SetExpr(rule.to_str().toStdWString());
        ret = parser.Eval();
        if (ret)
            return true;
        else
            return false;
    }
    catch(mu::Parser::exception_type &e)
    {
        std::wcout << "Error: " << e.GetMsg() << std::endl;
        return false;
    }
    return true;
}

double ParamMgr::parse(const QString &exp)
{
    mu::Parser parser;
    for (const auto &val : m_params)
    {
        auto name = val->key().toStdWString();
        parser.DefineConst(name, val->value());
    }
    double ret;
    try
    {
        parser.SetExpr(exp.toStdWString());
        ret = parser.Eval();
    }
    catch(mu::Parser::exception_type &e)
    {
        std::wcout << "Error: " << e.GetMsg() << std::endl;
        return 0;
    }
    return ret;
}

double ParamMgr::calc_expr(const QString &exp)
{
    mu::Parser parser;
    for (const auto &val : m_params)
    {
        auto name = val->key().toStdWString();
        parser.DefineConst(name, val->value());    // HINT@leixunyong。对于一些特殊字符，如“_”和“Δ”都是会直接崩溃的。
    }

    double ret;
    try
    {
        parser.SetExpr(exp.toStdWString());
        ret = parser.Eval();
    }
    catch(mu::Parser::exception_type &e)
    {
        std::wcout << "Error: " << e.GetMsg() << std::endl;
        return 0;
    }
    return ret;
}

double ParamMgr::calc_expr(const QString &exp, bool &ok)
{
    mu::Parser parser;
    for (const auto &val : m_params)
    {
        auto name = val->key().toStdWString();
        parser.DefineConst(name, val->value());    // HINT@leixunyong。对于一些特殊字符，如“_”和“Δ”都是会直接崩溃的。
    }

    double ret;
    try
    {
        parser.SetExpr(exp.toStdWString());
        ret = parser.Eval();
    }
    catch(mu::Parser::exception_type &e)
    {
        std::wcout << "Error: " << e.GetMsg() << std::endl;
        ok = false;
        return 0;
    }
    ok = true;
    return ret;
}


void ParamMgr::reset()
{
    for (auto &param : m_params)
    {
        delete param;
    }
    m_params.clear();
    m_rules.clear();
}

QVector<Rule> ParamMgr::clone_rules()
{
    QVector<Rule> rules;
    rules = m_rules;
    return rules;
}


}

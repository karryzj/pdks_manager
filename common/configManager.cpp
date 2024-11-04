#include "configManager.h"

using namespace cm;

ConfigManager::ConfigManager()
{
}

ConfigManager::~ConfigManager()
{
}

ConfigManager *ConfigManager::instance()
{
    static ConfigManager _instance;
    return &_instance;
}

void ConfigManager::register_new_config(const QString &key, const QVariant &value)
{
    auto itor = m_configs.find(key);
    if(itor == m_configs.end())
    {
        m_configs[key] = value;
    }
}

void ConfigManager::unregister_new_config(const QString &key)
{
    m_configs.remove(key);
}

const QVariant &ConfigManager::query(const QString &key) const
{
    auto itor = m_configs.find(key);
    Q_ASSERT(itor != m_configs.end());
    return itor.value();
}

void ConfigManager::setup_value(const QString &key, const QVariant &value)
{
    m_configs[key] = value;
}

bool ConfigManager::has_key(const QString &key)
{
    return m_configs.find(key) != m_configs.end();
}

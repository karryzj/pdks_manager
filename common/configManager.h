#ifndef CONFIGMANAGER_H
#define CONFIGMANAGER_H

#include "common.h"

#include <QVariant>
#include <qstring.h>

namespace cm
{
class CM_PUBLIC ConfigManager final
{
private:
    ConfigManager();
    ~ConfigManager();
public:
    static ConfigManager* instance();

    void register_new_config(const QString& key, const QVariant& value);    // 注册
    void unregister_new_config(const QString& key);                         // 反注册

    const QVariant& query(const QString& key) const;                        // 查询
    void setup_value(const QString& key, const QVariant& value);              // 设置
    bool has_key(const QString &key);
private:
    QMap<QString, QVariant> m_configs;
};
}

#endif // CONFIGMANAGER_H

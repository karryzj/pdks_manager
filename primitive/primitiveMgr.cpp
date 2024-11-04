#include <QDir>

#include "primitiveMgr.h"
#include "primitive.h"

namespace pr
{

PrimitiveMgr::PrimitiveMgr()
    : QObject()
{
    // conf();
    // init();
}

// PrimitiveMgr::PrimitiveMgr(const QString &dir)
// {
//     init(dir);
// }

PrimitiveMgr::~PrimitiveMgr()
{

}

// void PrimitiveMgr::conf(QString dir)
// {
//     m_dir = dir;
// }

// void PrimitiveMgr::init()
// {
//     QDir dir(m_dir);
//     Q_ASSERT(dir.exists());

//     dir.setFilter(QDir::Dirs | QDir::NoDotAndDotDot);
//     QStringList subdirectories = dir.entryList();

//     foreach (const QString &path, subdirectories)
//     {
//         QString subdir = m_dir + "_" + path;
//         m_primitives.insert(subdir, new Primitive(subdir));
//     }
// }

void PrimitiveMgr::init(const QString &root_path)
{
    QDir dir(root_path);
    Q_ASSERT(dir.exists());

    dir.setFilter(QDir::Dirs | QDir::NoDotAndDotDot);
    QStringList subdirectories = dir.entryList();

    foreach (const QString &path, subdirectories)
    {
        QString subdir = dir.dirName() + "#" + path;
        QString sub_path = root_path + "/" + path;
        m_primitives.insert(subdir, new Primitive(sub_path));
    }
}

Primitive *PrimitiveMgr::new_primitive(QGraphicsView *view, QString name)
{
    //std::map<std::string, Primitive*>::iterator it = m_primitives.find(name);
    //Q_ASSERT(it == m_primitives.end());

    Primitive *pri = new Primitive(name, view);
    m_primitives.insert(name, pri);
    return pri;
}

Primitive *PrimitiveMgr::load_primitive(QGraphicsView *view, QString name)
{
    auto it = m_primitives.find(name);
    // Q_ASSERT(it != m_primitives.end());
    if (it == m_primitives.end())
        return NULL;

    Primitive *pri = it.value();
    pri->load(view);
    return pri;
}

Primitive *PrimitiveMgr::load_primitive(QString name)
{
    auto it = m_primitives.find(name);
    if (it == m_primitives.end())
        return NULL;

    Primitive *pri = it.value();
    if (pri->load())
        return NULL;
    return pri;
}

PrimitiveWindow *PrimitiveMgr::get_primitive_window(QString name)
{
    auto it = m_primitive_windows.find(name);
    if (it == m_primitive_windows.end())
        return NULL;
    return it.value();
}

void PrimitiveMgr::set_primitive_window(const QString name, PrimitiveWindow *pri_wnd)
{
    m_primitive_windows[name] = pri_wnd;
}

void PrimitiveMgr::save_primitive(QString name)
{
    auto it = m_primitives.find(name);
    Q_ASSERT(it != m_primitives.end());

    Primitive *pri = it.value();
    pri->save();
}

Primitive *PrimitiveMgr::load_python_primitive(QString name)
{
    Primitive *pri;
    auto it = m_primitives.find(name);
    if (it == m_primitives.end())
    {
        pri = new Primitive(name);
        m_primitives.insert(name, pri);
    }
    else
    {
        pri = it.value();
        if (pri->load_py_json())
            return NULL;
    }
    return pri;

}

PrimitiveMgr *PrimitiveMgr::instance()
{
    static PrimitiveMgr sp_instance;  // HINT@leixunyong。之前的写法没有进行内存管理，把析构交给了程序关闭时的系统回收。
    return &sp_instance;
}

}

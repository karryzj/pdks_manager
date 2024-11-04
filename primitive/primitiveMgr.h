#ifndef PRIMITIVEMGR_H
#define PRIMITIVEMGR_H
#include <QObject>
#include <qmap.h>

QT_BEGIN_NAMESPACE

class QGraphicsView;

QT_END_NAMESPACE

namespace pr
{

class Primitive;
class PrimitiveWindow;

class PrimitiveMgr : public QObject
{
    Q_OBJECT
private:
    PrimitiveMgr();
    // PrimitiveMgr(const QString &dir);

public:
    ~PrimitiveMgr();
    // -- priDir
    // -- primitive1(dir)
    // -- primitive2(dir)
    // void conf(QString dir = "data/primitives");
    // void init();
    void init(const QString &root_path);
    Primitive *new_primitive(QGraphicsView *view, QString name = "default");
    Primitive *load_primitive(QGraphicsView *view, QString name = "default");
    Primitive *load_primitive(QString name = "default");
    PrimitiveWindow *get_primitive_window(QString name = "default");
    void set_primitive_window(const QString name, PrimitiveWindow *pri_wnd);
    void save_primitive(QString name = "default");
    Primitive *load_python_primitive(QString name = "default");

public:
    static PrimitiveMgr *instance();

private:
    QString m_dir;
    QMap<QString, Primitive*> m_primitives;
    QMap<QString, PrimitiveWindow*> m_primitive_windows;

};
}
#endif // PRIMITIVEMGR_H

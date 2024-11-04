#ifndef PRIGRAPHICSSCENE_H
#define PRIGRAPHICSSCENE_H

#include <QGraphicsEllipseItem>
#include <QGraphicsScene>
#include <QUndoStack>

namespace cm
{
class Viewport;
class Ruler;
}

namespace sp
{
class ShapeDrawGraphicsItem;
class ShapePointGraphicsItem;
}

namespace at
{
class AttachTreeNode;
}

namespace pr
{
class PriGraphicsScene final : public QGraphicsScene
{
    Q_OBJECT
public:
    explicit PriGraphicsScene(QObject *parent = nullptr);
    ~PriGraphicsScene() override;

public:
    void init();
    void set_ruler(double dbu, cm::Viewport *viewport);
    void toggle_ruler();
    cm::Ruler * ruler() const;
private:
    void keyPressEvent(QKeyEvent *event) override;

signals:
    void before_delete_tree_node(at::AttachTreeNode* tree_node);  // HINT@leixunyong。暂时没有对应的槽函数需要触发，但是后续可能用得到。预留此接口
    void after_delete_tree_node();
private:
    cm::Ruler *mp_ruler;
};
}



#endif // PRIGRAPHICSSCENE_H

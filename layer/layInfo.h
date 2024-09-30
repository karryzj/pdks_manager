#ifndef LAYINFO_H
#define LAYINFO_H

#include <QObject>
#include <QColor>
#include <QPainter>

#include "layerCommon.h"

QT_BEGIN_NAMESPACE
class QColor;
QT_END_NAMESPACE

namespace ly
{

class LY_PUBLIC LayerInfo final : public QObject
{
    Q_OBJECT

public:
    enum Border_Style { SolidLine, DashLine, DotLine, DashDotLine, DashDotDotLine};
    enum Fill_Style {NotFill, HorizontalLine, VerticalLine, GridLine, LeftDiagonal, RightDiagonal, GridDiagonal, FullFill};
    enum Layer_Color {Color1 = 0xFF0014, Color2 = 0x1955FF, Color3 = 0x7DAEF7, Color4 = 0x9B51EE, Color5 = 0xC660F1,
                      Color6 = 0xA6B65A, Color7 = 0x7FB65A, Color8 = 0xE8CE3B, Color9 = 0xDEB22B, Color10 = 0xB069E7
                     };

    LayerInfo();

    // 拷贝构造函数
    LayerInfo(const LayerInfo& other);

    // 赋值运算符重载
    LayerInfo& operator=(const LayerInfo& other);

    inline bool operator==(const LayerInfo &rhs)
    {
        return m_layer_name == rhs.m_layer_name &&
               m_border_style == rhs.m_border_style &&
               m_border_color == rhs.m_border_color &&
               m_fill_style == rhs.m_fill_style &&
               m_fill_color == rhs.m_fill_color;
    }

    inline bool operator!=(const LayerInfo &rhs)
    {
        return !(*this == rhs);
    }

    ~LayerInfo() override;

    Border_Style border_line() const;
    void set_border_line(Border_Style border_style);

    QColor border_color() const;
    void set_border_color(QColor border_color);

    Fill_Style fill_line() const;
    void set_fill_line(Fill_Style fill_style);

    QColor fill_color() const;
    void set_fill_color(QColor fill_color);

    void getPainter(QPainter &painter) const;
    void getPainter(QPainter &painter, const LayerInfo &style) const;

    QString layer_name() const;
    void set_layer_name(const QString &layer_name);

    static QVector<unsigned int> s_layer_color;

    bool visible() const;
    void set_visible(bool visible);

private:
    QString m_layer_name;
    Border_Style m_border_style;
    QColor m_border_color;
    Fill_Style m_fill_style;
    QColor m_fill_color;
    bool m_visible = true;
};

}
#endif // LAYINFO_H

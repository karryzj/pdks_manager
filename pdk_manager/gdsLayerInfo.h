#ifndef GDSLAYERINFO_H
#define GDSLAYERINFO_H

#include <QString>
namespace pdk
{

enum Border_Style { SolidLine, DashLine, DotLine, DashDotLine, DashDotDotLine};
enum Fill_Style {NotFill, HorizontalLine, VerticalLine, GridLine, LeftDiagonal, RightDiagonal, GridDiagonal, FullFill};
enum Border_Color {Color1 = 0xFF0014, Color2 = 0x1955FF, Color3 = 0x7DAEF7, Color4 = 0x9B51EE, Color5 = 0xC660F1,
                   Color6 = 0xA6B65A, Color7 = 0x7FB65A, Color8 = 0xE8CE3B, Color9 = 0xDEB22B, Color10 = 0xB069E7
                  };

struct BorderStyle
{
    int border_line;
    int border_color;
    int fill_line;
    int fill_color;
};

struct GdsLayerInfo
{
    int layer_num;
    int data_type;
    QString layer_name;
    BorderStyle border_style;
};

inline bool operator==(const GdsLayerInfo &lhs, const GdsLayerInfo &rhs)
{
    return lhs.layer_name == rhs.layer_name &&
           lhs.layer_num == rhs.layer_num &&
           lhs.data_type == rhs.data_type;
}

inline bool operator!=(const GdsLayerInfo &lhs, const GdsLayerInfo &rhs)
{
    return !(lhs == rhs);
}

inline bool operator==(const BorderStyle &lhs, const BorderStyle &rhs)
{
    return lhs.border_line == rhs.border_line &&
           lhs.border_color == rhs.border_color &&
           lhs.fill_line == rhs.fill_line &&
           lhs.fill_color == rhs.fill_color;
}

inline bool operator!=(const BorderStyle &lhs, const BorderStyle &rhs)
{
    return !(lhs == rhs);
}

inline int get_color(int idx)
{
    switch(idx)
    {
    case 1:
        return Color1;
    case 2:
        return Color2;
    case 3:
        return Color3;
    case 4:
        return Color4;
    case 5:
        return Color5;
    case 6:
        return Color6;
    case 7:
        return Color7;
    case 8:
        return Color8;
    case 9:
        return Color9;
    case 10:
        return Color10;
    default:
        return Color1;
    }
}
}

#endif // GDSLAYERINFO_H

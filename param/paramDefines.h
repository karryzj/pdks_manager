#ifndef PARAMDEFINES_H
#define PARAMDEFINES_H

// HINT@leixunyong。在曲线的求解中，存在曲线方程和约束2部分信息。这两部分信息会以参数的形式在param_utils中进行求解，名字用下面的两个字符串定义。
// 例如：现在有一个方程y=x^2+1, 约束为1<x<2。则在业务层会传入2个参数：
// 参数1： key 为equation 1, expression为y=x^2+1；
// 参数2： key 为constraint 1, expression为1<x<2;
#define EQUATION = "equation";
#define CONSTRAINT = "constraint";

#endif // PARAMDEFINES_H

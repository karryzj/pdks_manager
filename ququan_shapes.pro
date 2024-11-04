include(ququan_shapes.pri)

TEMPLATE = subdirs

SUBDIRS += \
    command \
    main \
    attach_tree \
    layer \
    muParser \
    param \
    pdk_manager \
    primitive \
    resources \
    shape \
    common \
    ququan_layout_sample \
    pymod \

#存在ququan_shapes_test子项目时编译test子项目
exists(../ququan_shapes_test/ququan_shapes_test.pro) {
    CONFIG(debug, debug|release){
        SUBDIRS += ../ququan_shapes_test
    }
}
param.depends += muParser common
attach_tree.depends += param layer shape
primitive.depends += attach_tree
pymod.depends += param

pdk_manager.depends += attach_tree layer param shape common primitive resources muParser
main.depends += pdk_manager attach_tree layer param shape common primitive resources muParser


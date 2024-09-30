include(ququan_shapes.pri)

TEMPLATE = subdirs
SUBDIRS += \
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

param.depends += muParser common
attach_tree.depends += param layer shape
primitive.depends += attach_tree
pymod.depends += param

main.depends += attach_tree layer param shape common primitive resources muParser

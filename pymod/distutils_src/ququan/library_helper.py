# -*- coding: utf-8 -*-
import os
import sys
import math
import json

from pathlib import Path
import inspect
import importlib

from ququan.config import QUQUAN_ROOT_PATH,QUQUAN_PDKS_PATH,QUQUAN_PRIMITIVES_PATH
from ququan.QuquanShape import QuquanShape

import pya
#lib_basic=pya.Library.library_by_name("cate1")
#lib_basic.layout().read("D:\project\zkd\KQCircuits-main\klayout_package\python\kqcircuits\junctions\QCD1.oas")
#lib_basic.layout().read("D:/project/python/Demo1.GDS")


def file_exists(file_path):
    return os.path.exists(file_path)

def get_subdirs(path):
    return [os.path.join(path, d) for d in os.listdir(path) if os.path.isdir(os.path.join(path, d))]

def _get_pcell_classes(module=None):
    if module is None:
        return []
    
    class_list = []
    for member in inspect.getmembers(module, inspect.isclass):
        if member[1].__module__ == module.__name__:
            #(member,module)
            value = getattr(module, member[0])
            if isinstance(value, type) and issubclass(value, pya.PCellDeclarationHelper):
                class_list.append(value)

    return class_list

def _get_library(library_name):
    library = pya.Library.library_by_name(library_name)  # returns only registered libraries
    if (library is None):
        library = pya.Library()
        library.register(library_name)
    return library

def _register_pcell(lib_dir,cell_dir,reload=False):    
    #path_pre=str(cellname.joinpath('data'))
    cell_dir=Path(cell_dir)
    json_path=cell_dir.joinpath('data.json')
    gds_path=cell_dir.joinpath('data.gds')
    py_path=cell_dir.joinpath('data.py')

    libname=os.path.basename(lib_dir)
    cellname=os.path.basename(cell_dir)

    library = _get_library(libname)
    if file_exists(json_path):# json pcell
        #return
        qs=QuquanShape(json_path)
        layer_maps={'Layer1':[1,0],'Layer2':[10,0]}
        qs.set_layer_maps(layer_maps)
        library.layout().register_pcell(cellname, qs)

    elif file_exists(gds_path):# gds pcell
        #return
        library.layout().read(gds_path) # library里显示的名称为gds里面的topcell名称
    elif file_exists(py_path):# python pcell
        #return
        import_path_parts=cellname.parts[len(QUQUAN_ROOT_PATH.parts)-1:]
        import_path = ".".join(import_path_parts)
        #import_path=libname+'.'+cellname+'.data'    
        module = importlib.import_module(import_path)
        if reload:
            importlib.reload(module)
        classes=_get_pcell_classes(module)
        if classes: #取文件中最后一个pcell的类
            pcell_class=classes[-1]()
            library.layout().register_pcell(cellname, pcell_class)
    else:
        print("can not find pcell:",cell_dir)

def _register_all_pcells(dir_path,reload=False):
    #sys.path.append(dir_path)
    libs = get_subdirs(dir_path)
    for lib in libs:
        pcells=get_subdirs(lib)
        for pcell in pcells:
            _register_pcell(lib,pcell,reload)

# 注册指定pdk所有library
def register_pdk_librarys(pdk_name,reload=False):
    pkd_lib_dir=QUQUAN_PDKS_PATH.joinpath(pdk_name).joinpath('librarys')
    _register_all_pcells(pkd_lib_dir,reload)


# 注册某一个pcell
def register_pdk_pcell(pdk_name,lib_name,cell_name,reload=False):
    pkd_dir=QUQUAN_PDKS_PATH.joinpath(pdk_name).joinpath('librarys')
    lib_dir=pkd_dir.joinpath(lib_name)
    cell_dir=lib_dir.joinpath(cell_name)
    _register_pcell(lib_dir,cell_dir,reload)

# 将某个图元加入指定layout的cell中，并移动位置到[tran_x,tran_y] layer_maps
def add_shape_to_cell(layout,cell, lib_name,cell_name,layer_maps, param_vals=None, tran_x=0, tran_y=0):
    if not layer_maps or len(layer_maps.keys())==0:
        print("layer_maps has no data.")
        return False
    
    lib_dir=QUQUAN_PRIMITIVES_PATH.joinpath(lib_name)
    cell_dir=lib_dir.joinpath(cell_name)
    json_path=cell_dir.joinpath('data.json')

    if not file_exists(json_path):# json pcell
        print("can not find shape:",json_path)
        return False
    
    shape=QuquanShape(json_path)
    shape.set_layer_maps(layer_maps)
    if param_vals:
        shape.set_param_defaults(param_vals)
        if not shape.is_valid():
            print("param and rule is not valid.")
            return False

    layer_polys=shape.get_layer_polys()
    for layer in layer_polys.keys():
        polys=layer_polys[layer]
        layer_ld=shape.get_act_layer(layer)
        if layer_ld ==None:
            continue

        layer_idx=layout.layer(layer_ld[0],layer_ld[1])

        for poly in polys:
            if tran_x!=0 and tran_y!=0:
                poly=poly.transformed(pya.DCplxTrans(1, 0, False, tran_x, tran_y))
            cell.shapes(layer_idx).insert(poly)

    return True

# 导出一个图元内的pcell到固定gds
def export_primitive_pcell_to_gds(lib_name,cell_name,gds_file,layer_maps, param_vals=None, tran_x=0, tran_y=0):
    layout_view=pya.LayoutView(True)  # Creates a new LayoutView in editable mode
    layout_view.show_layout(pya.Layout(), True)

    layout = layout_view.cellview(layout_view.active_cellview_index()).layout()
    top = layout.create_cell(cell_name) #top
    layout_view.active_cellview().cell = top

    is_add=add_shape_to_cell(layout,top,lib_name,cell_name,layer_maps, param_vals,tran_x, tran_y)
    if not is_add:
        return False

    layout_view.add_missing_layers()

    layout_view.max_hier()
    layout_view.zoom_fit()
    top.write(gds_file)

    return True

# 导出一个pdk内的pcell为固定gds
def export_pcell_to_gds(pdk_name,lib_name,cell_name,gds_file):
    register_pdk_pcell(pdk_name,lib_name,cell_name,True)

    layout_view=pya.LayoutView(True)  # Creates a new LayoutView in editable mode
    layout_view.show_layout(pya.Layout(), True)

    layout = layout_view.cellview(layout_view.active_cellview_index()).layout()
    top = layout.create_cell(cell_name) #'top'
    layout_view.active_cellview().cell = top

    cell=layout.create_cell(cell_name,lib_name,{})    
    top.insert(pya.CellInstArray(cell.cell_index(), pya.Trans()))
    top.flatten(True)
    layout_view.add_missing_layers()

    layout_view.max_hier()
    layout_view.zoom_fit()
    top.write(gds_file)


# 移除指定pdk所有的library
def unregister_pdk_library(pdk_name):
    pkd_dir=QUQUAN_PDKS_PATH.joinpath(pdk_name).joinpath('librarys')

    libs = get_subdirs(pkd_dir)
    for lib in libs:
        library_name=os.path.basename(lib)
        library = pya.Library.library_by_name(library_name)  # returns only registered libraries
        if library:
            library.delete()


#pkd导入一个gds，拷贝到对应目录下，并修改top cell的名称，保存缩略图
def import_gds_to_pdk(input_gds_file, output_gds_file, top_cell_name, png_file, png_width=500,png_height=500):
    layout_view=pya.LayoutView(True)  # Creates a new LayoutView in editable mode
    layout_view.show_layout(pya.Layout(), True)

    layout = layout_view.cellview(layout_view.active_cellview_index()).layout()
    layout.read(input_gds_file)
    top = layout.top_cells()[0]
    top.name=top_cell_name
    layout_view.active_cellview().cell = top

    layout_view.add_missing_layers()

    layout_view.max_hier()
    layout_view.zoom_fit()
    top.write(output_gds_file)

    layout_view.save_image(png_file,png_width,png_height)


#sys.path.append('C:/Users/su_pe/KLayout/python')
#sys.path.append('D:/project/ququan_shapes/build/mingw-Debug')
#export_pcell_to_gds("pdk1","cate1","t3","d:/ttt.gds")
#_register_all_pcells('C:/Users/su_pe/KLayout/python/ququan/pcells')
#_register_all_pcells('D:/project/ququan_shapes/build/mingw-Debug/primitives')

#import_path='test_sympy'
#module = importlib.import_module(import_path)
#_get_pcell_classes(module)

'''
import sys
sys.path.append("C:/Users/su_pe/KLayout/python")
sys.path.append("D:/project/ququan_shapes/build/mingw-Debug/pymod")
from ququan.library_helper import export_primitive_pcell_to_gds
export_primitive_pcell_to_gds('cate1','t1',"d:/t1.gds",{"Layer1":[1,0],"Layer2":[2,0]})
'''
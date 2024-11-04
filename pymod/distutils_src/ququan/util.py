# -*- coding: utf-8 -*-
import os
import json
import importlib
import inspect
import importlib.util


import pya
import klayout.lay

# no use
def _register_and_draw_pcell_by_file(module_path):
    if os.path.isfile(module_path) and module_path.endswith('.py'):
        # 获取文件名（不包含路径和扩展名）
        module_name = os.path.basename(module_path).split('.')[0]
    
        # 动态导入模块
        spec = importlib.util.spec_from_file_location(module_name, module_path)
        module = importlib.util.module_from_spec(spec)
        spec.loader.exec_module(module)
        #importlib.reload(module)

        #获取pcell类
        class_list = []
        for member in inspect.getmembers(module, inspect.isclass):
            if member[1].__module__ == module.__name__:
                #(member,module)
                value = getattr(module, member[0])
                if isinstance(value, type) and issubclass(value, pya.PCellDeclarationHelper):
                    class_list.append(value)

        if len(class_list)==0:
            print("can not find pcell class.")
            return
        
        register_and_draw_pcell(class_list[-1],os.path.dirname(module_path))
    else:
        print("Error: Invalid module path or not a .py file")


# 用于注册和显示python pcell
def register_and_draw_pcell(pcell,file_path=''):
    try:
        file_path=os.path.dirname(file_path)
        #print(file_path)
        if not klayout.lay or not klayout.lay.MyView or not klayout.lay.MyView.view:
            return
        
        view=klayout.lay.MyView.view()
        view.cancel()

        layout=klayout.lay.MyView.view().cellview(view.active_cellview_index()).layout()
        top=layout.top_cells()[0]        
        top.clear_insts()
        #top.clear_shapes()

        pcell_name="QuquanTestPcell"
        lib_name="Test"
        library=pya.Library.library_by_name(lib_name) 
        if library:
            #library.delete()
            pass
        else:
            library = pya.Library()
            library.register(lib_name)

        library.layout().register_pcell(pcell_name, pcell)

        cell=layout.create_cell(pcell_name,lib_name,{})
        top.insert(pya.CellInstArray(cell.cell_index(), pya.Trans()))
        view.add_missing_layers()
        klayout.lay.MyView.mywindow().refresh()
        view.max_hier()
        view.zoom_fit()

        write_pcell_info(pcell,file_path+"/info.json")
        png_file=file_path+"/thumbnail.png"
        view.save_image(png_file,500,500)

        #make_background_transparent(png_file)

    except:
        pass

def write_pcell_info(pcell,file_name):
    if not pcell or not file_name:
        return

    data={}
    params=[]
    for i in range(len(pcell._param_decls)):
            p=pcell._param_decls[i]
            if not pcell.is_mgr_param(p.name):
                continue

            #只保存可修改的 double和int类型的参数 #not p.readonly and 
            if not p.hidden and (p.type == type(pcell).TypeDouble or p.type == type(pcell).TypeInt or p.type == type(pcell).TypeString):
                params.append({"key":p.name,"value":str(p.default),"desc":p.description})

    layers=[]
    if hasattr(pcell,'layer_maps'):
        for k in pcell.layer_maps.keys():
            layers.append(k)
        pass

    rules=[]
    if hasattr(pcell,'_rules'):
        rules = pcell._rules

    anchors=[]
    if hasattr(pcell,'_anchors'):
        anchors = pcell._anchors
        for i in range(len(anchors)):
            anchors[i]['x']=str(anchors[i]['x'])
            anchors[i]['y']=str(anchors[i]['y'])
    
    #current_directory = os.path.dirname(__file__)
    #file_name=current_directory+"/info.json"

    data['param_rules']={}
    data['param_rules']['params']=params   
    data['param_rules']['rules']=rules 
    data['layers']=layers    
    data['anchors']=anchors
    with open(file_name, "w") as file:
      json.dump(data, file)


def make_background_transparent(png_file):
    try:
        from PIL import Image

        # 打开图片
        img = Image.open(png_file)    

        # 假设背景为白色，可以通过img.getpixel((0,0))获取左上角像素的颜色值
        color_to_remove = img.getpixel((0, 0))

        x, y = img.size
        pix = img.load()
    
        # 使背景透明
        for i in range(x):
            for j in range(y):
                if pix[i, j] == color_to_remove:
                    pix[i, j] = (0, 0, 0, 0)  # 设置为RGBA的透明色
    
        # 保存图片
        img.save(png_file, 'PNG')
    except ModuleNotFoundError:
        pass


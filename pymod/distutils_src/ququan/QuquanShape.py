# -*- coding: utf-8 -*-
import os
import math
import json
import pya
import klayout.param
from enum import Enum
from datetime import datetime


from ququan.config import POINT_DISTANCE

#POINT_DISTANCE=1 # um
DPoint=pya.DPoint
#Point=pya.Point

class NodeDirection(Enum):
    TOP_LEFT = 0
    BOTTOM_LEFT = 1
    BOTTOM_RIGHT = 2
    TOP_RIGHT = 3
    NONE=4

class NodeType(Enum):
    LOCATION=0
    ADD=1
    DELETE=2
    NONE=3

class Point:
    def __init__(self,x=0,y=0):
        self.move(x,y)

    def move(self,x,y):
        self.x = x
        self.y = y

    def reset(self):
        self.move(0,0)

    def distance(self,point2):
        return math.sqrt((self.x-point2.x)**2 + (self.y - point2.y)**2)
    
    def __str__(self):
        return "("+str(self.x)+" "+str(self.y)+")"


'''
给定矩形的原点，朝向，长和高，按附着点顺序返回polys
'''
def get_rectangle(point, direct, width, height):
    polys=[]

    if direct == NodeDirection.TOP_LEFT.value:
        polys.append(Point(point.x,point.y+height))
        polys.append(Point(point.x,point.y))
        polys.append(Point(point.x-width,point.y))
        polys.append(Point(point.x-width,point.y+height))
    elif direct == NodeDirection.BOTTOM_LEFT.value:
        polys.append(Point(point.x,point.y-height))
        polys.append(Point(point.x,point.y))
        polys.append(Point(point.x-width,point.y))
        polys.append(Point(point.x-width,point.y-height))
    elif direct == NodeDirection.BOTTOM_RIGHT.value:
        polys.append(Point(point.x,point.y-height))
        polys.append(Point(point.x,point.y))
        polys.append(Point(point.x+width,point.y))
        polys.append(Point(point.x+width,point.y-height))
    elif direct == NodeDirection.TOP_RIGHT.value:
        polys.append(Point(point.x,point.y+height))
        polys.append(Point(point.x,point.y))
        polys.append(Point(point.x+width,point.y))
        polys.append(Point(point.x+width,point.y+height))

    return polys

def get_rectangle_center(point,width,height):
    polys=[]
    x2=width/2
    y2=height/2

    polys.append(Point(point.x-x2,point.y+y2))
    polys.append(Point(point.x-x2,point.y-y2))
    polys.append(Point(point.x+x2,point.y-y2))
    polys.append(Point(point.x+x2,point.y+y2))

    return polys

def arc(point, r, n=64, angle0=0, angle1=360):
    n_auto=int(math.ceil(r*(angle1-angle0)*math.pi/180/POINT_DISTANCE)+2)
    n=n_auto
    #n=max(n,n_auto)
    angles = [angle0+1.0*x/(n-1)*(angle1-angle0) for x in range(n)]
    arc_points = [Point(point.x+r*math.cos(angle*math.pi/180), point.y+r*math.sin(angle*math.pi/180)) for angle in angles]
        
    return arc_points

# 圆
def get_circle_points(point,r,n=64):
    return arc(point,r,n)

# 圆弧、圆环
def get_pie_points(point,r_out,r_in=0,angle0=0,angle1=360,n=64):
    if angle1 <= angle0:
        angle1 = angle1+360

    if angle1-angle0>=360:
        angle0=0
        angle1=360

    if r_in==0 and angle1-angle0>=360:
        return arc(point,r_out,n)
    
    if r_in !=0:
        poly_in = arc(point,r_in,n,angle0,angle1)
        poly_out = arc(point,r_out,n,angle0,angle1)
        poly_out.reverse()
        return poly_in+poly_out
    else:
        poly_out = arc(point,r_out,n,angle0,angle1)
        poly_out.append(point)
        return poly_out

def ellipse_arc(point, r_x,r_y, n=64, angle0=0, angle1=360):
    n_auto=int(math.ceil(max(r_x,r_y)*(angle1-angle0)*math.pi/180/POINT_DISTANCE)+2)
    n=max(n,n_auto)
    angles = [angle0+1.0*x/(n-1)*(angle1-angle0) for x in range(n)]
    arc_points = [Point(point.x+r_x*math.cos(angle*math.pi/180), point.y+r_y*math.sin(angle*math.pi/180)) for angle in angles]
        
    return arc_points

# 椭圆
def get_ellipse_points(point,r_x,r_y,angle0=0,angle1=360,n=64):
    if angle1 <= angle0:
        angle1 = angle1+360

    if angle1-angle0>=360:
        angle0=0
        angle1=360

    points=ellipse_arc(point,r_x,r_y,n,angle0,angle1)
    if angle1-angle0<360:
        points.append(point)

    return points

# 直角三角形
def get_triangle_points(point, direct, width, height):
    points = get_rectangle(point, direct, width, height)
    return points[:3]

# 四边形
def get_quadrangle_points(point, direct, width, height,x1,x2,x3,x4):
    polys=[]

    if direct == NodeDirection.TOP_LEFT.value:
        polys.append(Point(point.x-x2,point.y+height))
        polys.append(Point(point.x,point.y+x1))
        polys.append(Point(point.x-width+x4,point.y))
        polys.append(Point(point.x-width,point.y+height-x3))
    elif direct == NodeDirection.BOTTOM_LEFT.value:
        polys.append(Point(point.x-x2,point.y-height))
        polys.append(Point(point.x,point.y-x1))
        polys.append(Point(point.x-width+x4,point.y))
        polys.append(Point(point.x-width,point.y-height+x3))
    elif direct == NodeDirection.BOTTOM_RIGHT.value:
        polys.append(Point(point.x+x2,point.y-height))
        polys.append(Point(point.x,point.y-x1))
        polys.append(Point(point.x+width-x4,point.y))
        polys.append(Point(point.x+width,point.y-height+x3))
    elif direct == NodeDirection.TOP_RIGHT.value:
        polys.append(Point(point.x+x2,point.y+height))
        polys.append(Point(point.x,point.y+x1))
        polys.append(Point(point.x+width-x4,point.y))
        polys.append(Point(point.x+width,point.y+height-x3))

    return polys

def trans_scale(points,odd):
    return [Point(p.x*odd,p.y*odd) for p in points]

def trans_pos(points,pos):
    return [Point(p.x+pos.x,p.y+pos.y) for p in points]

def get_param_value(params,k,global_params_func):
    for i in range(len(params)):
        if params[i]['key'] == k:
            val = params[i]['value']
            if type(val) == type(1) or type(val)==type(1.0):
                return val
            elif global_params_func:
                return global_params_func(val)
            else:
                return None
        
    return None

def get_shape_polys(shape_name,params,direction,global_params_func,ori_pos):
    points=[]
    if shape_name=='rectangle':
        width = get_param_value(params,'width',global_params_func)
        height = get_param_value(params,'height',global_params_func)
        return get_rectangle(ori_pos,direction,width,height)
    elif shape_name=='circle':
        r = get_param_value(params,'radius',global_params_func)
        return get_circle_points(ori_pos, r)
    elif shape_name=='triangle':
        width = get_param_value(params,'width',global_params_func)
        height = get_param_value(params,'height',global_params_func)
        return get_triangle_points(ori_pos, direction, width, height)
    elif shape_name=='ellipse':
        rx = get_param_value(params,'semi major axis',global_params_func)
        ry = get_param_value(params,'semi minor axis',global_params_func)
        angle0 = get_param_value(params,'start angle',global_params_func)
        angle1 = get_param_value(params,'end angle',global_params_func)
        return get_ellipse_points(ori_pos, rx, ry, angle0, angle1)
    elif shape_name=='quadrangle':
        width = get_param_value(params,'width',global_params_func)
        height = get_param_value(params,'height',global_params_func)
        x1 = get_param_value(params,'parameter one',global_params_func)
        x2 = get_param_value(params,'parameter two',global_params_func)
        x3 = get_param_value(params,'parameter three',global_params_func)
        x4 = get_param_value(params,'parameter four',global_params_func)
        return get_quadrangle_points(ori_pos, direction,width, height, x1, x2, x3, x4)
    elif shape_name=='sector':
        rin = get_param_value(params,'inside radius',global_params_func)
        rout = get_param_value(params,'outside radius',global_params_func)
        angle0 = get_param_value(params,'start angle',global_params_func)
        angle1 = get_param_value(params,'end angle',global_params_func)
        return get_pie_points(ori_pos, rout, rin, angle0, angle1)
    elif shape_name=='polygen':
        p_num=int((len(params)-1)/2)
        for i in range(p_num):
            x=get_param_value(params,'x'+str(i+1),global_params_func) + ori_pos.x
            y=get_param_value(params,'y'+str(i+1),global_params_func) + ori_pos.y
            points.append(Point(x,y))
        if len(points)>0:
            points.append(Point(ori_pos.x,ori_pos.y))
        return points
    else:
        print("not support shape:"+str(shape_name))

    return points

def reset_node_updated(node):
    node["updated_points"]=0
    if 'children' in node.keys():
      for i in range(len(node['children'])):
          node_i=node['children'][i]
          reset_node_updated(node_i)

def get_node_points(node,parent,global_params_func):
    layers={}

    #把每层的polys放一起

    if parent:
        ori_attach_point=parent['attach_points'][node['parent_attach_point_idx']]
        #ori_pos=Point(ori_attach_point['x'],ori_attach_point['y'])
    else:
        ori_attach_point={"x_val":0,"y_val":0}

    ori_pos=Point(ori_attach_point['x_val'],ori_attach_point['y_val'])

    # 1.更新attach_points坐标
    if not parent and 'attach_points' not in node.keys():   #root节点自动加
        node['attach_points']=[{"id": 0,"x": 0,"y": 0}]
        node['parent_attach_point_idx']=-1

    if "updated_points" not in node.keys() or not node["updated_points"]:
        #附着点进行rotate
        '''rotate=None
        if 'params' in node.keys():
            rotate=get_param_value(node['params'],'rotate',global_params_func)
            if rotate and rotate!=0:
                tr = pya.DCplxTrans(1, rotate, False, 0, 0)
        '''
            
        for k in range(len(node['attach_points'])):
            node['attach_points'][k]['x_val'] = ori_pos.x+global_params_func(node['attach_points'][k]['x'])
            node['attach_points'][k]['y_val'] = ori_pos.y+global_params_func(node['attach_points'][k]['y'])

            '''if rotate and rotate!=0: #传进来的已经旋转过了
                pn=pya.DPoint(node['attach_points'][k]['x_val'],node['attach_points'][k]['y_val'])
                pn=pn*tr
                node['attach_points'][k]['x_val'] = pn.x
                node['attach_points'][k]['y_val'] = pn.y
            '''
        node["updated_points"]=1

    # 2.将形状加入points
    if node['parent_attach_point_idx'] != -1:
        if node['node_type']==NodeType.ADD.value or node['node_type']==NodeType.DELETE.value:
            layer = node['layer']
            if layer not in layers:
                layers[layer]=[]

            points=get_shape_polys(node['shape_name'],node['params'],node['node_direction'],global_params_func,ori_pos)
            #转成 DPloygon
            poly=pya.DPolygon([pya.DPoint(p.x,p.y) for p in points])

            #rotate
            rotate=get_param_value(node['params'],'rotate',global_params_func)
            if rotate and rotate!=0:
                #tr = pya.DCplxTrans(1, rotate, False, 0, 0)
                #poly=poly.transformed(tr)

                #先移动到原点，再旋转，最后再偏移
                poly=poly.transformed(pya.DCplxTrans(1, 0, False, -ori_pos.x, -ori_pos.y))
                poly=poly.transformed(pya.DCplxTrans(1, rotate, False, 0, 0))
                poly=poly.transformed(pya.DCplxTrans(1, 0, False, ori_pos.x, ori_pos.y))
            
            #points=trans_pos(points,ori_pos)
            layers[layer].append({'points':poly,'id':node['id'],'node_type':node['node_type']})

    # 3.将子节点的points加入
    if 'children' in node.keys():
      for i in range(len(node['children'])):
          node_i=node['children'][i]
          layers_i=get_node_points(node_i,node,global_params_func)
  
          for layer in layers_i:
              if layer not in layers:
                  layers[layer]=[]
              layers[layer]+=layers_i[layer]

    return layers

# ploys1: vector<ploygon>
def ploys_boolean(polys1, polys2,action,ep):
    polys=[]
    #ep=pya.EdgeProcessor()
    t1=datetime.now()
    if action==NodeType.ADD.value:
        if len(polys1)==0:
            return polys2
        polys1.append(polys2)
        polys=polys1
        #polys=ep.boolean_p2p(polys1,polys2,pya.EdgeProcessor.ModeOr,False,False) # False,False
    elif action==NodeType.DELETE.value:
        polys=ep.boolean_p2p(polys1,polys2,pya.EdgeProcessor.ModeANotB,False,False)
    else:
        pass
    #polys=ep.simple_merge_p2p(polys,False,False)
    t2=datetime.now()
    print((t2-t1).total_seconds(),action)
    return polys

def get_result_polys(polys,dbu,ep):
    results=[]
    types=[]
    poly_multi1ks=[]
    for i in range(len(polys)):
        poly=polys[i]
        if poly['node_type'] == NodeType.LOCATION.value:
            continue

        poly_multi1k=poly['points']*(1.0/dbu)
        poly_multi1k=pya.Polygon(poly_multi1k)        
        types.append(poly['node_type'])
        poly_multi1ks.append(poly_multi1k)
        results.append([poly_multi1k])

    return_polys=[]
    for i in range(len(types)):
        if types[i]==NodeType.ADD.value:
            for j in range(i+1,len(types)):
                if types[j]==NodeType.DELETE.value:
                    results[i]=ep.boolean_p2p(results[i],[poly_multi1ks[j]],pya.EdgeProcessor.ModeANotB,False,False)
        else:
            results[i]=[]

        return_polys += results[i]
    return return_polys

# 将各层的poly按先后顺序进行加减操作,每个点进行相对坐标锚点位移
def format_points(layer_points,coord_point,dbu=0.001):
    points_results={}
    ep=pya.EdgeProcessor()
    tr = pya.DCplxTrans(1, 0, False, -coord_point.x, -coord_point.y) # mag, angel, mirrx, dx,dy
    for k in layer_points:
        points_arr=layer_points[k]
        # 先按id排序
        layer_points[k]=sorted(points_arr, key=lambda x: x['id'])
        points_arr=layer_points[k]
        # 再进行merge操作
        #t1=datetime.now()
        results=get_result_polys(points_arr,dbu,ep)
        '''
        results=[]
        for i in range(len(points_arr)):
            if points_arr[i]['node_type']==NodeType.LOCATION.value:
                continue
            #由于boolean操作只支持int型point，所以先*1000，转pya.Point

            #points_arr_multi1k=trans_scale(points_arr[i]['points'],1000)
            #points_arr_multi1k=[pya.Point(p.x,p.y) for p in points_arr_multi1k]
            #results=ploys_boolean(results,[pya.Polygon(points_arr_multi1k)],points_arr[i]['node_type'])
            #ploy_multi1k=pya.DPolygon([pya.DPoint(p.x,p.y) for p in points_arr[i]['points']])*1000
            ploy_multi1k=points_arr[i]['points']*(1.0/dbu)
            ploy_multi1k=pya.Polygon(ploy_multi1k)
            results=ploys_boolean(results,[ploy_multi1k],points_arr[i]['node_type'],ep)
        '''
        results=ep.simple_merge_p2p(results,False,False)
        #t2=datetime.now()
        #print(k,(t2-t1).total_seconds())

        points_results[k]=[]
        for p in results:
            p=pya.DPolygon(p)*dbu
            p=p.transformed(tr)
            points_results[k].append(p)
        #points_results[k]=[pya.DPolygon(p)*0.001 for p in results]
        #points_results[k]=[pya.DPolygon([DPoint(p.x/1000.0-coord_point.x,p.y/1000.0-coord_point.y) for p in results[i]]) for i in range(len(results))]

    return points_results

def load_file(fname):
    with open(fname,'r',encoding='utf-8') as file:
        data=json.load(file)
        
    layer_points=get_node_points(data['primitive']['node'],None,{})
    layer_polys=format_points(layer_points,Point(0,0))

    return layer_polys

# layer_maps={'Layer1':[1,0],'Layer2':[10,0]}
def insert_ploys(layer_ploys,layer_maps,layout,cell):
    for l in layer_ploys.keys():
        l_layer=layout.layer(layer_maps[l][0], layer_maps[l][1])
        for i in range(len(layer_ploys[l])):
            cell.shapes(l_layer).insert(layer_ploys[l][i])


#load_file("data.json")
#exit()
from ququan.QuquanPcellHelper import QuquanPcellHelper
class QuquanShape(QuquanPcellHelper):
    def __init__(self,file_name):
        super(QuquanShape, self).__init__()
        self.file_name = file_name        
        
        self._init_layers=[] #['Layer1','Layer2']
        self._display_text=''

        self.load_data()

    def load_data(self):
        #with open(self.file_name,'r',encoding='utf-8') as file:
            #self.data=json.load(file)

        json_str=klayout.param.Crypto.decode(self.file_name)
        self.data=json.loads(json_str)

        # 设置参数
        for i in range(len(self.data['primitive']['param_rules']['params'])):
            param_i=self.data['primitive']['param_rules']['params'][i]
            self.add_param(param_i['key'],param_i['value'],param_i['desc'])

        # 设置规则
        for i in range(len(self.data['primitive']['param_rules']['rules'])):
            rule_i=self.data['primitive']['param_rules']['rules'][i]
            self.add_rule(rule_i)

        # 设置坐标锚点
        if 'coord_anchor' in self.data['primitive']:
            self.set_coordinate_anchor(self.data['primitive']['coord_anchor'])

        # 设置锚点
        anchors=self.data['primitive']['anchors']
        anchors=sorted(anchors, key=lambda x: x['node_id'])
        for i in range(len(anchors)):
            self.add_anchor(anchors[i])

        # 设置图层
        for l in self.data['primitive']['layers']:
            self._init_layers.append(l['layer_name'])
        
    def get_layer_polys(self,dbu=0.001):
        layer_points=get_node_points(self.data['primitive']['node'],None,self.get_exp_val)
        layer_polys=format_points(layer_points,self._coordinate_point,dbu)

        return layer_polys

    #设置初始值
    def set_param_defaults(self,datas):
        super().set_param_defaults(datas)

        self.reset_node_attach()
    
    def reset_node_attach(self):
        reset_node_updated(self.data['primitive']['node'])

    def set_display_text(self,text):
        self._display_text=text

    def display_text_impl(self):
        return self._display_text

    def coerce_parameters_impl(self):
        super().coerce_parameters_impl()
        
        self.reset_node_attach()

    
    def produce_impl(self):
        if not self.is_valid():
            return
        
        layer_polys=self.get_layer_polys(self.layout.dbu)
        for layer in layer_polys.keys():
            polys=layer_polys[layer]
            layer_ld=self.get_act_layer(layer)
            if layer_ld ==None:
                continue

            layer_idx=self.layout.layer(layer_ld[0],layer_ld[1])

            for poly in polys:
                self.cell.shapes(layer_idx).insert(poly)

        pass


#test
'''
qs=QuquanShape('D:/project/ququan_shapes/build/mingw-Debug/primitives/cate2/t1/data.json')
qs.get_layer_polys()
qs.set_param_val('x',2)
qs.reset_node_attach()
qs.get_layer_polys() 
'''
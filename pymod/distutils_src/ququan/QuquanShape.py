# -*- coding: utf-8 -*-
import os
import math
import json
import pya
import klayout.param
from enum import Enum
from datetime import datetime


#from ququan.config import POINT_DISTANCE

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

class DelType(Enum):
    NONE=0
    ONLY_PARENT=1
    PARENT_AND_BROTHER=2
    PARENT_AND_CHILDRENS=3

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


    polys.append(DPoint(point.x,point.y+height))
    polys.append(DPoint(point.x,point.y))
    polys.append(DPoint(point.x+width,point.y))
    polys.append(DPoint(point.x+width,point.y+height))

    return polys


def arc(point, r, point_distance, angle0=0, angle1=360):
    n_auto=int(math.ceil(r*(angle1-angle0)*math.pi/180/point_distance)+2)
    n=n_auto
    #n=max(n,n_auto)
    angles = [angle0+1.0*x/(n-1)*(angle1-angle0) for x in range(n)]
    arc_points = [DPoint(point.x+r*math.cos(angle*math.pi/180), point.y+r*math.sin(angle*math.pi/180)) for angle in angles]
        
    return arc_points

# 圆
def get_circle_points(point,r,point_distance):
    return arc(point,r,point_distance)

# 圆弧、圆环
def get_pie_points(point,r_out,r_in=0,angle0=0,angle1=360,point_distance=1.0):
    if angle1 <= angle0:
        angle1 = angle1+360

    if angle1-angle0>=360:
        angle0=0
        angle1=360

    if r_in==0 and angle1-angle0>=360:
        return arc(point,r_out,point_distance)
    
    if r_in !=0:
        poly_in = arc(point,r_in,point_distance,angle0,angle1)
        poly_out = arc(point,r_out,point_distance,angle0,angle1)
        poly_out.reverse()
        return poly_in+poly_out
    else:
        poly_out = arc(point,r_out,point_distance,angle0,angle1)
        poly_out.append(point)
        return poly_out

def ellipse_arc(point, r_x,r_y, angle0=0, angle1=360,point_distance=1.0):
    n_auto=int(math.ceil(max(r_x,r_y)*(angle1-angle0)*math.pi/180/point_distance)+2)
    n=n_auto
    #n=max(n,n_auto)
    angles = [angle0+1.0*x/(n-1)*(angle1-angle0) for x in range(n)]
    arc_points = [DPoint(point.x+r_x*math.cos(angle*math.pi/180), point.y+r_y*math.sin(angle*math.pi/180)) for angle in angles]
        
    return arc_points

# 椭圆
def get_ellipse_points(point,r_x,r_y,angle0=0,angle1=360,point_distance=1.0):
    if angle1 <= angle0:
        angle1 = angle1+360

    if angle1-angle0>=360:
        angle0=0
        angle1=360

    points=ellipse_arc(point,r_x,r_y,angle0,angle1,point_distance)
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

    polys.append(DPoint(point.x+x2,point.y+height))
    polys.append(DPoint(point.x,point.y+x1))
    polys.append(DPoint(point.x+width-x4,point.y))
    polys.append(DPoint(point.x+width,point.y+height-x3))

    return polys


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

def get_param_value_from_map(param_maps,k,global_params_func):
    if k in param_maps:
        val = param_maps[k]

        if type(val) == type(1) or type(val)==type(1.0):
            return val
        elif global_params_func:
            return global_params_func(val)
        else:
            return None
    else:
        return None


def get_shape_polys(shape_name,params,direction,global_params_func,point_distance,param_mgr):
    points=[]
    ori_pos=DPoint(0,0)
    param_maps={p['key']:p['value'] for p in params}
    if shape_name=='rectangle':
        width = get_param_value_from_map(param_maps,'width',global_params_func)
        height = get_param_value_from_map(param_maps,'height',global_params_func)
        return get_rectangle(ori_pos,direction,width,height)
    elif shape_name=='circle':        
        r = get_param_value_from_map(param_maps,'radius',global_params_func)
        ori_pos=DPoint(r,r)
        return get_circle_points(ori_pos, r,point_distance)
    elif shape_name=='triangle':
        width = get_param_value_from_map(param_maps,'width',global_params_func)
        height = get_param_value_from_map(param_maps,'height',global_params_func)
        return get_triangle_points(ori_pos, direction, width, height)
    elif shape_name=='ellipse':
        rx = get_param_value_from_map(param_maps,'semi major axis',global_params_func)
        ry = get_param_value_from_map(param_maps,'semi minor axis',global_params_func)
        angle0 = get_param_value_from_map(param_maps,'start angle',global_params_func)
        angle1 = get_param_value_from_map(param_maps,'end angle',global_params_func)
        ori_pos=DPoint(rx,ry)
        return get_ellipse_points(ori_pos, rx, ry, angle0, angle1,point_distance)
    elif shape_name=='quadrangle':
        width = get_param_value_from_map(param_maps,'width',global_params_func)
        height = get_param_value_from_map(param_maps,'height',global_params_func)
        x1 = get_param_value_from_map(param_maps,'parameter one',global_params_func)
        x2 = get_param_value_from_map(param_maps,'parameter two',global_params_func)
        x3 = get_param_value_from_map(param_maps,'parameter three',global_params_func)
        x4 = get_param_value_from_map(param_maps,'parameter four',global_params_func)
        return get_quadrangle_points(ori_pos, direction,width, height, x1, x2, x3, x4)
    elif shape_name=='sector':
        rin = get_param_value_from_map(param_maps,'inside radius',global_params_func)
        rout = get_param_value_from_map(param_maps,'outside radius',global_params_func)
        angle0 = get_param_value_from_map(param_maps,'start angle',global_params_func)
        angle1 = get_param_value_from_map(param_maps,'end angle',global_params_func)
        ori_pos=DPoint(rout,rout)
        return get_pie_points(ori_pos, rout, rin, angle0, angle1,point_distance)
    elif shape_name=='polygen':
        p_num=int((len(params)-1)/2)
        for i in range(p_num):
            x=get_param_value_from_map(param_maps,'x'+str(i+1),global_params_func) + ori_pos.x
            y=get_param_value_from_map(param_maps,'y'+str(i+1),global_params_func) + ori_pos.y
            points.append(Point(x,y))
        if len(points)>0:
            points.append(Point(ori_pos.x,ori_pos.y))
        return points
    elif shape_name=="curve":#曲线
        equations_fixs=[]
        
        for i in range(len(params)):    #必须按params顺序遍历
            k=params[i]['key']
            if k.find("reverse")>=0:
                continue

            reverse_key=k+" reverse"
            if k.find("equation")>=0:   #方程表达式                
                if reverse_key in param_maps:
                    equations_fixs.append([0,param_maps[k],param_maps[reverse_key]])    # type[0:equations 1:fixs],表达式,reverse
                else:
                    print("invalid curve")
                    return points
            elif k.find("fixed point set")>=0:   #方程表达式                
                if reverse_key in param_maps:
                    equations_fixs.append([1,param_maps[k],param_maps[reverse_key]])
                else:
                    print("invalid curve")
                    return points
                
        for i in range(len(equations_fixs)):
            pointi=[]
            if equations_fixs[i][0]==0:     #方程
                pointi=param_mgr.gen_curve_points(equations_fixs[i][1],point_distance)
            elif equations_fixs[i][0]==1:   #固定点集
                lines=equations_fixs[i][1].split("\n")
                for line in lines:
                    vals=line.strip().split(",")
                    if len(vals)==2:
                        x=global_params_func(vals[0])
                        y=global_params_func(vals[1])
                        pointi.append(DPoint(x,y))

            if equations_fixs[i][2]=="true":
                pointi.reverse()
            points+=pointi

    else:
        print("not support shape:"+str(shape_name))

    return points

def reset_node_updated(node):
    node["updated_points"]=0
    if 'children' in node.keys():
      for i in range(len(node['children'])):
          node_i=node['children'][i]
          reset_node_updated(node_i)

# xy轴镜像
def mirr_poly(poly,mirrx,mirry):
    new_poly=[]
    for p in poly:
        pt=DPoint(p.x,p.y)
        if mirrx:
            pt.x = pt.x*-1
        if mirry:
            pt.y = pt.y*-1

        new_poly.append(pt)

    return new_poly
    
#把所有层的polys放一起
def get_node_points(node,parent,global_params_func,point_distance,param_mgr):
    return_polys=[]

    if parent:
        ori_attach_point=parent['attach_points'][node['parent_attach_point_idx']]
        #ori_pos=Point(ori_attach_point['x'],ori_attach_point['y'])
    else:
        ori_attach_point={"x_val":0,"y_val":0}
        node["parent"]=None
        node['node_type']=NodeType.LOCATION.value

    ori_pos=DPoint(ori_attach_point['x_val'],ori_attach_point['y_val'])

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
            # 先获取图形相对于附着点的相对坐标
            points=get_shape_polys(node['shape_name'],node['params'],node['node_direction'],global_params_func,point_distance,param_mgr)

            #根据direction翻转x,y
            if node['node_direction'] == NodeDirection.TOP_LEFT.value:
                points = mirr_poly(points,True,False)
            elif node['node_direction'] == NodeDirection.BOTTOM_LEFT.value:
                points = mirr_poly(points,True,True)
            elif node['node_direction'] == NodeDirection.BOTTOM_RIGHT.value:
                points = mirr_poly(points,False,True)

            #转成 DPloygon
            #poly=pya.DPolygon([pya.DPoint(p.x,p.y) for p in points])
            poly=pya.DPolygon(points)

            #rotate
            rotate=get_param_value(node['params'],'rotate',global_params_func)
            if rotate and rotate!=0:
                #先移动到原点，再旋转，最后再偏移
                #poly=poly.transformed(pya.DCplxTrans(1, 0, False, -ori_pos.x, -ori_pos.y))
                poly=poly.transformed(pya.DCplxTrans(1, rotate, False, 0, 0))
                #poly=poly.transformed(pya.DCplxTrans(1, 0, False, ori_pos.x, ori_pos.y))

            #从相对坐标移动到绝对坐标
            poly=poly.transformed(pya.DCplxTrans(1, 0, False, ori_pos.x, ori_pos.y))
            
            return_polys.append({'points':poly,'id':node['id'],'node_type':node['node_type'],'node':node})

    # 3.将子节点的points加入
    if 'children' in node.keys():
      for i in range(len(node['children'])):
          node_i=node['children'][i]
          node_i["parent"]=node
          children_i=get_node_points(node_i,node,global_params_func,point_distance,param_mgr)
          
          return_polys+=children_i
    
    return return_polys

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

#获取第一个为ADD的父亲节点
def get_node_first_add_parent(node):    
    parent = None
    while 1:
        parent = node['parent']
        if parent == None or parent['node_type'] == NodeType.ADD.value:
            break

    return parent

#获取包含自己以及自己所有子孙的add类型的节点
def get_all_add_childs(node):
    adds=[]

    if node['node_type']==NodeType.ADD.value:
        adds.append(node['id'])

    if 'children' in node.keys():
        for i in range(len(node['children'])):
            node_i=node['children'][i]
            adds_i = get_all_add_childs(node_i)
            adds+=adds_i

    return adds
    
def get_del_ids(node):
    if node['node_boolean_subtract_type'] == DelType.ONLY_PARENT.value:
        #获取第一个为ADD的父亲节点
        parent = get_node_first_add_parent(node)
        if parent==None:
            return []
        else:
            return [parent['id']]
    elif node['node_boolean_subtract_type'] == DelType.PARENT_AND_BROTHER.value:
        parent = get_node_first_add_parent(node)
        if parent==None:
            return []
        else:
            dels=[parent['id']]
            #获取父亲的直属第一层add子节点
            if 'children' in node.keys():
                for i in range(len(node['children'])):
                    node_i=node['children'][i]
                    if node_i['node_type']==NodeType.ADD.value:
                        dels.append(node['id'])

            return dels
    elif node['node_boolean_subtract_type'] == DelType.PARENT_AND_CHILDRENS.value:
        parent = get_node_first_add_parent(node)
        if parent==None:
            return []
        else:
            dels=get_all_add_childs(parent)

            return dels
    else:
        #print("unsupport del type.")
        return []

#三个点组成的一个夹角，较短边外点到较长边的垂直距离如果小于1nm，则将中间的点替换成垂直相交的点
def is_1nm_error(p1,p2,p3):
    if p1==p2 or p2==p3:
        return False,None
    
    #判断夹角
    #计算斜率
    '''m1=(p2.y-p1.y)/(p2.x-p1.x)
    m2=(p3.y-p2.y)/(p3.x-p2.x)
    angle=math.atan((m2-m1)/(1+m1*m2))
    angle=math.degrees(angle)
    if angle<10:
        return False'''
    
    dis12 = (p1.x-p2.x)*(p1.x-p2.x)+(p1.y-p2.y)*(p1.y-p2.y)
    dis23 = (p3.x-p2.x)*(p3.x-p2.x)+(p3.y-p2.y)*(p3.y-p2.y)

    short_p=p1
    long_p=p3
    if dis12 > dis23:
        short_p=p3
        long_p=p1

    x0=short_p.x
    y0=short_p.y
    x1=p2.x
    y1=p2.y
    x2=long_p.x
    y2=long_p.y

    k = -((x1 - x0) * (x2 - x1) + (y1 - y0) * (y2 - y1)) / ((x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2))
    xf = k * (x2 - x1) + x1
    yf = k * (y2 - y1) + y1
    if k < 0 or k > 1:  #垂点不在线段上
        return False,None

    #判断垂线长度是否小于1nm
    if abs(xf-x0)<=1 and abs(yf-y0)<=1:
        xf=x0+math.ceil(xf-x0)
        yf=y0+math.ceil(yf-y0)
        return True,pya.Point(xf,yf)
    return False,None

#1nm误差 todo
def repair_1nm(poly):
    points=list(poly.each_point_hull())

    if len(points)>2:
        for i in range(len(points)-2):
            is_1nm,p = is_1nm_error(points[i],points[i+1],points[i+2])
            if is_1nm:
                points[i+1] = p

        # -2 -1 0
        is_1nm,p = is_1nm_error(points[-2],points[-1],points[0])
        if is_1nm:
            points[-1] = p

        # -1 0 1
        is_1nm,p = is_1nm_error(points[-1],points[0],points[1])
        if is_1nm:
            points[0] = p

    return pya.Polygon(points)

#一层里面进行add和del，得到一层最后的ploys结果集（除以dbu放大后的结果）
def get_result_polys(polys,dbu,ep):
    layer_polys={}

    ployid_maps={}
    ployid_layer_maps={}
    delid_maps={}
    del_nodes=[]
    for i in range(len(polys)):
        poly=polys[i]
        
        poly_multi1k=poly['points']*(1.0/dbu)
        poly_multi1k=pya.Polygon(poly_multi1k)

        if poly['node_type'] == NodeType.ADD.value:
            ployid_maps[poly['id']]=poly_multi1k
            layer=poly['node']['layer']
            ployid_layer_maps[poly['id']]=layer
        elif poly['node_type'] == NodeType.DELETE.value:
            delid_maps[poly['id']]=poly_multi1k
            del_nodes.append(poly['node'])
        else:
            print("get_result_polys not support type")

    del_ids={}  # id->需要做删除操作的polys
    for node in del_nodes:
        del_i = get_del_ids(node)
        
        for id in del_i:
            if id not in del_ids:
                del_ids[id]=[]

            del_ids[id].append(delid_maps[node['id']])

    #按图层进行切分
    for id in ployid_maps.keys():
        layer=ployid_layer_maps[id]
        if layer not in layer_polys:
            layer_polys[layer]=[]

        if id not in del_ids:
            layer_polys[layer].append(ployid_maps[id])
        else:
            result=ep.boolean_p2p([ployid_maps[id]],del_ids[id],pya.EdgeProcessor.ModeANotB,False,False)
            layer_polys[layer]+=result            

    return layer_polys

def get_result_polys2(polys,dbu,ep):
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
        #t1=datetime.now()
        if types[i]==NodeType.ADD.value:
            del_polys=[]
            for j in range(i+1,len(types)):
                if types[j]==NodeType.DELETE.value:
                    #results[i]=ep.boolean_p2p(results[i],[poly_multi1ks[j]],pya.EdgeProcessor.ModeANotB,False,False)
                    del_polys.append(poly_multi1ks[j])

            if len(del_polys)>0:
                results[i]=ep.boolean_p2p(results[i],del_polys,pya.EdgeProcessor.ModeANotB,False,False)
                    
        else:
            results[i]=[]

        return_polys += results[i]
        #t2=datetime.now()
        #print(i,(t2-t1).total_seconds(),len(list(poly_multi1ks[i].each_point_hull())))
    return return_polys

# 将各层的poly进行加减操作,每个点进行相对坐标锚点位移
def format_points(polys,coord_point,dbu=0.001):
    points_results={}
    ep=pya.EdgeProcessor()
    tr = pya.DCplxTrans(1, 0, False, -coord_point.x, -coord_point.y) # mag, angel, mirrx, dx,dy

    layer_polys=get_result_polys(polys,dbu,ep)
    for k in layer_polys:
        results=layer_polys[k]
        # 再进行merge操作
        #t1=datetime.now()
        results=ep.simple_merge_p2p(results,False,False)
        #t2=datetime.now()
        #print(k,(t2-t1).total_seconds())

        points_results[k]=[]
        for p in results:
            #p = repair_1nm(p)

            p=pya.DPolygon(p)*dbu
            p=p.transformed(tr)
            points_results[k].append(p)

    return points_results




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
        #anchors=sorted(anchors, key=lambda x: x['node_id'])
        for i in range(len(anchors)):
            self.add_anchor(anchors[i])

        # 设置图层
        for l in self.data['primitive']['layers']:
            self._init_layers.append(l['layer_name'])
        
    def get_layer_polys(self,dbu=0.001):
        polys=get_node_points(self.data['primitive']['node'],None,self.get_exp_val,float(self.data['primitive']['arc']['length']),self._param_mgr) #POINT_DISTANCE
        layer_polys=format_points(polys,self._coordinate_point,dbu)

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
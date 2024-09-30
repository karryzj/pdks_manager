# -*- coding: utf-8 -*-
import math
import pya
import klayout.param

import re

def is_number_or_decimal(s):
    return bool(re.fullmatch(r'[-+]?[0-9]*\.?[0-9]+', s))


class QuquanPcellHelper(pya.PCellDeclarationHelper):
    def __init__(self):
        # Important: initialize the super class
        super(QuquanPcellHelper, self).__init__()
        
        self.layer_maps={} # {'Layer1':[1,0],'Layer2':[10,0]}

        self._coordinate={"x":'0',"y":'0'}
        self._coordinate_point=pya.DPoint(0,0)
        self._anchors=[]
        self._rules=[]
        self._param_mgr=klayout.param.ParamMgr.new()
	
    #添加参数
    def add_param(self,p_name,p_default,desc):
        desc_tmp=desc
        if not desc or len(desc)==0:
            desc_tmp=p_name

        if is_number_or_decimal(str(p_default)):
            if type(p_default)==type(1):
                self.param(p_name, self.TypeInt, desc_tmp, default = p_default)
            else:
                self.param(p_name, self.TypeDouble, desc_tmp, default = p_default)
        else:
            self.param(p_name, self.TypeString, desc_tmp, default = p_default,readonly=True)

        self.set_param_val(p_name,p_default,desc)

    def set_param_val(self,name,val,desc=''):
        if self._param_mgr.exist_param(name):
            self._param_mgr.update_param(name,val,desc)
        else:
            self._param_mgr.add_param(name,val,desc)

        #self._param_mgr.refresh_value()
        #self.update_coordinate_point()

    def refresh_value(self):
        self._param_mgr.refresh_value()
        self.update_coordinate_point()

    # 计算表达式取值
    def get_exp_val(self,exp):
        if type(exp)==type(0.1) or type(exp)==type(1):
            return exp
        
        if(is_number_or_decimal(str(exp))):
            return float(exp)
        
        return self._param_mgr.parse(exp)

    #添加参数规则 rule:"x+y"
    def add_rule(self,rule):        
        ret = self._param_mgr.add_rule(rule)
        if not ret:
            print("add rule fail:",rule)
            return False
        
        self._rules.append(rule)
        return True
    
    def get_rule(self,idx):
        if idx>=len(self._rules):
            return None
        
        return self._rules[idx]
    
    def get_rule_count(self):
        return len(self._rules)

    # 设置坐标锚点 coordinate:{'x':'w','y':'1'}
    def set_coordinate_anchor(self,coordinate):
        self._coordinate = coordinate
        self.update_coordinate_point()

    # 更新锚点坐标
    def update_coordinate_point(self):
        x=self.get_exp_val(self._coordinate['x'])
        y=self.get_exp_val(self._coordinate['y'])
        self._coordinate_point=pya.DPoint(x,y)

    #添加锚点 anchor:{"x":1,"y":2, "id":1}
    def add_anchor(self,anchor):
        self._anchors.append(anchor)

    # 获取第idx个锚点的坐标
    def get_anchor(self,idx):
        if idx>= len(self._anchors):
            return None
        
        # 需要移动坐标锚点的距离
        x=self.get_exp_val(self._anchors[idx]['x']) - self._coordinate_point.x
        y=self.get_exp_val(self._anchors[idx]['y']) - self._coordinate_point.y
        return pya.DPoint(x,y)
    
    # 获取锚点个数
    def get_anchor_count(self):
        return len(self._anchors)

    #设置图层对应关系
    def set_layer_maps(self,layer_maps):
        self.layer_maps=layer_maps

    # 获取图层对应的实际图层 {'Layer1':[1,0],'Layer2':[10,0]}
    def get_act_layer(self,layer):
        if layer in self.layer_maps.keys():
            return self.layer_maps[layer]
        
        return None

    #判断参数和rule是否合法
    def is_valid(self):
        #return self._param_mgr.isValid()

        for i in range(len(self._rules)):
            if not self._param_mgr.isValid(self._rules[i]):
                print('rule not valid:',self._rules[i])
                return False
            
        return True
    
    #设置初始值
    #value={'w':1,'h':'w+1'} 仅支持int double str格式
    def set_param_defaults(self,datas):
        for k in datas:
            if self._param_mgr.exist_param(k):
                self.set_param_default(k,datas[k])
            else:
                print("no param name:",k)
        self.refresh_value()

    def get_param_value(self,name):
        param=self._param_mgr.find_param(name)
        if param:
            return param.value()
        
        print("can not find param:",name)
        
        return None

    # 不对外暴露此接口
    def set_param_default(self,name,value):
        for i in range(len(self._param_decls)):
            p=self._param_decls[i]
            ptype=p.type
            if p.name==name:
                if ptype==self.TypeInt:
                    p.default=int(value)
                elif ptype==self.TypeDouble:
                    p.default=float(value)
                else:
                    p.default=str(value)

                #p.default=value
                self.set_param_val(name,value)
                #self.update_coordinate_point()
                return True            
        
        return False
    
    def coerce_parameters_impl(self):
        for i in range(len(self._param_decls)):
            p=self._param_decls[i]
            name=p.name
            if self._param_mgr.exist_param(name):
                #self.set_param_val(p.name,str(getattr(self,name)))
                self.set_param_val(p.name,getattr(self,name))

        self.refresh_value()

    def can_create_from_shape_impl(self):
        return self.shape.is_box() or self.shape.is_polygon() or self.shape.is_path()
    
    def parameters_from_shape_impl(self):
        pass

    def transformation_from_shape_impl(self):
        return pya.Trans(self.shape.bbox().center())

    def display_text_impl(self):
        return 'QuquanPcellHelper'
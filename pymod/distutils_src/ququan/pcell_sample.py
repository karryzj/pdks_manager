# -*- coding: utf-8 -*-
import math
import pya

from ququan.QuquanPcellHelper import QuquanPcellHelper

class TestPcell(QuquanPcellHelper):
  def __init__(self):
    # Important: initialize the super class
    super(TestPcell, self).__init__()

    #定义图层信息 layer_key:[layer_number,layer_datatype]
    self.layer_maps={'Layer1':[1,0],'Layer2':[2,0]}

    #在第一层画一个w_h的正方形，坐标原点在正方形中心，并在中心删除一个width*height的矩形；在中心加一个第二层的小矩形
    #参数管理中定义参数 数字或参数组合形式的字符串，其他非参数组合的字符串请用 self.param("p_str", self.TypeString, "str desc", default = "this is string")
    self.add_param("width",1.0,"width")   # (参数名称，参数默认值，参数描述)
    self.add_param("height",2.0,"height")
    self.add_param("w_h","width+height","w+h")

    #其他非加入参数管理中的参数定义方式
    self.param("p_str", self.TypeString, "str desc", default = "this is string")

    #参数管理中的参数定义约束
    self.add_rule("width>0")
    self.add_rule("width<10")
    self.add_rule("height>0")
    self.add_rule("height<20")

    #增加锚点 按添加顺序获取：self.get_anchor(idx)
    self.add_anchor({"x":1,"y":"w_h"})        #idx:0
    self.add_anchor({"x":"width","y":"w_h"})  #idx:1


  # 根据参数生成图形的回调函数
  def produce_impl(self):
    #检测约束
    if not self.is_valid():
      return
    
    #获取锚点
    anchor_1=self.get_anchor(0) #返回pya.DPoint; idx超出则返回None
    if anchor_1:
      print(anchor_1)
    
    # 需要根据此方法获取layer层
    layer1=self.get_layer_by_name('Layer1')
    layer2=self.get_layer_by_name('Layer2')

    #以self.add_param定义的参数，以self.get_param_value(name)获取参数值 参数名称以字母开头加下划线和数字，不能为x,y
    w=self.get_param_value("width")
    h=self.get_param_value("height")
    wh=self.get_param_value("w_h")
    #print(w,h,wh)

    s=self.p_str # self.param方式定义的参数，以self.+参数名称直接获取值
    pts=[pya.DPoint(-wh/2,-wh/2),pya.DPoint(-wh/2,wh/2),pya.DPoint(wh/2,wh/2),pya.DPoint(wh/2,-wh/2)]
    #self.cell.shapes(layer1).insert(pya.DPolygon(pts)) #如果只是加图形，只调用该接口即可

    #减去某个图形的操作 ep.boolean_p2p只支持整形Polygon，所以先*1000，计算完成后再除以1000
    pts_del=[pya.DPoint(-w/2,-h/2),pya.DPoint(-w/2,h/2),pya.DPoint(w/2,h/2),pya.DPoint(w/2,-h/2)]
    ep=pya.EdgeProcessor()
    polys=ep.boolean_p2p([pya.DPolygon(pts)*1000],[pya.DPolygon(pts_del)*1000],pya.EdgeProcessor.ModeANotB,False,False)
    for p in polys:
      self.cell.shapes(layer1).insert(pya.DPolygon(p)*0.001)

    #再加个第二层的小矩形
    pts_layer2=[pya.DPoint(-w/4,-h/4),pya.DPoint(-w/4,h/4),pya.DPoint(w/4,h/4),pya.DPoint(w/4,-h/4)]
    self.cell.shapes(layer2).insert(pya.DPolygon(pts_layer2))

  def display_text_impl(self):
    # Provide a descriptive text for the cell
    return "TestPcell(w=" + ('%.3f' % self.width) + ")"
  
  def get_layer_by_name(self,name):
    if name in self.layer_maps:
      l=self.layer_maps[name]
      return self.layout.layer(l[0],l[1])
     
    return None     

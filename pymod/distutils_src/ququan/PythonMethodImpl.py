# -*- coding: utf-8 -*-
import klayout
import klayout.common
from ququan.library_helper import export_pcell_to_gds

class PythonMethodImpl(klayout.common.PythonMethod):
    def __init__(self):
        self.instance = self

    def export_pcell_to_gds(self,pdk,lib,cell,gds):
        #print("this is export callback")
        export_pcell_to_gds(pdk,lib,cell,gds)
        return True
    

ins=klayout.common.PythonMethod.Instance
ins_impl=PythonMethodImpl()
ins.set_instance(ins_impl)

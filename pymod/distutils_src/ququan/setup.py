# -*- coding: utf-8 -*-
from distutils.core import setup
from Cython.Build import cythonize
import os
import sys
import sysconfig

file_paths=[
	"library_helper.py",
	"QuquanPcellHelper.py",
	"QuquanShape.py",
	"util.py"
]

setup(ext_modules=cythonize(file_paths))
#exit()

def delete_file(filename):
    if os.path.exists(filename):
        os.remove(filename)
        #print(f"文件 {filename} 删除成功！")
    else:
        print(f"文件 {filename} 不存在。")

def get_cp_os():
	cp='.cp'+str(sys.version_info.major)+str(sys.version_info.minor)+"-"
	os=sysconfig.get_platform().replace('-','_')
	return cp+os
	
# clean
cpos=get_cp_os()
for f in file_paths:
	pre=os.path.basename(f)
	pre=os.path.splitext(pre)[0]

	#delete_file(pre+cpos+".pyd")
	delete_file(pre+".c")


# python < 3.12
# python setup.py build_ext --inplace


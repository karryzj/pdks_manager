# -*- coding: utf-8 -*-
import os
import sys
from pathlib import Path


_ququan_path = Path(os.path.dirname(os.path.realpath(__file__))).parent.parent
#print(_ququan_path)

"""
# 目录结构如下
ququan
    -- xx.exe
    -- data
        -- primitives #图元
            -- 图元分类1
                -- 图元1
                    -- data.json
                    -- xx.png
                -- 图元2
                    -- data.py
                    -- xx.png
        -- pdks
            -- pdk1
            -- pdk2
                -- layer.json
                -- drc
                    -- drc_1.py
                    -- drc_2.py
                -- librarys
                    -- lib1
                    -- lib2
                        -- pcell_1
                            -- data.json
                            -- extra.json
                            -- xx.png
        -- workspaces #工程文件夹

"""

#设置为自己的路径 或者在环境变量中设置
#os.environ['QUQUAN_ROOT_PATH'] = "D:/project/ququan_shapes/build/msvc2017_release"

#os.putenv("QUQUAN_ROOT_PATH", "D:/project/ququan_shapes/build/mingw-Debug")

QUQUAN_ROOT_PATH = _ququan_path #Path(os.getenv("QUQUAN_ROOT_PATH", os.getcwd()))    #exe的运行目录

QUQUAN_DATA_PATH = QUQUAN_ROOT_PATH.joinpath("data")
QUQUAN_PRIMITIVES_PATH = QUQUAN_DATA_PATH.joinpath("primitives")
QUQUAN_PDKS_PATH = QUQUAN_DATA_PATH.joinpath("pdks")

QUQUAN_PYMOD_PATH = QUQUAN_ROOT_PATH.joinpath("pymod")

POINT_DISTANCE=1 # um 用作生成圆取样点

sys.path.append(str(QUQUAN_PYMOD_PATH))
os.add_dll_directory(QUQUAN_ROOT_PATH)


# TEST
#pkg="mingw-Debug"
#cellname=Path("D:/project/ququan_shapes/build/mingw-Debug/data/pdks/pdk1/librarys/cate1/t1")
#import_path_parts=cellname.parts[len(QUQUAN_ROOT_PATH.parts)-1:]
#import_path = ".".join(import_path_parts)
#print(import_path)
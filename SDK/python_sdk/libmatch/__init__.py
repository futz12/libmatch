from ctypes import *
import os

cur_path = os.path.dirname(__file__)
# if windows
if os.name == 'nt':
    lib = cdll.LoadLibrary(os.path.join(cur_path, 'libmatch.dll'))
# if linux
else:
    lib = cdll.LoadLibrary(os.path.join(cur_path, 'libmatch.so'))


# 输出版本号
# LIBMATCH_API const char* libmatch_version()

lib.libmatch_version.restype = c_char_p

def libmatch_version():
    return lib.libmatch_version().decode('utf-8')


print("当前版本号：",libmatch_version())


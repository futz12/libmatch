import io

from libmatch import lib
from ctypes import *
from PIL import Image


lib.show_bitmap.argtypes = [c_void_p,c_char_p]
lib.show_bitmap.restype = c_bool

def show_bitmap(img:Image,title:str):
    byte_stream = io.BytesIO()

    img.save(byte_stream, format='BMP')

    bitmap_file = byte_stream.getvalue()

    return lib.show_bitmap(bitmap_file,title.encode('utf-8'))


import io

from . import lib
from ctypes import *
import PIL.Image as Image

lib.create_ppocr.argtypes = [c_void_p, c_int, c_char_p, c_void_p, c_int, c_char_p, c_char_p, c_int, c_bool]
lib.create_ppocr.restype = c_void_p

lib.release_ppocr.argtypes = [c_void_p]
lib.release_ppocr.restype = None

lib.ppocr_detect.argtypes = [c_void_p, c_void_p, c_int]
lib.ppocr_detect.restype = c_void_p

lib.ppocr_result_size.argtypes = [c_void_p]
lib.ppocr_result_size.restype = c_uint32

lib.ppocr_get_textbox.argtypes = [c_void_p, c_uint32, c_void_p]
lib.ppocr_get_textbox.restype = None

lib.release_ppocr_textbox.argtypes = [c_void_p]
lib.release_ppocr_textbox.restype = None

lib.release_ppocr_result.argtypes = [c_void_p]
lib.release_ppocr_result.restype = None


class _Point(Structure):
    _fields_ = [
        ('x', c_int),
        ('y', c_int)
    ]

    def __str__(self):
        return "Point(x={}, y={})".format(self.x, self.y)


class _TextBox(Structure):
    _fields_ = [
        ('boxPoint', _Point * 4),
        ('score', c_float),
        ('text', c_char_p),
        ('size_charPositions', c_int),
        ('charPositions', POINTER(c_int))
    ]

    def __str__(self):
        return "TextBox(boxPoint=[{}, {}, {}, {}], score={}, text={}, size_charPositions={}, charPositions={})".format(
            self.boxPoint[0], self.boxPoint[1], self.boxPoint[2], self.boxPoint[3], self.score, self.text,
            self.size_charPositions, self.charPositions
        )


class Point:
    def __init__(self, x: int, y: int):
        self.x = x
        self.y = y

    def __str__(self):
        return "Point(x={}, y={})".format(self.x, self.y)


class TextBox:
    def __init__(self, boxPoint: list[Point], score: float, text: str, charPositions: list[int]):
        self.boxPoint = boxPoint
        self.score = score
        self.text = text
        self.charPositions = charPositions

    def __str__(self):
        return "TextBox(boxPoint=[{}, {}, {}, {}], score={}, text={}, charPositions={})".format(
            self.boxPoint[0], self.boxPoint[1], self.boxPoint[2], self.boxPoint[3], self.score, self.text,
            self.charPositions
        )


class ppocr:
    def __init__(self, model_dir: str, det_bin: str = 'ch_PP-OCRv3_det_fp16.bin',
                 det_param: str = 'ch_PP-OCRv3_det_fp16.param', rec_bin: str = 'ch_PP-OCRv3_rec_fp16.bin',
                 rec_param: str = 'ch_PP-OCRv3_rec_fp16.param', keylist: str = 'paddleocr_keys.txt',
                 num_thread: int = 4, use_vulkan: bool = False):
        _det_bin = open(model_dir + det_bin, 'rb').read()
        _det_param = open(model_dir + det_param, 'rb').read()
        _rec_bin = open(model_dir + rec_bin, 'rb').read()
        _rec_param = open(model_dir + rec_param, 'rb').read()
        _keylist = open(model_dir + keylist, 'rb').read()
        self._model = lib.create_ppocr(_det_bin, len(_det_bin), _det_param, _rec_bin, len(_rec_bin), _rec_param,
                                       _keylist, num_thread, use_vulkan)

    def __del__(self):
        lib.release_ppocr(self._model)

    def detect(self, image: Image) -> list:
        byte_stream = io.BytesIO()

        image.save(byte_stream, format='BMP')
        bitmap_file = byte_stream.getvalue()

        hresult = lib.ppocr_detect(self._model, bitmap_file, len(bitmap_file))
        result_size = lib.ppocr_result_size(hresult)

        result = []

        for i in range(result_size):
            box = _TextBox()
            lib.ppocr_get_textbox(hresult, i, byref(box))
            boxPoint = [Point(box.boxPoint[0].x, box.boxPoint[0].y), Point(box.boxPoint[1].x, box.boxPoint[1].y),
                        Point(box.boxPoint[2].x, box.boxPoint[2].y), Point(box.boxPoint[3].x, box.boxPoint[3].y)]
            charPositions = [box.charPositions[j] for j in range(box.size_charPositions)]
            result.append(TextBox(boxPoint, box.score, box.text.decode('gbk'), charPositions))
            lib.release_ppocr_textbox(byref(box))

        lib.release_ppocr_result(hresult)

        return result

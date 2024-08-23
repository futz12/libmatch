import io

from . import lib
from ctypes import *
from PIL import Image

lib.create_ddddrec.argtypes = [c_void_p, c_int, c_char_p, c_char_p, c_int, c_bool]
lib.create_ddddrec.restype = c_void_p

lib.release_ddddrec.argtypes = [c_void_p]
lib.release_ddddrec.restype = None

lib.ddddrec_detect.argtypes = [c_void_p, c_void_p, c_int, c_char_p, c_void_p]
lib.ddddrec_detect.restype = c_uint32

lib.release_ddddrec_result.argtypes = [c_void_p]
lib.release_ddddrec_result.restype = None


class _RecResult(Structure):
    _fields = [
        ('text', c_char_p),
        ('charPositions', POINTER(c_int)),
        ('prob', c_float)
    ]

    def __str__(self):
        return "RecResult(text={}, charPositions={}, prob={})".format(self.text, self.charPositions, self.prob)


class RecResult:
    def __init__(self, text: str, charPositions: list, prob: float):
        self.text = text
        self.charPositions = charPositions
        self.prob = prob

    def __str__(self):
        return "RecResult(text={}, charPositions={}, prob={})".format(self.text, self.charPositions, self.prob)


class ddddocr:
    def __init__(self, model_dir: str, rec_bin: str = 'ddddrec.bin',
                 rec_param: str = 'ddddrec.param', keylist: str = 'ddddocr_keys.txt',
                 num_thread: int = 4, use_vulkan: bool = False):
        _rec_bin = open(model_dir + rec_bin, 'rb').read()
        _rec_param = open(model_dir + rec_param, 'rb').read()
        _keylist = open(model_dir + keylist, 'rb').read()
        self._model = lib.create_ddddrec(_rec_bin, len(_rec_bin), _rec_param, _keylist, num_thread, use_vulkan)

    def __del__(self):
        lib.release_ddddrec(self._model)

    def detect(self, image: Image, options: list[str]) -> RecResult:
        byte_stream = io.BytesIO()

        image.save(byte_stream, format='BMP')
        bitmap_file = byte_stream.getvalue()

        opt_str = ''
        for opt in options:
            opt_str += opt + '\n'

        opt_str = opt_str.encode('gbk')

        ret = _RecResult()
        text_len = lib.ddddrec_detect(self._model, bitmap_file, len(bitmap_file), opt_str, byref(ret))

        text = ret.text.decode('gbk')
        charPositions = [ret.charPositions[i] for i in range(text_len)]
        prob = ret.prob

        lib.release_ddddrec_result(ret)

        return RecResult(text, charPositions, prob)

import io

from libmatch import lib
from ctypes import *
import PIL.Image as Image

lib.template_matcher_compute.argtypes = [c_void_p, c_char_p, c_int, c_float, c_float]
lib.template_matcher_compute.restype = c_void_p

lib.template_matcher_result_size.argtypes = [c_void_p]
lib.template_matcher_result_size.restype = c_int

lib.template_matcher_result_get.argtypes = [c_void_p, c_size_t, c_void_p]
lib.template_matcher_result_get.restype = c_void_p

lib.release_template_matcher_result.argtypes = [c_void_p]

lib.create_template_matcher.argtypes = [c_char_p, c_int, c_uint]
lib.create_template_matcher.restype = c_void_p

lib.release_template_matcher.argtypes = [c_void_p]


class mode:
    COLOR_GRAY = 0  # Use target image AS a gray image.
    COLOR_BGRA = 1  # Use target image AS a BGR image.
    COLOR_BGR = 2  # Use target image AS image has Aplha channel but gray
    COLOR_BGRA_COLOR = 3  # Use target image AS image has Aplha channel
    COLOR_BGR_MASK = 4  # Use target image AS a BGR image and set (0,0) color as mask color
    COLOR_GRAY_MASK = 5  # Use target image AS a gray image and set (0,0) color as mask color


class _Rect(Structure):
    _fields_ = [
        ('x', c_int),
        ('y', c_int),
        ('width', c_int),
        ('height', c_int)
    ]

    def __str__(self):
        return "Rect(x={}, y={}, width={}, height={})".format(self.x, self.y, self.width, self.height)


class _objectEx(Structure):
    _fields_ = [
        ('rect', _Rect),
        ('prob', c_float),
    ]

    def __str__(self):
        return "ObjectEx(rect={}, prob={})".format(self.rect, self.prob)


class Rect:
    def __init__(self, x, y, width, height):
        self.x = x
        self.y = y
        self.width = width
        self.height = height

    def __str__(self):
        return "Rect(x={}, y={}, width={}, height={})".format(self.x, self.y, self.width, self.height)

    def __repr__(self):
        return self.__str__()


class objectEx:
    def __init__(self, rect, prob):
        self.rect = rect
        self.prob = prob

    def __str__(self):
        return "objectEx(rect={}, prob={})".format(self.rect, self.prob)

    def __repr__(self):
        return self.__str__()


class template_matcher:

    def __init__(self, target_image, mode):
        # LIBMATCH_C_API void* create_template_matcher(uint8_t *target_img_data, int target_img_size, uint32_t mode)

        byte_stream = io.BytesIO()

        target_image.save(byte_stream, format='BMP')

        target_img_data = byte_stream.getvalue()
        target_img_size = len(target_img_data)

        self.matcher = lib.create_template_matcher(target_img_data, target_img_size, mode)

    def __del__(self):
        # LIBMATCH_C_API void release_template_matcher(void* matcher)
        lib.release_template_matcher(self.matcher)

    def compute(self, source_image, prob_threshold=0.65, nms_threshold=0.25):
        # LIBMATCH_C_API void* template_matcher_compute(void* matcher, uint8_t *src_img_data, int src_img_size, float prob_threshold, float nms_threshold)
        byte_stream = io.BytesIO()
        source_image.save(byte_stream, format='BMP')
        src_img_data = byte_stream.getvalue()
        src_img_size = len(src_img_data)

        # LIBMATCH_C_API int template_matcher_result_count(void* result)

        hresult = lib.template_matcher_compute(self.matcher, src_img_data, src_img_size, prob_threshold, nms_threshold)

        result_count = lib.template_matcher_result_size(hresult)

        # LIBMATCH_C_API void template_matcher_result_get(void* result, size_t index, void* result_obj)

        result_list = []

        for i in range(result_count):
            result_obj = _objectEx()
            lib.template_matcher_result_get(hresult, i, byref(result_obj))

            rect = Rect(result_obj.rect.x, result_obj.rect.y, result_obj.rect.width, result_obj.rect.height)
            result_list.append(objectEx(rect, result_obj.prob))

        # LIBMATCH_C_API void release_template_matcher_result(void* result)

        lib.release_template_matcher_result(hresult)

        return result_list

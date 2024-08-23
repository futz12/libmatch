# LibMatch GUIDE

## Version 0.9.6

## C API

### Template Match FUNCTIONS

```cpp
LIBMATCH_C_API void* create_template_matcher(uint8_t *target_img_data, int target_img_size, uint32_t mode)
```

Create a template matcher.

- `target_img_data`: The data of the target image.
- `target_img_size`: The size of the target image.
- `mode`: The mode of the template matcher.
-

| Mode             | Description                                                        |
|------------------|--------------------------------------------------------------------|
| COLOR_GRAY       | Use target image AS a gray image.                                  |
| COLOR_BGR        | Use target image AS a BGR image.                                   |
| COLOR_BGRA       | Use target image AS image has Aplha channel but gray               |
| COLOR_BGRA_COLOR | Use target image AS image has Aplha channel                        |
| COLOR_BGR_MASK   | Use target image AS a BGR image and set (0,0) color as mask color  |
| COLOR_GRAY_MASK  | Use target image AS a gray image and set (0,0) color as mask color |

Return the handle of the template matcher.

---

```cpp
LIBMATCH_C_API void release_template_matcher(void* matcher)
```

Destroy a template matcher.

- `matcher`: The template matcher.

---

```cpp
LIBMATCH_C_API void* template_matcher_compute(void* matcher, uint8_t *src_img_data, int src_img_size, float prob_threshold, float nms_threshold)
```

Compute the template match.

- `matcher`: The template matcher.
- `src_img_data`: The data of the source image.
- `src_img_size`: The size of the source image.
- `prob_threshold`: The probability threshold.
- `nms_threshold`: The NMS threshold.

Return the handle of the result.

---

```cpp
LIBMATCH_C_API void release_template_matcher_result(void* result)
```

Destroy the result of the template match.

- `result`: The result of the template match.

---

```cpp
LIBMATCH_C_API int template_matcher_result_size(void* result)
```

Get the count of the result.

- `result`: The result of the template match.

Return the count of the result.

---

```cpp
LIBMATCH_C_API void template_matcher_result_get(void* result, size_t index, void* result_obj)
```

Get the result object.

- `result`: The handle of the result.
- `index`: The index you want to get.
- `result_obj`: The pointer of the objectEx(Input)

---

### Template Match OBJECT

```cpp
struct Rect {
    int x, y, width, height;
};
```

The rectangle.

---

```cpp
struct objectEx {
    Rect rect;
    float prob;
};
```

The objectEx.

- `rect`: The rectangle.
- `prob`: The probability.

---

### ORB Match FUNCTIONS

```cpp
LIBMATCH_C_API void *orb_create_featurer(uint8_t *img_data, int img_size, void *param, int mode);
```

Create a ORB featurer.

- `img_data`: The data of the image.
- `img_size`: The size of the image.
- `param`: The param of the ORB(orb_param pointer), set 0 will use default param.
- `mode`: The mode of the ORB.(reserved)

Return the handle of the ORB featurer.

---

```cpp
LIBMATCH_C_API void orb_release_featurer(void *featurer);
```

Destroy a ORB featurer.

- `featurer`: The ORB featurer.

---

```cpp
LIBMATCH_C_API void *orb_create_matcher(int mode);
```

Create a ORB matcher.

- `mode`: The mode of the ORB matcher.(0 is flann, 1 is hamming)

Return the handle of the ORB matcher.

---

```cpp
LIBMATCH_C_API void orb_release_matcher(void *matcher);
```

Destroy a ORB matcher.

- `matcher`: The ORB matcher.

---

```cpp
LIBMATCH_C_API uint32_t orb_matcher_compute(void *matcher, void *source, void *target, float thresh, void *result);
```

Compute the ORB match.

- `matcher`: The ORB matcher.
- `source`: The source featurer.
- `target`: The target featurer.
- `thresh`: The threshold.(the range of flann mode is [0,1], the range of hamming mode is [0,256])
- `result`: The result of the ORB match.

Return the count of the result.

---

### ORB Match OBJECT

```cpp
struct objectEx2 {
    struct {
        float x, y;
    } dots[4];
    float prob;
};
```

The objectEx2.

- `dots`: The dots.
- `x`: The x of the dot.
- `y`: The y of the dot.
- `prob`: The probability.

---

```cpp
struct orb_param {
    int nfeatures = 500;
    float scaleFactor = 1.2f;
    int nlevels = 8;
    int edgeThreshold = 31;
    int firstLevel = 0;
    int WTA_K = 2;
    int scoreType = 0;
    int patchSize = 31;
    int fastThreshold = 20;
};
```

The orb_param.

- `nfeatures`: The number of features you want to get.
- `scaleFactor`: The scale factor.
- `nlevels`: The number of levels.(如果目标缩放比例较大，可以增加这个值)
- `edgeThreshold`: The edge threshold.(如果目标为文本，可以减小这个值)
- `firstLevel`: The first level.
- `WTA_K`: The WTA_K.
- `scoreType`: The score type, set 1 will be faster but less accurate.
- `patchSize`: The patch size.
- `fastThreshold`: The fast threshold.

### OCR FUNCTIONS

```cpp
LIBMATCH_C_API void* create_ppocr(uint8_t *det_bin, int det_bin_size, char *det_param, uint8_t *rec_bin, int rec_bin_size, char *rec_param, const char *keylist, int num_thread, bool use_vulkan)
```

Create a PP-OCR.

- `det_bin`: The data of the det model.
- `det_bin_size`: The size of the det model.
- `det_param`: The param of the det model.
- `rec_bin`: The data of the rec model.
- `rec_bin_size`: The size of the rec model.
- `rec_param`: The param of the rec model.
- `keylist`: The keylist.
- `num_thread`: The number of thread.
- `use_vulkan`: Use vulkan or not.

Return the handle of the PP-OCR.

---

```cpp
LIBMATCH_C_API void release_ppocr(void* ppocr)
```

Destroy a PP-OCR.

- `ppocr`: The PP-OCR.

---

```cpp
LIBMATCH_C_API void* ppocr_detect(void* ppocr, uint8_t *src_img_data, int src_img_size)
```

Detect the text.

- `ppocr`: The PP-OCR.
- `src_img_data`: The data of the source image.
- `src_img_size`: The size of the source image.

Return the handle of the result.

---

```cpp
LIBMATCH_C_API size_t ppocr_result_size(void* result)
```

Get the size of the result.

- `result`: The result of the PP-OCR.

Return the size of the result.

---

```cpp
LIBMATCH_C_API void ppocr_get_textbox(void* result, size_t index, void* result_obj)
```

Get the textbox.

- `result`: The result of the PP-OCR.
- `index`: The index you want to get.
- `result_obj`: The pointer of the TextBox(Input).

---

```cpp
LIBMATCH_C_API void release_ppocr_textbox(void* result_obj)
```

Destroy the textbox.

- `result_obj`: The textbox.

---

```cpp
LIBMATCH_C_API void release_ppocr_result(void* result)
```

Destroy the result of the PP-OCR.

- `result`: The result of the PP-OCR.

### OCR OBJECT

```cpp
struct Point {
    int x, y;
};
```

The point.

---

```cpp
struct TextBox {
    Point boxPoint[4];
    float score;
    char *text;
    int size_charPositions;
    int *charPositions;
};
```

The TextBox.

- `boxPoint`: The box point.
- `score`: The score.
- `text`: The text.
- `size_charPositions`: The size of the per-char positions.
- `charPositions`: The array of the per-char positions.

---

### DDDDOCR REC Function

```cpp
LIBMATCH_C_API void *create_ddddrec(uint8_t *rec_bin, int rec_bin_size, char *rec_param, const char *keylist, int num_thread, bool use_vulkan)
```

Create a DDDD-REC.

- `rec_bin`: The data of the rec model.
- `rec_bin_size`: The size of the rec model.
- `rec_param`: The param of the rec model.
- `keylist`: The keylist.
- `num_thread`: The number of thread.
- `use_vulkan`: Use vulkan or not.

Return the handle of the DDDD-REC.

---

```cpp
LIBMATCH_C_API void release_ddddrec(void *ddddrec)
```

Destroy a DDDD-REC.

- `ddddrec`: The handle of the DDDD-REC.

---

```cpp
LIBMATCH_C_API uint32_t ddddrec_detect(void *ddddrec, uint8_t *src_img_data, int src_img_size, const char *options, void *result)
```

Detect the text.

- `ddddrec`: The handle of the DDDD-REC.
- `src_img_data`: The data of the source image.
- `src_img_size`: The size of the source image.
- `options`: The text the image may contain. Set "" will detect all text.
- `result`: The result(pointer of RecResult) of the DDDD-REC.

Return the count of the text.

---

```cpp
LIBMATCH_C_API void release_ddddrec_result(void *result)
```

Destroy the result of the DDDD-REC.

- `result`: The result of the DDDD-REC.

### DDDDOCR REC OBJECT

```cpp
struct RecResult {
    char *text;
    int *charPositions;
    float prob;
};
```

The RecResult.
- `text`: The text.
- `charPositions`: The array of the per-char positions.
- `prob`: The probability of all the text.

### Release The Library

```cpp
LIBMATCH_C_API void unregister_vulkan()
```

Unregister the vulkan.

> ATTENTION: You should call this function when you use the vulkan and you want to exit the program.






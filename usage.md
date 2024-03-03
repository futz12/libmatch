# LibMatch GUIDE

## Version 0.9.5

## C API

### Template Match FUNCTIONS

```
LIBMATCH_C_API void* create_template_matcher(uint8_t *target_img_data, int target_img_size, uint32_t mode)
```

Create a template matcher.

- `target_img_data`: The data of the target image.
- `target_img_size`: The size of the target image.
- `mode`: The mode of the template matcher.
- 
| Mode | Description                                          |
| ---- |------------------------------------------------------|
| COLOR_GRAY | Use target image AS a gray image.                    |
| COLOR_BGR  | Use target image AS a BGR image.                     |
| COLOR_BGRA | Use target image AS image has Aplha channel but gray |
| COLOR_BGRA_COLOR | Use target image AS image has Aplha channel          |

Return the handle of the template matcher.

---

```
LIBMATCH_C_API void release_template_matcher(void* matcher)
```

Destroy a template matcher.

- `matcher`: The template matcher.

---

```
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

```
LIBMATCH_C_API void release_template_matcher_result(void* result)
```

Destroy the result of the template match.

- `result`: The result of the template match.

---

```
LIBMATCH_C_API int template_matcher_result_count(void* result)
```

Get the count of the result.

- `result`: The result of the template match.

Return the count of the result.

---

```
LIBMATCH_C_API void template_matcher_result_get(void* result, size_t index, void* result_obj)
```

Get the result object.

- `result`: The handle of the result.
- `index`: The index you want to get.
- `result_obj`: The pointer of the objectEx(Input)

---

### Template Match OBJECT

```
struct Rect {
    int x, y, width, height;
};
```

The rectangle.

---

```
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

```
LIBMATCH_C_API void* create_orb_matcher(uint8_t *target_img_data, int target_img_size, uint32_t mode)
```

Create a ORB matcher.

- `target_img_data`: The data of the target image.
- `target_img_size`: The size of the target image.
- `mode`: The mode of the ORB matcher.(Not used now)

Return the handle of the ORB matcher.

---

```
LIBMATCH_C_API void release_orb_matcher(void* matcher)
```

Destroy a ORB matcher.

- `matcher`: The ORB matcher.

---

```
LIBMATCH_C_API bool orb_matcher_compute(void* matcher, uint8_t *src_img_data, int src_img_size, int n_features, int max_distance, void* result);
```

Compute the ORB match.

- `matcher`: The ORB matcher.
- `src_img_data`: The data of the source image.
- `src_img_size`: The size of the source image.
- `n_features`: The number of features.
- `max_distance`: The max distance.(0-256)
- `result`: The result of the ORB match.(objectEx2)

Return the handle of the result.

---

### ORB Match OBJECT

```
struct objectEx2 {
    struct {
        float x, y;
    } dots[4];
};
```

The objectEx2.

- `dots`: The dots.
- `x`: The x of the dot.
- `y`: The y of the dot.

---

### OCR FUNCTIONS

```
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

```
LIBMATCH_C_API void release_ppocr(void* ppocr)
```

Destroy a PP-OCR.

- `ppocr`: The PP-OCR.

---

```
LIBMATCH_C_API void* ppocr_detect(void* ppocr, uint8_t *src_img_data, int src_img_size)
```

Detect the text.

- `ppocr`: The PP-OCR.
- `src_img_data`: The data of the source image.
- `src_img_size`: The size of the source image.

Return the handle of the result.

---

```
LIBMATCH_C_API size_t ppocr_result_size(void* result)
```

Get the size of the result.

- `result`: The result of the PP-OCR.

Return the size of the result.

---

```
LIBMATCH_C_API void ppocr_get_textbox(void* result, size_t index, void* result_obj)
```

Get the textbox.

- `result`: The result of the PP-OCR.
- `index`: The index you want to get.
- `result_obj`: The pointer of the TextBox(Input).

---

```
LIBMATCH_C_API void release_ppocr_textbox(void* result_obj)
```

Destroy the textbox.

- `result_obj`: The textbox.

---

```
LIBMATCH_C_API void release_ppocr_result(void* result)
```

Destroy the result of the PP-OCR.

- `result`: The result of the PP-OCR.

### OCR OBJECT

```
struct Point {
    int x, y;
};
```

The point.

---

```
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

### YOLO Match FUNCTIONS

```
LIBMATCH_C_API void* create_yolo57(uint8_t *bin, int bin_size, char *param, char *config)
```

Create a YOLO57.

- `bin`: The data of the bin.
- `bin_size`: The size of the bin.
- `param`: The param of the model.
- `config`: The config of the model.

Return the handle of the YOLO57.

```
LIBMATCH_C_API void release_yolo57(void* yolo)
```

Destroy a YOLO57.

- `yolo`: The YOLO57.

---

```
LIBMATCH_C_API void* yolo57_detect(void* yolo57, uint8_t *src_img_data, int src_img_size,float prob_threshold, float nms_threshold,bool agnostic)
```

Detect the object.

- `yolo57`: The YOLO57.
- `src_img_data`: The data of the source image.
- `src_img_size`: The size of the source image.
- `prob_threshold`: The probability threshold.
- `nms_threshold`: The NMS threshold.
- `agnostic`: The agnostic.(Can it show only one class at a position)

Return the handle of the result.
---
```
LIBMATCH_C_API size_t yolo57_result_size(void* result)
```

Get the size of the result.

- `result`: The result of the YOLO57.

Return the size of the result.

---
```
LIBMATCH_C_API void yolo57_get_object(void* result, size_t index, void* result_obj)
```

Get the object.

- `result`: The result of the YOLO57.
- `index`: The index you want to get.
- `result_obj`: The pointer of the object.(Input)

---
```
LIBMATCH_C_API void release_yolo57_result(void* result)
```

Destroy the result of the YOLO57.

- `result`: The result of the YOLO57.

### YOLO OBJECT

```
struct RectF {
    float x, y, width, height;
};
```

The rectangle.

---

```
struct object
{
    RectF rect;
    int label;
    float prob;
};
```

The object.

- `rect`: The rectangle.
- `label`: The index of label.
- `prob`: The probability.

---

### Screenshot FUNCTIONS

```
LIBMATCH_C_API void* create_window_capture(void *hwnd, int sx, int sy, int ex, int ey)
```

Create a window capture.

- `hwnd`: The handle of the window.
- `sx`: The start x.
- `sy`: The start y.
- `ex`: The end x.
- `ey`: The end y.

> ATTENTION: The size of the capture is `ex-sx+1` and `ey-sy+1`.
> The begin of sx and sy is 1.

Return the handle of the window capture.

---
```
LIBMATCH_C_API void release_window_capture(void *capture)
```

Destroy a window capture.

- `capture`: The window capture.

---
```
LIBMATCH_C_API void capture_update(void *capture)
```

Update the window capture.

- `capture`: The window capture.

---
```
LIBMATCH_C_API int capture_get_bitmap(void *capture,void **bitmap)
```

Get the bitmap of the window capture.

- `capture`: The window capture.
- `bitmap`: The pointer of the bitmap.(Input)

> ATTENTION: The bitmap is a pointer of the bitmap.
> And it will be free when you call `release_window_capture`.

---

### Release The Library

```
LIBMATCH_C_API void unregister_vulkan()
```

Unregister the vulkan.

> ATTENTION: You should call this function when you use the vulkan and you want to exit the program.






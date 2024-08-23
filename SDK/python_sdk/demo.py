from libmatch import matcher, ddddocr, ppocr
from PIL import Image, ImageFont, ImageDraw

# Template Match

target_image = Image.open("./assets/4.png")

m = matcher.template_matcher(target_image, matcher.mode.COLOR_BGR)

src_image = Image.open("./assets/3.png")

ans = m.compute(src_image, 0.8, 0.8)

# =========================

# PPOCR

ppocr = ppocr.ppocr("./models/")
input_image = Image.open("./assets/3.png")
ocr_result = ppocr.detect(input_image)

font_style = ImageFont.truetype("simsun.ttc", 20, encoding="utf-8")

# 绘制文本框

for i in range(len(ocr_result)):
    box = ocr_result[i].boxPoint
    draw = ImageDraw.Draw(input_image)
    draw.polygon([box[0].x, box[0].y, box[1].x, box[1].y, box[2].x, box[2].y, box[3].x, box[3].y], outline=(255, 0, 0))
    font = ImageFont.load_default()
    draw.text((box[0].x, box[0].y), ocr_result[i].text, font=font_style, fill=(255, 0, 0))

input_image.show()

# =========================

# DDODCR

ddddocr = ddddocr.ddddocr("./models/")
input_image = Image.open("./assets/3.png")
ocr_result = ddddocr.detect(input_image, [])

print(ocr_result)

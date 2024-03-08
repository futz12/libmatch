from libmatch import matcher
from PIL import Image

target_image = Image.open("./4.jpg")

m = matcher.template_matcher(target_image, matcher.mode.COLOR_BGR)

src_image = Image.open("./3.jpg")

ans = m.compute(src_image, 0.8, 0.8)
print(ans)
from PIL import Image, ImageDraw
import math


def convert_point(p, height):
    """将左下角原点坐标转换为 Pillow 左上角坐标"""
    x, y = p
    return (x, height - 1 - y)


def draw_transparent_line(input_path, output_path, p1, p2):
    img = Image.open(input_path).convert("RGBA")
    width, height = img.size

    # 左下 → 左上
    p1p = convert_point(p1, height)
    p2p = convert_point(p2, height)

    # 第一方向的在线间距
    dx = 28
    dy = 21
    offset1 = (dx, dy)

    # 第一方向的主方向（p1->p2）
    dir1 = (p2p[0] - p1p[0], p2p[1] - p1p[1])
    len_dir1 = math.hypot(dir1[0], dir1[1])

    # 均分成44份
    step = len_dir1 / 44
    unit_dir1 = (dir1[0] / len_dir1, dir1[1] / len_dir1)
    offset2 = (unit_dir1[0] * step, unit_dir1[1] * step)

    overlay = Image.new("RGBA", img.size, (0, 0, 0, 0))
    draw = ImageDraw.Draw(overlay)
    white = (255, 255, 255, 127)

    # 第一组线
    for i in range(45):
        draw.line(
            [(p1p[0] + offset1[0] * i, p1p[1] + offset1[1] * i), (p2p[0] + offset1[0] * i, p2p[1] + offset1[1] * i)], fill=white, width=4
        )

    # 第二组线
    for j in range(45):
        start = (p1p[0] + offset2[0] * j, p1p[1] + offset2[1] * j)  # 沿 AB 方向平移
        end = (start[0] + offset1[0] * 44, start[1] + offset1[1] * 44)  # 沿 AD 方向扫满整个网格高度

        draw.line([start, end], fill=white, width=4)

    out = Image.alpha_composite(img, overlay)
    if output_path.lower().endswith(".jpg"):
        out = out.convert("RGB")

    out.save(output_path)


if __name__ == "__main__":
    draw_transparent_line("Resources/BaseMap.jpg", "Resources/LinedBaseMap.jpg", (660, 1370), (660 + 1235, 1370 + 925))

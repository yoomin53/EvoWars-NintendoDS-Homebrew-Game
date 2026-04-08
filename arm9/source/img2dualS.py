#Copyright (c) 2026 Jung, Yoomin. All rights reserved.
#!/usr/bin/env python3
import sys
from PIL import Image

TARGET_W = 256
TARGET_H = 384
VISIBLE_H = 192
CANVAS_SIZE = 256
MAGENTA = (255, 0, 255)

def resize_and_split(image_path):
    img = Image.open(image_path).convert("RGB")
    w, h = img.size

    # ----- Step 1: Resize (cover) -----
    scale = max(TARGET_W / w, TARGET_H / h)
    new_w = int(w * scale)
    new_h = int(h * scale)
    resized = img.resize((new_w, new_h), Image.BICUBIC)

    # ----- Step 2: Center crop to 256x384 -----
    left = (new_w - TARGET_W) // 2
    top = (new_h - TARGET_H) // 2
    cropped = resized.crop((left, top, left + TARGET_W, top + TARGET_H))

    # ----- Step 3: Split -----
    top_img = cropped.crop((0, 0, TARGET_W, VISIBLE_H))
    bottom_img = cropped.crop((0, VISIBLE_H, TARGET_W, TARGET_H))

    # ----- Step 4: Create 256x256 canvases -----
    top_canvas = Image.new("RGB", (CANVAS_SIZE, CANVAS_SIZE), MAGENTA)
    bottom_canvas = Image.new("RGB", (CANVAS_SIZE, CANVAS_SIZE), MAGENTA)

    top_canvas.paste(top_img, (0, 0))
    bottom_canvas.paste(bottom_img, (0, 0))

    # ----- Step 5: Save -----
    top_canvas.save("nds_top.png")
    bottom_canvas.save("nds_bottom.png")

    print("Saved:")
    print(" - nds_top.png (256x256, bottom padded)")
    print(" - nds_bottom.png (256x256, bottom padded)")

def main():
    if len(sys.argv) != 2:
        print("Usage: python nds_bg_split.py <image.png|image.jpg>")
        sys.exit(1)

    resize_and_split(sys.argv[1])

if __name__ == "__main__":
    main()

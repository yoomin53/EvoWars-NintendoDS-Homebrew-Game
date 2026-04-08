#!/usr/bin/env python3
#Copyright (c) 2026 Jung, Yoomin. All rights reserved.
import sys
from PIL import Image

def crop_and_split(image_path):
    # Load image
    img = Image.open(image_path)

    # ===== STEP 1: Crop center to 192×192 =====
    w, h = img.size
    crop_w, crop_h = 192, 192

    left = (w - crop_w) // 2
    top = (h - crop_h) // 2
    right = left + crop_w
    bottom = top + crop_h

    cropped = img.crop((left, top, right, bottom))
    cropped.save("cropped_192.png")
    print("Saved cropped_192.png")

    # ===== STEP 2: Split to 9 x 64×64 =====
    tile_size = 64
    index = 0

    for row in range(3):
        for col in range(3):
            x1 = col * tile_size
            y1 = row * tile_size
            x2 = x1 + tile_size
            y2 = y1 + tile_size

            tile = cropped.crop((x1, y1, x2, y2))
            filename = f"tile_{index}.png"
            tile.save(filename)
            print("Saved", filename)
            index += 1

    print("Done! 9 tiles created.")

def main():
    if len(sys.argv) != 2:
        print("Usage: python split_sprite.py <image_path>")
        sys.exit(1)

    image_path = sys.argv[1]
    crop_and_split(image_path)

if __name__ == "__main__":
    main()

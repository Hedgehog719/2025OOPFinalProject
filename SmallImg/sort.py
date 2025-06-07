import os
import json
from PIL import Image
import numpy as np
from collections import defaultdict
from tqdm import tqdm

INPUT_DIR = "output_gray"
OUTPUT_IMG_DIR = "renamed_gray"
OUTPUT_JSON_DIR = "output_json_renamed"

os.makedirs(OUTPUT_IMG_DIR, exist_ok=True)
os.makedirs(OUTPUT_JSON_DIR, exist_ok=True)

def compute_histogram(img):
    array = np.array(img).flatten()
    hist, _ = np.histogram(array, bins=16, range=(0, 256), density=True)
    return hist.tolist()

def compute_brightness(img):
    return int(np.mean(img))

# 準備資料結構
class_histograms = defaultdict(dict)
class_counters = defaultdict(lambda: defaultdict(int))  # class -> brightness -> count

# 遍歷每個類別資料夾
for class_name in os.listdir(INPUT_DIR):
    class_path = os.path.join(INPUT_DIR, class_name)
    if not os.path.isdir(class_path):
        continue

    # 建立輸出資料夾
    out_class_dir = os.path.join(OUTPUT_IMG_DIR, class_name)
    os.makedirs(out_class_dir, exist_ok=True)

    # 遍歷圖片
    for filename in tqdm(os.listdir(class_path), desc=f"處理中: {class_name}"):
        if not filename.lower().endswith(".png"):
            continue

        image_path = os.path.join(class_path, filename)
        try:
            img = Image.open(image_path).convert("L")
            brightness = compute_brightness(img)
            hist = compute_histogram(img)

            count = class_counters[class_name][brightness]
            new_name = f"{brightness:03d}_{count:04d}.png"
            new_path = os.path.join(out_class_dir, new_name)
            img.save(new_path)

            class_histograms[class_name][f"{class_name}/{new_name}"] = hist
            class_counters[class_name][brightness] += 1

        except Exception as e:
            print(f"⚠️ Error: {filename} skipped due to: {e}")

# 輸出 JSON
for class_name, data in class_histograms.items():
    out_json_path = os.path.join(OUTPUT_JSON_DIR, f"{class_name}.json")
    with open(out_json_path, "w", encoding="utf-8") as f:
        json.dump(data, f, indent=2)

print("✅ 完成：所有圖片重新命名、JSON 也已更新")

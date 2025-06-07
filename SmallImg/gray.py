import os
import pickle
import numpy as np
from PIL import Image
import json

# 類別語意對照
labels_text = ['airplane', 'automobile', 'bird', 'cat', 'deer',
               'dog', 'frog', 'horse', 'ship', 'truck']

def unpickle(file):
    with open(file, 'rb') as fo:
        return pickle.load(fo, encoding='bytes')

def compute_gray_histogram(img, bins=16):
    arr = np.array(img).flatten()
    hist, _ = np.histogram(arr, bins=bins, range=(0, 256))
    hist = hist.astype(np.float32)
    return (hist / hist.sum()).tolist()

def process_batch(batch_file, output_dir, features_dict, index_offset=0):
    batch = unpickle(batch_file)
    data = batch[b'data']
    labels = batch[b'labels']
    filenames = batch[b'filenames']

    for i in range(len(data)):
        img_rgb = data[i].reshape(3, 32, 32).transpose(1, 2, 0)
        img = Image.fromarray(img_rgb).convert("L")  # to grayscale

        label_index = labels[i]
        label_name = labels_text[label_index]
        label_dir = os.path.join(output_dir, label_name)
        os.makedirs(label_dir, exist_ok=True)

        filename = f"{index_offset + i}.png"
        full_path = os.path.join(label_dir, filename)
        img.save(full_path)

        relative_path = f"{label_name}/{filename}"
        hist = compute_gray_histogram(img)
        features_dict[relative_path] = hist

def main():
    output_dir = "output_gray"
    os.makedirs(output_dir, exist_ok=True)

    features = {}

    for batch_idx in range(1, 6):
        batch_file = f"data_batch_{batch_idx}"
        process_batch(batch_file, output_dir, features, index_offset=(batch_idx - 1) * 10000)

    process_batch("test_batch", output_dir, features, index_offset=50000)

    with open("image_features.json", "w") as f:
        json.dump(features, f, indent=2)

if __name__ == "__main__":
    main()

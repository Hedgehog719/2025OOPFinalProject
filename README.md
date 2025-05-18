# 2024 OOP Final Project

## 專案介紹

本專案是 2024 第二學期 OOP 期末專題，由 **Hedgehog719** 與 **Henry071-ee** 共同開發，目標是建立一個影像處理工具。專案會處理 `Image-Folder/` 中的影像檔案，支援多種濾鏡效果、PNG LSB 解密、隱寫術解密，以及 RSA/Caesar/XOR 的二重加密功能。此外，專案還能夠在終端機中顯示彩色圖片。

## 功能說明

1. **影像處理功能**

   * 支援灰階 (gray) 與 RGB (rgb) 影像格式
   * 八種不同濾鏡效果：包括增加亮度、色彩反轉、降噪、色度門檻處理、銳化、馬賽克、水平翻轉、魚眼效果

2. **隱寫術 (Steganography)**

   * 支援 PNG LSB 解密 (pngLSB decrypt)
   * 支援 LSB 解密 (LSB decrypt)

3. **訊息加解密**

   * 支援 RSA、Caesar、XOR 的二重加密
   * 隱寫術支援將加密後的訊息寫入圖片中

4. **影像顯示**

   * 可於終端機透過 `catimg` 顯示彩色圖片
   * 支援 ASCII 藝術化顯示

## 安裝與執行

1. 安裝所需套件

   ```sh
   make install
   ```

   * 這會自動安裝 `CImg`, `libjpeg`, `catimg`。

2. 建立資料夾並放置圖片

   * 確認專案目錄下有 `Image-Folder/` 資料夾，若無則手動建立，並將圖片放置其中。
   * 或是使用專案內建範例圖片。

3. 編譯專案

   ```sh
   make
   ```
   or
   ```sh
   make -j
   ```

5. 執行專案

   ```sh
   ./Image-Processing
   ```

## 程式架構

```
├── Image-Folder/         # 影像資料夾

```

## 使用方法

1. 影像讀取與顯示

   * 透過 `LoadImage()` 函數讀取影像
   * 使用 `Display_ASCII()` 可以在終端機上顯示影像(大圖形建議縮小視窗字元大小)
   * 使用 `Display_X_Server()` 可以在MobaXterm跳出視窗顯示圖片
   * 灰階圖片使用 `Disp_small()` 顯示最常邊=40pixels的縮圖

2. 濾鏡應用

   * 使用 `ApplyAllFilters(uint8_t options)` 函數可以對影像進行八種不同濾鏡處理，內部以bitwise or 判斷要套用的濾鏡效果

3. 隱寫術操作

   * 使用 `encrypt()` 與 `decrypt()` 進行隱寫術操作
   * 如果圖片大小不足會顯示
   ```
   Encrypt Failed. SOmething bad happened. Maybe message is too long.\nMessage may not be fully stored.
   ```
   需要更大解析度圖片或更短訊息。

4. 訊息加解密

   * 支援 RSA、Caesar、XOR 的訊息二重加密，
   

## 操作方法

## 作者

* **Hedgehog719**
* **Henry071-ee**
* **OOP助教群和開源程式碼作者**

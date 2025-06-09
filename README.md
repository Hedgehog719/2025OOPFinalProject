# 2025 OOP Final Project

## 專案介紹

本專案是 2024 第二學期 OOP 期末專題，由 **Hedgehog719** 與 **Henry071-ee** 共同開發，目標是建立一個影像處理工具。專案會處理 `Image-Folder/` 中的影像檔案，支援多種濾鏡效果、PNG LSB 解密、隱寫術解密，以及 RSA/Caesar/XOR 的二重加密功能。此外，專案還能夠在終端機中顯示彩色圖片。

## 功能說明

1. **影像處理功能**

   * 支援灰階 (gray) 與 RGB (rgb) 影像格式
   * 八種不同濾鏡效果：包括增加亮度、色彩反轉、降噪、色度門檻處理、銳化、馬賽克、水平翻轉、**魚眼**效果
   * 灰階小圖組大圖功能：以十種分類("airplane","automobile","bird","cat","deer","dog","frog","horse","ship","truck")中每個分類6000張32*32的圖片以灰階亮度與結構判斷比較，結合成新的圖片。

2. **隱寫 (Steganography)**

   * 支援 PNG LSB 加解密
   * 支援 PNG DCT 加解密

3. **訊息加解密**

   * 支援 RSA、Caesar、XOR 的二重加密
   * 隱寫術支援將加密後的訊息寫入圖片中

4. **影像顯示**

   * 支援以終端機透過 `data_loader` 顯示圖片
   * 支援 ASCII 終端機化顯示縮圖或原始圖片

## 安裝與執行

1. 安裝所需套件

   ```sh
   make install
   ```

   * 這會自動安裝 `CImg`, `libjpeg`, `catimg`。

2. 建立資料夾並放置圖片

   * 確認專案目錄下有 `Image-Folder/` 資料夾，若無則需手動建立，並將圖片放置其中。
   * 或是使用專案內建範例圖片。

3. 編譯專案

   ```sh
   make all
   ```
   or 加速
   ```sh
   make -j
   ```
   or
   ```sh
   make
   ```

5. 執行專案

   ```sh
   ./Image_Processing
   ```

## 函數介紹

1. 影像讀取與顯示

   * 透過 `LoadImage()` 函數讀取影像
   * 使用 `Display_ASCII()` 可以在終端機上顯示影像(大圖形建議縮小視窗字元大小)
   * 使用 `Display_X_Server()` 可以在MobaXterm跳出視窗顯示圖片
   * 使用 `Disp_small()` 在終端機顯示符合視窗大小的縮圖

2. 濾鏡

   * 使用 `ApplyAllFilters(uint8_t options)` 函數可以對影像進行八種不同濾鏡處理，內部以bitwise or 判斷要套用的濾鏡效果

3. 隱寫術操作

   * 使用 `EncryptMessage()` 與 `DecryptMessage()` 呼叫 `ImageEncryption::EncodeMessage(string message)` `string ImageEncryption::DecodeMessage(pixels,w,h)` 進行隱寫術操作
   * 如果圖片大小不足會顯示
   ```
   Encrypt Failed. SOmething bad happened. Maybe message is too long.\nMessage may not be fully stored.
   ```
   需要更大解析度圖片或更短訊息。

4. 訊息加解密
   * 在main()中使用，支援 RSA、Caesar、XOR 的訊息二重加密。
   

## 操作方法
1. 簡介
   程式執行是一個迴圈，每次迴圈執行順序是：
      選擇圖片 -> RGB/Gray -> Filter -> Encrypt -> Decrypt。
      ex.執行時若要解密圖片便要起始選擇圖片 -> rgb/gray -> filter選擇n -> Encrypt選擇n -> Decrypt選擇y。

2. 詳細步驟
   執行
   ```sh
      ./Image_Processing
   ```
   會顯示歡迎畫面酷酷小龍龍：
   ```sh
      ''''''''''''''''''''''''''''''''''''''''''''''
                     ______________
               ,===:'.,            `-._
                  `:.`---.__         `-._
                     `:.     `--.         `.
                        \.        `.         `.
               (,,(,    \.         `.   ____,-`.,
            (,'     `/   \.   ,--.___`.'
         ,  ,'  ,--.  `,   \.;'         `
         `{o, {    \  :    \;
            V,,'    /  /    //
            |;;    /  ,' ,-//.    ,---.      ,
            \;'   /  ,' /  _  \  /  _  \   ,'/
                  \   `'  / \  `'  / \  `.' /
                  `.___,'   `.__,'   `.__,'

      ''''''''''''''''''''''''''''''''''''''''''''''
      0. Show All Small Images
      1. Image-Folder/3-2.jpg
      ...
      Pease enter the number of image file or 0 for show all: 
   ```

   後面會出現在圖片編號名稱，使用者輸入要進行操作的圖片**編號**或**0**以縮圖顯示所有圖片預覽再次進行選擇 或取消操作

   接下來可以選擇要處裡灰階圖片或是RGB圖片：
   ```sh
      ========================
      = CHOOSE YOUR IMG TYPE =
      ========================
      1. Gray Image
      2. RGB Image
      0. Exit
      ========================
      Please enter your choice:
   ```

   選擇後可以決定是否要顯示縮圖預覽，再決定使否要使用濾鏡。
   ```sh
      ==============================
      Display small img? (y/n)
      ==============================
      y
      ****************************++++++==++++++++++++==++********************
      ****************++++++++++++++++====++++++****++++++==++****************
      **************++++++++++++++++====++++++**##++++++++++==++++++**********
      ++********++++++++++++++++++====++++++++##%%++++++++++++++==++++++******
      ++==++++++++++++++++++++++====++++++++**####==++++++**++++++====++++**++
      **++==++++++++++++++++++====++++++++++**####==++++++++++++++++++==++++==
      ****++====++++++++++++====++++++++++++**####++++++**++++++++++++++++--++
      **++++++====++++++++====++++++**####**######**++++++++++++++++++++==++==
      ++++++++++++--++++====**####%%%%%%%%%%%%%%****++++++++++++++++++==++****
      ++++++++++++++====++%%@@%%%%%%%%@@@@@@%%%%##++++++++++++++++++==++******
      ++++++++++++++++**@@@@@@@@%%%%@@@@@@%%%%%%##++====++++++++++==++********
      ++++++++++++**%%@@@@@@@@@@@@%%@@@@@@%%%%%%##**##----==++++++==++********
      ++++++++++==++%%@@@@@@@@@@@@%%@@@@%%##%%%%%%##%%++----==++==++++++++++**
      ++++++++======%%@@%%%%%%@@%%%%%%%%##--::++##%%##%%----====++++++++++****
      ++++++====++**@@%%++++%%@@@@@@%%%%**....++######%%++--====++++++++******
      ++++====++++%%%%**....++%%@@%%%%%%%%##**############++--==++++++++****++
      ++====++++++%%%%%%**##%%@@%%%%##%%%%##############%%====--++++++++++++**
      ====++++++++%%%%@@%%%%%%%%%%****##%%######**##**##%%++----++++++++++++++
      ==++++++++++##%%%%%%%%%%@@##====**########******####**--==++++++++++++++
      ++++++++++++**%%%%%%@@@@%%==------++####++++++**######==++--==++++++++++
      ++++++++++++++%%####%%@@++========::****==++++**######====--==++++++++++
      ++++++++++++**##******%%==------....======++++**####++==----====++++++++
      ++++++++++++++####**++%%--..........--::++++++++##**----==--====++++++++
      ++++++++++++++**##******==....    ..::..++++++**##++::::::--======++++++
      ++++++++++++++++##****==----::..::::::..++==++****--::::..----====++++++
      ++++++++++++++++++****++..----::::::::::==++++**--::----::::--====++++++
      ++++++++++++++++##==**++::----::::::::====++++--....==--::--::--====++++
      ++++++++++++++**##++--==--==----::----==++==--::::--==--::--------====++
      --==++++++++==****----::::==--------::--::....--++======::----====--==++
      ++--++++++++++++--..==++--::::--::::..::....::++++==----::----==========
      ++++====++++++--::====++==--::..  ..::....  --++==--::::::----==========
      ++++++====++++++========++==--::::::........--++==..::::------==========
      ++++++++====++++++====--++==--::::::::......==++++::::::------==========
      ++++++++++====++====::--++++----::::::::..::==**++::::--------========++
      ++++++++++++======----==++==--::::::::::..==++**++::----------========++
      ++++++++++++++==----==++**++==::::::::::..--++**++::----------==========
      ++++++++++++++==----==++**++--::::::::::::--++==::------------==========
      ++++++++++++==--++==--##%%**::::::::::::::::==--------------==========--
      ++++++++++==--++++++==**%%**::::--------------------------==============
      ++++++++++--++++++++++****--::----------------------------==========--++
      ++++++++--++++++++++++==--::----------------------------==========--==++
      ++++++--==++++++++++++++====------------------------============--==++++
      Do you want to use filter? (y/n)
   ```
   此處因為README無法顯示背景顏色因此只能以ASCII顯示，實際如圖：。
   ![image](https://github.com/Hedgehog719/2024OOPFinalProject/blob/main/scr1.png)

   接下來選擇濾鏡 分別選擇y/n最後會顯示image size和選擇的結果：
   ```sh
      Do you want to use filter? (y/n) y

      ======================
      = DECIDE YOUR FILTER =
      ======================
      1.Brightening?(y/n) y
      2.Inversion?(y/n) n
      3.Denoising?(y/n) y
      4.Thresholding?(y/n) n
      5.Sharpening?(y/n) n
      6.Mosaic?(y/n) n
      7.Flip Horizontal?(y/n) y
      8.Fish Eye?(y/n) y

      RGB image: 360x425
      your options:11000101
      ==============================
      Do you want to display img?
      ------------------------------
      0.None
      1.ASCII
      2.X_Server
      3.Both
      4. Preview
      ==============================
      4

   ```
   ![image](https://github.com/Hedgehog719/2024OOPFinalProject/blob/main/scr2.png)

   在濾鏡選擇後可以進行小圖組大圖功能：
   ```sh
      ======================
      = SMALL IMAGE MOSAIC =
      ======================
      DO YOU WANT TO USE SMALL IMAGE MOSAIC???????????? (y/n) y
      === Mosaic using Small Images ===
      Choose a collection:
      1. airplane
      2. automobile
      3. bird
      4. cat
      5. deer
      6. dog
      7. frog
      8. horse
      9. ship
      10. truck
      2
      Loading valid images from database...
      Loaded valid_db size: 6000
      ==============================
       Do you want to display img?
      ------------------------------
      0. None
      1. ASCII
      2. X_Server
      3. Both
      4. Preview(not recommended here)
      ==============================
      2
      Image showed. make sure to close it before next move.
      Enter Image-Folder/"filename".png, default imgg.png or "n" for not saving: cnliu2gray_new
   ```
   可以選擇小圖分類後展示與儲存圖片，注意圖片越大程式執行時間越久，圖片解析度越高效果越明顯。

   或是這張RGB：
   ![image](https://github.com/Hedgehog719/2024OOPFinalProject/blob/main/Image-Folder/bz.jpg)


   和：
   ![image](https://github.com/Hedgehog719/2024OOPFinalProject/blob/main/Image-Folder/bzt1.png)

   
   接下來可以選擇是否要儲存檔案以及後續加密 
   ```sh
      Enter Image-Folder/"filename".png, default imgg.png or "n" for not saving: n

      ======================
      = ENCRYPT YOUR IMAGE =
      ======================

      Encrypt Filtered Image(y)? Original Image(o)? Exit(e)? Skip(n)?(y|o|e|n)
   ```
   可以選擇加密原始圖片(o 未濾鏡)或濾鏡後圖片(y)或是結束此次圖片編輯繼續下次回圈，或是跳過加密過程進入解密圖片過程。

   如果繼續就可以輸入加密資料、選擇加密方式：
   圖片加密有兩種方式: 1. LSB 2. DCTEncrypt
   字串加密有三種方式: 1.RSA 2.XOR 3.CaesarChiper
   依照自己所需選擇
   ```sh
      ======================
      = ENCRYPT YOUR IMAGE =
      ======================

      Encrypt Filtered Image(y)? Original Image(o)? Exit(e)? Skip(n)?(y|o|e|n)
      y
      Enter message to encrypt: This is the secret.
      ====================================
      Choose Your String Encrypt Type:
      ------------------------------------
      1.RSA
      2.XOR
      3.CaesarChiper
      4.None
      ====================================
      3
      Do Caesar Chiper on your message...
      Caesar shift initial number?: 1
      Choose your Img hidden method
      1.  LSB
      2. DCTEncrypt
      :2
      Encrpting Image: Image-Folder/3-2.jpg
      with message: Uijt jt uif tfdsfu.
      by method DCT
      success Encrypt!!!
      Message encrypted into filtered image.
      Enter Image-Folder/"filename".png, default imgg.png or "n" for not saving:
   ```
   可以選擇儲存檔案名稱，以便後續解密。
   注意此處若選擇RSA加密，請使用者盡量使用程式推薦的質數(實作展示用)，以免加密後長度超過上限或是無法儲存所有資料。

   同加密方法
   圖片加密有兩種方式: 1.  LSB 2. DCTEncrypt
   字串加密有三種方式: 1.RSA 2.XOR 3.CaesarChiper
   依照自己所需選擇

   解密部分：
   ```sh
      Enter Image-Folder/"filename".png, default imgg.png or "n" for not saving: test
      ==============================
      Do you want to display img?
      ------------------------------
      0. None
      1. ASCII
      2. X_Server
      3. Both
      4. Preview
      ==============================
      0
      =====================
      = DECODE YOUR IMAGE =
      =====================

      Decode Image(y)? Exit(e)? Skip(s)?(y|e|s)
      y
      
      Choose your Img hidden method
      1. LSB
      2. DCTEncrypt
      :2
      Decoded message:
      Uijt jt uif tfdsfu.
      ==========================================
      Choose Your String Encrypt Type(if any):
      ------------------------------------
      1.RSA
      2.XOR
      3.CaesarChiper
      4.None
      ==========================================
      3
      Do deCaesar Chiper on your message...
      Caesar shift initial number?: 1
      Decrypt message:This is the secret.
   ```
   可以看到輸入的caesar位移量是1，"This is the secret" 變成 "Uijt jt uif tfdsfu." ，需要使用者自行記憶shifted number以便解密。

   接下來按下enter便可以進行下次回圈繼續操作。

## 檔案結構
   ```sh
   tree -L 3
.
├── 111511164.tar
└── 2025_NYCU_OOPFP_Image_Processing
    ├── Data-Loader
    │   ├── data_loader.cpp
    │   └── data_loader.h
    ├── githublink.txt
    ├── Image-Folder
    │   ├── 1-1.jpg
    │   ├── 1-2.jpg
    │   ├── 2-1.jpg
    │   ├── 2-2.jpg
    │   ├── 23444.png
    │   ├── 3-1.jpg
    │   ├── 3-2.jpg
    │   ├── 4-1.jpg
    │   ├── 4-2.jpg
    │   ├── cnliu2gray.png
    │   ├── cnliu2.jpg
    │   ├── cnliu.jpg
    │   ├── en1.png
    │   ├── GreatBySmall.png
    │   ├── GreatView.jpg
    │   ├── lena.jpg
    │   ├── liu_small.png
    │   ├── n.png
    │   ├── or1.png
    │   ├── \003.png
    │   ├── sailboat.jpg
    │   ├── test.png
    │   ├── truck.png
    │   └── 下載.jpg
    ├── Image_Processing
    ├── inc
    │   ├── bit_field_filter.h
    │   ├── gray_image.h
    │   ├── image_encryption.h
    │   ├── image.h
    │   └── rgb_image.h
    ├── LICENSE
    ├── main.cpp
    ├── Makefile
    ├── obj
    │   ├── bit_field_filter.d
    │   ├── bit_field_filter.o
    │   ├── data_loader.d
    │   ├── data_loader.o
    │   ├── gray_image.d
    │   ├── gray_image.o
    │   ├── image.d
    │   ├── image_encryption.d
    │   ├── image_encryption.o
    │   ├── image.o
    │   ├── rgb_image.d
    │   └── rgb_image.o
    ├── README.md
    ├── saveimg
    │   ├── 123.png
    │   ├── b1en.png
    │   ├── b1.png
    │   ├── bosn.png
    │   ├── en1.png
    │   ├── en22.png
    │   ├── enbird.png
    │   └── n.png
    ├── scr1.png
    ├── scr2.png
    ├── scripts
    │   └── clone_env.sh
    ├── SmallImg
    │   ├── gray.py
    │   ├── gray_small
    │   ├── out_json
    │   └── sort.py
    ├── src
    │   ├── bit_field_filter.cpp
    │   ├── gray_image.cpp
    │   ├── image.cpp
    │   ├── image_encryption.cpp
    │   └── rgb_image.cpp
    ├── third-party
    │   ├── catimg
    │   ├── CImg
    │   └── libjpeg
    ├── vgcore.2185
    └── vgcore.32459

15 directories, 70 files

   ```
   
## 已知狀況
   * 使用解析度太高的圖片時會有Corrupt問題，但程式應該可以正常執行，只是可能要等很久(已測試200M Pixels圖片會有錯誤但可行)。

## 作者

* **Hedgehog719** 
* **Henry071-ee**
* **OOP助教群和開源程式碼作者**

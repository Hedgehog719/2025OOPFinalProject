#include "image_encryption.h"
#include <iostream>
bool ImageEncryption::EncodeMessage(int **pixels, int w, int h, const std::string &message) {
    int totalBits = w * h;
    int requiredBits = message.length() * 8;

    if (requiredBits > totalBits) return false; // 不夠空間存訊息

    // ?? 清除整張圖所有像素的最低三位元
    for (int i = 0; i < h; ++i) {
        for (int j = 0; j < w; ++j) {
            pixels[i][j] = pixels[i][j] & 0b11111110;
        }
    }

    // ?? 開始將訊息 bit 寫入每個像素的第三位元（bit 0）
    int bitIndex = 0;
    for (char ch : message) {
        for (int b = 7; b >= 0; --b) {
            int row = bitIndex / w;
            int col = bitIndex % w;
            if (row >= h) return false;

            int bit = (ch >> b) & 1;
            pixels[row][col] |= (bit << 0); // 寫入第三位元（bit 2）
            ++bitIndex;
        }
    }

    return true;
}

std::string ImageEncryption::DecodeMessage(int **pixels, int w, int h) {
    int totalBits = w * h;
    std::string result = "";
    char ch = 0;
    int bitCount = 0;
    int length=-1;
    for (int i = 0; i < totalBits; ++i) {
        int row = i / w;
        int col = i % w;

        // 取第三位元（bit 2）
        int bit = (pixels[row][col] >> 0) & 1;

        // 印出該位元（二進位格式）
        //std::cout << bit;

        ch = (ch << 1) | bit;
        ++bitCount;

        if (bitCount == 8) {
            if(length==-1){
                length=ch;
                ch = 0;
                bitCount = 0;
                continue;
            }
            length--;
            if (length == 0) {

                ch = 0;  // Reset the char to avoid adding empty characters
                break;
            } else {
                result += ch;
            }
            ch = 0;
            bitCount = 0;
            //std::cout << " "; // 每 8 位分隔一次
        }
    }

    //std::cout << std::endl; // 換行結束
    return result;
}

bool ImageEncryption::EncodeMessage(int ***pixels, int w, int h, const std::string &message) {
    int totalBits = w * h * 3;  // 每個 pixel 有 3 個通道可寫入 1 bit
    int requiredBits = message.length() * 8;

    if (requiredBits > totalBits) return false; // 空間不足

    // 清除每個通道最低三位元，並強制最低位為 1（根據你原始邏輯）
    for (int i = 0; i < h; ++i) {
        for (int j = 0; j < w; ++j) {
            for (int c = 0; c < 3; ++c) {
                pixels[i][j][c] = pixels[i][j][c] & 0b11111110;
            }
        }
    }

    // 寫入 message 到 bit 2
    int bitIndex = 0;
    for (char ch : message) {
        for (int b = 7; b >= 0; --b) {
            int pixelNum = bitIndex / 3;
            int channel = bitIndex % 3;
            int i = pixelNum / w;
            int j = pixelNum % w;

            if (i >= h) return false; // 防止越界

            int bit = (ch >> b) & 1;
            pixels[i][j][channel] |= (bit << 0); // 寫入 bit 2

            ++bitIndex;
        }
    }

    return true;
}


std::string ImageEncryption::DecodeMessage(int ***pixels, int w, int h) {
    std::string result = "";
    char ch = 0;
    int bitCount = 0;
    int length=-1;
    for (int i = 0; i < h; ++i) {
        for (int j = 0; j < w; ++j) {
            for (int c = 0; c < 3; ++c) {
                int bit = (pixels[i][j][c] >> 0) & 1;

                ch = (ch << 1) | bit;
                ++bitCount;

                if (bitCount == 8) {
                    if(length==-1){
                        length=(int)ch;
                        ch = 0;
                        bitCount = 0;
                        continue;
                    }
                    length--;
                    
                    result += ch;
                    //std::cout<<ch<<endl;
                    ch = 0;
                    bitCount = 0;
                    if (length == 0) return result;  // 偵測到 null，結束解碼
                }
            }
        }
    }
    
    return result;
}


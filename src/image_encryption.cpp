#include "image_encryption.h"
#include <iostream>
#include <cmath>
#include <bitset>
template<typename T>
T clamp(T val, T low, T high) {
    return std::min(std::max(val, low), high);
}

// DCT block size
const int BLOCK = 8;
const double PI = 3.141592653589793;

// Forward DCT
void dct8x8(int input[BLOCK][BLOCK], double output[BLOCK][BLOCK]) {
    for (int u = 0; u < BLOCK; ++u) {
        for (int v = 0; v < BLOCK; ++v) {
            double sum = 0.0;
            for (int x = 0; x < BLOCK; ++x) {
                for (int y = 0; y < BLOCK; ++y) {
                    sum += input[x][y] *
                           cos((2 * x + 1) * u * PI / 16) *
                           cos((2 * y + 1) * v * PI / 16);
                }
            }
            double cu = (u == 0) ? sqrt(0.5) : 1.0;
            double cv = (v == 0) ? sqrt(0.5) : 1.0;
            output[u][v] = 0.25 * cu * cv * sum;
        }
    }
}

// Inverse DCT
void idct8x8(double input[BLOCK][BLOCK], int output[BLOCK][BLOCK]) {
    for (int x = 0; x < BLOCK; ++x) {
        for (int y = 0; y < BLOCK; ++y) {
            double sum = 0.0;
            for (int u = 0; u < BLOCK; ++u) {
                for (int v = 0; v < BLOCK; ++v) {
                    double cu = (u == 0) ? sqrt(0.5) : 1.0;
                    double cv = (v == 0) ? sqrt(0.5) : 1.0;
                    sum += cu * cv * input[u][v] *
                           cos((2 * x + 1) * u * PI / 16) *
                           cos((2 * y + 1) * v * PI / 16);
                }
            }
            output[x][y] = std::min(255, std::max(0, (int)(0.25 * sum)));
        }
    }
}

// 嵌入一個字元到 DCT block
static const int COEFFICIENT_INDICES[8][2] = {
    // 推薦的中頻係數（常見於 JPEG 壓縮中的較不重要但非零的係數）
    {1, 1}, // 位於 (0,0) 斜下方的第二個係數
    {0, 2}, // 次低頻，水平方向
    {2, 0}, // 次低頻，垂直方向
    {2, 1}, // 再往外一些
    {1, 2}, // 再往外一些
    {3, 0}, // 更低頻一些，但仍承載部分結構
    {0, 3}, // 更低頻一些，但仍承載部分結構
    {2, 2}  // 典型的中頻係數
    // 其他可能的選擇：
    // {1,3}, {3,1}, {0,4}, {4,0} 等
};
long long force_closest_LSD(double original_val, int target_LSD) {
    long long rounded_val = static_cast<long long>(std::floor(original_val + 0.5));


    long long best_candidate ;

    best_candidate=(rounded_val/10)*10+target_LSD;

    return best_candidate;
}


// --- Embed Character into DCT Block ---
void embed_char(double dct[BLOCK][BLOCK], char c) {
    std::bitset<8> bits(c); // Convert char to 8 bits
    //std::cout << "--- Embed Char: '" << c << "' (ASCII: " << (int)c << ", Binary: " << bits.to_string() << ") ---" << std::endl;

    for (int i = 0; i < 8; ++i) {
        int row_coeff = COEFFICIENT_INDICES[i][0]; // Use predefined coordinates
        int col_coeff = COEFFICIENT_INDICES[i][1]; // Use predefined coordinates

        if (row_coeff >= BLOCK || col_coeff >= BLOCK) {
            std::cerr << "Error: Coefficient index out of bounds in embed_char at COEFFICIENT_INDICES[" << i << "]!" << std::endl;
            break;
        }

        double current_dct_val = dct[row_coeff][col_coeff];

        int target_bit = bits[7 - i]; // bits[7-i] for MSB first

        // Determine the target LSD (0 for bit 0, 5 for bit 1)
        int target_LSD = (target_bit == 0) ? 0 : 5;

        // Force the closest integer value that meets the LSD criteria
        long long new_val_int = force_closest_LSD(current_dct_val, target_LSD);

        // Assign the modified integer value back to the DCT coefficient as double
        dct[row_coeff][col_coeff] = static_cast<double>(new_val_int);

        // --- Debug Output for Embedding ---
        /*std::cout << "  Bit " << (7 - i) << " (MSB index): Target=" << target_bit
                  << ", Coeff(" << row_coeff << "," << col_coeff << ")"
                  << ", Original_DCT_val=" << current_dct_val
                  << ", New_Rounded_val=" << new_val_int
                  << ", New_DCT_val=" << dct[row_coeff][col_coeff]
                  << ", LastDigitModified=" << (std::abs(new_val_int) % 10) << std::endl;*/
    }
    //std::cout << "--- End Embed ---" << std::endl << std::endl;
}

// --- Extract Character from DCT Block ---
char extract_char(double dct[BLOCK][BLOCK]) {
    std::bitset<8> bits;
    //std::cout << "--- Extract Char ---" << std::endl;

    for (int i = 0; i < 8; ++i) {
        int row_coeff = COEFFICIENT_INDICES[i][0]; // Use predefined coordinates
        int col_coeff = COEFFICIENT_INDICES[i][1]; // Use predefined coordinates

        if (row_coeff >= BLOCK || col_coeff >= BLOCK) {
            std::cerr << "Error: Coefficient index out of bounds in extract_char at COEFFICIENT_INDICES[" << i << "]!" << std::endl;
            break;
        }

        double current_dct_val_extracted = dct[row_coeff][col_coeff];
        // Round the extracted DCT value to the nearest integer
        long long val_extracted_rounded = static_cast<long long>(std::floor(current_dct_val_extracted + 0.5));
        long long last_digit_extracted = (val_extracted_rounded % 10 + 10) % 10; // 確保取得正的個位數 (0-9)

        int extracted_bit;
        if (last_digit_extracted == 0 || last_digit_extracted == 1 || last_digit_extracted == 9||last_digit_extracted == 2 || last_digit_extracted == 8) {
            extracted_bit = 0; // Considered a '0'
        } else if (last_digit_extracted == 4 || last_digit_extracted == 5 || last_digit_extracted == 6||last_digit_extracted == 7 || last_digit_extracted == 3) {
            extracted_bit = 1; // Considered a '1'
        } else {
            // This indicates an unexpected value, meaning the bit was corrupted beyond the tolerance
            extracted_bit = -1; // Indicate error
            std::cerr << "Warning: Extracted coefficient (" << row_coeff << "," << col_coeff << ") has last digit "
                      << last_digit_extracted << ", which is neither 0/1/9 nor 4/5/6. Bit may be corrupted." << std::endl;
        }

        bits[7 - i] = extracted_bit; // Assign to bitset (MSB first)

        // --- Debug Output for Extraction ---
        /*std::cout << "  Bit " << (7 - i) << " (MSB index): Coeff(" << row_coeff << "," << col_coeff << ")"
                  << ", Extracted_DCT_val=" << current_dct_val_extracted
                  << ", Rounded_val=" << val_extracted_rounded
                  << ", LastDigitExtracted=" << last_digit_extracted
                  << ", Extracted_Bit=" << extracted_bit << std::endl;*/
    }
    char extracted_char = static_cast<char>(bits.to_ulong());
    //std::cout << "--- End Extract: '" << extracted_char << "' (ASCII: " << (int)extracted_char << ", Binary: " << bits.to_string() << ") ---" << std::endl << std::endl;
    return extracted_char;
}
//-----------------------------
bool ImageEncryption::EncodeMessage(int **pixels, int w, int h, const std::string &message) {
    int totalBits = w * h;
    int requiredBits = message.length() * 8;

    if (requiredBits > totalBits) return false; // 不夠空間存訊息


    for (int i = 0; i < h; ++i) {
        for (int j = 0; j < w; ++j) {
            pixels[i][j] = pixels[i][j] & 0b11111110;
        }
    }


    int bitIndex = 0;
    for (char ch : message) {
        for (int b = 7; b >= 0; --b) {
            int row = bitIndex / w;
            int col = bitIndex % w;
            if (row >= h) return false;

            int bit = (ch >> b) & 1;
            pixels[row][col] |= (bit << 0); 
            ++bitIndex;
        }
    }

    return true;
}

string ImageEncryption::DecodeMessage(int **pixels, int w, int h) {
    int totalBits = w * h;
    std::string result = "";
    char ch = 0;
    int bitCount = 0;
    int length=-1;
    for (int i = 0; i < totalBits; ++i) {
        int row = i / w;
        int col = i % w;

        int bit = (pixels[row][col] >> 0) & 1;


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


string ImageEncryption::DecodeMessage(int ***pixels, int w, int h) {
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

bool ImageEncryption::DCTEncodeMessage(int **pixels, int w, int h, const string &message) {
    int m = 0;
    int totalChars = message.size() + 1; // 多嵌入一個 '\0' 結尾符號
    int blocksAvailable = (w / BLOCK) * (h / BLOCK) ;  // 每個 8x8 區塊 * 3 個通道 (RGB)

    if (totalChars > blocksAvailable) {
        return false;
    }
    

    for (int i = 0; i <= h - BLOCK && m < totalChars; i += BLOCK) {
        for (int j = 0; j <= w - BLOCK && m < totalChars; j += BLOCK) {
            int block[BLOCK][BLOCK];
            for (int y = 0; y < BLOCK; ++y)
                for (int x = 0; x < BLOCK; ++x)
                    block[y][x] = pixels[i + y][j + x];

            double dct[BLOCK][BLOCK];
            dct8x8(block, dct);

            // 若 m < message.size()，嵌入字元；否則嵌入 '\0'
            char ch = (m < message.size()) ? message[m] : '\0';
            embed_char(dct, ch);
            m++;

            int new_block[BLOCK][BLOCK];
            idct8x8(dct, new_block);

            for (int y = 0; y < BLOCK; ++y)
                for (int x = 0; x < BLOCK; ++x)
                    pixels[i + y][j + x] = new_block[y][x];
        }
    }
    return true;
}
string ImageEncryption::DCTDecodeMessage(int **pixels, int w, int h) {
    string result;
    for (int i = 0; i <= h - BLOCK; i += BLOCK) {
        for (int j = 0; j <= w - BLOCK; j += BLOCK) {
            int block[BLOCK][BLOCK];
            for (int y = 0; y < BLOCK; ++y)
                for (int x = 0; x < BLOCK; ++x)
                    block[y][x] = pixels[i + y][j + x];

            double dct[BLOCK][BLOCK];
            dct8x8(block, dct);
            char ch = extract_char(dct);

            if (ch == '\0')  // 終止條件：讀到 null terminator 表示訊息結束
                return result;

            result += ch;
        }
    }
    return result;  // 若沒遇到 '\0'，回傳目前結果
}
//RGB

bool ImageEncryption::DCTEncodeMessage(int ***pixels, int w, int h, const string &message) {
    int m = 0;
    int totalChars = message.size() + 1;  // 包含結尾的 '\0'
    int colorChannel = 0;  // 0: R, 1: G, 2: B
    int blocksAvailable = (w / BLOCK) * (h / BLOCK) * 3;  // 每個 8x8 區塊 * 3 個通道 (RGB)

    if (totalChars > blocksAvailable) {
        return false;
    }
    for (int colorChannel = 0; colorChannel < 3; ++colorChannel) {
        for (int i = 0; i <= h - BLOCK; i += BLOCK) {
            for (int j = 0; j <= w - BLOCK; j += BLOCK) {
                int block[BLOCK][BLOCK];
                if (m >= totalChars) {
                    return true; // All characters embedded, exit successfully
                }
    
                // 擷取當前通道的 8x8 區塊
                for (int y = 0; y < BLOCK; ++y)
                    for (int x = 0; x < BLOCK; ++x)
                        block[y][x] = pixels[i + y][j + x][colorChannel];
    
                double dct[BLOCK][BLOCK];
                dct8x8(block, dct);
    
                char ch = (m < message.size()) ? message[m] : '\0';
                embed_char(dct, ch);
                m++;
    
                int new_block[BLOCK][BLOCK];
                idct8x8(dct, new_block);
    
                for (int y = 0; y < BLOCK; ++y)
                    for (int x = 0; x < BLOCK; ++x)
                        pixels[i + y][j + x][colorChannel] = clamp(new_block[y][x], 0, 255);
            }
        }
    }
    return true;
}

string ImageEncryption::DCTDecodeMessage(int ***pixels, int w, int h) {
    string result;
    int colorChannel = 0;
    for (int colorChannel = 0; colorChannel < 3; ++colorChannel) {
        for (int i = 0; i <= h - BLOCK; i += BLOCK) {
            for (int j = 0; j <= w - BLOCK; j += BLOCK) {
                int block[BLOCK][BLOCK];
    
                // 擷取對應通道的 8x8 區塊
                for (int y = 0; y < BLOCK; ++y)
                    for (int x = 0; x < BLOCK; ++x)
                        block[y][x] = pixels[i + y][j + x][colorChannel];
    
                double dct[BLOCK][BLOCK];
                dct8x8(block, dct);
    
                char ch = extract_char(dct);
                if (ch == '\0') return result;  // 偵測結尾
                result += ch;
            }
        }
    }
    return result;
}


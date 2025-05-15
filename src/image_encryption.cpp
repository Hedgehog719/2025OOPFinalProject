#include "image_encryption.h"
#include <iostream>
bool ImageEncryption::EncodeMessage(int **pixels, int w, int h, const std::string &message) {
    int totalBits = w * h;
    int requiredBits = message.length() * 8;

    if (requiredBits > totalBits) return false; // �����Ŷ��s�T��

    // ?? �M����i�ϩҦ��������̧C�T�줸
    for (int i = 0; i < h; ++i) {
        for (int j = 0; j < w; ++j) {
            pixels[i][j] = pixels[i][j] & 0b11111110;
        }
    }

    // ?? �}�l�N�T�� bit �g�J�C�ӹ������ĤT�줸�]bit 0�^
    int bitIndex = 0;
    for (char ch : message) {
        for (int b = 7; b >= 0; --b) {
            int row = bitIndex / w;
            int col = bitIndex % w;
            if (row >= h) return false;

            int bit = (ch >> b) & 1;
            pixels[row][col] |= (bit << 0); // �g�J�ĤT�줸�]bit 2�^
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

        // ���ĤT�줸�]bit 2�^
        int bit = (pixels[row][col] >> 0) & 1;

        // �L�X�Ӧ줸�]�G�i��榡�^
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
            //std::cout << " "; // �C 8 ����j�@��
        }
    }

    //std::cout << std::endl; // ���浲��
    return result;
}

bool ImageEncryption::EncodeMessage(int ***pixels, int w, int h, const std::string &message) {
    int totalBits = w * h * 3;  // �C�� pixel �� 3 �ӳq�D�i�g�J 1 bit
    int requiredBits = message.length() * 8;

    if (requiredBits > totalBits) return false; // �Ŷ�����

    // �M���C�ӳq�D�̧C�T�줸�A�ñj��̧C�쬰 1�]�ھڧA��l�޿�^
    for (int i = 0; i < h; ++i) {
        for (int j = 0; j < w; ++j) {
            for (int c = 0; c < 3; ++c) {
                pixels[i][j][c] = pixels[i][j][c] & 0b11111110;
            }
        }
    }

    // �g�J message �� bit 2
    int bitIndex = 0;
    for (char ch : message) {
        for (int b = 7; b >= 0; --b) {
            int pixelNum = bitIndex / 3;
            int channel = bitIndex % 3;
            int i = pixelNum / w;
            int j = pixelNum % w;

            if (i >= h) return false; // ����V��

            int bit = (ch >> b) & 1;
            pixels[i][j][channel] |= (bit << 0); // �g�J bit 2

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
                    if (length == 0) return result;  // ������ null�A�����ѽX
                }
            }
        }
    }
    
    return result;
}


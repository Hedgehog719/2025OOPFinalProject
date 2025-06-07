#ifndef IMAGE_ENCRYPTION_H
#define IMAGE_ENCRYPTION_H

#include <string>
using namespace std;

class ImageEncryption {
public:
    // 加密訊息到灰階圖中
    static bool EncodeMessage(int **pixels, int w, int h, const string &message);

    // 從灰階圖解碼訊息
    static string DecodeMessage(int **pixels, int w, int h);
    static bool EncodeMessage(int ***pixels, int w, int h, const string &message);

    // 從 RGB 圖像解碼出訊息
    static string DecodeMessage(int ***pixels, int w, int h);
    //DCT 加密
    static bool DCTEncodeMessage(int **pixels, int w, int h, const string &message);
    //DCT 解密
    static string DCTDecodeMessage(int **pixels, int w, int h);
    //DCT 加密RGB
    static bool DCTEncodeMessage(int ***pixels, int w, int h, const string &message);
    //DCT 解密RGB
    static string DCTDecodeMessage(int ***pixels, int w, int h);
};

#endif
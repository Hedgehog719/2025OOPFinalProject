#ifndef IMAGE_ENCRYPTION_H
#define IMAGE_ENCRYPTION_H

#include <string>
using namespace std;

class ImageEncryption {
public:
    // �[�K�T����Ƕ��Ϥ�
    static bool EncodeMessage(int **pixels, int w, int h, const string &message);

    // �q�Ƕ��ϸѽX�T��
    static string DecodeMessage(int **pixels, int w, int h);
    static bool EncodeMessage(int ***pixels, int w, int h, const string &message);

    // �q RGB �Ϲ��ѽX�X�T��
    static string DecodeMessage(int ***pixels, int w, int h);
    //DCT �[�K
    static bool DCTEncodeMessage(int **pixels, int w, int h, const string &message);
    //DCT �ѱK
    static string DCTDecodeMessage(int **pixels, int w, int h);
    //DCT �[�KRGB
    static bool DCTEncodeMessage(int ***pixels, int w, int h, const string &message);
    //DCT �ѱKRGB
    static string DCTDecodeMessage(int ***pixels, int w, int h);
};

#endif
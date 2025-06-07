// RGBImage.cpp
#include "rgb_image.h"
#include<iostream>
#include<sys/ioctl.h>
#include <string>
#include "gray_image.h"
void RGBImage::small(){
    cout<<"error: smallmosaic is not implemented for RGBImage."<<endl;
}
bool RGBImage::IsSameAs(const Image& other) const {
    const RGBImage* rgb = dynamic_cast<const RGBImage*>(&other);
    if (!rgb || rgb->w != w || rgb->h != h) return false;
    int max=0;

    for (int i = 0; i < h; ++i)
        for (int j = 0; j < w; ++j)
            for (int c = 0; c < 3; ++c)
                if (abs(pixels[i][j][c] - (rgb->pixels[i][j][c]))>2){
                    if(abs(pixels[i][j][c] - (rgb->pixels[i][j][c]))>max)
                        max=abs(pixels[i][j][c] - (rgb->pixels[i][j][c]));
                    //return false;
                }
    cout<<max<<endl;
    return true;
}
RGBImage::RGBImage() : pixels(nullptr) {}
RGBImage::RGBImage(int width, int height, int ***pixels) : Image(), pixels(pixels) {

    this->w = width;
    this->h = height;
}
void RGBImage::Disp_small() {
    int neww, newh;
    double sz;
    struct winsize ws;
    ioctl(STDOUT_FILENO, TIOCGWINSZ,&ws);

    if (w > h) {
        if (w < ws.ws_col/2) {
            Display_ASCII();  // ������
            return;
        }
        sz = w / (ws.ws_col/2.0);
        neww = w / sz;
        newh = h / sz;
    } else {
        if (h < ws.ws_row) {
            Display_ASCII();  // ������
            return;
        }
        sz = 1.0*h / ws.ws_row;
        neww = w / sz;
        newh = h / sz;
    }

    // �إ� RGB �Y�Ϲ����}�C
    int*** small_pixels = new int**[newh];
    for (int i = 0; i < newh; i++) {
        small_pixels[i] = new int*[neww];
        for (int j = 0; j < neww; j++) {
            small_pixels[i][j] = new int[3]{0, 0, 0};  // ��l�� R,G,B = 0
            int count = 0;
            for (int y = 0; y < sz; y++) {
                for (int x = 0; x < sz; x++) {
                    int src_y = i * sz + y;
                    int src_x = j * sz + x;
                    if (src_y < h && src_x < w) {
                        for (int c = 0; c < 3; c++) {
                            small_pixels[i][j][c] += pixels[src_y][src_x][c];
                        }
                        count++;
                    }
                }
            }
            if (count > 0) {
                for (int c = 0; c < 3; c++) {
                    small_pixels[i][j][c] /= count;  // �D����
                }
            }
        }
    }

    // ��� RGB �Y�ϡ]�H ASCII �m�����^
    string shades = " .:-=+*#%@";
    for (int y = 0; y < newh; y++) {
        for (int x = 0; x < neww; x++) {
            int r = small_pixels[y][x][0];
            int g = small_pixels[y][x][1];
            int b = small_pixels[y][x][2];
            int gray = (r + g + b) / 3;
            int index = (gray * shades.length()) / 256;
            if (index >= (int)shades.length()) index = shades.length() - 1;

            cout << "\033[48;2;" << r << ";" << g << ";" << b << "m"
                 << "\033[38;2;" << r << ";" << g << ";" << b << "m"
                 << shades[index] << shades[index]
                 << "\033[0m";
        }
        cout << endl;
    }

    // �M���O����
    for (int i = 0; i < newh; i++) {
        for (int j = 0; j < neww; j++) {
            delete[] small_pixels[i][j];
        }
        delete[] small_pixels[i];
    }
    delete[] small_pixels;
}


/*RGBImage& RGBImage::operator=(RGBImage &img) {
    if (this != &img) {
        // ����쥻 pixels ���O����
        if (pixels) {
            for (int i = 0; i < h; ++i) {
                for (int j = 0; j < w; ++j) {
                    delete[] pixels[i][j]; // �ĤT�h�]RGB�^
                }
                delete[] pixels[i]; // �ĤG�h
            }
            delete[] pixels; // �Ĥ@�h
        }

        w = img.w;
        h = img.h;

        // ���t�s�O����
        pixels = new int**[h];
        for (int i = 0; i < h; ++i) {
            pixels[i] = new int*[w];
            for (int j = 0; j < w; ++j) {
                pixels[i][j] = new int[3];
                for (int k = 0; k < 3; ++k) {
                    pixels[i][j][k] = img.pixels[i][j][k];
                }
            }
        }
    }
    return *this;
}*/

RGBImage* RGBImage::Clone() const {
    RGBImage* newImg = new RGBImage();  
    newImg ->w =w;
    newImg ->h = h;
    newImg->pixels = new int**[h];
    for (int i = 0; i < h; ++i) {
        newImg->pixels[i] = new int*[w];
        for (int j = 0; j < w; ++j) {
            newImg->pixels[i][j] = new int[3];
            for (int c = 0; c < 3; ++c)
                newImg->pixels[i][j][c] = pixels[i][j][c];
        }
    }
    return newImg;
}
RGBImage& RGBImage::operator=(Image& img) {
    // ���ձN img �ഫ�� RGBImage ����
    const RGBImage* rgbImg = dynamic_cast<RGBImage*>(&img);
    if (rgbImg) {  // �p�G img �T��O RGBImage ����
        if (this != rgbImg) {
            // ����쥻 pixels ���O����
            if (pixels) {
                for (int i = 0; i < h; ++i) {
                    for (int j = 0; j < w; ++j) {
                        delete[] pixels[i][j];  // ����ĤT�h�]RGB�^
                    }
                    delete[] pixels[i];  // ����ĤG�h
                }
                delete[] pixels;  // ����Ĥ@�h
            }

            // �ƻs�s���
            w = rgbImg->w;
            h = rgbImg->h;

            // ���t�s�O����
            pixels = new int**[h];
            for (int i = 0; i < h; ++i) {
                pixels[i] = new int*[w];
                for (int j = 0; j < w; ++j) {
                    pixels[i][j] = new int[3];
                    for (int k = 0; k < 3; ++k) {
                        pixels[i][j][k] = rgbImg->pixels[i][j][k];
                    }
                }
            }
        }
    }
    return *this;
}

RGBImage::~RGBImage() {
    if (pixels) {
        for (int i = 0; i < h; ++i) {
            for (int j = 0; j < w; ++j) {
                delete[] pixels[i][j];
            }
            delete[] pixels[i];
        }
        delete[] pixels;
    }
}


bool RGBImage::LoadImage(string filename) {
    pixels = data_loader.Load_RGB(filename, &w, &h);
    
    return pixels != nullptr;
}

void RGBImage::DumpImage(string filename) {
    data_loader.Dump_RGB(w, h, pixels, filename);
}

void RGBImage::Display_X_Server() {
    data_loader.Display_RGB_X_Server(w, h, pixels);
}

void RGBImage::Display_ASCII() {
    data_loader.Display_RGB_ASCII(w, h, pixels);
}

void RGBImage::Display_CMD() {
    cout << "RGB image: " << w << "x" << h <<endl;
}
void RGBImage::ApplyAllFilters(uint8_t options) {
    cout << "RGB image: " << w << "x" << h << endl;
    BitFieldFilter::ApplyAllFilters(options, pixels, w, h);
}
void RGBImage::EncryptMessage(const string &message){
    if(ImageEncryption::EncodeMessage( pixels, w,  h, message)){
        cout<<"success Encrypt!!!"<<endl;
    }else{
        cout<<"Encrypt Failed. SOmething bad happened. Maybe message is too long.\nMessage may not be fully stored."<<endl;
    } 
}
string RGBImage::DecryptMessage() const {
    string ms;
    ms=ImageEncryption::DecodeMessage( pixels,  w,  h);
    return ms;

}
void RGBImage::DCTEncryptMessage(const string &message){
    if(ImageEncryption::DCTEncodeMessage( pixels, w,  h, message)){
        cout<<"success Encrypt!!!"<<endl;
    }else{
        cout<<"Encrypt Failed. SOmething bad happened. Maybe message is too long.\nMessage may not be fully stored."<<endl;
    } 
}
string RGBImage::DCTDecryptMessage() const {
    string ms;
    ms=ImageEncryption::DCTDecodeMessage( pixels,  w,  h);
    return ms;

}
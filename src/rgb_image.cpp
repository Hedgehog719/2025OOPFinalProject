// RGBImage.cpp
#include "rgb_image.h"
#include<iostream>
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
/*RGBImage& RGBImage::operator=(RGBImage &img) {
    if (this != &img) {
        // 釋放原本 pixels 的記憶體
        if (pixels) {
            for (int i = 0; i < h; ++i) {
                for (int j = 0; j < w; ++j) {
                    delete[] pixels[i][j]; // 第三層（RGB）
                }
                delete[] pixels[i]; // 第二層
            }
            delete[] pixels; // 第一層
        }

        w = img.w;
        h = img.h;

        // 分配新記憶體
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
    // 嘗試將 img 轉換為 RGBImage 物件
    const RGBImage* rgbImg = dynamic_cast<RGBImage*>(&img);
    if (rgbImg) {  // 如果 img 確實是 RGBImage 類型
        if (this != rgbImg) {
            // 釋放原本 pixels 的記憶體
            if (pixels) {
                for (int i = 0; i < h; ++i) {
                    for (int j = 0; j < w; ++j) {
                        delete[] pixels[i][j];  // 釋放第三層（RGB）
                    }
                    delete[] pixels[i];  // 釋放第二層
                }
                delete[] pixels;  // 釋放第一層
            }

            // 複製新資料
            w = rgbImg->w;
            h = rgbImg->h;

            // 分配新記憶體
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
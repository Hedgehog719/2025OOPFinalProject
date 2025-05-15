// GrayImage.cpp
#include <bitset>
#include "gray_image.h"

bool GrayImage::IsSameAs(const Image& other) const {
    const GrayImage* otherGray = dynamic_cast<const GrayImage*>(&other);
    if (!otherGray) return false; // 型別不同

    if (w != otherGray->w || h != otherGray->h) return false;
    
    for (int i = 0; i < h; ++i) {
        for (int j = 0; j < w; ++j) {
            
            //cout << bitset<8>(pixels[i][j]) << " " << bitset<8>(otherGray->pixels[i][j]) << endl;
            if (abs(pixels[i][j] - (otherGray->pixels[i][j]))>2){
                    cout<<int((otherGray->pixels[i][j])-pixels[i][j])<<"diff"<<endl;
                    return false;
                }
        }
    }
    return true;
}
GrayImage::GrayImage() : pixels(nullptr) {}
GrayImage::GrayImage(int width, int height, int **pixels) : Image(), pixels(pixels) {
    this->w = width;
    this->h = height;
}
/*
GrayImage& GrayImage::operator=(GrayImage &img) {
    if (this != &img) {
        // 先釋放原有記憶體
        if (pixels) {
            for (int i = 0; i < h; ++i) {
                delete[] pixels[i];
            }
            delete[] pixels;
        }

        w = img.w;
        h = img.h;

        // 分配新記憶體
        pixels = new int*[h];
        
        for (int i = 0; i < h; ++i) {
            pixels[i] = new int[w];
            for (int j = 0; j < w; ++j) {
                pixels[i][j] = img.pixels[i][j];
            }
        }
    }
    return *this;
}*/


GrayImage* GrayImage::Clone() const {
    GrayImage* newImg = new GrayImage();  
    newImg ->w =w;
    newImg ->h = h;
    newImg->pixels = new int*[h];
    for (int i = 0; i < h; ++i) {
        newImg->pixels[i] = new int[w];
        for (int j = 0; j < w; ++j) {
            newImg->pixels[i][j] = pixels[i][j];
        }
    }
    return newImg;
}
GrayImage& GrayImage::operator=( Image &img) {
    const GrayImage* grayImg = dynamic_cast<GrayImage*>(&img);
    if (grayImg) { 
        if (this != grayImg) {
            // 釋放原本 pixels 的記憶體
            if (pixels) {
                for (int i = 0; i < h; ++i) {
                    delete[] pixels[i];
                }
                delete[] pixels;
            }

            // 複製新資料
            w = grayImg->w;
            h = grayImg->h;

            // 分配新記憶體
            pixels = new int*[h];
            for (int i = 0; i < h; ++i) {
                pixels[i] = new int[w];
                for (int j = 0; j < w; ++j) {
                    pixels[i][j] = grayImg->pixels[i][j];
                }
            }
        }
    }
    return *this;
}
GrayImage::~GrayImage() {
    if (pixels) {
        for (int i = 0; i < h; ++i) delete[] pixels[i];
        delete[] pixels;
    }
}

bool GrayImage::LoadImage(string filename) {
    pixels = data_loader.Load_Gray(filename, &w, &h);
    return pixels != nullptr;
}

void GrayImage::DumpImage(string filename) {
    data_loader.Dump_Gray(w, h, pixels, filename);
}

void GrayImage::Display_X_Server() {
    data_loader.Display_Gray_X_Server(w, h, pixels);
}

void GrayImage::Display_ASCII() {
    data_loader.Display_Gray_ASCII(w, h, pixels);
}

void GrayImage::Display_CMD() {
    cout << "Gray image: " << w << "x" << h << endl;
}
void GrayImage::ApplyAllFilters(uint8_t options) {
    cout << "Gray image: " << w << "x" << h << endl;
    BitFieldFilter bitFieldFilter;
    bitFieldFilter.ApplyAllFilters(options, pixels, w, h);
}
void GrayImage::EncryptMessage(const string &message){
    if(ImageEncryption::EncodeMessage( pixels, w,  h, message)){
        cout<<"success Encrypt!!!"<<endl;
    }
}
string GrayImage::DecryptMessage() const {
    string ms;
    ms=ImageEncryption::DecodeMessage( pixels,  w,  h);
    
    return ms;

}
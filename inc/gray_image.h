#ifndef _GRAY_IMAGE_H_
#define _GRAY_IMAGE_H_

#include "image.h"
using namespace std;

class GrayImage : public Image{
private:
    int **pixels;

public:
    GrayImage();
    GrayImage(int width, int height,int **pixels);
    ~GrayImage();
    GrayImage& operator=(Image &img);
    void setPixel(int x, int y, int value);
    int getPixel(int x, int y) const;
    
    GrayImage* Clone() const override ;
    bool LoadImage(string filename) override;
    void DumpImage(string filename) override;
    void Display_X_Server() override;
    void Display_ASCII() override;
    void Display_CMD() override;
    void ApplyAllFilters(uint8_t) override;
    bool IsSameAs(const Image& other)const;
    void EncryptMessage(const string &message)override;
    void Disp_small()override;
    string DecryptMessage() const override;

    
    // Add any additional methods you need

};

#endif
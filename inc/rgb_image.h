#ifndef _RGB_IMAGE_H_
#define _RGB_IMAGE_H_

#include "image.h"

class RGBImage : public Image{
private:
    int ***pixels;

public:
    RGBImage();
    RGBImage(int width, int height,int ***pixels);
    ~RGBImage();
    RGBImage& operator=(Image &img);
    RGBImage* Clone() const override ;
    bool LoadImage(string filename) override;
    void DumpImage(string filename) override;
    void Display_X_Server() override;
    void Display_ASCII() override;
    void Display_CMD() override;
    void ApplyAllFilters(uint8_t) override;
    bool IsSameAs(const Image& other) const;
    void EncryptMessage(const string &message) override;
    string DecryptMessage() const override;
};

#endif
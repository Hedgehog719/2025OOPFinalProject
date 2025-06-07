#ifndef _IMAGE_H_
#define _IMAGE_H_
#include "data_loader.h"
#include "bit_field_filter.h"
#include "image_encryption.h"
using namespace std;


class Image{
protected:
    int w;
    int h;
    static Data_Loader data_loader;    

public:
    Image() : w(0), h(0) {}
    virtual ~Image() {}

    virtual bool LoadImage(string filename) = 0;
    virtual void DumpImage(string filename) = 0;
    virtual void Display_X_Server() = 0;
    virtual void Display_ASCII() = 0;
    virtual void Display_CMD() = 0;
    virtual bool IsSameAs(const Image& other)const=0;
    virtual Image* Clone() const = 0;
    
    virtual void ApplyAllFilters(uint8_t)=0;
    
    virtual void EncryptMessage(const string &message) = 0;
    virtual string DecryptMessage() const = 0;
    virtual void DCTEncryptMessage(const string &message) = 0;
    virtual string DCTDecryptMessage() const = 0;
    
    virtual void Disp_small() =0;
    virtual void small()=0;
    int get_width() const { return w; }
    int get_height() const { return h; }

};

#endif
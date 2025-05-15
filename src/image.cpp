/*protected:
    int width;
    int height;
    Data_Loader ;    

public:
    Image() : width(0), height(0) {}
    virtual ~Image() {}

    virtual void LoadImage(const char *filename) = 0;
    virtual void DumpImage(const char *filename) = 0;
    virtual void Display_X_Server() = 0;
    virtual void Display_ASCII() = 0;
    virtual void Display_CMD() = 0;

    int get_width() const { return width; }
    int get_height() const { return height; }
    */
#include "image.h"
#include "data_loader.h"
#include <iostream>
#include <cassert>
#include <string>
#include <vector>
Data_Loader Image::data_loader;

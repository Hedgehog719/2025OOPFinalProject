#ifndef _BIT_FIELD_FILTER_H_
#define _BIT_FIELD_FILTER_H_
#include <cstdint>
#include <cmath>
#include <algorithm> // For std::min and std::max
#include <iostream>  // For error handling output (optional)

// Assuming PI is defined, or include <cmath> and use M_PI
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#define CASE_ONE    0b00000001  // �W�G�ާ@
#define CASE_TWO    0b00000010  // ����ާ@
#define CASE_THREE  0b00000100  // �����ާ@
#define CASE_FOUR   0b00001000  // ���e�ƾާ@
#define CASE_FIVE   0b00010000  // �U�ƾާ@ (Laplacian filter)
#define CASE_SIX    0b00100000  // ���ɧJ�ާ@ (Mosaic filter)
#define CASE_SEVEN  0b01000000  // ����½��
#define CASE_EIGHT  0b10000000  // ����½��
#include <iostream>
using namespace std;
class BitFieldFilter {
public:
    static void ApplyAllFilters(uint8_t options, int **pixels, int width, int height);
    static void ApplyAllFilters(uint8_t options, int ***pixels, int width, int height);
    
    
private:
    static void Brightness(int **pixels, int w, int h);
    static void Invert(int **pixels, int w, int h);
    static void Denoise(int **pixels, int w, int h);
    static void Threshold(int **pixels, int w, int h);
    static void LaplacianSharpening(int **pixels, int w, int h);  // �U�ƾާ@
    static void Mosaic(int **pixels, int w, int h, int block_size = 8);  // ���ɧJ�ާ@
    static void FlipHorizontal(int **pixels, int w, int h);
    static void FishEye(int **pixels, int w, int h);
    
    static void Brightness(int ***pixels, int w, int h);
    static void Invert(int ***pixels, int w, int h);
    static void Denoise(int ***pixels, int w, int h);
    static void Threshold(int ***pixels, int w, int h);
    static void LaplacianSharpening(int ***pixels, int w, int h);  // �U�ƾާ@
    static void Mosaic(int ***pixels, int w, int h, int block_size = 8);  // ���ɧJ�ާ@
    static void FlipHorizontal(int ***pixels, int w, int h);
    static void FishEye(int ***pixels, int w, int h);
};
#endif  
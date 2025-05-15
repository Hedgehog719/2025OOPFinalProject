#include "bit_field_filter.h"
#include <bitset>
#include <algorithm>  // for std::min, std::max
template<typename T>
T clamp(T value, T low, T high) {
    return std::max(low, std::min(value, high));
}

// �Ƕ��v���B�z
void BitFieldFilter::ApplyAllFilters(uint8_t options, int **pixels, int w, int h) {
    cout<<"your options:"<<bitset<8>(options)<<endl;
    if (options & CASE_ONE) {
        Brightness(pixels, w, h);
    }
    if (options & CASE_TWO) {
        Invert(pixels, w, h);
    }
    if (options & CASE_THREE) {
        Denoise(pixels, w, h);
    }
    if (options & CASE_FOUR) {
        Threshold(pixels, w, h);
    }
    if (options & CASE_FIVE) {
        LaplacianSharpening(pixels, w, h);
    }
    if (options & CASE_SIX) {
        Mosaic(pixels, w, h);
    }
    if (options & CASE_SEVEN) {
        FlipHorizontal(pixels, w, h);
    }
    
    if (options & CASE_EIGHT) {
        FishEye(pixels, w, h);
    }
}

// RGB�v���B�z
void BitFieldFilter::ApplyAllFilters(uint8_t options, int ***pixels, int w, int h) {
    cout<<"your options:"<<bitset<8>(options)<<endl;
    if (options & CASE_ONE) {
        Brightness(pixels, w, h);
    }
    if (options & CASE_TWO) {
        Invert(pixels, w, h);
    }
    if (options & CASE_THREE) {
        Denoise(pixels, w, h);
    }
    if (options & CASE_FOUR) {
        Threshold(pixels, w, h);
    }
    if (options & CASE_FIVE) {
        LaplacianSharpening(pixels, w, h);
    }
    if (options & CASE_SIX) {
        Mosaic(pixels, w, h);
    }
    if (options & CASE_SEVEN) {
        FlipHorizontal(pixels, w, h);
    }
    if (options & CASE_EIGHT) {
        FishEye(pixels, w, h);
    }
    
}

void BitFieldFilter::FishEye(int **pixels, int w, int h) {
    if (w <= 0 || h <= 0 || pixels == nullptr) {
        // �B�z�L�Ŀ�J
        std::cerr << "FishEyeGrayscale Error: Invalid image dimensions or null pixel data." << std::endl;
        return;
    }

    // --- �]�w ---
    // ���u�Y�ơC���Ȳ��ͱ����u�M�g�]���ߦV�~���ȡ^�C
    // �վ㦹�Ȩӧ��ܮĪG�j�סC�ȶV�j�A���ĪG�V����C
    const double distortion_coefficient_k = 0.7; // �վ㦹�ȥH���ܿ��ȱj��
    // --- �]�w���� ---

    // �p��Ϥ������I�y��
    double centerX = w / 2.0;
    double centerY = h / 2.0;

    // �p��̤j�b�|�]���ߨ�̪���t���Z���^
    // �o�w�q�F��l�Ϥ����D�n���u�ϰ쪺��ɡC
    double max_radius = std::min(centerX, centerY);

    // �B�z�Ϥ��ؤo�� 1x1 �Χ�p�����p (�̤j�b�|�i�ର 0)
    if (max_radius <= 0) {
        std::cerr << "FishEyeGrayscale Warning: Image too small for distortion." << std::endl;
        return; // �L�k�M�Υ��u�ĪG
    }

    // --- ���t�Ȧs�w�İ� ---
    // �̷� int** ���c�Ӥ��t�O���� (�Ƕ�)
    int **temp_pixels = new (std::nothrow) int*[h];
    if (temp_pixels == nullptr) {
        std::cerr << "FishEyeGrayscale Error: Failed to allocate memory for rows array." << std::endl;
        return; // ���t����
    }

    bool allocation_failed = false;
    for (int y = 0; y < h; ++y) {
        temp_pixels[y] = new (std::nothrow) int[w]; // �C�� w �� int
        if (temp_pixels[y] == nullptr) {
            allocation_failed = true;
            std::cerr << "FishEyeGrayscale Error: Failed to allocate memory for pixel data in row " << y << std::endl;
            break; // ���t���ѡA���X�j��H�i��M�z
        }
        // �i�H��ܦb�o�̪�l�ƼȦs�����A�Ҧp�]�w���¦� (0)
        for (int x = 0; x < w; ++x) {
            temp_pixels[y][x] = 0;
        }
    }

    // �p�G���t���ѡA����M�z�ê�^
    if (allocation_failed) {
        // �M�z�w���t������
        for (int y = 0; y < h; ++y) {
            delete[] temp_pixels[y]; // �R�������}�C (��)
        }
        delete[] temp_pixels; // �R������а}�C
        return; // ��^
    }
    // --- �Ȧs�w�İϤ��t���� ---


    // --- �M�γ����S�� (�q��X�����ϦV�M�g���J����) ---
    // �M����X�Ϥ� (�Ȧs�w�İ�) �����C�ӹ��� (x_out, y_out)
    for (int y_out = 0; y_out < h; ++y_out) {
        for (int x_out = 0; x_out < w; ++x_out) {

            // �p���X�����۹�󤤤��I���y��
            double dx_out = x_out - centerX;
            double dy_out = y_out - centerY;

            // �p���X���������y�� (�۹�󤤤��I���Z���M����)
            double radius_out = std::sqrt(dx_out * dx_out + dy_out * dy_out);
            double angle_out = std::atan2(dy_out, dx_out); // ���סA��쬰���� [-PI, PI]

            // --- �M�α����u�ƾǼҫ� (�Ω󤤤߿��ȮĪG) ---
            // �N��X�b�|�зǤƨ� [0, 1] �d��A��� max_radius
            double normalized_radius_out = radius_out / max_radius;

            // �M�α����u�M�g��зǤƫ᪺�b�|�C
            // ����: d = d' * (1 + k * d'^2)�A�䤤 d' �O�зǤƿ�X�Z���Ad �O�зǤƿ�J�Z���C
            double normalized_radius_in = normalized_radius_out * (1.0 + distortion_coefficient_k * normalized_radius_out * normalized_radius_out);

            // �N�зǤƫ᪺��J�b�|�ഫ�^�����Z��
            double radius_in = normalized_radius_in * max_radius;
            // --- ���u�ƾǼҫ����� ---

            // �p���J�Ϥ����A�۹�󤤤��I���åd���y��
            double dx_in = radius_in * std::cos(angle_out);
            double dy_in = radius_in * std::sin(angle_out);

            // �N�۹��J�y���ഫ�^��ڹϤ��y��
            double x_in_double = centerX + dx_in;
            double y_in_double = centerY + dy_in;

            // --- �ļ� ---
            // �ϥγ̪�F���ȡA�N�B�I�y�Х|�ˤ��J��̪񪺾�Ʈy��
            int x_in = static_cast<int>(std::round(x_in_double));
            int y_in = static_cast<int>(std::round(y_in_double));

            // �T�O��J�y�Цb��l�Ϥ������Ľd�� [0, w-1] �M [0, h-1] ��
            // Clamping (����) �����
            x_in = std::max(0, std::min(w - 1, x_in));
            y_in = std::max(0, std::min(h - 1, y_in));

            // �q��l�Ƕ��Ϥ� (pixels) ���A�b�p��X����J�y�� (x_in, y_in) �B�ļ˹����ȡA
            // �ñN��ƻs��Ȧs�w�İ� (temp_pixels) ����e��X�y�� (x_out, y_out) �B�C
            temp_pixels[y_out][x_out] = pixels[y_in][x_in];
            // --- �ļ˵��� ---
        }
    }
    // --- �����S�ĭp�⵲�� ---


    // --- �N�B�z�᪺�����q�Ȧs�w�İϽƻs�^��l pixels �}�C ---
    for (int y = 0; y < h; ++y) {
        for (int x = 0; x < w; ++x) {
            pixels[y][x] = temp_pixels[y][x];
        }
    }
    // --- �ƻs���� ---


    // --- ����Ȧs�w�İϰO���� ---
    for (int y = 0; y < h; ++y) {
        delete[] temp_pixels[y]; // �R�������}�C (��)
    }
    delete[] temp_pixels; // �R������а}�C
    temp_pixels = nullptr; // �]�w�� nullptr �O�Ӧn�ߺD
    // --- �Ȧs�w�İ����񧹦� ---
}
void BitFieldFilter::FishEye(int ***pixels, int w, int h) {
    if (w <= 0 || h <= 0 || pixels == nullptr) {
        // �B�z�L�Ŀ�J
        std::cerr << "FishEye Error: Invalid image dimensions or null pixel data." << std::endl;
        return;
    }

    // --- �]�w ---
    const int num_channels = 3; // ���]�O RGB �Ϥ��C�p�G���O�A�Эק惡�B�C
    // ���u�Y�ơC���Ȳ��ͱ����u�]���������^�C
    // �վ㦹�Ȩӧ��ܮĪG�j�סC�嫬�Ȥ��� 0.01 �� 0.5 �����C
    const double distortion_coefficient_k = 0.5;
    // --- �]�w���� ---

    // �p��Ϥ������I�y��
    double centerX = w / 2.0;
    double centerY = h / 2.0;

    // �p��̤j�b�|�]���ߨ�̪���t���Z���^
    // �o�w�q�F��l�Ϥ����D�n���u�ϰ쪺��ɡC
    double max_radius = std::min(centerX, centerY);

    // �B�z�Ϥ��ؤo�� 1x1 �Χ�p�����p (�̤j�b�|�i�ର 0)
    if (max_radius <= 0) {
        std::cerr << "FishEye Warning: Image too small for distortion." << std::endl;
        return; // �L�k�M�Υ��u�ĪG
    }

    // --- ���t�Ȧs�w�İ� ---
    // �̷� int*** ���c�Ӥ��t�O����
    int ***temp_pixels = new (std::nothrow) int**[h];
    if (temp_pixels == nullptr) {
        std::cerr << "FishEye Error: Failed to allocate memory for rows array." << std::endl;
        return; // ���t����
    }

    bool allocation_failed = false;
    for (int y = 0; y < h; ++y) {
        temp_pixels[y] = new (std::nothrow) int*[w];
        if (temp_pixels[y] == nullptr) {
            allocation_failed = true;
            std::cerr << "FishEye Error: Failed to allocate memory for pixel pointers in row " << y << std::endl;
            break; // ���t���ѡA���X�j��H�i��M�z
        }
        for (int x = 0; x < w; ++x) {
            temp_pixels[y][x] = new (std::nothrow) int[num_channels];
            if (temp_pixels[y][x] == nullptr) {
                allocation_failed = true;
                std::cerr << "FishEye Error: Failed to allocate memory for pixel data at (" << x << "," << y << ")" << std::endl;
                break; // ���t���ѡA���X�����j��
            }
            // �i�H��ܦb�o�̪�l�ƼȦs�����A�Ҧp�]�w���¦� (0)
            for (int c = 0; c < num_channels; ++c) {
                *(temp_pixels[y][x] + c) = 0;
            }
        }
        if (allocation_failed) break; // ���t���ѡA���X�~���j��
    }

    // �p�G���t���ѡA����M�z�ê�^
    if (allocation_failed) {
        // �M�z�w���t������
        for (int y = 0; y < h; ++y) {
            if (temp_pixels[y] != nullptr) {
                for (int x = 0; x < w; ++x) {
                    delete[] temp_pixels[y][x]; // �R���q�D�}�C
                }
                delete[] temp_pixels[y]; // �R���������а}�C
            }
        }
        delete[] temp_pixels; // �R������а}�C
        return; // ��^
    }
    // --- �Ȧs�w�İϤ��t���� ---


    // --- �M�γ����S�� (�q��X�����ϦV�M�g���J����) ---
    // �M����X�Ϥ� (�Ȧs�w�İ�) �����C�ӹ��� (x_out, y_out)
    for (int y_out = 0; y_out < h; ++y_out) {
        for (int x_out = 0; x_out < w; ++x_out) {

            // �p���X�����۹�󤤤��I���y��
            double dx_out = x_out - centerX;
            double dy_out = y_out - centerY;

            // �p���X���������y�� (�۹�󤤤��I���Z���M����)
            double radius_out = std::sqrt(dx_out * dx_out + dy_out * dy_out);
            double angle_out = std::atan2(dy_out, dx_out); // ���סA��쬰���� [-PI, PI]

            // --- �M�Υ��u�ƾǼҫ� ---
            // �N��X�b�|�зǤƨ� [0, 1] �d��A��� max_radius
            double normalized_radius_out = radius_out / max_radius;
            
            // �M�α����u�M�g��зǤƫ᪺�b�|�C
            // �o�N��X�Ϥ����Z�������I���зǤƶZ�� d' ���I�A
            // �M�g���J�Ϥ����Z�������I���зǤƶZ�� d ���I�A�ϥΪ�������: d = d' * (1 + k * d'^2)
            // ��󳽲��u�ĪG�v�]�b�N��X�M�g�^��J�ɲ��ͱ����u�^�Ak �������ȡC
            double normalized_radius_in = normalized_radius_out * (1.0 + distortion_coefficient_k * normalized_radius_out * normalized_radius_out); // Barrel mapping

            // �N�зǤƫ᪺��J�b�|�ഫ�^�����Z��
            double radius_in = normalized_radius_in * max_radius;
            // --- ���u�ƾǼҫ����� ---

            // �p���J�Ϥ����A�۹�󤤤��I���åd���y��
            double dx_in = radius_in * std::cos(angle_out);
            double dy_in = radius_in * std::sin(angle_out);

            // �N�۹��J�y���ഫ�^��ڹϤ��y��
            double x_in_double = centerX + dx_in;
            double y_in_double = centerY + dy_in;

            // --- �ļ� ---
            // �ϥγ̪�F���ȡA�N�B�I�y�Х|�ˤ��J��̪񪺾�Ʈy��
            int x_in = static_cast<int>(std::round(x_in_double));
            int y_in = static_cast<int>(std::round(y_in_double));

            // �T�O��J�y�Цb��l�Ϥ������Ľd�� [0, w-1] �M [0, h-1] ��
            x_in = std::max(0, std::min(w - 1, x_in));
            y_in = std::max(0, std::min(h - 1, y_in));

            // �q��l�Ϥ� (pixels) ���A�b�p��X����J�y�� (x_in, y_in) �B�ļ˹�����ơA
            // �ñN��ƻs��Ȧs�w�İ� (temp_pixels) ����e��X�y�� (x_out, y_out) �B�C
            for (int c = 0; c < num_channels; ++c) {
                 // �z�L int*** ñ���s�� pixels[y][x][c]
                 // pixels[y_in][x_in] ���o���V�q�D��ƶ}�Y�� int* ����
                 // *(pixels[y_in][x_in] + c) �s���� c �ӳq�D����
                *(temp_pixels[y_out][x_out] + c) = *(pixels[y_in][x_in] + c);
            }
            // --- �ļ˵��� ---
        }
    }
    // --- �����S�ĭp�⵲�� ---


    // --- �N�B�z�᪺�����q�Ȧs�w�İϽƻs�^��l pixels �}�C ---
    for (int y = 0; y < h; ++y) {
        for (int x = 0; x < w; ++x) {
            for (int c = 0; c < num_channels; ++c) {
                // �q temp_pixels �ƻs�� pixels
                *(pixels[y][x] + c) = *(temp_pixels[y][x] + c);
            }
        }
    }
    // --- �ƻs���� ---


    // --- ����Ȧs�w�İϰO���� ---
    for (int y = 0; y < h; ++y) {
        if (temp_pixels[y] != nullptr) { // �ˬd���ЬO�_���ġA���ަb���t�ɤw�ɤO�T�O
            for (int x = 0; x < w; ++x) {
                delete[] temp_pixels[y][x]; // �R���q�D�}�C
            }
            delete[] temp_pixels[y]; // �R���������а}�C
        }
    }
    delete[] temp_pixels; // �R������а}�C
    temp_pixels = nullptr; // �]�w�� nullptr �O�Ӧn�ߺD
    // --- �Ȧs�w�İ����񧹦� ---
}


// �Ƕ��v���G�W�G�ާ@
void BitFieldFilter::Brightness(int **pixels, int w, int h) {
    for (int i = 0; i < h; ++i)
        for (int j = 0; j < w; ++j)
            pixels[i][j] = std::min(255, pixels[i][j] + 90);
}

// RGB�v���G�W�G�ާ@
void BitFieldFilter::Brightness(int ***pixels, int w, int h) {
    for (int i = 0; i < h; ++i)
        for (int j = 0; j < w; ++j) {
            pixels[i][j][0] = std::min(255, pixels[i][j][0] + 50); // Red
            pixels[i][j][1] = std::min(255, pixels[i][j][1] + 50); // Green
            pixels[i][j][2] = std::min(255, pixels[i][j][2] + 50); // Blue
        }
}

// ����ާ@
void BitFieldFilter::Invert(int **pixels, int w, int h) {
    if (pixels == nullptr) {
            cout << "Error: pixels is a null pointer!" << endl;
            return;
        }
    cout<<"start"<<endl;
    for (int i = 0; i < h; ++i)
        for (int j = 0; j < w; ++j)
            pixels[i][j] = 255 - pixels[i][j];
    cout<<"final"<<endl;
}

// RGB�v���G����ާ@
void BitFieldFilter::Invert(int ***pixels, int w, int h) {
    for (int i = 0; i < h; ++i)
        for (int j = 0; j < w; ++j) {
            pixels[i][j][0] = 255 - pixels[i][j][0]; // Red
            pixels[i][j][1] = 255 - pixels[i][j][1]; // Green
            pixels[i][j][2] = 255 - pixels[i][j][2]; // Blue
        }
}

// �����ާ@ (�Ƕ�)
void BitFieldFilter::Denoise(int **pixels, int w, int h) {
    int **copy = new int*[h];
    for (int i = 0; i < h; ++i) {
        copy[i] = new int[w];
        for (int j = 0; j < w; ++j)
            copy[i][j] = pixels[i][j];
    }

    for (int i = 1; i < h - 1; ++i) {
        for (int j = 1; j < w - 1; ++j) {
            int sum = 0;
            for (int di = -1; di <= 1; ++di)
                for (int dj = -1; dj <= 1; ++dj)
                    sum += copy[i + di][j + dj];
            pixels[i][j] = sum / 9;
        }
    }

    for (int i = 0; i < h; ++i)
        delete[] copy[i];
    delete[] copy;
}

// RGB�v���G�����ާ@
void BitFieldFilter::Denoise(int ***pixels, int w, int h) {
    int ***copy = new int**[h];
    for (int i = 0; i < h; ++i) {
        copy[i] = new int*[w];
        for (int j = 0; j < w; ++j) {
            copy[i][j] = new int[3];
            for (int k = 0; k < 3; ++k) {
                copy[i][j][k] = pixels[i][j][k];
            }
        }
    }

    for (int i = 1; i < h - 1; ++i) {
        for (int j = 1; j < w - 1; ++j) {
            for (int c = 0; c < 3; ++c) {
                int sum = 0;
                for (int di = -1; di <= 1; ++di)
                    for (int dj = -1; dj <= 1; ++dj)
                        sum += copy[i + di][j + dj][c];
                pixels[i][j][c] = sum / 9;
            }
        }
    }

    for (int i = 0; i < h; ++i)
        for (int j = 0; j < w; ++j)
            delete[] copy[i][j];
    for (int i = 0; i < h; ++i)
        delete[] copy[i];
    delete[] copy;
}
// --- ���e�� ---
void BitFieldFilter::Threshold(int **pixels, int w, int h) {
    for (int i = 0; i < h; ++i)
        for (int j = 0; j < w; ++j)
            pixels[i][j] = (pixels[i][j] > 128) ? 255 : 0;
}

void BitFieldFilter::Threshold(int ***pixels, int w, int h) {
    for (int i = 0; i < h; ++i)
        for (int j = 0; j < w; ++j)
            for (int c = 0; c < 3; ++c)
                pixels[i][j][c] = (pixels[i][j][c] > 128) ? 255 : 0;
}

// --- Laplacian �U�� ---
void BitFieldFilter::LaplacianSharpening(int **pixels, int w, int h) {
    int **copy = new int*[h];
    for (int i = 0; i < h; ++i) {
        copy[i] = new int[w];
        for (int j = 0; j < w; ++j)
            copy[i][j] = pixels[i][j];
    }

    int dx[] = {0, -1, 0, 1, 0};
    int dy[] = {0, 0, -1, 0, 1};

    for (int i = 1; i < h - 1; ++i) {
        for (int j = 1; j < w - 1; ++j) {
            int lap = 4 * copy[i][j] - copy[i-1][j] - copy[i+1][j] - copy[i][j-1] - copy[i][j+1];
            pixels[i][j] = clamp(copy[i][j] + lap, 0, 255);
        }
    }

    for (int i = 0; i < h; ++i)
        delete[] copy[i];
    delete[] copy;
}

void BitFieldFilter::LaplacianSharpening(int ***pixels, int w, int h) {
    int ***copy = new int**[h];
    for (int i = 0; i < h; ++i) {
        copy[i] = new int*[w];
        for (int j = 0; j < w; ++j) {
            copy[i][j] = new int[3];
            for (int c = 0; c < 3; ++c)
                copy[i][j][c] = pixels[i][j][c];
        }
    }

    for (int i = 1; i < h - 1; ++i) {
        for (int j = 1; j < w - 1; ++j) {
            for (int c = 0; c < 3; ++c) {
                int lap = 4 * copy[i][j][c]
                          - copy[i-1][j][c]
                          - copy[i+1][j][c]
                          - copy[i][j-1][c]
                          - copy[i][j+1][c];
                pixels[i][j][c] = clamp(copy[i][j][c] + lap, 0, 255);
            }
        }
    }

    for (int i = 0; i < h; ++i)
        for (int j = 0; j < w; ++j)
            delete[] copy[i][j];
    for (int i = 0; i < h; ++i)
        delete[] copy[i];
    delete[] copy;
}

// --- ���ɧJ�B�z ---
void BitFieldFilter::Mosaic(int **pixels, int w, int h, int block_size) {
    for (int y = 0; y < h; y += block_size) {
        for (int x = 0; x < w; x += block_size) {
            int sum = 0, count = 0;
            for (int i = 0; i < block_size && y + i < h; ++i)
                for (int j = 0; j < block_size && x + j < w; ++j) {
                    sum += pixels[y + i][x + j];
                    count++;
                }

            int avg = (count == 0) ? 0 : sum / count;

            for (int i = 0; i < block_size && y + i < h; ++i)
                for (int j = 0; j < block_size && x + j < w; ++j)
                    pixels[y + i][x + j] = avg;
        }
    }
}

void BitFieldFilter::Mosaic(int ***pixels, int w, int h, int block_size) {
    for (int y = 0; y < h; y += block_size) {
        for (int x = 0; x < w; x += block_size) {
            int sum[3] = {0, 0, 0}, count = 0;

            for (int i = 0; i < block_size && y + i < h; ++i) {
                for (int j = 0; j < block_size && x + j < w; ++j) {
                    for (int c = 0; c < 3; ++c)
                        sum[c] += pixels[y + i][x + j][c];
                    count++;
                }
            }

            int avg[3] = {0};
            if (count > 0) {
                for (int c = 0; c < 3; ++c)
                    avg[c] = sum[c] / count;
            }

            for (int i = 0; i < block_size && y + i < h; ++i) {
                for (int j = 0; j < block_size && x + j < w; ++j) {
                    for (int c = 0; c < 3; ++c)
                        pixels[y + i][x + j][c] = avg[c];
                }
            }
        }
    }
}
void BitFieldFilter::FlipHorizontal(int **pixels, int w, int h) {
    for (int i = 0; i < h; ++i) {
        for (int j = 0; j < w / 2; ++j) {
            std::swap(pixels[i][j], pixels[i][w - 1 - j]);
        }
    }
}
void BitFieldFilter::FlipHorizontal(int ***pixels, int w, int h) {
    for (int i = 0; i < h; ++i) {
        for (int j = 0; j < w / 2; ++j) {
            for (int c = 0; c < 3; ++c) {
                std::swap(pixels[i][j][c], pixels[i][w - 1 - j][c]);
            }
        }
    }
}
#include "bit_field_filter.h"
#include <bitset>
#include <algorithm>  // for std::min, std::max
template<typename T>
T clamp(T value, T low, T high) {
    return std::max(low, std::min(value, high));
}

// 灰階影像處理
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

// RGB影像處理
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
        // 處理無效輸入
        std::cerr << "FishEyeGrayscale Error: Invalid image dimensions or null pixel data." << std::endl;
        return;
    }

    // --- 設定 ---
    // 失真係數。正值產生桶狀失真映射（中心向外膨脹）。
    // 調整此值來改變效果強度。值越大，桶狀效果越明顯。
    const double distortion_coefficient_k = 0.7; // 調整此值以改變膨脹強度
    // --- 設定結束 ---

    // 計算圖片中心點座標
    double centerX = w / 2.0;
    double centerY = h / 2.0;

    // 計算最大半徑（中心到最近邊緣的距離）
    // 這定義了原始圖片中主要失真區域的邊界。
    double max_radius = std::min(centerX, centerY);

    // 處理圖片尺寸為 1x1 或更小的情況 (最大半徑可能為 0)
    if (max_radius <= 0) {
        std::cerr << "FishEyeGrayscale Warning: Image too small for distortion." << std::endl;
        return; // 無法套用失真效果
    }

    // --- 分配暫存緩衝區 ---
    // 依照 int** 結構來分配記憶體 (灰階)
    int **temp_pixels = new (std::nothrow) int*[h];
    if (temp_pixels == nullptr) {
        std::cerr << "FishEyeGrayscale Error: Failed to allocate memory for rows array." << std::endl;
        return; // 分配失敗
    }

    bool allocation_failed = false;
    for (int y = 0; y < h; ++y) {
        temp_pixels[y] = new (std::nothrow) int[w]; // 每行 w 個 int
        if (temp_pixels[y] == nullptr) {
            allocation_failed = true;
            std::cerr << "FishEyeGrayscale Error: Failed to allocate memory for pixel data in row " << y << std::endl;
            break; // 分配失敗，跳出迴圈以進行清理
        }
        // 可以選擇在這裡初始化暫存像素，例如設定為黑色 (0)
        for (int x = 0; x < w; ++x) {
            temp_pixels[y][x] = 0;
        }
    }

    // 如果分配失敗，執行清理並返回
    if (allocation_failed) {
        // 清理已分配的部分
        for (int y = 0; y < h; ++y) {
            delete[] temp_pixels[y]; // 刪除像素陣列 (行)
        }
        delete[] temp_pixels; // 刪除行指標陣列
        return; // 返回
    }
    // --- 暫存緩衝區分配完成 ---


    // --- 套用魚眼特效 (從輸出像素反向映射到輸入像素) ---
    // 遍歷輸出圖片 (暫存緩衝區) 中的每個像素 (x_out, y_out)
    for (int y_out = 0; y_out < h; ++y_out) {
        for (int x_out = 0; x_out < w; ++x_out) {

            // 計算輸出像素相對於中心點的座標
            double dx_out = x_out - centerX;
            double dy_out = y_out - centerY;

            // 計算輸出像素的極座標 (相對於中心點的距離和角度)
            double radius_out = std::sqrt(dx_out * dx_out + dy_out * dy_out);
            double angle_out = std::atan2(dy_out, dx_out); // 角度，單位為弧度 [-PI, PI]

            // --- 套用桶狀失真數學模型 (用於中心膨脹效果) ---
            // 將輸出半徑標準化到 [0, 1] 範圍，基於 max_radius
            double normalized_radius_out = radius_out / max_radius;

            // 套用桶狀失真映射到標準化後的半徑。
            // 公式: d = d' * (1 + k * d'^2)，其中 d' 是標準化輸出距離，d 是標準化輸入距離。
            double normalized_radius_in = normalized_radius_out * (1.0 + distortion_coefficient_k * normalized_radius_out * normalized_radius_out);

            // 將標準化後的輸入半徑轉換回像素距離
            double radius_in = normalized_radius_in * max_radius;
            // --- 失真數學模型結束 ---

            // 計算輸入圖片中，相對於中心點的笛卡爾座標
            double dx_in = radius_in * std::cos(angle_out);
            double dy_in = radius_in * std::sin(angle_out);

            // 將相對輸入座標轉換回實際圖片座標
            double x_in_double = centerX + dx_in;
            double y_in_double = centerY + dy_in;

            // --- 採樣 ---
            // 使用最近鄰插值，將浮點座標四捨五入到最近的整數座標
            int x_in = static_cast<int>(std::round(x_in_double));
            int y_in = static_cast<int>(std::round(y_in_double));

            // 確保輸入座標在原始圖片的有效範圍 [0, w-1] 和 [0, h-1] 內
            // Clamping (限制) 到邊界
            x_in = std::max(0, std::min(w - 1, x_in));
            y_in = std::max(0, std::min(h - 1, y_in));

            // 從原始灰階圖片 (pixels) 中，在計算出的輸入座標 (x_in, y_in) 處採樣像素值，
            // 並將其複製到暫存緩衝區 (temp_pixels) 的當前輸出座標 (x_out, y_out) 處。
            temp_pixels[y_out][x_out] = pixels[y_in][x_in];
            // --- 採樣結束 ---
        }
    }
    // --- 魚眼特效計算結束 ---


    // --- 將處理後的像素從暫存緩衝區複製回原始 pixels 陣列 ---
    for (int y = 0; y < h; ++y) {
        for (int x = 0; x < w; ++x) {
            pixels[y][x] = temp_pixels[y][x];
        }
    }
    // --- 複製完成 ---


    // --- 釋放暫存緩衝區記憶體 ---
    for (int y = 0; y < h; ++y) {
        delete[] temp_pixels[y]; // 刪除像素陣列 (行)
    }
    delete[] temp_pixels; // 刪除行指標陣列
    temp_pixels = nullptr; // 設定為 nullptr 是個好習慣
    // --- 暫存緩衝區釋放完成 ---
}
void BitFieldFilter::FishEye(int ***pixels, int w, int h) {
    if (w <= 0 || h <= 0 || pixels == nullptr) {
        // 處理無效輸入
        std::cerr << "FishEye Error: Invalid image dimensions or null pixel data." << std::endl;
        return;
    }

    // --- 設定 ---
    const int num_channels = 3; // 假設是 RGB 圖片。如果不是，請修改此處。
    // 失真係數。正值產生桶狀失真（模擬魚眼）。
    // 調整此值來改變效果強度。典型值介於 0.01 到 0.5 之間。
    const double distortion_coefficient_k = 0.5;
    // --- 設定結束 ---

    // 計算圖片中心點座標
    double centerX = w / 2.0;
    double centerY = h / 2.0;

    // 計算最大半徑（中心到最近邊緣的距離）
    // 這定義了原始圖片中主要失真區域的邊界。
    double max_radius = std::min(centerX, centerY);

    // 處理圖片尺寸為 1x1 或更小的情況 (最大半徑可能為 0)
    if (max_radius <= 0) {
        std::cerr << "FishEye Warning: Image too small for distortion." << std::endl;
        return; // 無法套用失真效果
    }

    // --- 分配暫存緩衝區 ---
    // 依照 int*** 結構來分配記憶體
    int ***temp_pixels = new (std::nothrow) int**[h];
    if (temp_pixels == nullptr) {
        std::cerr << "FishEye Error: Failed to allocate memory for rows array." << std::endl;
        return; // 分配失敗
    }

    bool allocation_failed = false;
    for (int y = 0; y < h; ++y) {
        temp_pixels[y] = new (std::nothrow) int*[w];
        if (temp_pixels[y] == nullptr) {
            allocation_failed = true;
            std::cerr << "FishEye Error: Failed to allocate memory for pixel pointers in row " << y << std::endl;
            break; // 分配失敗，跳出迴圈以進行清理
        }
        for (int x = 0; x < w; ++x) {
            temp_pixels[y][x] = new (std::nothrow) int[num_channels];
            if (temp_pixels[y][x] == nullptr) {
                allocation_failed = true;
                std::cerr << "FishEye Error: Failed to allocate memory for pixel data at (" << x << "," << y << ")" << std::endl;
                break; // 分配失敗，跳出內部迴圈
            }
            // 可以選擇在這裡初始化暫存像素，例如設定為黑色 (0)
            for (int c = 0; c < num_channels; ++c) {
                *(temp_pixels[y][x] + c) = 0;
            }
        }
        if (allocation_failed) break; // 分配失敗，跳出外部迴圈
    }

    // 如果分配失敗，執行清理並返回
    if (allocation_failed) {
        // 清理已分配的部分
        for (int y = 0; y < h; ++y) {
            if (temp_pixels[y] != nullptr) {
                for (int x = 0; x < w; ++x) {
                    delete[] temp_pixels[y][x]; // 刪除通道陣列
                }
                delete[] temp_pixels[y]; // 刪除像素指標陣列
            }
        }
        delete[] temp_pixels; // 刪除行指標陣列
        return; // 返回
    }
    // --- 暫存緩衝區分配完成 ---


    // --- 套用魚眼特效 (從輸出像素反向映射到輸入像素) ---
    // 遍歷輸出圖片 (暫存緩衝區) 中的每個像素 (x_out, y_out)
    for (int y_out = 0; y_out < h; ++y_out) {
        for (int x_out = 0; x_out < w; ++x_out) {

            // 計算輸出像素相對於中心點的座標
            double dx_out = x_out - centerX;
            double dy_out = y_out - centerY;

            // 計算輸出像素的極座標 (相對於中心點的距離和角度)
            double radius_out = std::sqrt(dx_out * dx_out + dy_out * dy_out);
            double angle_out = std::atan2(dy_out, dx_out); // 角度，單位為弧度 [-PI, PI]

            // --- 套用失真數學模型 ---
            // 將輸出半徑標準化到 [0, 1] 範圍，基於 max_radius
            double normalized_radius_out = radius_out / max_radius;
            
            // 套用桶狀失真映射到標準化後的半徑。
            // 這將輸出圖片中距離中心點為標準化距離 d' 的點，
            // 映射到輸入圖片中距離中心點為標準化距離 d 的點，使用的公式為: d = d' * (1 + k * d'^2)
            // 對於魚眼「效果」（在將輸出映射回輸入時產生桶狀失真），k 應為正值。
            double normalized_radius_in = normalized_radius_out * (1.0 + distortion_coefficient_k * normalized_radius_out * normalized_radius_out); // Barrel mapping

            // 將標準化後的輸入半徑轉換回像素距離
            double radius_in = normalized_radius_in * max_radius;
            // --- 失真數學模型結束 ---

            // 計算輸入圖片中，相對於中心點的笛卡爾座標
            double dx_in = radius_in * std::cos(angle_out);
            double dy_in = radius_in * std::sin(angle_out);

            // 將相對輸入座標轉換回實際圖片座標
            double x_in_double = centerX + dx_in;
            double y_in_double = centerY + dy_in;

            // --- 採樣 ---
            // 使用最近鄰插值，將浮點座標四捨五入到最近的整數座標
            int x_in = static_cast<int>(std::round(x_in_double));
            int y_in = static_cast<int>(std::round(y_in_double));

            // 確保輸入座標在原始圖片的有效範圍 [0, w-1] 和 [0, h-1] 內
            x_in = std::max(0, std::min(w - 1, x_in));
            y_in = std::max(0, std::min(h - 1, y_in));

            // 從原始圖片 (pixels) 中，在計算出的輸入座標 (x_in, y_in) 處採樣像素資料，
            // 並將其複製到暫存緩衝區 (temp_pixels) 的當前輸出座標 (x_out, y_out) 處。
            for (int c = 0; c < num_channels; ++c) {
                 // 透過 int*** 簽章存取 pixels[y][x][c]
                 // pixels[y_in][x_in] 取得指向通道資料開頭的 int* 指標
                 // *(pixels[y_in][x_in] + c) 存取第 c 個通道的值
                *(temp_pixels[y_out][x_out] + c) = *(pixels[y_in][x_in] + c);
            }
            // --- 採樣結束 ---
        }
    }
    // --- 魚眼特效計算結束 ---


    // --- 將處理後的像素從暫存緩衝區複製回原始 pixels 陣列 ---
    for (int y = 0; y < h; ++y) {
        for (int x = 0; x < w; ++x) {
            for (int c = 0; c < num_channels; ++c) {
                // 從 temp_pixels 複製到 pixels
                *(pixels[y][x] + c) = *(temp_pixels[y][x] + c);
            }
        }
    }
    // --- 複製完成 ---


    // --- 釋放暫存緩衝區記憶體 ---
    for (int y = 0; y < h; ++y) {
        if (temp_pixels[y] != nullptr) { // 檢查指標是否有效，儘管在分配時已盡力確保
            for (int x = 0; x < w; ++x) {
                delete[] temp_pixels[y][x]; // 刪除通道陣列
            }
            delete[] temp_pixels[y]; // 刪除像素指標陣列
        }
    }
    delete[] temp_pixels; // 刪除行指標陣列
    temp_pixels = nullptr; // 設定為 nullptr 是個好習慣
    // --- 暫存緩衝區釋放完成 ---
}


// 灰階影像：增亮操作
void BitFieldFilter::Brightness(int **pixels, int w, int h) {
    for (int i = 0; i < h; ++i)
        for (int j = 0; j < w; ++j)
            pixels[i][j] = std::min(255, pixels[i][j] + 90);
}

// RGB影像：增亮操作
void BitFieldFilter::Brightness(int ***pixels, int w, int h) {
    for (int i = 0; i < h; ++i)
        for (int j = 0; j < w; ++j) {
            pixels[i][j][0] = std::min(255, pixels[i][j][0] + 50); // Red
            pixels[i][j][1] = std::min(255, pixels[i][j][1] + 50); // Green
            pixels[i][j][2] = std::min(255, pixels[i][j][2] + 50); // Blue
        }
}

// 反轉操作
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

// RGB影像：反轉操作
void BitFieldFilter::Invert(int ***pixels, int w, int h) {
    for (int i = 0; i < h; ++i)
        for (int j = 0; j < w; ++j) {
            pixels[i][j][0] = 255 - pixels[i][j][0]; // Red
            pixels[i][j][1] = 255 - pixels[i][j][1]; // Green
            pixels[i][j][2] = 255 - pixels[i][j][2]; // Blue
        }
}

// 降噪操作 (灰階)
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

// RGB影像：降噪操作
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
// --- 門檻化 ---
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

// --- Laplacian 銳化 ---
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

// --- 馬賽克處理 ---
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
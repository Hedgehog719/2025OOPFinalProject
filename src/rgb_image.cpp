// RGBImage.cpp
#include "rgb_image.h"
#include <bitset>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <map>
#include <vector>
#include <cmath>
#include <algorithm>
#include <unordered_map>
#include "image.h"
#include "data_loader.h"
#include <cstdlib> // for rand(), srand()
#include <ctime>   // for time()
#include <stdexcept>
#include <unistd.h>
#include <sys/stat.h>
#include "rgb_image.h"
#include <termios.h>
#include <sys/ioctl.h>


const int dx_rgb[8][3][3] = {
    {{ 1, 0,-1}, { 2, 0,-2}, { 1, 0,-1}},    // 0. →
    {{ 0, 1, 2}, {-1, 0, 1}, {-2,-1, 0}},    // 1. ↗
    {{ 1, 2, 1}, { 0, 0, 0}, {-1,-2,-1}},    // 2. ↑
    {{ 2, 1, 0}, { 1, 0,-1}, { 0,-1,-2}},    // 3. ↖
    {{-1, 0, 1}, {-2, 0, 2}, {-1, 0, 1}},    // 4. ←
    {{ 0,-1,-2}, { 1, 0,-1}, { 2, 1, 0}},    // 5. ↙
    {{-1,-2,-1}, { 0, 0, 0}, { 1, 2, 1}},    // 6. ↓
    {{-2,-1, 0}, {-1, 0, 1}, { 0, 1, 2}}     // 7. ↘
};



bool file_exists(const std::string& name){
    struct stat buffer;
    return (stat(name.c_str(), &buffer) == 0);
}

// This JSON loader is identical to the one in GrayImage.cpp
// If C++11 `std::stof` is available, it's cleaner than `atof`.
// `atof` doesn't throw exceptions, so error checking is more manual.
std::map<std::string, std::vector<float> > load_simple_histogram_json_rgb(const std::string& filename){
    std::map<std::string, std::vector<float> > result;
    std::ifstream fin(filename.c_str());
    if (!fin.is_open()) {
        std::cerr << "Cannot open file: " << filename << std::endl;
        return result;
    }

    std::string line, current_key;
    bool in_array = false;

    while (getline(fin, line)) {
        line.erase(0, line.find_first_not_of(" \t\n\r\f\v"));
        line.erase(line.find_last_not_of(" \t\n\r\f\v") + 1);

        if (line.empty() || line == "{" || line == "}") {
             if (line == "}") in_array = false;
             continue;
        }

        if (in_array) {
            std::string content_to_parse = line;
            bool array_ended_this_line = false;
            size_t end_bracket_pos = line.find("]");
            if (end_bracket_pos != std::string::npos) {
                content_to_parse = line.substr(0, end_bracket_pos);
                array_ended_this_line = true;
            }

            std::stringstream ss(content_to_parse);
            std::string token;
            while (getline(ss, token, ',')) {
                token.erase(0, token.find_first_not_of(" \t\n\r\f\v"));
                token.erase(token.find_last_not_of(" \t\n\r\f\v") + 1);
                if (!token.empty()) {
                    try {
                         // std::stof is C++11, atof is C-style and part of <cstdlib>
                        result[current_key].push_back(static_cast<float>(std::atof(token.c_str())));
                    } catch (const std::exception& e) { // atof doesn't throw, so this catch might not be effective for atof related errors
                         std::cerr << "Warning: Could not parse float from token '" << token << "'" << std::endl;
                    }
                }
            }
            if (array_ended_this_line) {
                in_array = false;
            }
        } else {
            size_t colon_pos = line.find(":");
            if (colon_pos != std::string::npos) {
                size_t q1 = line.find("\"");
                size_t q2 = line.find("\"", q1 + 1);
                if (q1 != std::string::npos && q2 != std::string::npos && q2 > q1) {
                    current_key = line.substr(q1 + 1, q2 - q1 - 1);
                    result[current_key] = std::vector<float>(); 
                } else {
                    continue; 
                }

                size_t start_bracket_pos = line.find("[", colon_pos);
                if (start_bracket_pos != std::string::npos) {
                    in_array = true;
                    std::string content_to_parse = line.substr(start_bracket_pos + 1);
                    bool array_ended_this_line = false;
                    size_t end_bracket_pos = content_to_parse.find("]");
                     if (end_bracket_pos != std::string::npos) {
                        content_to_parse = content_to_parse.substr(0, end_bracket_pos);
                        array_ended_this_line = true;
                    }

                    std::stringstream ss(content_to_parse);
                    std::string token;
                    while (getline(ss, token, ',')) {
                        token.erase(0, token.find_first_not_of(" \t\n\r\f\v"));
                        token.erase(token.find_last_not_of(" \t\n\r\f\v") + 1);
                        if (!token.empty()) {
                             try {
                                result[current_key].push_back(static_cast<float>(std::atof(token.c_str())));
                            } catch (const std::exception& e) {
                                 std::cerr << "Warning: Could not parse float from token '" << token << "'" << std::endl;
                            }
                        }
                    }
                    if (array_ended_this_line) {
                        in_array = false;
                    }
                }
            }
        }
    }
    fin.close();
    return result;
}

float calc_hist_diff_rgb(const std::vector<float>& a, const std::vector<float>& b){
    float diff = 0;
    if (a.size() != b.size()) {
        // std::cerr << "Warning: RGB Histogram sizes differ! (" << a.size() << " vs " << b.size() << ")" << std::endl;
        return 1e18f;
    }
    for (size_t i = 0; i < a.size(); ++i) {
        diff += std::pow(a[i] - b[i], 2.0f);
    }
    return diff;
}

int calc_main_direction_rgb(unsigned char** gray_block, int blockSize){
    std::vector<float> direction_strength(8, 0.0f);
    for (int d = 0; d < 8; ++d) {
        float sum = 0;
        for (int i = 1; i < blockSize - 1; ++i) {
            for (int j = 1; j < blockSize - 1; ++j) {
                float val = 0;
                for (int ki = -1; ki <= 1; ++ki) {
                    for (int kj = -1; kj <= 1; ++kj) {
                        val += static_cast<float>(gray_block[i + ki][j + kj]) * dx_rgb[d][ki + 1][kj + 1];
                    }
                }
                sum += std::abs(val);
            }
        }
        direction_strength[d] = sum;
    }
    return std::distance(direction_strength.begin(), std::max_element(direction_strength.begin(), direction_strength.end()));
}

unsigned char** create_2d_uchar_array(int rows, int cols){
    unsigned char** arr = new unsigned char*[rows];
    for (int i = 0; i < rows; ++i) {
        arr[i] = new unsigned char[cols];
        for(int j=0; j<cols; ++j) arr[i][j] = 0; // Initialize
    }
    return arr;
}

void delete_2d_uchar_array(unsigned char** arr, int rows){
    if (!arr) return;
    for (int i = 0; i < rows; ++i) {
        delete[] arr[i];
    }
    delete[] arr;
}

unsigned char*** create_3d_uchar_array(int dim1_h, int dim2_w, int dim3_channels){
    unsigned char*** arr = new unsigned char**[dim1_h];
    for (int i = 0; i < dim1_h; ++i) {
        arr[i] = new unsigned char*[dim2_w];
        for (int j = 0; j < dim2_w; ++j) {
            arr[i][j] = new unsigned char[dim3_channels];
            for(int c=0; c<dim3_channels; ++c) arr[i][j][c] = 0; // Initialize
        }
    }
    return arr;
}

void delete_3d_uchar_array(unsigned char*** arr, int dim1_h, int dim2_w){
    if (!arr) return;
    for (int i = 0; i < dim1_h; ++i) {
        if (arr[i]) {
            for (int j = 0; j < dim2_w; ++j) {
                delete[] arr[i][j];
            }
            delete[] arr[i];
        }
    }
    delete[] arr;
}


// --- RGBImage::small_rgb() Implementation ---
void RGBImage::small() {
    std::cout << "=== Mosaic using Small RGB Images ===" << std::endl;
    std::cout << "Choose a collection:" << std::endl;
    std::string category_names[10] = { "airplane","automobile","bird","cat","deer","dog","frog","horse","ship","truck" };
    for (int i = 0; i < 10; ++i) {
        std::cout << i + 1 << ". " << category_names[i] << std::endl;
    }
    std::cout<<"====================================="<<std::endl;
    int selected_idx;
    std::cin >> selected_idx;
    if (selected_idx < 1 || selected_idx > 10) {
        std::cerr << "Error: Invalid selection." << std::endl;
        return;
    }
    std::string category = category_names[selected_idx - 1];

    // Path for RGB JSON database
    std::string json_path = "SmallImg/output_json_rgb/" + category + ".json";
    std::map<std::string, std::vector<float> > db = load_simple_histogram_json_rgb(json_path);

    if (db.empty()) {
        std::cerr << "Error: Failed to load or empty database from: " << json_path << std::endl;
        return;
    }

    const int blockSize = 32;
    const int num_hist_bins_per_channel_rgb = 16;
    const int total_hist_bins_rgb = num_hist_bins_per_channel_rgb * 3;

    if (this->h == 0 || this->w == 0 || !this->pixels) {
        std::cerr << "Error: Main RGB image not loaded or is empty." << std::endl;
        return;
    }

    int blocks_y = this->h / blockSize;
    int blocks_x = this->w / blockSize;

    if (blocks_y == 0 || blocks_x == 0) {
        std::cerr << "Error: Image is too small for the block size." << std::endl;
        return;
    }

    int out_h = blocks_y * blockSize;
    int out_w = blocks_x * blockSize;

    // Temporary storage for the mosaic result (using unsigned char for pixel values 0-255)
    unsigned char*** mosaic_rgb_uchar = create_3d_uchar_array(out_h, out_w, 3);
    if (!mosaic_rgb_uchar) {
        std::cerr << "Error: Failed to allocate memory for RGB mosaic buffer." << std::endl;
        return;
    }

    std::cout << "Loading valid small RGB images from database..." << std::endl;
    std::unordered_map<std::string, std::vector<float> > valid_db;
    char current_dir_path[1024];
    if (getcwd(current_dir_path, sizeof(current_dir_path)) == NULL) {
        std::cerr << "Error getting current working directory." << std::endl;
        delete_3d_uchar_array(mosaic_rgb_uchar, out_h, out_w);
        return;
    }

    for (std::map<std::string, std::vector<float> >::const_iterator it = db.begin(); it != db.end(); ++it) {
        const std::string& filename_key = it->first;
        const std::vector<float>& hist_vals = it->second;
        // Path to small RGB images
        std::string fullpath = std::string(current_dir_path) + "/SmallImg/small_rgb/" + filename_key;
        //cout<<fullpath<<endl;
        if (file_exists(fullpath)) {
            if (hist_vals.size() == total_hist_bins_rgb) {
                 valid_db[filename_key] = hist_vals;
            } else {
                 std::cout << "[DEBUG] RGB hist size mismatch for " << filename_key << ". Expected " << total_hist_bins_rgb << ", got " << hist_vals.size() << std::endl;
            }
        } else {
             std::cout << "[DEBUG] Small RGB File not found: [" << fullpath << "]" << std::endl;
        }
    }
    std::cout << "Loaded valid_db (RGB) size: " << valid_db.size() << std::endl;
    if (valid_db.empty()) {
        std::cerr << "Error: No valid small RGB images found in the database for path: "
                  << std::string(current_dir_path) << "/SmallImg/small_rgb/"<<category<<"/" << std::endl;
        delete_3d_uchar_array(mosaic_rgb_uchar, out_h, out_w);
        return;
    }
    double wgt[3]={0.33,0.34,0.33};
    srand(static_cast<unsigned int>(time(0)));

    for (int by = 0; by < blocks_y; ++by) {
        for (int bx = 0; bx < blocks_x; ++bx) {
            std::vector<float> block_hist_rgb(total_hist_bins_rgb, 0.0f);
            long long sum_r = 0, sum_g = 0, sum_b = 0;
            int pixel_count_in_block = 0;

            unsigned char** current_block_gray_for_dir = create_2d_uchar_array(blockSize, blockSize);
            if(!current_block_gray_for_dir) { std::cerr << "Mem alloc failed for current_block_gray_for_dir" << std::endl; continue; }


            for (int i = 0; i < blockSize; ++i) {
                for (int j = 0; j < blockSize; ++j) {
                    int y_img = by * blockSize + i;
                    int x_img = bx * blockSize + j;
                    
                    // this->pixels is int***, assume values are 0-255 for each channel
                    int r_val_int = this->pixels[y_img][x_img][0];
                    int g_val_int = this->pixels[y_img][x_img][1];
                    int b_val_int = this->pixels[y_img][x_img][2];

                    unsigned char r_val = static_cast<unsigned char>(std::max(0, std::min(255, r_val_int)));
                    unsigned char g_val = static_cast<unsigned char>(std::max(0, std::min(255, g_val_int)));
                    unsigned char b_val = static_cast<unsigned char>(std::max(0, std::min(255, b_val_int)));

                    sum_r += r_val;
                    sum_g += g_val;
                    sum_b += b_val;

                    // Binning for RGB histogram (15 bins per channel)
                    // val / (256/15) = val * 15 / 256
                    int r_bin = std::min(num_hist_bins_per_channel_rgb - 1, static_cast<int>(r_val * num_hist_bins_per_channel_rgb / 256.0));
                    int g_bin = std::min(num_hist_bins_per_channel_rgb - 1, static_cast<int>(g_val * num_hist_bins_per_channel_rgb / 256.0));
                    int b_bin = std::min(num_hist_bins_per_channel_rgb - 1, static_cast<int>(b_val * num_hist_bins_per_channel_rgb / 256.0));
                    
                    block_hist_rgb[r_bin] += 1.0f;
                    block_hist_rgb[num_hist_bins_per_channel_rgb + g_bin] += 1.0f; // Offset for G channel bins
                    block_hist_rgb[2 * num_hist_bins_per_channel_rgb + b_bin] += 1.0f; // Offset for B channel bins
                    
                    // Create grayscale version of the block for direction calculation
                    current_block_gray_for_dir[i][j] = static_cast<unsigned char>(wgt[0] * r_val + wgt[1] * g_val + wgt[2] * b_val);
                    pixel_count_in_block++;
                }
            }

            if (pixel_count_in_block == 0) {
                 delete_2d_uchar_array(current_block_gray_for_dir, blockSize);
                 continue;
            }

            for (size_t k=0; k < block_hist_rgb.size(); ++k) {
                 block_hist_rgb[k] /= pixel_count_in_block; // Normalize
            }

            unsigned char avg_r_block = static_cast<unsigned char>(sum_r / pixel_count_in_block);
            unsigned char avg_g_block = static_cast<unsigned char>(sum_g / pixel_count_in_block);
            unsigned char avg_b_block = static_cast<unsigned char>(sum_b / pixel_count_in_block);
            unsigned char block_avg_luminance = static_cast<unsigned char>(wgt[0] * avg_r_block + wgt[1] * avg_g_block + wgt[2] * avg_b_block);

            int block_dir = calc_main_direction_rgb(current_block_gray_for_dir, blockSize);
            delete_2d_uchar_array(current_block_gray_for_dir, blockSize);

            std::string best_img_filename = "";
            std::vector<std::pair<std::string, float> > candidates;
            
            for (std::unordered_map<std::string, std::vector<float> >::const_iterator it = valid_db.begin(); it != valid_db.end(); ++it) {
                const std::string& filename = it->first;
                const std::vector<float>& db_hist_vals = it->second;

                if (filename.rfind(category + "/", 0) != 0) continue;

                size_t slash_pos = filename.find('/');
                size_t underscore_pos = filename.find('_', slash_pos + 1);
                if (underscore_pos != std::string::npos && slash_pos != std::string::npos) {
                    std::string lum_str = filename.substr(slash_pos + 1, underscore_pos - (slash_pos + 1));
                    try {
                        int file_avg_luminance = std::atoi(lum_str.c_str());
                        if (std::abs(file_avg_luminance - block_avg_luminance) > 48) {
                            continue;
                        }
                    } catch (const std::exception& e) { /* ignore */ }
                }
                
                float hist_diff = calc_hist_diff_rgb(block_hist_rgb, db_hist_vals);
                candidates.push_back(std::make_pair(filename, hist_diff));
            }

            // 先用直方圖差排序，取前20名
            std::sort(candidates.begin(), candidates.end(),
                [](const std::pair<std::string, float>& a, const std::pair<std::string, float>& b) {
                    return a.second < b.second;
                });
            if (candidates.size() > 20) {
                candidates.resize(20);
            }

            // 在這20張裡找結構（主方向）最接近的10張
            std::vector<std::tuple<std::string, int, float>> structure_candidates; // <filename, dir_diff, hist_diff>
            for (size_t idx = 0; idx < candidates.size(); ++idx) {
                const std::pair<std::string, float>& cand_pair = candidates[idx];
                RGBImage smallImg;
                if (!smallImg.LoadImage(std::string(current_dir_path) + "/SmallImg/small_rgb/" + cand_pair.first)) continue;
                if (smallImg.w != blockSize || smallImg.h != blockSize || !smallImg.pixels) continue;

                // 轉灰階
                unsigned char** small_img_gray = create_2d_uchar_array(blockSize, blockSize);
                for (int i = 0; i < blockSize; ++i)
                    for (int j = 0; j < blockSize; ++j)
                        small_img_gray[i][j] = static_cast<unsigned char>(
                            wgt[0] * std::max(0, std::min(255, smallImg.pixels[i][j][0])) +
                            wgt[1] * std::max(0, std::min(255, smallImg.pixels[i][j][1])) +
                            wgt[2] * std::max(0, std::min(255, smallImg.pixels[i][j][2]))
                        );
                int cand_dir = calc_main_direction_rgb(small_img_gray, blockSize);
                delete_2d_uchar_array(small_img_gray, blockSize);

                int diff1 = abs(cand_dir - block_dir);
                int diff2 = 8 - diff1;
                int dir_diff = std::min(diff1, diff2);

                structure_candidates.push_back(std::make_tuple(cand_pair.first, dir_diff, cand_pair.second));
            }

            // 依照主方向差排序，取前10名
            std::sort(structure_candidates.begin(), structure_candidates.end(),
                [](const std::tuple<std::string, int, float>& a, const std::tuple<std::string, int, float>& b) {
                    return std::get<1>(a) < std::get<1>(b);
                });
            if (structure_candidates.size() > 10) {
                structure_candidates.resize(10);
            }

            // 在這10張裡，比RGB平均色距離，取前3名隨機選一張
            std::vector<std::pair<std::string, float>> rgb_dist_list;
            for (size_t idx = 0; idx < structure_candidates.size(); ++idx) {
                const std::string& fname = std::get<0>(structure_candidates[idx]);
                RGBImage smallImg;
                if (!smallImg.LoadImage(std::string(current_dir_path) + "/SmallImg/small_rgb/" + fname)) continue;
                if (smallImg.w != blockSize || smallImg.h != blockSize || !smallImg.pixels) continue;

                long long sum_r = 0, sum_g = 0, sum_b = 0;
                for (int i = 0; i < blockSize; ++i) {
                    for (int j = 0; j < blockSize; ++j) {
                        sum_r += std::max(0, std::min(255, smallImg.pixels[i][j][0]));
                        sum_g += std::max(0, std::min(255, smallImg.pixels[i][j][1]));
                        sum_b += std::max(0, std::min(255, smallImg.pixels[i][j][2]));
                    }
                }
                int total = blockSize * blockSize;
                float avg_r = sum_r / float(total);
                float avg_g = sum_g / float(total);
                float avg_b = sum_b / float(total);

                float rgb_dist = std::sqrt(
                    (avg_r - avg_r_block) * (avg_r - avg_r_block) +
                    (avg_g - avg_g_block) * (avg_g - avg_g_block) +
                    (avg_b - avg_b_block) * (avg_b - avg_b_block)
                );
                rgb_dist_list.push_back(std::make_pair(fname, rgb_dist));
            }

            // 取前3名
            std::sort(rgb_dist_list.begin(), rgb_dist_list.end(),
                [](const std::pair<std::string, float>& a, const std::pair<std::string, float>& b) {
                    return a.second < b.second;
                });
            int topN = std::min(3, (int)rgb_dist_list.size());
            if (topN > 0) {
                int pick = rand() % topN;
                best_img_filename = rgb_dist_list[pick].first;
            }

            if (best_img_filename != "") {
                RGBImage smallImgToPlace;
                if (smallImgToPlace.LoadImage(std::string(current_dir_path) + "/SmallImg/small_rgb/" + best_img_filename) &&
                    smallImgToPlace.w == blockSize && smallImgToPlace.h == blockSize && smallImgToPlace.pixels) {
                    for (int i = 0; i < blockSize; ++i) {
                        for (int j = 0; j < blockSize; ++j) {
                            mosaic_rgb_uchar[by * blockSize + i][bx * blockSize + j][0] = static_cast<unsigned char>(std::max(0, std::min(255, smallImgToPlace.pixels[i][j][0])));
                            mosaic_rgb_uchar[by * blockSize + i][bx * blockSize + j][1] = static_cast<unsigned char>(std::max(0, std::min(255, smallImgToPlace.pixels[i][j][1])));
                            mosaic_rgb_uchar[by * blockSize + i][bx * blockSize + j][2] = static_cast<unsigned char>(std::max(0, std::min(255, smallImgToPlace.pixels[i][j][2])));
                        }
                    }
                } else {
                    best_img_filename = ""; 
                }
            }

            if (best_img_filename == "") {
                for (int i = 0; i < blockSize; ++i) {
                    for (int j = 0; j < blockSize; ++j) {
                        mosaic_rgb_uchar[by * blockSize + i][bx * blockSize + j][0] = avg_r_block;
                        mosaic_rgb_uchar[by * blockSize + i][bx * blockSize + j][1] = avg_g_block;
                        mosaic_rgb_uchar[by * blockSize + i][bx * blockSize + j][2] = avg_b_block;
                    }
                }
            }
        }
        std::cout << "Processed RGB block row " << by + 1 << " of " << blocks_y << std::endl;
    }

    // Update main RGBImage's pixels
    if (this->pixels) {
        for (int i = 0; i < this->h; ++i) {
            if (this->pixels[i]) {
                for (int j = 0; j < this->w; ++j) delete[] this->pixels[i][j];
                delete[] this->pixels[i];
            }
        }
        delete[] this->pixels;
        this->pixels = NULL; // C++11 nullptr
    }

    this->w = out_w;
    this->h = out_h;
    this->pixels = new int**[this->h];
    for (int i = 0; i < this->h; ++i) {
        this->pixels[i] = new int*[this->w];
        for (int j = 0; j < this->w; ++j) {
            this->pixels[i][j] = new int[3];
            for (int c = 0; c < 3; ++c) {
                this->pixels[i][j][c] = static_cast<int>(mosaic_rgb_uchar[i][j][c]);
            }
        }
    }

    delete_3d_uchar_array(mosaic_rgb_uchar, out_h, out_w);
    std::cout << "RGB Mosaic generation complete. New dimensions: " << this->w << "x" << this->h << std::endl;
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
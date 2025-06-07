// GrayImage.cpp
#include <bitset>
#include "gray_image.h"
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
#include "gray_image.h"
#include "data_loader.h"
#include <cstdlib> // for rand(), srand()
#include <ctime>   // for time()
#include <unistd.h>
#include <sys/stat.h>
using namespace std;
const int dx[8][3][3] = {
    {{ 1, 0,-1}, { 2, 0,-2}, { 1, 0,-1}},    // 0. →
    {{ 0, 1, 2}, {-1, 0, 1}, {-2,-1, 0}},    // 1. ↗
    {{ 1, 2, 1}, { 0, 0, 0}, {-1,-2,-1}},    // 2. ↑
    {{ 2, 1, 0}, { 1, 0,-1}, { 0,-1,-2}},    // 3. ↖
    {{-1, 0, 1}, {-2, 0, 2}, {-1, 0, 1}},    // 4. ←
    {{ 0,-1,-2}, { 1, 0,-1}, { 2, 1, 0}},    // 5. ↙
    {{-1,-2,-1}, { 0, 0, 0}, { 1, 2, 1}},    // 6. ↓
    {{-2,-1, 0}, {-1, 0, 1}, { 0, 1, 2}}     // 7. ↘
};



// 計算一張灰階圖的主方向（回傳0~7，分別代表八個方向）
int calc_main_direction(const vector<vector<unsigned char>>& img, int blockSize) {
    vector<float> direction_strength(8, 0.0f);
    for (int d = 0; d < 8; ++d) {
        float sum = 0;
        for (int i = 1; i < blockSize-1; ++i) {
            for (int j = 1; j < blockSize-1; ++j) {
                float val = 0;
                for (int ki = -1; ki <= 1; ++ki)
                    for (int kj = -1; kj <= 1; ++kj)
                        val += img[i+ki][j+kj] * dx[d][ki+1][kj+1];
                sum += abs(val);
            }
        }
        direction_strength[d] = sum;
    }
    return max_element(direction_strength.begin(), direction_strength.end()) - direction_strength.begin();
}

bool File_Exists(const std::string& name) {
    struct stat buffer;
    return (stat(name.c_str(), &buffer) == 0);
}

map<string, vector<float>> load_simple_histogram_json(const string& filename) {
    map<string, vector<float>> result;
    ifstream fin(filename);
    if (!fin.is_open()) {
        cerr << "Cannot open file: " << filename << endl;
        return result;
    }

    string line, current_key;
    while (getline(fin, line)) {
        if (line.empty() || line == "{" || line == "}" || line == "]") continue;

        if (line.find(":") != string::npos) {
            int q1 = line.find("\"");
            int q2 = line.find("\"", q1 + 1);
            current_key = line.substr(q1 + 1, q2 - q1 - 1);

            // 檢查這一行有沒有 [
            size_t bracket_pos = line.find("[");
            if (bracket_pos != string::npos) {
                vector<float> values;
                size_t start = bracket_pos + 1;
                size_t end = line.find("]");
                string content = (end != string::npos) ? line.substr(start, end - start) : line.substr(start);

                stringstream ss(content);
                string token;
                while (getline(ss, token, ',')) {
                    if (!token.empty())
                        try { values.push_back(stof(token)); } catch (...) {}
                }

                // 如果沒遇到 ]，繼續讀多行
                while (end == string::npos && getline(fin, line)) {
                    end = line.find("]");
                    string line_content = (end != string::npos) ? line.substr(0, end) : line;
                    stringstream ss2(line_content);
                    while (getline(ss2, token, ',')) {
                        if (!token.empty())
                            try { values.push_back(stof(token)); } catch (...) {}
                    }
                }
                result[current_key] = values;
            }
        }
    }
    return result;
}

float calc_hist_diff(const vector<float>& a, const vector<float>& b) {
    float diff = 0;
    for (size_t i = 0; i < a.size(); ++i)
        diff += pow(a[i] - b[i], 2);
    return diff;
}
void GrayImage::small() {
    cout << "=== Mosaic using Small Images ===\n";
    cout << "Choose a collection:\n";
    string name[10] = { "airplane","automobile","bird","cat","deer","dog","frog","horse","ship","truck" };
    for (int i = 0; i < 10; ++i)
        cout << i + 1 << ". " << name[i] << "\n";
    int ind;
    cin >> ind;
    string category = name[ind - 1];

    string json_path = "SmallImg/out_json/" + category + ".json";
    auto db = load_simple_histogram_json(json_path);

    const int blockSize = 32;
    int blocks_y = h / blockSize;
    int blocks_x = w / blockSize;

    int out_w = blocks_x * blockSize;
    int out_h = blocks_y * blockSize;
    vector<vector<unsigned char>> mosaic(out_h, vector<unsigned char>(out_w));

    // 只做一次 valid_db 篩選
    cout<< "Loading valid images from database...\n";
    unordered_map<string, vector<float>> valid_db;
    for (auto it = db.begin(); it != db.end(); ++it) {
        string filename = it->first;
        vector<float> hist_vals = it->second;
        char cwd[1024];
        getcwd(cwd, sizeof(cwd));
        string fullpath = string(cwd) + "/SmallImg/gray_small/" + filename;
        if (File_Exists(fullpath)) {
            valid_db[filename] = hist_vals;
        } else {
            cout << "[DEBUG] File not found: [" << fullpath << "] len=" << fullpath.length() << endl;
            for (size_t i = 0; i < fullpath.length(); ++i) {
                cout << int(fullpath[i]) << " ";
            }
            cout << endl;
        }
    }
    cout << "Loaded valid_db size: " << valid_db.size() << endl;
    for (int by = 0; by < blocks_y; ++by) {
        for (int bx = 0; bx < blocks_x; ++bx) {
            vector<float> hist(16, 0.0);
            int cnt = 0;
            for (int i = 0; i < blockSize; ++i) {
                for (int j = 0; j < blockSize; ++j) {
                    int y = by * blockSize + i;
                    int x = bx * blockSize + j;
                    if (y < h && x < w) {
                        int level = pixels[y][x] / 16;
                        hist[level] += 1.0;
                        ++cnt;
                    }
                }
            }
            for (float& v : hist) v /= cnt;

            int sum = 0;
            for (int i = 0; i < blockSize; ++i)
                for (int j = 0; j < blockSize; ++j)
                    sum += pixels[by * blockSize + i][bx * blockSize + j];
            int avg = sum / (blockSize * blockSize);
            int avg_level = avg / 16;
            int avg_gray = avg_level * 16;
            string best_img = "";
            float best_diff = 1e9;

            // 1. 計算目前區塊的主方向
            vector<vector<unsigned char>> block_img(blockSize, vector<unsigned char>(blockSize));
            for (int i = 0; i < blockSize; ++i)
                for (int j = 0; j < blockSize; ++j)
                    block_img[i][j] = pixels[by * blockSize + i][bx * blockSize + j];
            int block_dir = calc_main_direction(block_img, blockSize);

            // 2. 先用直方圖挑前N名
            vector<pair<string, float>> candidates;
            for (auto it = valid_db.begin(); it != valid_db.end(); ++it) {
                const string& filename = it->first;
                const vector<float>& hist_vals = it->second;
                if (filename.find(category + "/") != 0) continue;
                size_t slash_pos = filename.find('/');
                string name_part = filename.substr(slash_pos + 1);
                size_t underscore_pos = name_part.find('_');
                if (underscore_pos == string::npos) continue;
                int file_avg_gray = stoi(name_part.substr(0, underscore_pos));
                if (abs(file_avg_gray - avg) > 48) continue;
                float diff = calc_hist_diff(hist, hist_vals);
                candidates.push_back(std::make_pair(filename, diff));
            }
            std::sort(candidates.begin(), candidates.end(),
                [](const std::pair<std::string, float>& a, const std::pair<std::string, float>& b) {
                    return a.second < b.second;
                });
            if (candidates.size() > 20) candidates.resize(20);

            // 3. 在候選中找方向最接近的
            float best_score = 1e9;
            for (size_t idx = 0; idx < candidates.size(); ++idx) {
                GrayImage smallImg;
                if (!smallImg.LoadImage("SmallImg/gray_small/" + candidates[idx].first)) continue;
                vector<vector<unsigned char>> small_block(blockSize, vector<unsigned char>(blockSize));
                for (int i = 0; i < blockSize; ++i)
                    for (int j = 0; j < blockSize; ++j)
                        small_block[i][j] = smallImg.pixels[i][j];
                int cand_dir = calc_main_direction(small_block, blockSize);
                int diff1 = abs(cand_dir - block_dir);
                int diff2 = 8 - diff1;
                int dir_diff = std::min(diff1, diff2);
                float score = dir_diff; // 1000 可依實驗調整
                if (score < best_score) {
                    best_score = score;
                    best_img = candidates[idx].first;
                } else if (score == best_score) {
                    if ((rand() % 10) < 3) {//變的機率
                        best_img = candidates[idx].first;
                    }
                }
            }

            // fallback: 若沒找到，還是用直方圖最小的
            if (best_img == "" && !candidates.empty()) {
                best_img = candidates[0].first;
            }

            // fallback: 只比對類別，不比 avg_gray
            if (best_img == "") {
                float best_diff = 1e9;
                for (auto it = valid_db.begin(); it != valid_db.end(); ++it) {
                    const string& filename = it->first;
                    const vector<float>& hist_vals = it->second;
                    if (filename.find(category + "/") != 0) continue;
                    float diff = calc_hist_diff(hist, hist_vals);
                    if (diff < best_diff) {
                        best_diff = diff;
                        best_img = filename;
                    }
                }
            }

            // fallback: 完全不比對類別，直接全庫找
            if (best_img == "") {
                float best_diff = 1e9;
                for (auto it = valid_db.begin(); it != valid_db.end(); ++it) {
                    const string& filename = it->first;
                    const vector<float>& hist_vals = it->second;
                    float diff = calc_hist_diff(hist, hist_vals);
                    if (diff < best_diff) {
                        best_diff = diff;
                        best_img = filename;
                    }
                }
            }

            if (best_img == "") {
                // 真的找不到才 fallback 灰階
                for (int i = 0; i < blockSize; ++i)
                    for (int j = 0; j < blockSize; ++j)
                        mosaic[by * blockSize + i][bx * blockSize + j] = avg;
                // debug
                //cout << "[WARN] Block (" << by << "," << bx << ") fallback to gray avg=" << avg << endl;
                continue;
            }

            GrayImage smallImg;
            bool ok = smallImg.LoadImage("SmallImg/gray_small/" + best_img);
            if (!ok) {
                cout << "Failed to load image: " << best_img << ", fallback to avg block.\n";
                for (int i = 0; i < blockSize; ++i)
                    for (int j = 0; j < blockSize; ++j)
                        mosaic[by * blockSize + i][bx * blockSize + j] = avg;
                continue;
            }

            for (int i = 0; i < blockSize; ++i) {
                for (int j = 0; j < blockSize; ++j) {
                    mosaic[by * blockSize + i][bx * blockSize + j] = smallImg.pixels[i][j];
                }
            }
        }
    }

    // 釋放舊 pixels
    if (pixels) {
        for (int i = 0; i < h; ++i) delete[] pixels[i];
        delete[] pixels;
    }

    pixels = new int*[out_h];
    for (int i = 0; i < out_h; ++i) {
        pixels[i] = new int[out_w];
        for (int j = 0; j < out_w; ++j) {
            pixels[i][j] = static_cast<int>(mosaic[i][j]);
        }
    }

    w = out_w;
    h = out_h;
}



bool GrayImage::IsSameAs(const Image& other) const {
    const GrayImage* otherGray = dynamic_cast<const GrayImage*>(&other);
    if (!otherGray) return false;

    if (w != otherGray->w || h != otherGray->h) return false;
    
    for (int i = 0; i < h; ++i) {
        for (int j = 0; j < w; ++j) {
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
void GrayImage::Disp_small(){
    //int **pixels, int w int h into small pic out ascii (max w or h =40)
    int neww,newh,sz;
    
    if(w>h){
        if(w<40){
            Display_ASCII();
            return;
        }
        sz=w/40;
        neww=w/sz;
        newh=h/sz;
    }else{
        if(h<40){
            Display_ASCII();
            return;
        }
        sz=h/40;
        neww=w/sz;
        newh=h/sz;
    }
    GrayImage *smallg=new GrayImage();
    smallg->w = neww;
    smallg->h = newh;
    cout<<"ahhhhhh"<<endl;
    smallg->pixels=new int*[newh];
    for(int i=0;i<newh;i++){
        smallg->pixels[i] = new int[neww];
        for (int j = 0; j < neww; j++) {
            int sum = 0, count = 0;
            for (int y = 0; y < sz; y++) {
                for (int x = 0; x < sz; x++) {
                    int src_y = i * sz + y;
                    int src_x = j * sz + x;
                    if (src_y < h && src_x < w) {
                        sum += pixels[src_y][src_x];
                        count++;
                    }
                }
            }
            smallg->pixels[i][j] = count ? sum / count : 0;
        }
    }

    data_loader.Display_Gray_ASCII(smallg->w, smallg->h, smallg->pixels);
    
    delete smallg;
}
/*
GrayImage& GrayImage::operator=(GrayImage &img) {
    if (this != &img) {
        // ������즳�O����
        if (pixels) {
            for (int i = 0; i < h; ++i) {
                delete[] pixels[i];
            }
            delete[] pixels;
        }

        w = img.w;
        h = img.h;

        // ���t�s�O����
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
            if (pixels) {
                for (int i = 0; i < h; ++i) {
                    delete[] pixels[i];
                }
                delete[] pixels;
            }

            w = grayImg->w;
            h = grayImg->h;

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
    }else{
        cout<<"Encrypt Failed. SOmething bad happened. Maybe message is too long.\nMessage may not be fully stored."<<endl;
    }  
}
string GrayImage::DecryptMessage() const {
    string ms;
    ms=ImageEncryption::DecodeMessage( pixels,  w,  h);
    
    return ms;

}
void GrayImage::DCTEncryptMessage(const string &message){
    if(ImageEncryption::DCTEncodeMessage( pixels, w,  h, message)){
        cout<<"success Encrypt!!!"<<endl;
    }else{
        cout<<"Encrypt Failed. SOmething bad happened. Maybe message is too long.\nMessage may not be fully stored."<<endl;
    }  
}
string GrayImage::DCTDecryptMessage() const {
    string ms;
    ms=ImageEncryption::DCTDecodeMessage( pixels,  w,  h);
    
    return ms;

}
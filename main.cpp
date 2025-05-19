#include "image.h"
#include "gray_image.h"
#include "rgb_image.h"
#include "image_encryption.h"
#include "bit_field_filter.h"

#include <stdexcept>
#include <algorithm>
#include <limits>
#include <vector>
#include <bitset>
#include <sstream>

#define CASE_ONE    0b00000001  // 增亮
#define CASE_TWO    0b00000010  // 反轉
#define CASE_THREE  0b00000100  // 降噪
#define CASE_FOUR   0b00001000  // 門檻化
#define CASE_FIVE   0b00010000  // 銳化
#define CASE_SEVEN  0b01000000  // 水平翻轉
#define CASE_EIGHT  0b10000000  // 魚眼

using namespace std;
/*
class SHA256 {
public:
    SHA256();
    string hash(const string &data);

private:
    static const uint32_t k[64];
    uint32_t h[8];
    vector<uint32_t> preprocess(const string &data);
    void transform(const vector<uint32_t> &blocks);
    static uint32_t rotr(uint32_t x, uint32_t n);
    static uint32_t choose(uint32_t e, uint32_t f, uint32_t g);
    static uint32_t majority(uint32_t a, uint32_t b, uint32_t c);
    static uint32_t sig0(uint32_t x);
    static uint32_t sig1(uint32_t x);
};

const uint32_t SHA256::k[64] = {
    0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5,
    0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
    0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3,
    0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
    0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc,
    0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
    0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7,
    0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
    0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13,
    0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
    0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3,
    0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
    0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5,
    0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
    0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208,
    0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2
};

SHA256::SHA256() {
    h[0] = 0x6a09e667;
    h[1] = 0xbb67ae85;
    h[2] = 0x3c6ef372;
    h[3] = 0xa54ff53a;
    h[4] = 0x510e527f;
    h[5] = 0x9b05688c;
    h[6] = 0x1f83d9ab;
    h[7] = 0x5be0cd19;
}

uint32_t SHA256::rotr(uint32_t x, uint32_t n) {
    return (x >> n) | (x << (32 - n));
}

uint32_t SHA256::choose(uint32_t e, uint32_t f, uint32_t g) {
    return (e & f) ^ (~e & g);
}

uint32_t SHA256::majority(uint32_t a, uint32_t b, uint32_t c) {
    return (a & b) ^ (a & c) ^ (b & c);
}

uint32_t SHA256::sig0(uint32_t x) {
    return rotr(x, 2) ^ rotr(x, 13) ^ rotr(x, 22);
}

uint32_t SHA256::sig1(uint32_t x) {
    return rotr(x, 6) ^ rotr(x, 11) ^ rotr(x, 25);
}

vector<uint32_t> SHA256::preprocess(const string &data) {
    size_t bit_len = data.size() * 8;
    vector<uint8_t> buffer(data.begin(), data.end());
    buffer.push_back(0x80);

    while ((buffer.size() * 8) % 512 != 448) {
        buffer.push_back(0x00);
    }

    for (int i = 7; i >= 0; --i) {
        buffer.push_back((bit_len >> (i * 8)) & 0xFF);
    }

    vector<uint32_t> blocks(buffer.size() / 4);
    for (size_t i = 0; i < blocks.size(); ++i) {
        blocks[i] = (buffer[4 * i] << 24) |
                    (buffer[4 * i + 1] << 16) |
                    (buffer[4 * i + 2] << 8) |
                    (buffer[4 * i + 3]);
    }
    return blocks;
}

void SHA256::transform(const vector<uint32_t> &blocks) {
    uint32_t w[64];
    for (size_t i = 0; i < blocks.size(); i += 16) {
        for (int t = 0; t < 16; ++t)
            w[t] = blocks[i + t];

        for (int t = 16; t < 64; ++t)
            w[t] = sig1(w[t - 2]) + w[t - 7] + sig0(w[t - 15]) + w[t - 16];

        uint32_t a = h[0], b = h[1], c = h[2], d = h[3];
        uint32_t e = h[4], f = h[5], g = h[6], h0 = h[7];

        for (int t = 0; t < 64; ++t) {
            uint32_t T1 = h0 + sig1(e) + choose(e, f, g) + k[t] + w[t];
            uint32_t T2 = sig0(a) + majority(a, b, c);
            h0 = g;
            g = f;
            f = e;
            e = d + T1;
            d = c;
            c = b;
            b = a;
            a = T1 + T2;
        }

        h[0] += a;
        h[1] += b;
        h[2] += c;
        h[3] += d;
        h[4] += e;
        h[5] += f;
        h[6] += g;
        h[7] += h0;
    }
}

string SHA256::hash(const string &data) {
    vector<uint32_t> blocks = preprocess(data);
    transform(blocks);

    stringstream ss;
    for (int i = 0; i < 8; ++i) {
        ss << hex << setw(8) << setfill('0') << h[i];
    }
    return ss.str();
}
*/
long long getLongLongInput(const std::string& prompt) {
    long long value;
    while (true) {
        std::cout << prompt;
        std::cin >> value;
        if (std::cin.good()) {
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            return value;
        } else {
            std::cout << "Invalid, again.\n";
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        }
    }
}

void CaesarEncrypt(std::string &s, int shift) {
    int actual_shift_alpha = (shift % 26 + 26) % 26;
    int actual_shift_digit = (shift % 10 + 10) % 10;

    for (char &c : s) {
        if (isalpha(c)) {
            char base = islower(c) ? 'a' : 'A';
            c = (c - base + actual_shift_alpha) % 26 + base;
        } else if (isdigit(c)) {
            c = (c - '0' + actual_shift_digit) % 10 + '0';
        }
    }
}

void CaesarDecrypt(std::string &s, int shift) {
    int actual_shift_alpha = (shift % 26 + 26) % 26;
    int actual_shift_digit = (shift % 10 + 10) % 10;

    for (char &c : s) {
        if (isalpha(c)) {
            char base = islower(c) ? 'a' : 'A';
            c = (c - base - actual_shift_alpha + 26) % 26 + base;
        } else if (isdigit(c)) {
            c = (c - '0' - actual_shift_digit + 10) % 10 + '0';
        }
    }
}


void XOREncryptDecrypt(std::string &s, const std::string& key) {
    if (key.empty()) {
        std::cerr << "XOR key cannot be null\n";
        return;
    }
    for (size_t i = 0; i < s.length(); ++i) {
        s[i] = s[i] ^ key[i % key.length()];
    }
}

long long power(long long base, long long exp, long long mod) {
    long long res = 1;
    base %= mod;
    while (exp > 0) {
        if (exp % 2 == 1) res = (res * base) % mod;
        base = (base * base) % mod;
        exp /= 2;
    }
    return res;
}

void RSAEncrypt(std::string &s, long long e_key, long long n_mod) {
    if (n_mod <= 0) {
         
         cout<<"RSA n must be positive. Error stop."<<endl;
         
         return;
    }
    if (n_mod <= 255 && n_mod > 1) {
    } else if (n_mod <=1) {
         std::cerr << "RSA 模數 n 太小！將不執行 RSA 加密。\n";
         return;
    }


    for (char &c : s) {
        long long m = static_cast<unsigned char>(c);
        long long encrypted_val = power(m, e_key, n_mod);
        
        if (encrypted_val > 255 || encrypted_val < 0) {
            
            cout<<endl<<"RSA warning: char out of ASCII 0-255 error stop."<<endl;
        }
        c = static_cast<char>(encrypted_val);
    }
}

void RSADecrypt(std::string &s, long long d_key, long long n_mod) {
    

    for (char &c : s) {
        long long encrypted_val = static_cast<unsigned char>(c);
        long long decrypted_val = power(encrypted_val, d_key, n_mod);
        c = static_cast<char>(decrypted_val);
    }
}


int main(int argc, char *argv[]){
 /*
    Image *img1 = new GrayImage();
    img1->LoadImage("Image-Folder/lena.jpg");
    img1->DumpImage("img1.jpg");
    img1->Display_X_Server();
    img1->Display_CMD();
    
   

    Image *img2 = new RGBImage();
    img2->LoadImage("Image-Folder/lena.jpg");
    img2->DumpImage("img2.jpg");
    img2->Display_X_Server();
    img2->Display_CMD();
*/
    // some bit field filter design driven code here

    // some photo mosaic driven code here
    vector<string> filenames;
    Image *img3 = nullptr;
    string savename;
    while(true){
        filenames.clear();
        cout<<"''''''''''''''''''''''''''''''''''''''''''''''"<<endl;
        cout<<"\033[5m\033[1;32m               ______________\n         ,===:'.,            `-._\n              `:.`---.__         `-._\n                `:.     `--.         `.\n                  \\.        `.         `.\n          (,,(,    \\.         `.   ____,-`.,\n       (,'     `/   \\.   ,--.___`.'\n   ,  ,'  ,--.  `,   \\.;'         `\n    `{o, {    \\  :    \\;\n      V,,'    /  /    //\n      |;;    /  ,' ,-//.    ,---.      ,\n      \\;'   /  ,' /  _  \\  /  _  \\   ,'/\n            \\   `'  / \\  `'  / \\  `.' /\n             `.___,'   `.__,'   `.__,'  \n\033[0m"<<endl;
        cout<<"''''''''''''''''''''''''''''''''''''''''''''''"<<endl;
        Data_Loader data_loader;
        cout<<"0. Show All Small Images"<<endl;
        data_loader.List_Directory("Image-Folder", filenames); 
        string filename;
        int imgchoose;
        for(size_t i=0; i<filenames.size(); i++){
            //GrayImage *gg=new GrayImage();
            cout<<i+1<<". "<<filenames[i]<<endl;
        }
        cout<<filenames.size()+1<<". Exit Program"<<endl;
        cout << "Please enter the number of image file or 0 for show all: ";
        cin>>imgchoose;
        if(imgchoose==0){
            for(size_t i=0; i<filenames.size(); i++){
                //GrayImage *gg=new GrayImage();
                string filename = filenames[i];
    
                string lower = filename;
                transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
                if (lower.size() >= 4 && (lower.substr(lower.size() - 4) == ".jpg" || lower.substr(lower.size() - 4) == ".png")) 
                {
                    cout << i + 1 << ". " << filenames[i] << endl;
                    RGBImage gg;
                    gg.LoadImage(filenames[i]);
                    gg.Disp_small();
                }
            }
            cout<<filenames.size()+1<<". Exit Program"<<endl;
            cout << "Please enter the number of image file or 0 for show all: ";
            cin>>imgchoose;
        }
        
        if(imgchoose==filenames.size()+1){
        cout<<"Exit!"<<endl;
            break;
        }
        filename=filenames[imgchoose-1];
        cout<<endl;
        cout<<"========================"<<endl;
        cout<<"= CHOOSE YOUR IMG TYPE ="<<endl;
        cout<<"========================"<<endl;
        cout<<"1. Gray Image"<<endl;
        cout<<"2. RGB Image"<<endl;
        cout<<"0. Exit"<<endl;
        cout<<"========================"<<endl;
        int choice;
        cout<<"Please enter your choice: ";
        cin>>choice;
        if(choice==1){
            img3 = new GrayImage();
            img3->LoadImage(filename);
        }
        else if(choice==2){
            img3 = new RGBImage();
            img3->LoadImage(filename);
        }
        else if(choice==0){
            cout<<"Exit!"<<endl;
            break;
        }
        else{
            cout<<"Invalid choice!"<<endl;
        }
        char display;
        
        cout<<"==============================\n";
        cout<<" Display small img? (y/n)\n";
        cout<<"==============================\n";
        cin >>display;
        switch (display) {
            case 'n': 
                break;
            case 'y':
                img3->Disp_small();;
                break;
            default:
                cout<<"wrong input, again."<<endl;
                break;
        }
        


        uint8_t options = 0b00000000;
        char yn;
        cout<<"Do you want to use filter? (y/n) ";
        cin>>yn;
        if(yn == 'y' || yn == 'Y'){
            cout<<endl;
            cout<<"======================"<<endl;
            cout<<"= DECIDE YOUR FILTER ="<<endl;
            cout<<"======================"<<endl;
            cout<<"1.Brightening?(y/n) ";
            cin>>yn;
            if(yn == 'y' || yn == 'Y'){
                options |= CASE_ONE;
            }
            cout<<"2.Inversion?(y/n) ";
            cin>>yn;
            if(yn == 'y' || yn == 'Y'){
                options |= CASE_TWO;
            }
            cout<<"3.Denoising?(y/n) ";
            cin>>yn;
            if(yn == 'y' || yn == 'Y'){
                options |= CASE_THREE;
            }
            cout<<"4.Thresholding?(y/n) ";
            cin>>yn;
            if(yn == 'y' || yn == 'Y'){
                options |= CASE_FOUR;
            }
            cout<<"5.Sharpening?(y/n) ";
            cin>>yn;
            if(yn == 'y' || yn == 'Y'){
                options |= CASE_FIVE;
            }
            cout<<"6.Mosaic?(y/n) ";
            cin>>yn;
            if(yn == 'y' || yn == 'Y'){
                options |= CASE_SIX;
            }
            cout<<"7.Flip Horizontal?(y/n) ";
            cin>>yn;
            if(yn == 'y' || yn == 'Y'){
                options |= CASE_SEVEN;
            }
            cout<<"8.Fish Eye?(y/n) ";
            cin>>yn;
            if(yn == 'y' || yn == 'Y'){
                options |= CASE_EIGHT;
            }
        }
        cout<<endl;
        Image *tempimg =img3->Clone();

        tempimg->ApplyAllFilters(options);


        int d=256;
        while(!(d<=3&&d>=0)||d==256){
            cout<<"==============================\n";
            cout<<" Do you want to display img? \n";
            cout<<"------------------------------\n";
            cout<<"0.None\n1.ASCII\n2.X_Server\n3.Both\n";
            cout<<"==============================\n";
            cin >>display;
            d=display-'0';
            switch (d) {
                case 0:
                    
                    break;
                case 1:
                    tempimg->Display_ASCII();
                    break;
                case 2:
                    cout<<"Image showed. make sure to close it before next move."<<endl;
                    tempimg->Display_X_Server();
                    break;
                case 3:
                    tempimg->Display_ASCII();
                    cout<<"Image showed. make sure to close it before next move."<<endl;
                    tempimg->Display_X_Server();
                    break;
                default:
                    cout<<"wrong input, again."<<endl;
                    break;
            }
        }
        
        cout<<"Enter Image-Folder/\"filename\".png, default imgg.png or \"n\" for not saving: ";
        cin>>savename;
        if(savename.empty()){
            savename = "imgg";
        }
        if(savename!="n")tempimg->DumpImage("Image-Folder/"+savename+".png");
        ////////////////////////////////////////////////////
        //儲存檔案避免被覆蓋++密/////////////////////////////
        ///////////////////////////////////////////////////
        cout<<endl;
        cout<<"======================"<<endl;
        cout<<"= ENCRYPT YOUR IMAGE ="<<endl;
        cout<<"======================"<<endl;
        cout<<endl<<"Encrypt Filtered Image(y)? Original Image(o)? Exit(e)? Skip(n)?(y|o|e|n)"<<endl;
        cin>>yn;
        if(yn=='e'||yn=='E'){
            if(tempimg!=nullptr)delete tempimg;
            if (img3 != nullptr) delete img3;
    
            continue;
        }
        if(yn == 'y' || yn == 'Y'){
            string message;
            if(tempimg!=nullptr){
                delete img3;
                img3=tempimg->Clone();
            }
            cout << "Enter message to encrypt: ";
            cin.ignore();
            getline(cin, message,'\n');
            cout<<"====================================\n";
            cout<<"  Choose Your String Encrypt Type:\n";
            cout<<"------------------------------------\n";
            cout<<"1.RSA\n2.XOR\n3.CaesarChiper\n4.None\n";
            cout<<"====================================\n";
            char enct;
            int encnum;
            cin>>enct;
            switch(enct){
                case '1':
                    enct='r';
                break;
                case '2':
                    enct='x';
                break;
                case '3':
                    enct='c';
                break;
                case '4':
                    enct='n';
                break;
            }
            while(enct!='r'&&enct!='x'&&enct!='c'&&enct!='n'){
                cout<<"wrong type, again:";
                cin.ignore();
                cin>>enct;
                switch(enct){
                    case '1':
                        enct='r';
                    break;
                    case '2':
                        enct='x';
                    break;
                    case '3':
                        enct='c';
                    break;
                    case '4':
                        enct='n';
                    break;
                }
            }
            
            long long p, q, e_key;
            string xor_key_str;
            int caesar_shift_key;
            long long nmod;
            long long phin;
            switch(enct){
                case 'r':
                    cout<<"Doing RSA on your message..."<<endl;
                    cout<<"recommendation\n p,q,e,d:"<<endl;
                    cout<<" 13,17,5,77\n 11,19,7,103\n 7,31,7,103\n "<<endl;
                    cout<<"Input two prime numbers P, Q:"<<endl;
                    p = getLongLongInput("P=");
                    q = getLongLongInput("Q=");
                    while(p==q){
                        cout<<"P can't be same as Q, again:";
                        p = getLongLongInput("P=");
                        q = getLongLongInput("Q=");
                    }
                    nmod=p*q;
                    phin=(p-1)*(q-1);
                    cout<<"n_mod(P*Q)= "<<nmod<<endl;
                    cout<<"phi_n=(P-1)*(Q-1)= "<<phin<<endl;
                    cout<<endl<<"Input public key e(1<e<"<<phin<<" && e mod "<<phin<<"=1 ):";
                    e_key = getLongLongInput("");
                    cout<<endl<<"Input private key d(d*e mod "<<phin<<" = 1):";
                    RSAEncrypt(message, e_key, nmod);
                break;
                case 'x':
                    cout<<"Doing XOR on your message..."<<endl;
                    cout<<"Input XOR KEY string:";
                    cin.ignore();
                    getline(cin, xor_key_str);
                    XOREncryptDecrypt(message, xor_key_str);
                    cout<<"Remember Your XOR KEY["<<xor_key_str<<"]"<<endl;
                break;
                case 'c':
                    cout<<"Do Caesar Chiper on your message..."<<endl;
                    caesar_shift_key = static_cast<int>(getLongLongInput("Caesar shift initial number?: "));
                    CaesarEncrypt(message,caesar_shift_key);
                break;
                case 'n':
                    cout<<"Nothing to do with your message."<<endl;
                break;
            }
            cout<<"Encrpting Image: "<<filename<<endl<<"with message: "<<message<<endl;
            
            char len = static_cast<char>(message.length());
            message.insert(0, 1, len);
            
            img3->EncryptMessage(message); 
            cout << "Message encrypted into filtered image." << endl;
            cout<<"Enter Image-Folder/\"filename\".png, default imgg.png or \"n\" for not saving: ";
            cin>>savename;
            if(savename.empty()){
                savename = "imgg";
            }
            if(savename!="n")img3->DumpImage("Image-Folder/"+savename+".png");
            
            d=256;
            while(!(d<=3&&d>=0)||d==256){
                cout<<"==============================\n";
                cout<<" Do you want to display img? \n";
                cout<<"------------------------------\n";
                cout<<"0.None\n1.ASCII\n2.X_Server\n3.Both\n";
                cout<<"==============================\n";
                cin >>display;
                d=display-'0';
                switch (d) {
                    case 0:
                        
                        break;
                    case 1:
                        tempimg->Display_ASCII();
                        break;
                    case 2:
                        cout<<"Image showed. make sure to close it before next move."<<endl;
                        tempimg->Display_X_Server();
                        break;
                    case 3:
                        tempimg->Display_ASCII();
                        cout<<"Image showed. make sure to close it before next move."<<endl;
                        tempimg->Display_X_Server();
                        break;
                    default:
                        cout<<"wrong input, again."<<endl;
                        break;
                }
            }
            
        }else if (yn == 'o' || yn == 'O') {
            string message;
            cout << "Enter message to encrypt: ";
            cin.ignore();
            getline(cin, message,'\n');
            cout<<"====================================\n";
            cout<<"  Choose Your String Encrypt Type:\n";
            cout<<"------------------------------------\n";
            cout<<"1.RSA\n2.XOR\n3.CaesarChiper\n4.None\n";
            cout<<"====================================\n";
            char enct;
            int encnum;
            cin>>enct;
            switch(enct){
                case '1':
                    enct='r';
                break;
                case '2':
                    enct='x';
                break;
                case '3':
                    enct='c';
                break;
                case '4':
                    enct='n';
                break;
            }
            while(enct!='r'&&enct!='x'&&enct!='c'&&enct!='n'){
                cout<<"wrong type, again:";
                
                cin>>enct;
                switch(enct){
                    case '1':
                        enct='r';
                    break;
                    case '2':
                        enct='x';
                    break;
                    case '3':
                        enct='c';
                    break;
                    case '4':
                        enct='n';
                    break;
                }
            }
            long long p, q, e_key, d_key;
            string xor_key_str;
            long long nmod;
            long long phin;
            int caesar_shift_key;
            switch(enct){
                case 'r':
                    cout<<"Doing RSA on your message..."<<endl;
                    cout<<"recommendation\n p  q  e  d :"<<endl;
                    cout<<" 13 17  5 77\n"<<endl;
                    cout<<"Input two prime numbers P, Q:"<<endl;
                    p = getLongLongInput("P=");
                    q = getLongLongInput("Q=");
                    while(p==q){
                        cout<<"P can't be same as Q, again:";
                        p = getLongLongInput("P=");
                        q = getLongLongInput("Q=");
                    }
                    nmod=p*q;
                    phin=(p-1)*(q-1);
                    cout<<"n_mod(P*Q)= "<<nmod<<endl;
                    cout<<"phi_n=(P-1)*(Q-1)= "<<phin<<endl;
                    cout<<endl<<"Input public key e(1<e<"<<phin<<" && e mod "<<phin<<"=1 ):";
                    e_key = getLongLongInput("");
                    cout<<endl<<"Input private key d(d*e mod "<<phin<<" = 1):";
                    d_key = getLongLongInput("");
                    RSAEncrypt(message, e_key, nmod);
                break;
                case 'x':
                    cout<<"Doing XOR on your message..."<<endl;
                    cout<<"Input XOR KEY string:";
                    cin.ignore();
                    getline(cin, xor_key_str);
                    XOREncryptDecrypt(message, xor_key_str);
                    cout<<"Remember Your XOR KEY["<<xor_key_str<<"]"<<endl;
                break;
                case 'c':
                    cout<<"Doing Caesar Chiper on your message..."<<endl;
                    caesar_shift_key = static_cast<int>(getLongLongInput("Caesar shift initial number?: "));
                    CaesarEncrypt(message,caesar_shift_key);
                break;
                case 'n':
                    cout<<"Nothing to do with your message."<<endl;
                break;
            }
            cout<<"Encrpting Image: "<<filename<<endl<<"with message: "<<message<<endl;
            char len = static_cast<char>(message.length());
            message.insert(0, 1, len);
            img3->EncryptMessage(message);
            cout << "Message encrypted into filtered image." << endl;
            cout<<"Enter Image-Folder/\"filename\".png, default imgg.png or \"n\" for not saving: ";
            cin>>savename;
            if(savename.empty()){
                savename = "imgg";
            }
            img3->DumpImage("Image-Folder/"+savename+".png");
            d=256;
            while(!(d<=3&&d>=0)||d==256){
                cout<<"==============================\n";
                cout<<" Do you want to display img? \n";
                cout<<"------------------------------\n";
                cout<<"0.None\n1.ASCII\n2.X_Server\n3.Both\n";
                cout<<"==============================\n";
                cin >>display;
                d=display-'0';
                switch (d) {
                    case 0:
                        
                        break;
                    case 1:
                        tempimg->Display_ASCII();
                        break;
                    case 2:
                        cout<<"Image showed. make sure to close it before next move."<<endl;
                        tempimg->Display_X_Server();
                        break;
                    case 3:
                        tempimg->Display_ASCII();
                        cout<<"Image showed. make sure to close it before next move."<<endl;
                        tempimg->Display_X_Server();
                        break;
                    default:
                        cout<<"wrong input, again."<<endl;
                        break;
                }
            }
            /*Image *imgt = new GrayImage();
            imgt->LoadImage("saveimg/"+savename+".png");
            imgt->DecryptMessage();
            cout<<endl;
            img3->DecryptMessage();
            //img4->Display_X_Server();
            if(img3->IsSameAs(*imgt)){
                cout<<"same"<<endl;
            }else{
                cout<<"not same"<<endl;
            }
            delete imgt;*/
        }
        if(tempimg!=nullptr)delete tempimg;
        cout<<"====================="<<endl;
        cout<<"= DECODE YOUR IMAGE ="<<endl;
        cout<<"====================="<<endl;
        cout<<endl<<"Decode Image(y)? Exit(e)? Skip(s)?(y|e|s)"<<endl;
        cin>>yn;
        if(yn=='e'){
            delete img3;
            continue;
        }

        if(yn == 'y' || yn == 'Y'){
        /*
            cout<<"Enter filename: "<<endl;
            string Decodename;
            cin>>Decodename;
            if(Decodename.empty()){
                cout<<"???"<<endl;
            }
            */
            Image *img4 =img3->Clone();
            string demessage;
            demessage = img4->DecryptMessage();
            
            if (demessage.empty()) {
                cout << "No hidden message found or image is not encrypted." << endl;
            } else {
                cout << "Decoded message: " <<endl<< demessage << endl;
            }
            
            

            cout<<"==========================================\n";
            cout<<" Choose Your String Encrypt Type(if any):\n";
            cout<<"------------------------------------\n";
            cout<<"1.RSA\n2.XOR\n3.CaesarChiper\n4.None\n";
            cout<<"==========================================\n";
            char enct;
            int encnum;
            cin>>encnum;
            switch(encnum){
                case 1:
                    enct='r';
                break;
                case 2:
                    enct='x';
                break;
                case 3:
                    enct='c';
                break;
                case 4:
                    enct='n';
                break;
            }
            while(enct!='r'&&enct!='x'&&enct!='c'&&enct!='n'){
                cout<<"wrong type, again:";
                cin>>encnum;
                switch(encnum){
                    case 1:
                        enct='r';
                    break;
                    case 2:
                        enct='x';
                    break;
                    case 3:
                        enct='c';
                    break;
                    case 4:
                        enct='n';
                    break;
                }
            }
            long long p, q, e_key, d_key;
            long long nmod;
            long long phin;
            string xor_key_str;
            int caesar_shift_key;
            switch(enct){
                case 'r':
                    cout<<"Do deRSA on your message..."<<endl;
                    
                    nmod = getLongLongInput("n_mod(P*Q)=");
                    
                    cout<<endl<<"Input private key d:";
                    d_key = getLongLongInput("");
                    RSADecrypt(demessage, d_key, nmod);
                break;
                case 'x':
                    cout<<"Do deXOR on your message..."<<endl;
                    cout<<"Input XOR KEY string:";
                    cin.ignore();
                    getline(cin, xor_key_str);
                    XOREncryptDecrypt(demessage, xor_key_str);
                break;
                case 'c':
                    cout<<"Do deCaesar Chiper on your message..."<<endl;
                    caesar_shift_key = static_cast<int>(getLongLongInput("Caesar shift initial number?: "));
                    CaesarDecrypt(demessage,caesar_shift_key);
                break;
                case 'n':
                    cout<<"Nothing to do with your message."<<endl;
                break;
            }
            if(enct!='n')cout<<"Decrypt message:"<<demessage<<endl;
            delete img4;
            
        }
        cin.ignore();
        delete img3;
 
    }
    
    
    return 0;
}

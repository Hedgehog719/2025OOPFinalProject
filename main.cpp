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

#define CASE_ONE    0b00000001  // �W�G
#define CASE_TWO    0b00000010  // ����
#define CASE_THREE  0b00000100  // ����
#define CASE_FOUR   0b00001000  // ���e��
#define CASE_FIVE   0b00010000  // �U��
#define CASE_SEVEN  0b01000000  // ����½��
#define CASE_EIGHT  0b10000000  // ����

using namespace std;

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
         std::cerr << "RSA �Ҽ� n �Ӥp�I�N������ RSA �[�K�C\n";
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
        cout<<"0. Show All Small Images\n"<<endl;
        data_loader.List_Directory("Image-Folder", filenames); 
        string filename;
        int imgchoose;
        for(size_t i=0; i<filenames.size(); i++){
            //GrayImage *gg=new GrayImage();
            cout<<i+1<<". "<<filenames[i]<<endl<<endl;
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
                    gg.Display_CMD();
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
        cout<<"\nCurrent Image: "<<filename<<endl;
        cout<<endl;
        cout<<"========================"<<endl;
        cout<<"= CHOOSE YOUR IMG TYPE ="<<endl;
        cout<<"========================"<<endl;
        cout<<"1. Gray Image"<<endl;
        cout<<"2. RGB Image"<<endl;
        cout<<"0. Exit"<<endl;
        cout<<"========================"<<endl;
        int colorchoice;
        cout<<"Please enter your choice: ";
        cin>>colorchoice;
        if(colorchoice==1){
            img3 = new GrayImage();
            img3->LoadImage(filename);
        }
        else if(colorchoice==2){
            img3 = new RGBImage();
            img3->LoadImage(filename);
        }
        else if(colorchoice==0){
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
        while(!(d<=4&&d>=0)||d==256){
            cout<<"==============================\n";
            cout<<" Do you want to display img? \n";
            cout<<"------------------------------\n";
            cout<<"0. None\n1. ASCII\n2. X_Server\n3. Both\n4. Preview\n";
            cout<<"==============================\n";
            cin >>display;
            d=display-'0';
            switch (d) {
                case 0:case 'n'-'0':
                    
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
                case 4:
                    tempimg->Disp_small();
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
        
        cout<<endl;
        if(true){
            cout<<"======================"<<endl;
            cout<<"= SMALL IMAGE MOSAIC ="<<endl;
            cout<<"======================"<<endl;
            cout<<"DO YOU WANT TO USE SMALL IMAGE MOSAIC?(y/n) ";
            char smlimg='x';
            cin>>smlimg;
            while(smlimg!='y'&&smlimg!='n'){
                cout<<"????????????????again????????????????????"<<endl;
                cin>>smlimg;
            }
            if(smlimg=='y'){
                tempimg->small();
                
                int d=256;
                while(!(d<=4&&d>=0)||d==256){
                    cout<<"==============================\n";
                    cout<<" Do you want to display img? \n";
                    cout<<"------------------------------\n";
                    cout<<"0. None\n1. ASCII\n2. X_Server\n3. Both\n4. Preview(not recommended here)\n";
                    cout<<"==============================\n";
                    cin >>display;
                    d=display-'0';
                    switch (d) {
                        case 0:case 'n'-'0':
                            
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
                        case 4:
                            tempimg->Disp_small();
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
            }
        }
        
            
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
            bool useDCT=false;
            cout<<"\nChoose your Img hidden method\n1.  LSB \n2. DCTEncrypt\n:"<<flush;
            char imgtemp='.';
            cin>>imgtemp;
            while(imgtemp!='1'&&imgtemp!='2'){
                cout<<"wrong input"<<endl;
                cin>>imgtemp;
            }
            if(imgtemp=='2')useDCT=true;
                
            /*img3->Display_CMD();
            img3->Display_X_Server();
            img3->DCTEncryptMessage("henry is good good test!!!!!"); 
            cout<<img3->DCTDecryptMessage()<<endl;
            img3->Display_X_Server();*/
            
            
            cout<<"Encrpting Image: "<<filename<<endl<<"with message: "<<message<<endl<<"by method ";
            if(useDCT)cout<<"DCT"<<endl;
            else cout<<"LSB"<<endl;

            char len = static_cast<char>(message.length());
            message.insert(0, 1, len);
            if(!useDCT)img3->EncryptMessage(message);
            else img3->DCTEncryptMessage(message);
            //img3->DCTEncryptMessage(message);
            //cout<<img3->DCTDecryptMessage()<<endl;
 
            cout << "Message encrypted into filtered image." << endl;
            cout<<"Enter Image-Folder/\"filename\".png, default imgg.png or \"n\" for not saving: ";
            cin>>savename;
            if(savename.empty()){
                savename = "imgg";
            }
            if(savename!="n")img3->DumpImage("Image-Folder/"+savename+".png");
            
            d=256;
            while(!(d<=4&&d>=0)||d==256){
                cout<<"==============================\n";
                cout<<" Do you want to display img? \n";
                cout<<"------------------------------\n";
                cout<<"0. None\n1. ASCII\n2. X_Server\n3. Both\n4. Preview\n";
                cout<<"==============================\n";
                cin >>display;
                d=display-'0';
                switch (d) {
                    case 0:
                        
                        break;
                    case 1:
                        img3->Display_ASCII();
                        break;
                    case 2:
                        cout<<"Image showed. make sure to close it before next move."<<endl;
                        img3->Display_X_Server();
                        break;
                    case 3:
                        img3->Display_ASCII();
                        cout<<"Image showed. make sure to close it before next move."<<endl;
                        img3->Display_X_Server();
                        break;
                    case 4:
                        img3->Disp_small();
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
            
            bool useDCT=false;
            cout<<"\nChoose your Img hidden method\n1. LSB\n2. DCTEncrypt\n:"<<flush;
            char imgtemp='.';
            cin>>imgtemp;
            while(imgtemp!='1'&&imgtemp!='2'){
                cout<<"wrong input"<<endl;
                cin>>imgtemp;
            }
            if(imgtemp=='2')useDCT=true;
            
            cout<<"Encrpting Image: "<<filename<<endl<<"with message: "<<message<<endl<<"by method ";
            if(useDCT)cout<<"DCT"<<endl;
            else cout<<"LSB"<<endl;
            
            char len = static_cast<char>(message.length());
            message.insert(0, 1, len);
            if(!useDCT)img3->EncryptMessage(message);
            else img3->DCTEncryptMessage(message);
            //img3->DCTEncryptMessage(message); 
            cout << "Message encrypted into unfiltered image." << endl;
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
                        img3->Display_ASCII();
                        break;
                    case 2:
                        cout<<"Image showed. make sure to close it before next move."<<endl;
                        img3->Display_X_Server();
                        break;
                    case 3:
                        img3->Display_ASCII();
                        cout<<"Image showed. make sure to close it before next move."<<endl;
                        img3->Display_X_Server();
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
            bool useDCT=false;
            cout<<"\nChoose your Img hidden method\n1. LSB \n2. DCTEncrypt\n:"<<flush;
            char imgtemp='.';
            cin>>imgtemp;
            while(imgtemp!='1'&&imgtemp!='2'){
                cout<<"wrong input"<<endl;
                cin>>imgtemp;
            }
            if(imgtemp=='2')useDCT=true;
            
            if(!useDCT)demessage = img4->DecryptMessage();
            else demessage = img4->DCTDecryptMessage();
            
            if (demessage.empty()) {
                cout << "No hidden message found or image is not encrypted." << endl;
            } else {
                cout << "Decoded message: " <<endl<< demessage << endl;
            }

            cout<<"==========================================\n";
            cout<<" Choose Your String Encrypt Type:\n";
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
                    cout<<"Nothing to do with your message. Enter to Continue."<<endl;
                    cin.ignore();
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

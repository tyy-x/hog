//
//  load_save.cpp
//  _image
//
//  Created by 唐宇阳 on 2019/01/29.
//  Copyright © 2019 唐宇阳. All rights reserved.
//

#include <iostream>
#include <vector>
#include <fstream>
#include "uyvy422.hpp"
using namespace std;

namespace img {
    void readUYVY422_(const string &filename, Image &img, int flags)
    {
        ifstream ifile(filename, ifstream::binary);
        
        if(!ifile){
            cerr<<"#Read Error: "<<filename<<" not opened!"<<endl;
            exit(1);
        }
        
        int round=img.cols*img.rows*2/8;
        char buffer[8];
        
        for(int i=0; i<round; i++){
            ifile.read(buffer, 8);
            for(int j=0; j<8; j++){
                img.data[8*i+j]=buffer[j];
            }
        }
        
        ifile.close();
    }
    
    void readUYVY422(const string &filename, Image &img, int flags)
    {
        readUYVY422_(filename, img, flags);
        if(flags==READ_GRAYSCALE) setGrayscale(img);
    }
    
    void writeUYVY422(const string &filename, const Image &img)
    {
        ofstream ofile(filename, ofstream::binary);
        char buffer[8];
        
        if(!ofile){
            cerr<<"#Write Error: file not opened!"<<endl;
            exit(1);
        }
        
        int round=img.rows*img.cols*2/8;
        for(int i=0; i<round; i++){
            for(int j=0; j<8; j++){
                buffer[j]=img.data[8*i+j];
            }
            ofile.write(buffer, 8);
        }
        
        ofile.close();
    }
    
    void writeYUV444(const string &filename, const Image &img)
    {
        ofstream ofile(filename, ofstream::binary);
        char buffer[6];
        
        //cout<<"Write file -> "<<file<<endl;
        if(!ofile){
            cerr<<"#Write Error: file not opened!"<<endl;
            exit(1);
        }
        
        int round=img.rows*img.cols*3/6;
        for(int i=0; i<round; i++){
            for(int j=0; j<6; j++){
                buffer[j]=img.data[6*i+j];
            }
            ofile.write(buffer, 6);
        }
        
        ofile.close();
    }
    
    void convertToYUV444(Image &dst, const Image &src)
    {
        int size=dst.rows*dst.cols;
        vector<uchar> luma(size);
        vector<uchar> chromU(size);
        vector<uchar> chromV(size);
        
        for(int i=0; i<size; i++) luma[i]=src.data[i*2+1];
        for(int i=0; i<size/2; i++){
            chromU[i]=src.data[i*4];
            chromV[i]=src.data[i*4+2];
        }
        
        for(int i=0; i<size; i++) dst.data[i*3]=luma[i];
        for(int i=0; i<size/2; i++){
            dst.data[i*6+1]=chromU[i];
            dst.data[i*6+4]=chromU[i];
            dst.data[i*6+2]=chromV[i];
            dst.data[i*6+5]=chromV[i];
        }
    }
}

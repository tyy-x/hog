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
#include "imgproc.hpp"
#include "uyvy422.hpp"
using namespace std;

namespace img {
    void readUYVY422_(const string &filename, Mat<uchar> &img, int flags)
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
    
    void readUYVY422(const string &filename, Mat<uchar> &img, int flags)
    {
        readUYVY422_(filename, img, flags);
        if(flags==READ_GRAYSCALE) setGrayscale(img);
    }
    
    void writeUYVY422(const string &filename, const Mat<uchar> &img)
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
    
    void writeYUV444(const string &filename, const Mat<uchar> &img)
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
    
    void convertToYUV444(Mat<uchar> &dst, const Mat<uchar> &src)
    {
        dst.create(src.rows, src.cols, IMG_UC3);
        int _size=src.rows*src.cols;
        
        for(int i=0; i<_size; i++) dst.data[i*3]=src.data[i*2+1];
        for(int i=0; i<_size/2; i++){
            dst.data[i*6+1]=src.data[i*4];
            dst.data[i*6+4]=src.data[i*4];
            dst.data[i*6+2]=src.data[i*4+2];
            dst.data[i*6+5]=src.data[i*4+2];
        }
    }
    
    void convertToUYVY422(Mat<uchar> &dst, const Mat<uchar> &src)
    {
        int _rows=src.rows;
        int _cols=src.cols;
        vector<uchar> chromaU(_rows*_cols/2);
        vector<uchar> chromaV(chromaU);
        
        for(int i=0; i<_rows*_cols/2; i++){
            chromaU[i]=(src.data[i*6+1]+src.data[i*6+4])/2;
            chromaV[i]=(src.data[i*6+2]+src.data[i*6+5])/2;
        }
        
        dst.create(_rows, _cols, IMG_UC2);
        for(int i=0; i<_rows*_cols; i++) dst.data[i*2+1]=src.data[i*3];
        for(int i=0; i<_rows*_cols/2; i++){
            dst.data[i*4]=chromaU[i];
            dst.data[i*4+2]=chromaV[i];
        }
    }
    
    void scaleUYVY422(Mat<uchar> &dst,  const Mat<uchar> &src, float factor)
    {
        Mat<uchar> src444(src.rows, src.cols, IMG_UC3);
        convertToYUV444(src444, src);
        Mat<uchar> channel0(src.rows, src.cols);
        Mat<uchar> channel1(channel0);
        Mat<uchar> channel2(channel0);
        
        split(src444, channel0, channel1, channel2);
        
        int nrows=src.rows*factor;
        int ncols=src.cols*factor;
        Mat<uchar> nchannel0(nrows, ncols);
        Mat<uchar> nchannel1(nchannel0);
        Mat<uchar> nchannel2(nchannel0);
        
        scale(nchannel0, channel0, factor);
        scale(nchannel1, channel1, factor);
        scale(nchannel2, channel2, factor);
        
        Mat<uchar> dst444(nrows, ncols, IMG_UC3);
        
        merge(dst444, nchannel0, nchannel1, nchannel2);
        dst.create(nrows, ncols, IMG_UC2);
        convertToUYVY422(dst, dst444);
    }
    
    Mat<uchar> cropUYVY422(const Point p1, const Point p2, const Mat<uchar> &img)
    {
        //create a new image matrix and convert uyvy422 to yuv444
        Mat<uchar> tempYUV444;
        convertToYUV444(tempYUV444, img);
        
        //create 3 one channel image matrix
        Mat<uchar> channelY(img.rows, img.cols);
        Mat<uchar> channelU(channelY);
        Mat<uchar> channelV(channelY);
        split(tempYUV444, channelY, channelU, channelV);
        
        //compute croped image width and height
        int nrows=p2.y-p1.y+1;
        int ncols=p2.x-p1.x+1;
        
        //create 3 new one channel image matrix to store croped image
        Mat<uchar> nchannelY(nrows, ncols);
        Mat<uchar> nchannelU(nchannelY);
        Mat<uchar> nchannelV(nchannelY);
        
        for(int i=0; i<nrows; i++){
            for(int j=0; j<ncols; j++){
                *nchannelY.at(i, j)=*channelY.at((int)p1.y+i, (int)p1.x+j);
                *nchannelU.at(i, j)=*channelU.at((int)p1.y+i, (int)p1.x+j);
                *nchannelV.at(i, j)=*channelV.at((int)p1.y+i, (int)p1.x+j);
            }
        }
        
        //create a new 3 channel image to store croped image
        Mat<uchar> ntempYUV444(nrows, ncols, IMG_UC3);
        merge(ntempYUV444, nchannelY, nchannelU, nchannelV);
        
        //create a new 3 channel image and convert yuv444 to uyvy422
        Mat<uchar> cropedImg(ntempYUV444.rows, ntempYUV444.cols, IMG_UC3);
        convertToUYVY422(cropedImg, ntempYUV444);
        
        return cropedImg;
    }
}

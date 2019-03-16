//
//  imgprocessing.cpp
//  _image
//
//  Created by 唐宇阳 on 2019/01/31.
//  Copyright © 2019 唐宇阳. All rights reserved.
//

#include <vector>
#include <iomanip>
#include <cmath>
#include "imgproc.hpp"
#include "image.hpp"
using namespace std;

namespace img {
    /*
     void split(const Image &img, Image &channel0, Image &channel1, Image &channel2)
     {
     for(int i=0; i<img.rows; i++){
     for(int j=0; j<img.cols; j++){
     *channel0.at(i, j)=img.at(i, j)[0];
     *channel1.at(i, j)=img.at(i, j)[1];
     *channel2.at(i, j)=img.at(i, j)[2];
     }
     }
     }
     */
    
    //contrast stretch
    void stretch(Mat<uchar> &img, int min, int max)
    {
        unsigned char low, high;
        int size=img.rows*img.cols;
        int LUT[256]={0};
        
        high=low=img.data[0];
        for(int i=0; i<size; i++){
            if(high<img.data[i]) high=img.data[i];
            if(low>img.data[i]) low=img.data[i];
        }
        
        for(int i=0; i<256; i++) LUT[i]=min+(max-min)*(i-low)/(high-low);
        for(int i=0; i<size; i++) img.data[i]=LUT[img.data[i]];
    }
    
    void makeBorderConstant(Mat<uchar> &dst, const Mat<uchar> &src, int borderWidth, const int value)
    {
        //dst.create(src.rows+borderWidth, src.cols+borderWidth, src.depth);
        int size=dst.rows*dst.cols;
        
        for(int i=0; i<size; i++) dst.data[i]=value;
        for(int i=0; i<src.rows; i++){
            for(int j=0; j<src.cols; j++){
                *dst.at(i+borderWidth, j+borderWidth)=*src.at(i, j);
            }
        }
    }
    
    void makeBorderCopy(Mat<uchar> &dst, const Mat<uchar> &src, int borderWidth)
    {
        //copy the top and bottom
        for(int i=0; i<borderWidth; i++){
            for(int j=0; j<src.cols; j++){
                *dst.at(0+i, j+borderWidth)=*src.at(0, j);
                *dst.at(borderWidth+src.rows+i, j+borderWidth)=*src.at(src.rows-1, j);
            }
        }
        //copy the right and left
        for(int i=0; i<borderWidth; i++){
            for(int j=0; j<src.rows; j++){
                *dst.at(j+borderWidth, 0+i)=*src.at(j, 0);
                *dst.at(j+borderWidth, borderWidth+src.cols+i)=*src.at(j, src.cols-1);
            }
        }
        //set the corner
        for(int i=0; i<borderWidth; i++){
            for(int j=0; j<borderWidth; j++){
                *dst.at(0+i, 0+j)=0;
                *dst.at(0+i, borderWidth+src.cols+j)=0;
                *dst.at(borderWidth+src.rows+i, 0+j)=0;
                *dst.at(borderWidth+src.rows+i, borderWidth+src.cols+j)=0;
            }
        }
        //copy source image
        for(int i=0; i<src.rows; i++){
            for(int j=0; j<src.cols; j++){
                *dst.at(i+borderWidth, j+borderWidth)=*src.at(i, j);
            }
        }
    }
    
    void scale(Mat<uchar> &dst, const Mat<uchar> &src, float factor)
    {
        float deltaX, deltaY;
        float srcX, srcY;
        int x, y;
        
        //bilinear interpolation
        for(int i=0; i<dst.rows; i++){
            srcY=i/factor;
            y=srcY;
            deltaY=srcY-y;
            for(int j=0; j<dst.cols; j++){
                srcX=j/factor;
                x=srcX;
                deltaX=srcX-x;
                if(y+1>=src.rows | x+1>=src.cols){
                    if(y+1<src.rows && x+1>=src.cols){
                        *dst.at(i, j)=*src.at(y, x)*(1-deltaY)+*src.at(y+1, x)*deltaY;
                    }
                    if(y+1>=src.rows && x+1<src.cols){
                        *dst.at(i, j)=*src.at(y, x)*(1-deltaX)+*src.at(y, x+1)*deltaX;
                    }
                    if(y+1>=src.rows && x+1>=src.cols){
                        *dst.at(i, j)=*src.at(y, x);
                    }
                }else *dst.at(i, j)=*src.at(y, x)*(1-deltaX-deltaY+deltaY*deltaX)+*src.at(y, x+1)*(deltaX-deltaY*deltaX)+*src.at(y+1, x)*(deltaY-deltaY*deltaX)+*src.at(y+1, x+1)*deltaY*deltaX;
            }
        }
    }
    
    void boxFiltering(const Mat<uchar> &padImg, Mat<uchar> &img, int filterSize)
    {
        Mat<uchar> filter(filterSize, filterSize);
        int factor=filterSize*filterSize;
        int borderWidth=filterSize/2;
        
        for(int i=0; i<filterSize*filterSize; i++) filter.data[i]=1;
        for(int i=borderWidth; i<padImg.rows-borderWidth; i++){
            for(int j=borderWidth; j<padImg.cols-borderWidth; j++){
                int sum=0;
                for(int k=-borderWidth; k<=borderWidth; k++){
                    for(int l=-borderWidth; l<=borderWidth; l++){
                        sum+=*padImg.at(i+k, j+l);
                    }
                }
                *img.at(i-borderWidth, j-borderWidth)=sum/factor;
            }
        }
    }
    
    void prewitt(const Mat<uchar> &padImg, Mat<uchar> &img)
    {
        for(int i=1; i<padImg.rows-1; i++){
            for(int j=1; j<padImg.cols-1; j++){
                int sumx=0, sumy=0;
                for(int k=-1; k<=1; k++){
                    for(int l=-1; l<=1; l++){
                        sumx+=l*(*padImg.at(i+k, j+l));
                        sumy+=k*(*padImg.at(i+k, j+l));
                    }
                }
                *img.at(i-1, j-1)=sqrt(sumx*sumx+sumy*sumy);
            }
        }
    }
    
    void sobel(const Mat<uchar> &padImg, Mat<uchar> &img)
    {
        for(int i=1; i<padImg.rows-1; i++){
            for(int j=1; j<padImg.cols-1; j++){
                int sumx=0, sumy=0;
                for(int k=-1; k<=1; k++){
                    for(int l=-1; l<=1; l++){
                        sumx+=(!k ? 2*l:l)*(*padImg.at(i+k, j+l));
                        sumy+=(!l ? 2*k:k)*(*padImg.at(i+k, j+l));
                    }
                }
                *img.at(i-1, j-1)=sqrt(sumx*sumx+sumy*sumy);
            }
        }
    }
    
    void gaussian(const Mat<uchar> &padImg, Mat<uchar> &img, int filterSize)
    {
        int borderWidth=filterSize/2;
        vector<int> filter3{1, 2, 1, 2, 4, 2, 1, 2, 1};
        vector<int> filter5{1, 4, 7, 4, 1, 4, 16, 26, 16, 4, 7, 26, 41, 26, 7, 4, 16, 26, 16, 4, 1, 4, 7, 4, 1};
        int factor3=16;
        int factor5=273;
        
        for(int i=borderWidth; i<padImg.rows-borderWidth; i++){
            for(int j=borderWidth; j<padImg.cols-borderWidth; j++){
                int sum=0;
                int count=0;
                for(int k=-borderWidth; k<=borderWidth; k++){
                    for(int l=-borderWidth; l<=borderWidth; l++){
                        switch(filterSize){
                            case 3: sum+=filter3[count++]*(*padImg.at(i+k, j+l)); break;
                            case 5: sum+=filter5[count++]*(*padImg.at(i+k, j+l)); break;
                        }
                    }
                }
                switch(filterSize){
                    case 3: *img.at(i-borderWidth, j-borderWidth)=sum/factor3; break;
                    case 5: *img.at(i-borderWidth, j-borderWidth)=sum/factor5; break;
                }
            }
        }
    }
    
    void gammaCorrection(Mat<uchar> &img, float gamma)
    {
        vector<unsigned char> LUT(256);
        
        //look-up table
        for(int i=0; i<256; i++) LUT[i]=255*pow(i/255.0, 1/gamma);
        
        for(int i=0; i<img.rows; i++){
            for(int j=0; j<img.cols; j++){
                *img.at(i, j)=LUT[*img.at(i, j)];
            }
        }
    }
}

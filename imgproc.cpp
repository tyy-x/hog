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
     void split(const Mat<uchar> &img, Mat<uchar> &channel0, Mat<uchar> &channel1, Mat<uchar> &channel2)
     {
         for(int i=0; i<img.rows; i++){
             for(int j=0; j<img.cols; j++){
                 *channel0.at(i, j)=img.at(i, j)[0];
                 *channel1.at(i, j)=img.at(i, j)[1];
                 *channel2.at(i, j)=img.at(i, j)[2];
             }
         }
     }
    
    void merge(Mat<uchar> &dst, const Mat<uchar> &channel0, const Mat<uchar> &channel1, const Mat<uchar> &channel2)
    {
        for(int i=0; i<dst.rows; i++){
            for(int j=0; j<dst.cols; j++){
                dst.at(i, j)[0]=*channel0.at(i, j);
                dst.at(i, j)[1]=*channel1.at(i, j);
                dst.at(i, j)[2]=*channel2.at(i, j);
            }
        }
    }
    
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
    
    Mat<double> get2DGaussianKernel(double sigma, int size)
    {
        Mat<double> kernel1D=get1DGaussianKernel(sigma, size);
        Mat<double> kernel2D(size, size);
        
        for(int i=0; i<kernel2D.rows; i++){
            for(int j=0; j<kernel2D.cols; j++){
                *kernel2D.at(i, j)=kernel1D.data[i]*kernel1D.data[j];
            }
        }
        
        return kernel2D;
    }
    
    Mat<double> get1DGaussianKernel(double sigma, int size)
    {
        int radius=size/2;
        Mat<double> kernel(1, size);
        
        double sum=0;
        double n=1/(sqrt(2*3.141592654)*sigma);
        for(int i=0; i<size; i++){
            kernel.data[i]=n*exp(-0.5*(i-radius)*(i-radius)/(sigma*sigma));
            sum+=kernel.data[i];
        }
        for(int i=0; i<size; i++) kernel.data[i]/=sum;
        
        return kernel;
    }
    
    void median(Mat<uchar> &img, int size)
    {
        int borderWidth=size/2;
        Mat<uchar> padImg(img.rows+borderWidth*2, img.cols+borderWidth*2);
        makeBorderCopy(padImg, img, borderWidth);
        
        for(int i=borderWidth; i<padImg.rows-borderWidth; i++){
            for(int j=borderWidth; j<padImg.cols-borderWidth; j++){
                uchar data[size*size];
                int index=0;
                for(int k=-borderWidth; k<=borderWidth; k++){
                    for(int l=-borderWidth; l<=borderWidth; l++){
                        data[index++]=*padImg.at(i+k, j+l);
                    }
                }
                bool sorted=false;
                int length=size*size-1;
                while(!sorted){
                    sorted=true;
                    for(int n=0; n<length; n++){
                        int temp;
                        if(data[n]>data[n+1]){
                            temp=data[n+1];
                            data[n+1]=data[n];
                            data[n]=temp;
                            sorted=false;
                        }
                    }
                    length--;
                }
                *img.at(i-borderWidth, j-borderWidth)=data[size*size/2];
            }
        }
    }
    
    void prewitt(Mat<uchar> &img)
    {
        Mat<uchar> padImg(img.rows+2, img.cols+2);
        makeBorderCopy(padImg, img, 1);

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
    
    void sobel(Mat<uchar> &img)
    {
        Mat<uchar> padImg(img.rows+2, img.cols+2);
        makeBorderCopy(padImg, img, 1);
        
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
    
    void gaussianFiltering2D(Mat<uchar> &img, double sigma, int size)
    {
        int border=size/2;
        Mat<double> kernel=get2DGaussianKernel(sigma, size);
        Mat<uchar> padImg(img.rows+border*2, img.cols+border*2);
        
        makeBorderCopy(padImg, img, border);
        for(int i=border; i<padImg.rows-border; i++){
            for(int j=border; j<padImg.cols-border; j++){
                double sum=0;
                int index=0;
                for(int k=-border; k<=border; k++){
                    for(int l=-border; l<=border; l++){
                        sum+=*padImg.at(i+k, j+l)*kernel.data[index++];
                    }
                }
                *img.at(i-border, j-border)=sum;
            }
        }
    }
    
    void gaussianFiltering1D(Mat<uchar> &img, double sigma, int size)
    {
        int border=size/2;
        Mat<double> kernel=get1DGaussianKernel(sigma, size);
        Mat<uchar> padImg(img.rows+border*2, img.cols+border*2);
        
        makeBorderCopy(padImg, img, border);
        for(int i=border; i<padImg.rows-border; i++){
            for(int j=border; j<padImg.cols-border; j++){
                double sum=0;
                int index=0;
                for(int k=-border; k<=border; k++){
                    sum+=*padImg.at(i, j+k)*kernel.data[index++];
                }
                *img.at(i-border, j-border)=sum;
            }
        }
        makeBorderCopy(padImg, img, border);
        for(int i=border; i<padImg.rows-border; i++){
            for(int j=border; j<padImg.cols-border; j++){
                double sum=0;
                int index=0;
                for(int k=-border; k<=border; k++){
                    sum+=*padImg.at(i+k, j)*kernel.data[index++];
                }
                *img.at(i-border, j-border)=sum;
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
    
    void drawBorder(const Rect &roi, Mat<uchar> &img)
    {
        for(int i=(int)roi.point1.y-1; i!=(int)roi.point2.y+1; i++){
            *img.at(i, (int)roi.point1.x-1)=255;
            *img.at(i, (int)roi.point2.x+1)=255;
        }
        for(int i=(int)roi.point1.x-1; i!=(int)roi.point2.x+1; i++){
            *img.at((int)roi.point1.y-1, i)=255;
            *img.at((int)roi.point2.y+1, i)=255;
        }
    }
}

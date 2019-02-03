//
//  imgprocessing.cpp
//  _image
//
//  Created by 唐宇阳 on 2019/01/31.
//  Copyright © 2019 唐宇阳. All rights reserved.
//

#include <vector>
#include <iomanip>
#include "imgproc.hpp"
#include "image.hpp"
using namespace std;
using namespace img;

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
void stretch(Image &img, int min, int max)
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

void makeBorderConstant(Image &dst, const Image &src, int borderWidth, const int value)
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

void makeBorderCopy(Image &dst, const Image &src, int borderWidth)
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

void gaussian(Image &padImg, Image &img)
{
    vector<int> filter{1, 2, 1, 2, 4, 2, 1, 2, 1};
    
    for(int i=1; i<padImg.rows-1; i++){
        for(int j=1; j<padImg.cols-1; j++){
            int sum=0;
            int count=0;
            for(int k=-1; k<=1; k++){
                for(int l=-1; l<=1; l++){
                    sum+=filter[count++]*(*padImg.at(i+k, j+l));
                }
            }
            *img.at(i-1, j-1)=sum/16;
        }
    }
    
}

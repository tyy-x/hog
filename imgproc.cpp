//
//  imgprocessing.cpp
//  _image
//
//  Created by 唐宇阳 on 2019/01/31.
//  Copyright © 2019 唐宇阳. All rights reserved.
//

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

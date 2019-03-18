//
//  meanshift_tracking.cpp
//  _image
//
//  Created by 唐宇阳 on 2019/03/19.
//  Copyright © 2019 唐宇阳. All rights reserved.
//

#include <vector>
#include <cmath>
#include "meanshift_tracking.hpp"
using namespace img;

void Tracker::init()
{
    float width=target.lrpoint.x-target.ulpoint.x;
    float height=target.lrpoint.y-target.ulpoint.y;
    target_center.x=width/2+target.ulpoint.x;
    target_center.y=height/2+target.ulpoint.y;
    kernel.create(height+1, width+1, 1);
    
    //compute Epanechnikov Kernel weight for each target pixel
    int indexY=0, indexX=0;
    for(int i=target.ulpoint.y; i<(int)target.lrpoint.y; i++){
        double deltaY=pow((i-target_center.y)/height, 2);
        for(int j=target.ulpoint.x; j<(int)target.lrpoint.x; j++){
            double deltaX=pow((j-target_center.x)/width, 2);
            *kernel.at(indexY, indexX++)=epanechnikov_kernel(sqrt(deltaX+deltaY));
        }
        indexX=0;
        indexY++;
    }
    
    //coumpute normalization constant C of q_u=C*∑k(∥x∥^2)*𝞭[b(x)-u]
    for(int i=0; i<kernel.rows*kernel.cols; i++) normalization_constant+=kernel.data[i];
    normalization_constant=1.0/normalization_constant;
    //std::cout<<normalization_constant<<std::endl;
}

void Tracker::computeHistogram(const Mat<uchar> &img)
{
    std::vector<double> hist(256);
    
    int indexX=0, indexY=0;
    for(int i=target.ulpoint.y; i<target.lrpoint.y; i++){
        for(int j=target.ulpoint.x; j<target.lrpoint.x; j++){
                hist[*img.at(i, j)]+=normalization_constant*(*kernel.at(indexY, indexX++));
        }
        indexX=0;
        indexY++;
    }
}

void Tracker::tracking(img::Mat<img::uchar> &pre_frame, img::Mat<img::uchar> &next_frame)
{
    computeHistogram(pre_frame);
}

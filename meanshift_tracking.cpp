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
using namespace std;
using namespace img;

void computeKernelWeights(Mat<double> &kernel, Rect _roi)
{
    int indexY=0, indexX=0;
    for(int i=_roi.point1.y; i<=(int)_roi.point2.y; i++){
        double deltaY=pow((i-_roi.center.y)/_roi.height, 2);
        for(int j=_roi.point1.x; j<=(int)_roi.point2.x; j++){
            double deltaX=pow((j-_roi.center.x)/_roi.width, 2);
            *kernel.at(indexY, indexX++)=epanechnikov_kernel(sqrt(deltaX+deltaY));
        }
        indexX=0;
        indexY++;
    }
}

void ROIData::computeHistogram(const Mat<uchar> &img)
{
    Mat<double> kernel(roi.height+1, roi.width+1);
    pdf.resize(256);
    
    computeKernelWeights(kernel, this->roi);
    //coumpute normalization constant C of q_u=C*∑k(∥x∥^2)*𝞭[b(x)-u]
    double normalization_constant=0;
    for(int i=0; i<kernel.rows*kernel.cols; i++) normalization_constant+=kernel.data[i];
    normalization_constant=1.0/normalization_constant;
    
    int indexX=0, indexY=0;
    for(int i=roi.point1.y; i<=roi.point2.y; i++){
        for(int j=roi.point1.x; j<=roi.point2.x; j++){
            pdf[*img.at(i, j)]+=normalization_constant*(*kernel.at(indexY, indexX++));
        }
        indexX=0;
        indexY++;
    }
}

void MeanShiftTracker::processing(const Mat<uchar> &_current_frame)
{
    candidate.roi=target.roi;
    candidate.computeHistogram(_current_frame);
}

void MeanShiftTracker::localize(Mat<uchar> &_current_frame)
{
    Point new_center=candidate.roi.center;
    float delta=10000.0f;
    float epislon=0.1f;
    int iteration=1;
    
    while(delta >= epislon && iteration <= maxIteration){
        computeWeights(_current_frame);
        new_center=computeNewLocation();
        
        Point new_point1, new_point2;
        
        new_point1.x=(int)(new_center.x-candidate.roi.width/2.f);
        new_point2.x=(int)(new_center.x+candidate.roi.width/2.f);
        new_point1.y=(int)(new_center.y-candidate.roi.height/2.f);
        new_point2.y=(int)(new_center.y+candidate.roi.height/2.f);
        
        Rect new_roi={new_point1, new_point2};
        
        delta=sqrt((new_roi.center.x-candidate.roi.center.x)*(new_roi.center.x-candidate.roi.center.x)+(new_roi.center.y-candidate.roi.center.y)*(new_roi.center.y-candidate.roi.center.y));
        candidate.roi=new_roi;
        candidate.computeHistogram(_current_frame);
        iteration++;
        
        if(delta < epislon){
            break;
        }
    }
    cout<<"("<<candidate.roi.center.x<<", "<<candidate.roi.center.y<<")"<<"repeat times: "<<iteration-1<<endl;
    drawBorder(candidate.roi, _current_frame);
}

void MeanShiftTracker::computeWeights(const Mat<uchar> &_current_frame)
{
    weights.create(candidate.roi.height+1, candidate.roi.width+1, 1);
    int indexX=0, indexY=0;
    
    for(int i=candidate.roi.point1.y; i<=candidate.roi.point2.y; i++){
        for(int j=candidate.roi.point1.x; j<=candidate.roi.point2.x; j++){
            *weights.at(indexY, indexX++)=sqrt(target.pdf[*_current_frame.at(i, j)]/candidate.pdf[*_current_frame.at(i, j)]);
        }
        indexX=0;
        indexY++;
    }
}

Point MeanShiftTracker::computeNewLocation()
{
    double x=0;
    double y=0;
    
    double sum=0;
    for(int i=0; i<weights.rows*weights.cols; i++) sum+=weights.data[i];
    
    int indexX=0, indexY=0;
    for(int i=candidate.roi.point1.y; i<=candidate.roi.point2.y; i++){
        for(int j=candidate.roi.point1.x; j<=candidate.roi.point2.x; j++){
            x+=j*(*weights.at(indexY, indexX++));
        }
        indexX=0;
        indexY++;
    }
    x/=sum;
    indexX=indexY=0;
    for(int i=candidate.roi.point1.y; i<=candidate.roi.point2.y; i++){
        for(int j=candidate.roi.point1.x; j<=candidate.roi.point2.x; j++){
            y+=i*(*weights.at(indexY, indexX++));
        }
        indexX=0;
        indexY++;
    }
    y/=sum;
    
    return {(float)x, (float)y};
}

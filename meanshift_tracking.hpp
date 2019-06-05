//
//  meanshift_tracking.hpp
//  _image
//
//  Created by 唐宇阳 on 2019/03/19.
//  Copyright © 2019 唐宇阳. All rights reserved.
//

#ifndef meanshift_tracking_hpp
#define meanshift_tracking_hpp

#include "image.hpp"
#include "imgproc.hpp"

class ROIData{
public:
    img::Rect roi;
    std::vector<double> pdf;
    
    ROIData() {}
    ROIData(img::Rect _roi):roi(_roi) {}
    ROIData(img::Point _point1, img::Point _point2):roi(_point1, _point2) {}
    ROIData(const ROIData &obj):roi(obj.roi), pdf(obj.pdf) {}
    void computeHistogram(const img::Mat<img::uchar> &img);
};

class MeanShiftTracker{
private:
    img::Mat<double> weights;
    void inital(const img::Rect _roi, img::Mat<img::uchar> &_initial_frame);
    void computeWeights(const img::Mat<img::uchar> &_current_frame);
    img::Point computeNewLocation();

public:
    ROIData target;
    ROIData candidate;
    int maxIteration=20;
    
    MeanShiftTracker() {}
    //MeanShiftTracker(const img::Point targetPoint, const int width, const int height, const img::Mat<img::uchar> &frame);
    MeanShiftTracker(const img::Rect _roi, img::Mat<img::uchar> &_initial_frame);
    void processing(const img::Mat<img::uchar> &_current_frame);
    void localize(img::Mat<img::uchar> &_current_frame);
    //void tracking(img::Mat<img::uchar> &frame);
};

inline MeanShiftTracker::MeanShiftTracker(const img::Rect _roi, img::Mat<img::uchar> &_initial_frame)
{
    inital(_roi, _initial_frame);
}

inline void MeanShiftTracker::inital(const img::Rect _roi, img::Mat<img::uchar> &_initial_frame)
{
    target.roi=_roi;
    target.computeHistogram(_initial_frame);
    //img::drawBorder(target.roi, _initial_frame);
}

inline double epanechnikov_kernel(double norm)
{
    return (norm<=1) ? 2.0/3.14159265*(1-norm) : 0;
}

void computeKernelWeights(img::Mat<double> &kernel, img::Rect _roi);
#endif /* meanshift_tracking_hpp */

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

class Tracker{
private:
    img::Rect target;
    img::Point target_center;
    double normalization_constant=0;
    img::Mat<double> kernel;
    void init();
    double epanechnikov_kernel(double norm);
    void computeHistogram(const img::Mat<img::uchar> &img);
    
public:
    Tracker() {}
    Tracker(img::Rect _target):target(_target) { init();}
    void tracking(img::Mat<img::uchar> &pre_frame, img::Mat<img::uchar> &next_frame);
};

inline double Tracker::epanechnikov_kernel(double norm)
{
    return (norm<=1) ? 2.0/3.14159265*(1-norm) : 0;
}

#endif /* meanshift_tracking_hpp */

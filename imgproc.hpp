//
//  imgproc.hpp
//  _image
//
//  Created by 唐宇阳 on 2019/01/29.
//  Copyright © 2019 唐宇阳. All rights reserved.
//

#ifndef IMGPROC_HPP
#define IMGPROC_HPP

#include "image.hpp"

namespace img {
    //contrast stretch
    void stretch(Mat<uchar> &img, int min, int max);
    
    /* make image border using constant value
     @param dst for new one channel image data matrix
     @param src for old one channel image data matrix
     @param borderWidth for pixel numbers of the border
     @param value for constant value used to fill the border
    */
    void makeBorderConstant(Mat<uchar> &dst, const Mat<uchar> &src, int borderWidth, const int value);
    
    /* make image border using copy method, copy the first and the last column to fill the
     * left and the right border, copy the first and the last row to fill the top and the
     * bottom border, and set the corner to 0
     @param dst for new one channel image data matrix
     @param src for old one channel image data matrix
     @param borderWidth for pixel numbers of the border
    */
    void makeBorderCopy(Mat<uchar> &dst, const Mat<uchar> &src, int borderWidth);
    
    /* image scaling using bilinear interpolation
     @param dst: new one channel image
     @param src: old one channel image
     @param factor: scale factor
    */
    void scale(Mat<uchar> &dst, const Mat<uchar> &src, float factor);
    void gaussian(Mat<uchar> &padImg, Mat<uchar> &img);
    void gammaCorrection(Mat<uchar> &img, float gamma);
}
#endif /* IMGPROC_HPP */

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
    //split 3-channel image to 3 one-channel image
    void split(const Mat<uchar> &src, Mat<uchar> &channel0, Mat<uchar> &channel1, Mat<uchar> &channel2);
    //merge 3 one-channel image into one 3-channel image
    void merge(Mat<uchar> &dst, const Mat<uchar> &channel0, const Mat<uchar> &channel1, const Mat<uchar> &channel2);
    
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
    
    //filter
    void boxFiltering(const Mat<uchar> &padImg, Mat<uchar> &img, int filterSize);
    void prewitt(const Mat<uchar> &padImg, Mat<uchar> &img);
    void sobel(const Mat<uchar> &padImg, Mat<uchar> &img);
    void gaussian(const Mat<uchar> &padImg, Mat<uchar> &img, int filterSize);
    void gammaCorrection(Mat<uchar> &img, float gamma);
    
    //filtering
    inline void filtering(Mat<uchar> &img, int type, int filterSize=3)
    {
        int borderWidth=filterSize/2;
        Mat<uchar> padImg(img.rows+borderWidth*2, img.cols+borderWidth*2);
        
        makeBorderCopy(padImg, img, borderWidth);
        switch(type){
            case Box: boxFiltering(padImg, img, filterSize); break;
            case Prewitt: prewitt(padImg, img); break;
            case Sobel: sobel(padImg, img); break;
            case Gaussian: gaussian(padImg, img, filterSize); break;
        }
    }
    
    //draw a box border on an gray image
    void drawBorder(const Rect &roi, Mat<uchar> &img);
}
#endif /* IMGPROC_HPP */

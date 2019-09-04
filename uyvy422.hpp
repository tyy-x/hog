//
//  uyvy422.hpp
//  _image
//
//  Created by 唐宇阳 on 2019/01/31.
//  Copyright © 2019 唐宇阳. All rights reserved.
//

#ifndef UYVY422_HPP
#define UYVY422_HPP

#include "image.hpp"

namespace img {
    void readUYVY422(const std::string &filename, Mat<uchar> &img, int flags=READ_GRAYSCALE);
    void readUYVY422_(const std::string &filename, Mat<uchar> &img, int flags);
    void writeUYVY422(const std::string &filename, const Mat<uchar> &img);
    void writeYUV444(const std::string &filename, const Mat<uchar> &img);
    Mat<uchar> cropUYVY422(const Point p1, const Point p2, const Mat<uchar> &img);
    //for a yvy422 image, set the value of U and V channel to 128 to generate a grayscale image
    inline void setGrayscale(Mat<uchar> &img)
    {
        int _size=img.rows*img.cols;
        for(int i=0; i<_size; i++) img.data[i*2]=128;
    }
    
    //data layout becomes y0u0v0y1u1v1... after convert
    void convertToYUV444(Mat<uchar> &dst, const Mat<uchar> &src);
    //convert yuv444 back to uyvy422
    void convertToUYVY422(Mat<uchar> &dst, const Mat<uchar> &src);
    
    /* extract luminance channel
     @param dst for one channel grayscale image matrix
     @param src for 3 channel yuv422 image matrix
    */
    inline void extractLuma(Mat<uchar> &dst, const Mat<uchar> &src)
    {
        int _size=src.rows*src.cols;
        for(int i=0; i<_size; i++) dst.data[i]=src.data[i*2+1];
    }
    
    //merge luminance channel
    inline void mergeLuma(const Mat<uchar> &luma, Mat<uchar> &img)
    {
        int _size=img.rows*img.cols;
        for(int i=0; i<_size; i++) img.data[i*2+1]=luma.data[i];
    }
    
    //scale uyvy422
    void scaleUYVY422(Mat<uchar> &dst, const Mat<uchar> &src, float factor);
}
#endif /* UYVY422_HPP */

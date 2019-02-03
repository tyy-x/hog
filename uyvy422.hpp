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

void readUYVY422(const std::string &filename, img::Image &img, int flags=img::READ_GRAYSCALE);
void readUYVY422_(const std::string &filename, img::Image &img, int flags);
void writeUYVY422(const std::string &filename, const img::Image &img);
void writeYUV444(const std::string &filename, const img::Image &img);
//for a yvy422 image, set the value of U and V channel to 128 to generate a grayscale image
inline void setGrayscale(img::Image &img)
{
    int size=img.rows*img.cols;
    for(int i=0; i<size; i++) img.data[i*2]=128;
}

//data layout becomes y0u0v0y1u1v1... after convert
void convertToYUV444(img::Image &dst, const img::Image &src);

//sperate luminance channel
inline void luma(img::Image &dst, const img::Image &src)
{
    int size=src.rows*src.cols;
    for(int i=0; i<size; i++) dst.data[i]=src.data[i*2+1];
}

//merge luminance channel
inline void merge(const img::Image &luma, img::Image &img)
{
    int size=img.rows*img.cols;
    for(int i=0; i<size; i++) img.data[i*2+1]=luma.data[i];
}

#endif /* UYVY422_HPP */

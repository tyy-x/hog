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
    void readUYVY422(const std::string &filename, Image &img, int flags=READ_GRAYSCALE);
    void readUYVY422_(const std::string &filename, Image &img, int flags);
    void writeUYVY422(const std::string &filename, const Image &img);
    void writeYUV444(const std::string &filename, const Image &img);
    //for a yvy422 image, set the value of U and V channel to 128 to generate a grayscale image
    inline void setGrayscale(Image &img)
    {
        int size=img.rows*img.cols;
        for(int i=0; i<size; i++) img.data[i*2]=128;
    }
    
    //data layout becomes y0u0v0y1u1v1... after convert
    void convertToYUV444(Image &dst, const Image &src);
    
    /* extract luminance channel
     @param dst for one channel grayscale image matrix
     @param src for 3 channel yuv422 image matrix
    */
    inline void extractLuma(Image &dst, const Image &src)
    {
        int size=src.rows*src.cols;
        for(int i=0; i<size; i++) dst.data[i]=src.data[i*2+1];
    }
    
    //merge luminance channel
    inline void merge(const Image &luma, Image &img)
    {
        int size=img.rows*img.cols;
        for(int i=0; i<size; i++) img.data[i*2+1]=luma.data[i];
    }
}
#endif /* UYVY422_HPP */

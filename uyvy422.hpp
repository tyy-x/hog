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

void readInterlacedYUV422(const std::string &filename, img::Image &img, int flags=img::READ_GRAYSCALE);
void readInterlacedYUV422_(const std::string &filename, img::Image &img, int flags);
void writeInterlacedYUV422(const std::string &filename, const img::Image &img);
void writeYUV444(const std::string &filename, const img::Image &img);
//data layout becomes y0u0v0y1u1v1... after convert
void convertToYUV444(img::Image &dst, const img::Image &src);
#endif /* UYVY422_HPP */

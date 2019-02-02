//
//  main.cpp
//  _image
//
//  Created by 唐宇阳 on 2019/01/29.
//  Copyright © 2019 唐宇阳. All rights reserved.
//

#include <iostream>
#include <vector>
#include <iomanip>
#include "image.hpp"
#include "uyvy422.hpp"
using namespace std;
using namespace img;

int main(void){
    // insert code here...
    Image img(480, 704, COLOR);
    Image dst(480, 704, COLOR);
    
    readInterlacedYUV422("quentin.yuv", img, READ_COLOR);
    convertToYUV444(dst, img);
    writeYUV444("444.yuv", dst);
    
    return 0;
}

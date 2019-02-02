//
//  imgprocessing.cpp
//  _image
//
//  Created by 唐宇阳 on 2019/01/31.
//  Copyright © 2019 唐宇阳. All rights reserved.
//

#include <iomanip>
#include "imgproc.hpp"
#include "image.hpp"
using namespace std;
using namespace img;

void split(const Image &img, Image &channel0, Image &channel1, Image &channel2)
{
    for(int i=0; i<img.rows; i++){
        for(int j=0; j<img.cols; j++){
            channel0.at(i, j)=img.at3channel(i, j)[0];
            channel1.at(i, j)=img.at3channel(i, j)[1];
            channel2.at(i, j)=img.at3channel(i, j)[2];
        }
    }
    /*
    for(int i=0; i<img.rows; i++){
        for(int j=0; j<img.cols; j++){
            for(int k=0; k<3; k++){
                cout<<right;
                cout<<setw(3)<<(int)img.at3channel(i, j)[k]<<"|";
            }
            cout<<"     ";
        }
        cout<<endl;
    }
    */
}

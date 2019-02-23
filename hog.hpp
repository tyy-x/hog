//
//  hog.hpp
//  _image
//
//  Created by 唐宇阳 on 2019/02/21.
//  Copyright © 2019 唐宇阳. All rights reserved.
//

#ifndef HOG_HPP
#define HOG_HPP

#include <vector>
#include "image.hpp"

namespace hog {
    const double PI=3.14159265;
    
    class HOGFeature{
    private:
        typedef std::vector<std::vector<std::vector<double>>> Hist;
        static int LUT[9];
        int bins=9;
        int cellSize=8;
        int blockSize=16;
        int winWidth=64;
        int winHeight=128;
        int cellHistRows;
        int cellHistCols;
        int blockHistRows;
        int blockHistCols;
        int blockHistSize;
        int blockStride=8;
        int hogVectorSize;
        std::vector<double> magnitude;
        std::vector<double> angle;
        Hist cellHist;
        Hist blockHist;
        std::vector<double> hogVector;
        
        void initial();
        void computeGradient(const img::Image &img);
        void assignGradientMagnitude(img::Image &img);
        void computeCellHistogram(int xWin, int yWin);
    public:
        HOGFeature() { initial();}
        void processing(img::Image &img, int type);
        void computeHOGFeature(int xWin, int yWin);
    };
    
    inline void HOGFeature::processing(img::Image &img, int type)
    {
        computeGradient(img);
        if(type) assignGradientMagnitude(img);
    }
    
    inline void HOGFeature::assignGradientMagnitude(img::Image &img)
    {
        for(int i=0; i<img.rows*img.cols; i++){
            if(magnitude[i]>255){
                img.data[i]=255;
            }else img.data[i]=magnitude[i];
        }
    }
}
#endif /* HOG_HPP */

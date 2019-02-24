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
        std::vector<std::vector<double>> magnitude;
        std::vector<std::vector<double>> angle;
        Hist cellHist;
        Hist blockHist;
        std::vector<double> hogVector;
        
        void initial();
        void computeGradient(const img::Image &img);
        void assignGradientMagnitude(img::Image &img);
        void computeCellHistogram(int yWin, int xWin);
        void normalizeBlockHistogram();
    public:
        HOGFeature() { initial();}
        ~HOGFeature() { std::cout<<"debug"<<std::endl;}
        void processing(img::Image &img, int type);
        void computeHOGFeature(int xWin, int yWin);
        void save(const std::string &filename);
    };
    
    inline void HOGFeature::processing(img::Image &img, int type)
    {
        computeGradient(img);
        if(type) assignGradientMagnitude(img);
    }
    
    inline void HOGFeature::assignGradientMagnitude(img::Image &img)
    {
        for(int i=0; i<img.rows; i++){
            for(int j=0; j<img.cols; j++){
                if(magnitude[i][j]>255){
                    *img.at(i, j)=255;
                }else *img.at(i, j)=magnitude[i][j];
            }
        }
    }
}
#endif /* HOG_HPP */

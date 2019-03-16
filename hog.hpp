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
    //Implementation of HOG Algorithm
    class HOGFeature{
    private:
        int bins=9; //number of cell histogram bins
        int cellSize=8; //pixel number of one cell
        int blockSize=16; //pixel number of one block
        int winWidth=64; //width of scanning window
        int winHeight=128; //height of scanning window
        int cellHistRows; //number of cell histogram along x direction in one window
        int cellHistCols; //number of cell histogram along y direction in one window
        int blockHistRows; //number of block histogram along x direction in one window
        int blockHistCols; //number of block histogram along y direction in one window
        int blockHistSize; //number of bins of one block histogram
        int hogVectorSize; //dimension of hog descriptor
        img::Mat<double> magnitude; //data matrix of gradient magnitude
        img::Mat<double> angle; //data matrix of gradient orientation
        img::Mat<double> cellHist; //data matrix of cell histogram
        img::Mat<double> blockHist; //data matrix of block histogram
        std::vector<double> hogVector; //final hog descriptor in one window
        
        void initial();
        void computeGradient(const img::Mat<img::uchar> &img);
        void assignGradientMagnitude(img::Mat<img::uchar> &img);
        void computeCellHistogram(int yWin, int xWin); //compute cell histogram use trilinear interpolation
        void computeBlockHistogram();
        void computeL2norm(); //normalize block histogram usr L2-norm method
    public:
        HOGFeature() { initial();}
        HOGFeature(int _winHeight, int _winWidth):winHeight(_winHeight), winWidth(_winWidth) { initial();}
        ~HOGFeature() { /*std::cout<<"debug"<<std::endl;*/}
        void processing(img::Mat<img::uchar> &img, int type=0); //preprocessing image to compute gradient
        void computeHOGFeature(int xWin, int yWin); //compute hog feature in the scanning window
        void save(const std::string &filename); //save hog descriptor data to file
        int get_hogVector_size() { return hogVectorSize;}
    };
    
    inline void HOGFeature::processing(img::Mat<img::uchar> &img, int flag)
    {
        computeGradient(img);
        if(flag) assignGradientMagnitude(img);
    }
    
    inline void HOGFeature::assignGradientMagnitude(img::Mat<img::uchar> &img)
    {
        int _size=img.rows*img.cols;
        for(int i=0; i<_size; i++) img.data[i]=magnitude.data[i];
    }
}
#endif /* HOG_HPP */

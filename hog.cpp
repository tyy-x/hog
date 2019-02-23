//
//  hog.cpp
//  _image
//
//  Created by 唐宇阳 on 2019/02/21.
//  Copyright © 2019 唐宇阳. All rights reserved.
//

#include <cmath>
#include "imgproc.hpp"
#include "hog.hpp"
using namespace hog;
using namespace img;

int HOGFeature::LUT[9]={0, 20, 40, 60, 80, 100, 120, 140, 160};

void HOGFeature::initial()
{
    cellHistRows=winHeight/cellSize;
    cellHistCols=winWidth/cellSize;
    blockHistRows=cellHistRows-blockSize/cellSize+1;
    blockHistCols=cellHistCols-blockSize/cellSize+1;
    blockHistSize=9*blockSize/cellSize*blockSize/cellSize;
    hogVectorSize=blockHistSize*blockHistRows*blockHistCols;
    
    cellHist.resize(cellHistRows);
    for(int i=0; i<cellHistRows; i++){
        cellHist[i].resize(cellHistCols);
        for(int j=0; j<cellHistCols; j++){
            cellHist[i][j].resize(9, 0);
        }
    }
    
    blockHist.resize(blockHistRows);
    for(int i=0; i<blockHistRows; i++){
        blockHist[i].resize(blockHistCols);
        for(int j=0; j<blockHistCols; j++){
            blockHist[i][j].resize(blockHistSize, 0);
        }
    }
    
    hogVector.resize(hogVectorSize, 0);
}

void HOGFeature::computeGradient(const Image &img)
{
    magnitude.resize(img.rows*img.cols);
    angle.resize(img.rows*img.cols);
    Image padImg(img.rows+2, img.cols+2, IMG_UC1);
    
    makeBorderCopy(padImg, img, 1);
    
    double theta=0.0, xDelta=0.0, yDelta=0.0;
    
    //calculate magnitude and orientation of gradient
    for(int i=1; i<padImg.rows-1; i++){
        for(int j=1; j<padImg.cols-1; j++){
            for(int k=-1; k<=1; k++){
                xDelta+=k*(*padImg.at(i, j+k));
                yDelta+=k*(*padImg.at(i+k, j));
            }
            magnitude.at((i-1)*img.cols+j-1)=sqrt(xDelta*xDelta+yDelta*yDelta);
            if(xDelta!=0.0){
                theta=atan2(yDelta, xDelta)*180.0/PI;
            }else if(yDelta!=0){
                theta=90.0;
            }else theta=0.0;
            if(theta>=0){
                angle.at((i-1)*img.cols+j-1)=theta;
            }else angle.at((i-1)*img.cols+j-1)=theta+180.0;
            xDelta=yDelta=0.0;
        }
    }
}

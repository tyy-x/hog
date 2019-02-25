//
//  hog.cpp
//  _image
//
//  Created by 唐宇阳 on 2019/02/21.
//  Copyright © 2019 唐宇阳. All rights reserved.
//

#include <iostream>
#include <fstream>
#include <iomanip>
#include <cmath>
#include "imgproc.hpp"
#include "hog.hpp"
using namespace hog;
using namespace img;
using namespace std;

void HOGFeature::initial()
{
    cellHistRows=winHeight/cellSize;
    cellHistCols=winWidth/cellSize;
    blockHistRows=cellHistRows-blockSize/cellSize+1;
    blockHistCols=cellHistCols-blockSize/cellSize+1;
    blockHistSize=bins*blockSize/cellSize*blockSize/cellSize;
    hogVectorSize=blockHistSize*blockHistRows*blockHistCols;
    
    cellHist.create(cellHistRows, cellHistCols, bins);
    blockHist.create(blockHistRows, blockHistCols, blockHistSize);
    
    hogVector.resize(hogVectorSize, 0);
}

void HOGFeature::computeGradient(const Mat<uchar> &img)
{
    magnitude.create(img.rows, img.cols, 1);
    angle.create(img.rows, img.cols, 1);
    
    Mat<uchar> padImg(img.rows+2, img.cols+2, IMG_UC1);
    
    makeBorderCopy(padImg, img, 1);
    
    double theta=0.0, xDelta=0.0, yDelta=0.0;
    
    //calculate magnitude and orientation of gradient
    for(int i=1; i<padImg.rows-1; i++){
        for(int j=1; j<padImg.cols-1; j++){
            for(int k=-1; k<=1; k++){
                xDelta+=k*(*padImg.at(i, j+k));
                yDelta+=k*(*padImg.at(i+k, j));
            }
            *magnitude.at(i-1, j-1)=sqrt(xDelta*xDelta+yDelta*yDelta);
            if(xDelta!=0.0){
                theta=atan2(yDelta, xDelta)*180.0/PI;
            }else if(yDelta!=0){
                theta=90.0;
            }else theta=0.0;
            if(theta>=0){
                *angle.at(i-1, j-1)=theta;
            }else *angle.at(i-1, j-1)=theta+180.0;
            xDelta=yDelta=0.0;
        }
    }
}

//compute cell histogram use trilinear interpolation
void HOGFeature::computeCellHistogram(int yWin, int xWin)
{
    int cellY=0, cellX=0; //cordinates of cell
    int cellRightBorder, cellTopBorder; //right border and top border of every cell
    int _bin; //bin center
    double _angle;
    double factorX, factorY, factorBin;
    int index;
    
    //trilinear interpolation of weights
    for(int i=yWin; i<=yWin+winHeight-cellSize; i+=8){
        for(int j=xWin; j<=xWin+winWidth-cellSize; j+=8){
            cellTopBorder=cellY*cellSize+yWin;
            cellRightBorder=cellX*cellSize+xWin;
            for(int y=0; y<cellSize; y++){
                for(int x=0; x<cellSize; x++){
                    _angle=*angle.at(i+y, j+x);
                    index=_angle/20;
                    factorX=(j+x-cellRightBorder)/7;
                    factorY=(i+y-cellTopBorder)/7;
                    if(index<8){
                        _bin=index*20;
                        factorBin=(_angle-_bin)/20;
                        if(cellY+1>=cellHistRows | cellX+1>=cellHistCols){
                            if(cellY+1<cellHistRows && cellX+1>=cellHistCols){
                                cellHist.at(cellY, cellX)[index]+=*magnitude.at(i+y, j+x)*(1-factorY)*(1-factorBin);
                                cellHist.at(cellY, cellX)[index+1]+=*magnitude.at(i+y, j+x)*(1-factorY)*factorBin;
                                cellHist.at(cellY+1, cellX)[index]+=*magnitude.at(i+y, j+x)*factorY*(1-factorBin);
                                cellHist.at(cellY+1, cellX)[index+1]+=*magnitude.at(i+y, j+x)*factorY*factorBin;
                            }
                            if(cellY+1>=cellHistRows && cellX+1<cellHistCols){
                                cellHist.at(cellY, cellX)[index]+=*magnitude.at(i+y, j+x)*(1-factorX)*(1-factorBin);
                                cellHist.at(cellY, cellX)[index+1]+=*magnitude.at(i+y, j+x)*(1-factorX)*factorBin;
                                cellHist.at(cellY, cellX+1)[index]+=*magnitude.at(i+y, j+x)*factorX*(1-factorBin);
                                cellHist.at(cellY, cellX+1)[index+1]+=*magnitude.at(i+y, j+x)*factorX*factorBin;
                            }
                            if(cellY+1>=cellHistRows && cellX+1>=cellHistCols){
                                cellHist.at(cellY, cellX)[index]+=*magnitude.at(i+y, j+x)*(1-factorBin);
                                cellHist.at(cellY, cellX)[index+1]+=*magnitude.at(i+y, j+x)*factorBin;
                            }
                        }
                        else{
                            cellHist.at(cellY, cellX)[index]+=*magnitude.at(i+y, j+x)*(1-factorX)*(1-factorY)*(1-factorBin);
                            cellHist.at(cellY, cellX)[index+1]+=*magnitude.at(i+y, j+x)*(1-factorX)*(1-factorY)*factorBin;
                            cellHist.at(cellY, cellX+1)[index]+=*magnitude.at(i+y, j+x)*factorX*(1-factorY)*(1-factorBin);
                            cellHist.at(cellY, cellX+1)[index+1]+=*magnitude.at(i+y, j+x)*factorX*(1-factorY)*factorBin;
                            cellHist.at(cellY+1, cellX)[index]+=*magnitude.at(i+y, j+x)*(1-factorX)*factorY*(1-factorBin);
                            cellHist.at(cellY+1, cellX)[index+1]+=*magnitude.at(i+y, j+x)*(1-factorX)*factorY*factorBin;
                            cellHist.at(cellY+1, cellX+1)[index]+=*magnitude.at(i+y, j+x)*factorX*factorY*(1-factorBin);
                            cellHist.at(cellY+1, cellX+1)[index+1]+=*magnitude.at(i+y, j+x)*factorX*factorY*factorBin;
                        }
                    }
                    else{
                        _bin=160;
                        factorBin=(_angle-_bin)/20;
                        if(cellY+1>=cellHistRows | cellX+1>=cellHistCols){
                            if(cellY+1<cellHistRows && cellX+1>=cellHistCols){
                                cellHist.at(cellY, cellX)[8]+=*magnitude.at(i+y, j+x)*(1-factorY)*(1-factorBin);
                                cellHist.at(cellY, cellX)[0]+=*magnitude.at(i+y, j+x)*(1-factorY)*factorBin;
                                cellHist.at(cellY+1, cellX)[8]+=*magnitude.at(i+y, j+x)*factorY*(1-factorBin);
                                cellHist.at(cellY+1, cellX)[0]+=*magnitude.at(i+y, j+x)*factorY*factorBin;
                            }
                            if(cellY+1>=cellHistRows && cellX+1<cellHistCols){
                                cellHist.at(cellY, cellX)[8]+=*magnitude.at(i+y, j+x)*(1-factorX)*(1-factorBin);
                                cellHist.at(cellY, cellX)[0]+=*magnitude.at(i+y, j+x)*(1-factorX)*factorBin;
                                cellHist.at(cellY, cellX+1)[8]+=*magnitude.at(i+y, j+x)*factorX*(1-factorBin);
                                cellHist.at(cellY, cellX+1)[0]+=*magnitude.at(i+y, j+x)*factorX*factorBin;
                            }
                            if(cellY+1>=cellHistRows && cellX+1>=cellHistCols){
                                cellHist.at(cellY, cellX)[8]+=*magnitude.at(i+y, j+x)*(1-factorBin);
                                cellHist.at(cellY, cellX)[0]+=*magnitude.at(i+y, j+x)*factorBin;
                            }
                        }
                        else{
                            cellHist.at(cellY, cellX)[8]+=*magnitude.at(i+y, j+x)*(1-factorX)*(1-factorY)*(1-factorBin);
                            cellHist.at(cellY, cellX)[0]+=*magnitude.at(i+y, j+x)*(1-factorX)*(1-factorY)*factorBin;
                            cellHist.at(cellY, cellX+1)[8]+=*magnitude.at(i+y, j+x)*factorX*(1-factorY)*(1-factorBin);
                            cellHist.at(cellY, cellX+1)[0]+=*magnitude.at(i+y, j+x)*factorX*(1-factorY)*factorBin;
                            cellHist.at(cellY+1, cellX)[8]+=*magnitude.at(i+y, j+x)*(1-factorX)*factorY*(1-factorBin);
                            cellHist.at(cellY+1, cellX)[0]+=*magnitude.at(i+y, j+x)*(1-factorX)*factorY*factorBin;
                            cellHist.at(cellY+1, cellX+1)[8]+=*magnitude.at(i+y, j+x)*factorX*factorY*(1-factorBin);
                            cellHist.at(cellY+1, cellX+1)[0]+=*magnitude.at(i+y, j+x)*factorX*factorY*factorBin;
                        }
                    }
                }
            }
            cellX++;
        }
        cellY++;
        cellX=0;
    }
}

void HOGFeature::computeBlockHistogram()
{
    int index=0;
    
    for(int i=0; i<blockHistRows; i++){
        for(int j=0; j<blockHistCols; j++){
            for(int y=0; y<blockSize/cellSize; y++){
                for(int x=0; x<blockSize/cellSize; x++){
                    for(int k=0; k<bins; k++){
                        blockHist.at(i, j)[index++]=cellHist.at(i+y, j+x)[k];
                    }
                }
            }
            index=0;
        }
    }
}

void HOGFeature::computeL2norm()
{
    for(int i=0; i<blockHistRows; i++){
        for(int j=0; j<blockHistCols; j++){
            int sum=0;
            for(int k=0; k<blockHistSize; k++){
                sum+=blockHist.at(i, j)[k]*blockHist.at(i, j)[k];
            }
            sum=sqrt(sum+0.001);
            for(int k=0; k<blockHistSize; k++){
                blockHist.at(i, j)[k]=blockHist.at(i, j)[k]/sum;
            }
        }
    }
}

void HOGFeature::computeHOGFeature(int yWin, int xWin)
{
    computeCellHistogram(yWin, xWin);
    computeBlockHistogram();
    computeL2norm();
    
    int index=0;
    for(int i=0; i<blockHistRows; i++){
        for(int j=0; j<blockHistCols; j++){
            for(int k=0; k<blockHistSize; k++){
                hogVector[index++]=blockHist.at(i, j)[k];
            }
        }
    }
}

void HOGFeature::save(const string &filename)
{
    ofstream output(filename, ofstream::binary);
    
    if(!output){
        cerr<<"#Error: file not opened!"<<endl;
        exit(1);
    }
    
    output<<"#HOG feature"<<endl;
    int i=0;
    for(auto item:hogVector){
        output<<left;
        output<<setw(6)<<++i<<item<<endl;
    }
}

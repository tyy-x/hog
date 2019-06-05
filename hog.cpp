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
    
    Mat<uchar> padImg(img.rows+2, img.cols+2);
    
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
    int startX=xWin+4;
    int endX=xWin+(cellHistCols-2)*cellSize+4;
    int startY=yWin+4;
    int endY=yWin+(cellHistRows-2)*cellSize+4;
    int cellY=0, cellX=0;
    int index;
    double _angle, weight;
    float factorX, factorY, factorBin;
    
    //trilinear interpolation
    //interpolation over pixel region except the border of image
    for(int i=startY; i<=endY; i+=cellSize){
        float topBorder=yWin+cellY*cellSize+3.5f;
        for(int j=startX; j<=endX; j+=cellSize){
            float leftBorder=xWin+cellX*cellSize+3.5f;
            for(int y=0; y<cellSize; y++){
                for(int x=0; x<cellSize; x++){
                    _angle=*angle.at(i+y, j+x);
                    index=_angle/20;
                    weight=*magnitude.at(i+y, j+x);
                    factorX=(j+x-leftBorder)/cellSize;
                    factorY=(i+y-topBorder)/cellSize;
                    if(index<8){
                        factorBin=(_angle-index*20)/20;
                        cellHist.at(cellY, cellX)[index]+=weight*(1-factorX)*(1-factorY)*(1-factorBin);
                        cellHist.at(cellY, cellX)[index+1]+=weight*(1-factorX)*(1-factorY)*factorBin;
                        cellHist.at(cellY, cellX+1)[index]+=weight*factorX*(1-factorY)*(1-factorBin);
                        cellHist.at(cellY, cellX+1)[index+1]+=weight*factorX*(1-factorY)*factorBin;
                        cellHist.at(cellY+1, cellX)[index]+=weight*(1-factorX)*factorY*(1-factorBin);
                        cellHist.at(cellY+1, cellX)[index+1]+=weight*(1-factorX)*factorY*factorBin;
                        cellHist.at(cellY+1, cellX+1)[index]+=weight*factorX*factorY*(1-factorBin);
                        cellHist.at(cellY+1, cellX+1)[index+1]+=weight*factorX*factorY*factorBin;
                    }else{
                        factorBin=(_angle-160)/20;
                        cellHist.at(cellY, cellX)[8]+=weight*(1-factorX)*(1-factorY)*(1-factorBin);
                        cellHist.at(cellY, cellX)[0]+=weight*(1-factorX)*(1-factorY)*factorBin;
                        cellHist.at(cellY, cellX+1)[8]+=weight*factorX*(1-factorY)*(1-factorBin);
                        cellHist.at(cellY, cellX+1)[0]+=weight*factorX*(1-factorY)*factorBin;
                        cellHist.at(cellY+1, cellX)[8]+=weight*(1-factorX)*factorY*(1-factorBin);
                        cellHist.at(cellY+1, cellX)[0]+=weight*(1-factorX)*factorY*factorBin;
                        cellHist.at(cellY+1, cellX+1)[8]+=weight*factorX*factorY*(1-factorBin);
                        cellHist.at(cellY+1, cellX+1)[0]+=weight*factorX*factorY*factorBin;
                    }
                }
            }
            cellX++;
        }
        cellX=0;
        cellY++;
    }
    
    //interpolation at the upper left corner
    for(int i=yWin; i<startY; i++){
        for(int j=xWin; j<startX; j++){
            _angle=*angle.at(i, j);
            index=_angle/20;
            weight=*magnitude.at(i, j);
            factorX=(j-xWin+4.5)/cellSize;
            factorY=(i-yWin+4.5)/cellSize;
            if(index<8){
                factorBin=(_angle-index*20)/20;
                cellHist.at(0, 0)[index]+=weight*factorX*factorY*(1-factorBin);
                cellHist.at(0, 0)[index+1]+=weight*factorX*factorY*factorBin;
            }else{
                factorBin=(_angle-160)/20;
                cellHist.at(0, 0)[8]+=weight*factorX*factorY*(1-factorBin);
                cellHist.at(0, 0)[0]+=weight*factorX*factorY*factorBin;
            }
        }
    }
    
    //interpolation at the upper right corner
    for(int i=yWin; i<startY; i++){
        for(int j=startX+(cellHistCols-1)*cellSize; j<xWin+winWidth; j++){
            _angle=*angle.at(i, j);
            index=_angle/20;
            weight=*magnitude.at(i, j);
            factorX=(j-xWin-3.5-(cellHistCols-1)*8)/cellSize;
            factorY=(i-yWin+4.5)/cellSize;
            if(index<8){
                factorBin=(_angle-index*20)/20;
                cellHist.at(0, cellHistCols-1)[index]+=weight*(1-factorX)*factorY*(1-factorBin);
                cellHist.at(0, cellHistCols-1)[index+1]+=weight*(1-factorX)*factorY*factorBin;
            }else{
                factorBin=(_angle-160)/20;
                cellHist.at(0, cellHistCols-1)[8]+=weight*(1-factorX)*factorY*(1-factorBin);
                cellHist.at(0, cellHistCols-1)[0]+=weight*(1-factorX)*factorY*factorBin;
            }
        }
    }
    
    //interpolation at the lower left corner
    for(int i=startY+(cellHistRows-1)*cellSize; i<yWin+winHeight; i++){
        for(int j=xWin; j<startX; j++){
            _angle=*angle.at(i, j);
            index=_angle/20;
            weight=*magnitude.at(i, j);
            factorX=(j-xWin+4.5)/cellSize;
            factorY=(i-yWin-3.5-(cellHistRows-1)*8)/cellSize;
            if(index<8){
                factorBin=(_angle-index*20)/20;
                cellHist.at(cellHistRows-1, 0)[index]+=weight*factorX*(1-factorY)*(1-factorBin);
                cellHist.at(cellHistRows-1, 0)[index+1]+=weight*factorX*(1-factorY)*factorBin;
            }else{
                factorBin=(_angle-160)/20;
                cellHist.at(cellHistRows-1, 0)[8]+=weight*factorX*(1-factorY)*(1-factorBin);
                cellHist.at(cellHistRows-1, 0)[0]+=weight*factorX*(1-factorY)*factorBin;
            }
        }
    }
    
    //interpolation at the lower right corner
    for(int i=startY+(cellHistRows-1)*cellSize; i<yWin+winHeight; i++){
        for(int j=startX+(cellHistCols-1)*cellSize; j<xWin+winWidth; j++){
            _angle=*angle.at(i, j);
            index=_angle/20;
            weight=*magnitude.at(i, j);
            factorX=(j-xWin-3.5-(cellHistCols-1)*8)/cellSize;
            factorY=(i-yWin-3.5-(cellHistRows-1)*8)/cellSize;
            if(index<8){
                factorBin=(_angle-index*20)/20;
                cellHist.at(cellHistRows-1, cellHistCols-1)[index]+=weight*(1-factorX)*(1-factorY)*(1-factorBin);
                cellHist.at(cellHistRows-1, cellHistCols-1)[index+1]+=weight*(1-factorX)*(1-factorY)*factorBin;
            }else{
                factorBin=(_angle-160)/20;
                cellHist.at(cellHistRows-1, cellHistCols-1)[8]+=weight*(1-factorX)*(1-factorY)*(1-factorBin);
                cellHist.at(cellHistRows-1, cellHistCols-1)[0]+=weight*(1-factorX)*(1-factorY)*factorBin;
            }
        }
    }
    
    //interpolation at the upper boundary
    for(int i=yWin; i<startY; i++){
        for(int j=startX; j<=endX; j+=cellSize){
            float leftBorder=xWin+cellX*cellSize+3.5f;
            for(int x=0; x<cellSize; x++){
                _angle=*angle.at(i, j+x);
                index=_angle/20;
                weight=*magnitude.at(i, j+x);
                factorX=(j+x-leftBorder)/cellSize;
                factorY=(i-yWin+4.5)/cellSize;
                if(index<8){
                    factorBin=(_angle-index*20)/20;
                    cellHist.at(0, cellX)[index]+=weight*(1-factorX)*factorY*(1-factorBin);
                    cellHist.at(0, cellX)[index+1]+=weight*(1-factorX)*factorY*factorBin;
                    cellHist.at(0, cellX+1)[index]+=weight*factorX*factorY*(1-factorBin);
                    cellHist.at(0, cellX+1)[index+1]+=weight*factorX*factorY*factorBin;
                }else{
                    factorBin=(_angle-160)/20;
                    cellHist.at(0, cellX)[8]+=weight*(1-factorX)*factorY*(1-factorBin);
                    cellHist.at(0, cellX)[0]+=weight*(1-factorX)*factorY*factorBin;
                    cellHist.at(0, cellX+1)[8]+=weight*factorX*factorY*(1-factorBin);
                    cellHist.at(0, cellX+1)[0]+=weight*factorX*factorY*factorBin;
                }
            }
            cellX++;
        }
        cellX=0;
    }

    //interpolation at the lower boundary
    for(int i=startY+(cellHistRows-1)*cellSize; i<yWin+winHeight; i++){
        for(int j=startX; j<=endX; j+=cellSize){
            float leftBorder=xWin+cellX*cellSize+3.5f;
            for(int x=0; x<cellSize; x++){
                _angle=*angle.at(i, j+x);
                index=_angle/20;
                weight=*magnitude.at(i, j+x);
                factorX=(j+x-leftBorder)/cellSize;
                factorY=(i-yWin-3.5-(cellHistRows-1)*8)/cellSize;
                if(index<8){
                    factorBin=(_angle-index*20)/20;
                    cellHist.at(cellHistRows-1, cellX)[index]+=weight*(1-factorX)*(1-factorY)*(1-factorBin);
                    cellHist.at(cellHistRows-1, cellX)[index+1]+=weight*(1-factorX)*(1-factorY)*factorBin;
                    cellHist.at(cellHistRows-1, cellX+1)[index]+=weight*factorX*(1-factorY)*(1-factorBin);
                    cellHist.at(cellHistRows-1, cellX+1)[index+1]+=weight*factorX*(1-factorY)*factorBin;
                }else{
                    factorBin=(_angle-160)/20;
                    cellHist.at(cellHistRows-1, cellX)[8]+=weight*(1-factorX)*(1-factorY)*(1-factorBin);
                    cellHist.at(cellHistRows-1, cellX)[0]+=weight*(1-factorX)*(1-factorY)*factorBin;
                    cellHist.at(cellHistRows-1, cellX+1)[8]+=weight*factorX*(1-factorY)*(1-factorBin);
                    cellHist.at(cellHistRows-1, cellX+1)[0]+=weight*factorX*(1-factorY)*factorBin;
                }
            }
            cellX++;
        }
        cellX=0;
    }
    
    //interpolation at the left boundary
    cellY=0;
    for(int i=startY; i<=endY; i+=cellSize){
        float topBorder=yWin+cellY*cellSize+3.5f;
        for(int j=xWin; j<startX; j++){
            for(int y=0; y<cellSize; y++){
                _angle=*angle.at(i+y, j);
                index=_angle/20;
                weight=*magnitude.at(i+y, j);
                factorX=(j-xWin+4.5)/cellSize;
                factorY=(i+y-topBorder)/cellSize;
                if(index<8){
                    factorBin=(_angle-index*20)/20;
                    cellHist.at(cellY, 0)[index]+=weight*factorX*(1-factorY)*(1-factorBin);
                    cellHist.at(cellY, 0)[index+1]+=weight*factorX*(1-factorY)*factorBin;
                    cellHist.at(cellY+1, 0)[index]+=weight*factorX*factorY*(1-factorBin);
                    cellHist.at(cellY+1, 0)[index+1]+=weight*factorX*factorY*factorBin;
                }else{
                    factorBin=(_angle-160)/20;
                    cellHist.at(cellY, 0)[8]+=weight*factorX*(1-factorY)*(1-factorBin);
                    cellHist.at(cellY, 0)[0]+=weight*factorX*(1-factorY)*factorBin;
                    cellHist.at(cellY+1, 0)[8]+=weight*factorX*factorY*(1-factorBin);
                    cellHist.at(cellY+1, 0)[0]+=weight*factorX*factorY*factorBin;
                }
            }
        }
        cellY++;
    }
    
    //interpolation at the right boundary
    cellY=0;
    for(int i=startY; i<=endY; i+=cellSize){
        float topBorder=yWin+cellY*cellSize+3.5f;
        for(int j=startX+(cellHistCols-1)*cellSize; j<xWin+winWidth; j++){
            for(int y=0; y<cellSize; y++){
                _angle=*angle.at(i+y, j);
                index=_angle/20;
                weight=*magnitude.at(i+y, j);
                factorX=(j-xWin-3.5-(cellHistCols-1)*8)/cellSize;
                factorY=(i+y-topBorder)/cellSize;
                if(index<8){
                    factorBin=(_angle-index*20)/20;
                    cellHist.at(cellY, cellHistCols-1)[index]+=weight*(1-factorX)*(1-factorY)*(1-factorBin);
                    cellHist.at(cellY, cellHistCols-1)[index+1]+=weight*(1-factorX)*(1-factorY)*factorBin;
                    cellHist.at(cellY+1, cellHistCols-1)[index]+=weight*(1-factorX)*factorY*(1-factorBin);
                    cellHist.at(cellY+1, cellHistCols-1)[index+1]+=weight*(1-factorX)*factorY*factorBin;
                }else{
                    factorBin=(_angle-160)/20;
                    cellHist.at(cellY, cellHistCols-1)[8]+=weight*(1-factorY)*(1-factorBin);
                    cellHist.at(cellY, cellHistCols-1)[0]+=weight*(1-factorY)*factorBin;
                    cellHist.at(cellY+1, cellHistCols-1)[8]+=weight*(1-factorX)*factorY*(1-factorBin);
                    cellHist.at(cellY+1, cellHistCols-1)[0]+=weight*(1-factorX)*factorY*factorBin;
                }
            }
        }
        cellY++;
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

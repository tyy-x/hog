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

int HOGFeature::LUT[9]={0, 20, 40, 60, 80, 100, 120, 140, 160};

void HOGFeature::initial()
{
    cellHistRows=winHeight/cellSize;
    cellHistCols=winWidth/cellSize;
    blockHistRows=cellHistRows-blockSize/cellSize+1;
    blockHistCols=cellHistCols-blockSize/cellSize+1;
    blockHistSize=9*blockSize/cellSize*blockSize/cellSize;
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

//calculate cell histogram and generate block histogoram
void HOGFeature::computeCellHistogram(int yWin, int xWin)
{
    int index;
    int count1=0;
    int count2=0;
    int count3=0;
    
    //calculate histogram of each cell
    for(int i=yWin; i<=yWin+winHeight-cellSize; i+=cellSize){
        for(int j=xWin; j<=xWin+winWidth-cellSize; j+=cellSize){
            for(int y=0; y<cellSize; y++){
                for(int x=0; x<cellSize; x++){
                    index=*angle.at(i+y, j+x)/20.0;
                    if(index<8){
                        cellHist.at(count1, count2)[index]+=(1-(*angle.at(i+y, j+x)-LUT[index])/20.0)*(*magnitude.at(i+y, j+x));
                        cellHist.at(count1, count2)[index+1]+=(*angle.at(i+y, j+x)-LUT[index])/20.0*(*magnitude.at(i+y, j+x));
                    }else if(index==8){
                        cellHist.at(count1, count2)[8]+=(1-(*angle.at(i+y, j+x)-LUT[8])/20.0)*(*magnitude.at(i+y, j+x));
                        cellHist.at(count1, count2)[0]+=(*angle.at(i+y, j+x)-LUT[8])/20.0*(*magnitude.at(i+y, j+x));
                    }else cellHist.at(count1, count2)[0]+=(*magnitude.at(i+y, j+x));
                }
            }
            count2++;
        }
        count2=0;
        count1++;
    }
    
    //generate histogram of each block
    for(int i=0; i<blockHistRows; i++){
        for(int j=0; j<blockHistCols; j++){
            for(int y=0; y<blockSize/cellSize; y++){
                for(int x=0; x<blockSize/cellSize; x++){
                    for(int k=0; k<bins; k++){
                        blockHist.at(i, j)[count3]=cellHist.at(i+y, j+x)[k];
                        count3++;
                    }
                }
            }
            count3=0;
        }
    }
}

//normalize block histogram
void HOGFeature::normalizeBlockHistogram()
{
    vector<vector<double>> sum;
    
    sum.resize(blockHistRows);
    for(int i=0; i<blockHistRows; i++){
        sum[i].resize(blockHistCols);
    }
    
    for(int i=0; i<blockHistRows; i++){
        for(int j=0; j<blockHistCols; j++){
            for(int k=0; k<blockHistSize; k++){
                sum[i][j]+=blockHist.at(i, j)[k]*blockHist.at(i, j)[k];
            }
            sum[i][j]=sqrt(sum[i][j])+1.0;
        }
    }
    
    //normalize histogram for each block
    for(int i=0; i<blockHistRows; i++){
        for(int j=0; j<blockHistCols; j++){
            for(int k=0; k<blockHistSize; k++){
                blockHist.at(i, j)[k]/=sum[i][j];
            }
        }
    }
    
    //debug
    //for(int i=0; i<36; i++) cout<<i+1<<" "<<" "<<" "<<bHist[0][0][i]<<endl;
}

//compute hog feature in the scanning window
void HOGFeature::computeHOGFeature(int yWin, int xWin)
{
    //clear up data
    //for(int i=0; i<hogVectorSize; i++) hogVector[i]=0;
    
    computeCellHistogram(yWin, xWin);
    normalizeBlockHistogram();
    
    int count=0;
    for(int i=0; i<blockHistRows; i++){
        for(int j=0; j<blockHistCols; j++){
            for(int k=0; k<blockHistSize; k++){
                hogVector[count++]=blockHist.at(i, j)[k];
            }
        }
    }
    //debug
    //for(int i=0; i<36; i++) cout<<i+1<<" "<<" "<<HOGDescriptor[i]<<endl;
}

//save hog feature data to a file
void HOGFeature::save(const string &filename)
{
    ofstream output(filename, ofstream::binary);
    //FILE *fp=fopen("hog_feature.txt", "w");
    
    if(!output){
        cerr<<"#Error: file not opened!"<<endl;
        exit(1);
    }
    
    output<<"#hog feature"<<endl;
    int i=0;
    for(auto item:hogVector){
        output<<left;
        output<<setw(6)<<++i<<item<<endl;
    }
}

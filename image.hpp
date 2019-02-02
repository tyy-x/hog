//
//  image.hpp
//  _image
//
//  Created by 唐宇阳 on 2019/01/29.
//  Copyright © 2019 唐宇阳. All rights reserved.
//

#ifndef IMAGE_HPP
#define IMAGE_HPP

#include <iostream>

namespace img {
    typedef unsigned char uchar;
    enum {GRAYSCALE=1, COLOR=3};
    enum {READ_GRAYSCALE=1, READ_COLOR=3};
    /* image class for 8-bit images*/
    class Image{
    private:
        int counter=0; //when counter reacher 0, release the memory for data matrix
        int channels=GRAYSCALE; //channels of image
        uchar *begin=nullptr; //pointer pointed to the start of the data matrix
        uchar *end=nullptr; //pointer pointed to the location just immediately pass the last element of the data matrix
        
        void allocate(int _rows, int _cols, int _channels); //allocate memory
        
    public:
        int rows=0; //rows of image
        int cols=0; //columns of image
        uchar *data=nullptr; //image data matrix
        //constructor
        Image() { /*debug */ std::cout<<"default constructor"<<std::endl; std::cout<<"counter: "<<counter<<std::endl;}
        Image(int _rows, int _cols, int _channels);
        Image(int _rows, int _cols);
        Image(const Image &img);
        
        //destructor
        ~Image();
        //overload assignment operator
        Image& operator=(const Image &img);
        
        //return pointer which points to the first element of every row of the data matrix
        uchar* rowPointer(int _rows) { return data+_rows*cols*channels;}
        
        //calculate the position of the pixel of one channel image and return its value by reference
        uchar& at(int _rows, int _cols) { return data[_rows*cols+_cols];}
        //overload at function to allow const object to call it
        const uchar at(int _rows, int _cols) const { return data[_rows*cols+_cols];}
        
        //return pointer of every element of a 3 channel image
        uchar* at3channel(int _rows, int _cols) { return data+_rows*cols*3+_cols*3;}
        //overload at3channel function to aloow const object to call it
        const uchar* at3channel(int _rows, int _cols) const { return data+_rows*cols*3+_cols*3;}
        
        //copy an existence Image object with the same channel number
        void copy(const Image &img);
    };
    
    inline Image::Image(int _rows, int _cols, int _channels):rows(_rows), cols(_cols), channels(_channels)
    {
        std::cout<<"constructor1"<<std::endl;
        allocate(_rows, _cols, _channels);
        begin=data;
        end=data+rows*cols*channels;
        std::cout<<"counter: "<<counter<<std::endl;
    }
    
    inline Image::Image(int _rows, int _cols):rows(_rows), cols(_cols)
    {
        allocate(_rows, _cols, GRAYSCALE);
        begin=data;
        end=data+rows*cols*channels;
    }
    
    inline Image::Image(const Image &img)
    {
        std::cout<<"copy constructor"<<std::endl;
        rows=img.rows;
        cols=img.cols;
        channels=img.channels;
        counter=(img.counter==0) ? img.counter : img.counter+1;
        data=img.data;
        begin=img.begin;
        end=img.end;
        std::cout<<"counter: "<<counter<<std::endl;
    }
    
    inline Image::~Image()
    {
        counter--;
        std::cout<<"~Image() counter: "<<counter<<std::endl;
        if(!counter){
            std::cout<<"free"<<std::endl;
            delete[] data;
            data=nullptr;
            begin=nullptr;
            end=nullptr;
        }
    }
    
    inline void Image::allocate(int _rows, int _cols, int _channels)
    {
        //std::cout<<"allocate"<<std::endl;
        data=new uchar[_channels*_rows*_cols]();
        counter++;
    }
    
    inline Image& Image::operator=(const Image &img)
    {
        std::cout<<"operator="<<std::endl;
        if(counter==1){
            delete[] data;
            data=nullptr;
            begin=nullptr;
            end=nullptr;
            counter--;
            std::cout<<"counter: "<<counter<<" free"<<std::endl;
        }
        rows=img.rows;
        cols=img.cols;
        channels=img.channels;
        counter=(img.counter==0) ? img.counter : img.counter+1;
        data=img.data;
        begin=img.begin;
        end=img.end;
        std::cout<<"counter: "<<counter<<std::endl;
        
        return *this;
    }
    
    inline void Image::copy(const Image &img)
    {
        if(channels!=img.channels){
            std::cerr<<"the image channel is not the same"<<std::endl;
            exit(1);
        }
        if(rows!=img.rows || cols!=img.cols){
            std::cerr<<"not the same size image"<<std::endl;
            exit(1);
        }
        for(int i=0; i<rows*cols*channels; i++) data[i]=img.data[i];
    }
}

#endif /* IMAGE_HPP */

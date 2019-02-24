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
    enum {READ_GRAYSCALE, READ_COLOR};
    enum {IMG_UC1=1, IMG_UC2=2, IMG_UC3=3};
    /* image class for 8-bit images*/
    class Image{
    private:
        int counter=0; //when counter reacher 0, release the memory for data matrix
        int size=0; //data size
        int depth=IMG_UC1; //depth of pixel value
        void allocate(int _rows, int _cols, int _depth); //allocate memory
        
    public:
        int rows=0; //rows of image
        int cols=0; //columns of image
        uchar *data=nullptr; //image data matrix
        
        //constructor
        Image() { /*debug */ std::cout<<"default constructor"<<std::endl; std::cout<<"counter: "<<counter<<std::endl;}
        Image(int _rows, int _cols, int _depth);
        
        //overload contructor, default generate a one-channel image data matrix
        Image(int _rows, int _cols);
        
        //copy constructor
        Image(const Image &img);
        
        //destructor
        ~Image();
        //overload assignment operator
        Image& operator=(const Image &img);
        
        //create a data matrix
        void create(int _rows, int _cols, int _depth);
        
        //return pointer which points to the first element of every row of the data matrix
        uchar* rowPtr(int _rows);
        const uchar* rowPtr(int _rows) const;
        
        /*
         * calculate element position and return its pointer
         * example: *at(1, 2) for one channel image
         *          at(1, 2)[0] at(1, 2)[1] for two-channel image
        */
        uchar* at(int _rows, int _cols);
        const uchar* at(int _rows, int _cols) const;
        
        //copy an existence Image object with the same channel number
        void copy(const Image &img);
    };
    
    inline Image::Image(int _rows, int _cols, int _depth):rows(_rows), cols(_cols), depth(_depth)
    {
        std::cout<<"constructor1"<<std::endl;
        allocate(_rows, _cols, _depth);
        size=rows*cols*depth;
        std::cout<<"counter: "<<counter<<std::endl;
    }
    
    inline Image::Image(int _rows, int _cols):rows(_rows), cols(_cols)
    {
        allocate(_rows, _cols, IMG_UC1);
        size=_rows*_cols*depth;
        std::cout<<"constructor2"<<std::endl;
        std::cout<<"counter: "<<counter<<std::endl;
    }
    
    inline Image::Image(const Image &img)
    {
        std::cout<<"copy constructor"<<std::endl;
        size=img.size;
        rows=img.rows;
        cols=img.cols;
        depth=img.depth;
        counter=(img.counter==0) ? img.counter : img.counter+1;
        data=img.data;
        std::cout<<"counter: "<<counter<<std::endl;
    }
    
    inline Image::~Image()
    {
        counter--;
        std::cout<<"~Image() counter: "<<counter<<std::endl;
        if(!counter){
            std::cout<<"release"<<std::endl;
            delete[] data;
            data=nullptr;
        }
    }
    
    inline void Image::allocate(int _rows, int _cols, int _depth)
    {
        std::cout<<"allocate"<<std::endl;
        data=new uchar[_depth*_rows*_cols]();
        counter++;
    }
    
    inline Image& Image::operator=(const Image &img)
    {
        std::cout<<"operator="<<std::endl;
        if(counter==1){
            delete[] data;
            data=nullptr;
            counter--;
            std::cout<<"counter: "<<counter<<" free"<<std::endl;
        }
        size=img.size;
        rows=img.rows;
        cols=img.cols;
        depth=img.depth;
        counter=(img.counter==0) ? img.counter : img.counter+1;
        data=img.data;
        std::cout<<"counter: "<<counter<<std::endl;
        
        return *this;
    }
    
    inline void Image::create(int _rows, int _cols, int _depth)
    {
        allocate(_rows, _cols, _depth);
        size=_rows*_cols*_depth;
        rows=_rows;
        cols=_cols;
        depth=_depth;
    }

    inline uchar* Image::rowPtr(int _rows)
    {
        if(_rows*cols*depth>=size){
            std::cerr<<"#ERROR: BAD ACCESS"<<std::endl;
            exit(1);
        }
        return data+_rows*cols*depth;
    }
    
    inline const uchar* Image::rowPtr(int _rows) const
    {
        if(_rows*cols*depth>=size){
            std::cerr<<"#ERROR: BAD ACCESS"<<std::endl;
            exit(1);
        }
        return data+_rows*cols*depth;
    }
    
    inline uchar* Image::at(int _rows, int _cols)
    {
        if((_rows*cols*depth+_cols*depth)>=size){
            std::cerr<<"#ERROR: BAD ACCESS"<<std::endl;
            exit(1);
        }
        return data+_rows*cols*depth+_cols*depth;
    }
    
    inline const uchar* Image::at(int _rows, int _cols) const
    {
        if((_rows*cols*depth+_cols*depth)>=size){
            std::cerr<<"#ERROR: BAD ACCESS"<<std::endl;
            exit(1);
        }
        return data+_rows*cols*depth+_cols*depth;
    }
    
    inline void Image::copy(const Image &img)
    {
        if(depth!=img.depth | rows!=img.rows | cols!=img.cols){
            std::cerr<<"copy failed"<<std::endl;
            exit(1);
        }
        int _size=rows*cols*depth;
        for(int i=0; i<_size; i++) data[i]=img.data[i];
    }
}

#endif /* IMAGE_HPP */

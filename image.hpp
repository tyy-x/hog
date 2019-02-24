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
#include <vector>

namespace img {
    typedef unsigned char uchar;
    typedef std::vector<uchar>::iterator Ptr;
    typedef std::vector<uchar>::const_iterator constPtr;
    enum {READ_GRAYSCALE, READ_COLOR};
    enum {IMG_UC1=1, IMG_UC2=2, IMG_UC3=3};
    /* image class for 8-bit images*/
    class Image{
    private:
        int depth=IMG_UC1; //depth of pixel value
        
    public:
        int rows=0; //rows of image
        int cols=0; //columns of image
        int size=0; //data size
        std::vector<uchar> data; //image data matrix
        
        //constructor
        Image() {}
        Image(int _rows, int _cols, int _depth);
        
        //overload contructor, default generate a one-channel image data matrix
        Image(int _rows, int _cols);
        
        //copy constructor
        Image(const Image &img);
        
        //destructor
        //~Image() {}
        //overload assignment operator
        Image& operator=(const Image &img);
        
        //return iterator which points to the first element of every row of the data matrix
        Ptr rowPtr(int _rows);
        constPtr rowPtr(int _rows) const;
        
        /*
         * calculate element position and return its iterator
         * example: *at(1, 2) for one channel image
         *           at(1, 2)[0] at(1, 2)[1] for two-channel image
        */
        Ptr at(int _rows, int _cols);
        constPtr at(int _rows, int _cols) const;
    };
    
    inline Image::Image(int _rows, int _cols, int _depth):rows(_rows), cols(_cols), depth(_depth)
    {
        size=rows*cols*depth;
        data.resize(size);
    }
    
    inline Image::Image(int _rows, int _cols):rows(_rows), cols(_cols)
    {
        size=_rows*_cols*depth;
        data.resize(size);
    }
    
    inline Image::Image(const Image &img)
    {
        size=img.size;
        rows=img.rows;
        cols=img.cols;
        depth=img.depth;
        data=img.data;
    }
    
    /*
    inline void Image::allocate(int _rows, int _cols, int _depth)
    {
        std::cout<<"allocate"<<std::endl;
        data=new uchar[_depth*_rows*_cols]();
        counter++;
    }
    */
    
    inline Image& Image::operator=(const Image &img)
    {
        size=img.size;
        rows=img.rows;
        cols=img.cols;
        depth=img.depth;
        data=img.data;
        
        return *this;
    }

    inline Ptr Image::rowPtr(int _rows)
    {
        if((data.begin()+_rows*cols*depth)==data.end()){
            std::cerr<<"#ERROR: BAD ACCESS"<<std::endl;
            exit(1);
        }
        return data.begin()+_rows*cols*depth;
    }
    
    inline constPtr Image::rowPtr(int _rows) const
    {
        if((data.begin()+_rows*cols*depth)==data.end()){
            std::cerr<<"#ERROR: BAD ACCESS"<<std::endl;
            exit(1);
        }
        return data.begin()+_rows*cols*depth;
    }
    
    inline std::vector<uchar>::iterator Image::at(int _rows, int _cols)
    {
        if((data.begin()+_rows*cols*depth+_cols*depth)==data.end()){
            std::cerr<<"#ERROR: BAD ACCESS"<<std::endl;
            exit(1);
        }
        return data.begin()+_rows*cols*depth+_cols*depth;
    }
    
    inline constPtr Image::at(int _rows, int _cols) const
    {
        if((data.begin()+_rows*cols*depth+_cols*depth)==data.end()){
            std::cerr<<"#ERROR: BAD ACCESS"<<std::endl;
            exit(1);
        }
        return data.begin()+_rows*cols*depth+_cols*depth;
    }
}

#endif /* IMAGE_HPP */

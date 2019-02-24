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
    template <typename T> class Mat{
    private:
        int counter=0; //when counter reacher 0, release the memory for data matrix
        int size=0; //data size
        int depth=IMG_UC1; //depth of pixel value
        void allocate(int _rows, int _cols, int _depth); //allocate memory
        
    public:
        int rows=0; //rows of image
        int cols=0; //columns of image
        T *data=nullptr; //image data matrix
        
        //constructor
        Mat() { /*debug */ std::cout<<"default constructor"<<std::endl; std::cout<<"counter: "<<counter<<std::endl;}
        Mat(int _rows, int _cols, int _depth);
        
        //overload contructor, default generate a one-channel image data matrix
        Mat(int _rows, int _cols);
        
        //copy constructor
        Mat(const Mat &img);
        
        //destructor
        ~Mat();
        //overload assignment operator
        Mat& operator=(const Mat &img);
        
        //create a data matrix
        void create(int _rows, int _cols, int _depth);
        
        //return pointer which points to the first element of every row of the data matrix
        T* rowPtr(int _rows);
        const T* rowPtr(int _rows) const;
        
        /*
         * calculate element position and return its pointer
         * example: *at(1, 2) for one channel image
         *          at(1, 2)[0] at(1, 2)[1] for two-channel image
        */
        T* at(int _rows, int _cols);
        const T* at(int _rows, int _cols) const;
        
        //copy an existence Image object with the same channel number
        void copy(const Mat &img);
    };
    
    template <typename T>
    inline Mat<T>::Mat(int _rows, int _cols, int _depth):rows(_rows), cols(_cols), depth(_depth)
    {
        std::cout<<"constructor1"<<std::endl;
        allocate(_rows, _cols, _depth);
        size=rows*cols*depth;
        std::cout<<"counter: "<<counter<<std::endl;
    }
    
    template <typename T>
    inline Mat<T>::Mat(int _rows, int _cols):rows(_rows), cols(_cols)
    {
        std::cout<<"constructor2"<<std::endl;
        allocate(_rows, _cols, depth);
        size=_rows*_cols*depth;
        std::cout<<"counter: "<<counter<<std::endl;
    }
    
    template <typename T>
    inline Mat<T>::Mat(const Mat &img)
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
    
    template <typename T>
    inline Mat<T>::~Mat()
    {
        counter--;
        std::cout<<"~Mat() counter: "<<counter<<std::endl;
        if(!counter){
            std::cout<<"release"<<std::endl;
            delete[] data;
            data=nullptr;
        }
    }
    
    template <typename T>
    inline void Mat<T>::allocate(int _rows, int _cols, int _depth)
    {
        std::cout<<"allocate"<<std::endl;
        data=new T[_depth*_rows*_cols]();
        counter++;
    }
    
    template <typename T>
    Mat<T>& Mat<T>::operator=(const Mat &img)
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
    
    template <typename T>
    inline void Mat<T>::create(int _rows, int _cols, int _depth)
    {
        allocate(_rows, _cols, _depth);
        size=_rows*_cols*_depth;
        rows=_rows;
        cols=_cols;
        depth=_depth;
    }

    template <typename T>
    inline T* Mat<T>::rowPtr(int _rows)
    {
        if(_rows*cols*depth>=size){
            std::cerr<<"#ERROR: BAD ACCESS"<<std::endl;
            exit(1);
        }
        return data+_rows*cols*depth;
    }
    
    template <typename T>
    inline const T* Mat<T>::rowPtr(int _rows) const
    {
        if(_rows*cols*depth>=size){
            std::cerr<<"#ERROR: BAD ACCESS"<<std::endl;
            exit(1);
        }
        return data+_rows*cols*depth;
    }
    
    template <typename T>
    inline T* Mat<T>::at(int _rows, int _cols)
    {
        if((_rows*cols*depth+_cols*depth)>=size){
            std::cerr<<"#ERROR: BAD ACCESS"<<std::endl;
            exit(1);
        }
        return data+_rows*cols*depth+_cols*depth;
    }
    
    template <typename T>
    inline const T* Mat<T>::at(int _rows, int _cols) const
    {
        if((_rows*cols*depth+_cols*depth)>=size){
            std::cerr<<"#ERROR: BAD ACCESS"<<std::endl;
            exit(1);
        }
        return data+_rows*cols*depth+_cols*depth;
    }
    
    template <typename T>
    inline void Mat<T>::copy(const Mat &img)
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

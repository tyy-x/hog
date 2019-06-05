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
    enum {Box, Prewitt, Sobel, Gaussian};
    /* image class for 8-bit images*/
    template <typename T> class Mat{
    private:
        int size=0; //data size
        uchar depth=IMG_UC1; //depth of pixel value
        void allocate(int _rows, int _cols, uchar _depth); //allocate memory
        void release(); //release memory
        void copyFrom(const Mat<T> &img); //copy data
        
    public:
        int rows=0; //rows of image
        int cols=0; //columns of image
        T *data=nullptr; //image data matrix
        
        //constructor
        Mat() { /*debug */ /*std::cout<<"default constructor"<<std::endl;*/}
        Mat(int _rows, int _cols, uchar _depth);
        
        //overload contructor, default generate a one-channel image data matrix
        Mat(int _rows, int _cols);
        
        //copy constructor
        Mat(const Mat &img);
        
        //To do
        //copy data from vector or array
        Mat(int _rows, int _cols, std::vector<T> &vec);
        Mat(int _rows, int _cols, std::vector<std::vector<T>> &vec);
        Mat(int _rows, int _cols, const T *_data);
        Mat(int _rows, int _cols, const T **_data);
        
        //destructor
        ~Mat();
        //overload assignment operator
        Mat& operator=(const Mat &img);
        
        //create a data matrix
        void create(int _rows, int _cols, uchar _depth);
        
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
    };
    
    struct Point{
        float x;
        float y;
        Point() {}
        Point(float _x, float _y):x(_x), y(_y) {}
        Point(const Point &obj):x(obj.x), y(obj.y) {}
        Point operator=(const Point &obj);
    };
    
    inline Point Point::operator=(const Point &obj)
    {
        x=obj.x;
        y=obj.y;
        return *this;
    }
    
    struct Rect{
        Point point1;
        Point point2;
        Point center;
        float width;
        float height;
        Rect() {}
        Rect(Point _point1, Point _point2);
        Rect(const Rect &obj):point1(obj.point1), point2(obj.point2), center(obj.center), width(obj.width), height(obj.height) {}
        Rect operator=(const Rect &obj);
    };
    
    inline Rect::Rect(Point _point1, Point _point2):point1(_point1), point2(_point2)
    {
        width=point2.x-point1.x;
        height=point2.y-point1.y;
        center.x=point1.x+width/2.f;
        center.y=point1.y+height/2.f;
    }
    
    inline Rect Rect::operator=(const Rect &obj)
    {
        point1=obj.point1;
        point2=obj.point2;
        width=obj.width;
        height=obj.height;
        center=obj.center;
        return *this;
    }
    
    template <typename T>
    inline Mat<T>::Mat(int _rows, int _cols, uchar _depth):rows(_rows), cols(_cols), depth(_depth)
    {
        //std::cout<<"constructor1"<<std::endl;
        allocate(_rows, _cols, _depth);
        size=rows*cols*depth;
    }
    
    template <typename T>
    inline Mat<T>::Mat(int _rows, int _cols):rows(_rows), cols(_cols)
    {
        //std::cout<<"constructor2"<<std::endl;
        allocate(_rows, _cols, depth);
        size=_rows*_cols*depth;
    }
    
    template <typename T>
    inline Mat<T>::Mat(const Mat &img)
    {
        //std::cout<<"copy constructor"<<std::endl;
        size=img.size;
        rows=img.rows;
        cols=img.cols;
        depth=img.depth;
        data=new T[depth*rows*cols];
        copyFrom(img);
    }
    
    template <typename T>
    inline Mat<T>::~Mat()
    {
        //std::cout<<"~Mat()"<<std::endl;
        release();
    }
    
    template <typename T>
    inline void Mat<T>::allocate(int _rows, int _cols, uchar _depth)
    {
        //std::cout<<"allocate"<<std::endl;
        data=new T[_depth*_rows*_cols]();
    }
    
    template <typename T>
    inline void Mat<T>::release()
    {
        if(data){
            //std::cout<<"release memory"<<std::endl;
            delete[] data;
            data=nullptr;
        }
    }
    
    template <typename T>
    inline void Mat<T>::copyFrom(const Mat<T> &img)
    {
        for(int i=0; i<size; i++) data[i]=img.data[i];
    }
    
    template <typename T>
    Mat<T>& Mat<T>::operator=(const Mat &img)
    {
        //std::cout<<"operator="<<std::endl;
        size=img.size;
        rows=img.rows;
        cols=img.cols;
        depth=img.depth;
        release();
        data=new T[depth*rows*cols];
        copyFrom(img);
        
        return *this;
    }
    
    template <typename T>
    inline void Mat<T>::create(int _rows, int _cols, uchar _depth)
    {
        if(!data){
            allocate(_rows, _cols, _depth);
            size=_rows*_cols*_depth;
            rows=_rows;
            cols=_cols;
            depth=_depth;
        }
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
            std::cerr<<_rows*cols*depth+_cols*depth<<std::endl;
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
}

#endif /* IMAGE_HPP */

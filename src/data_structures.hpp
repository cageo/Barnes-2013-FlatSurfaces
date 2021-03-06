/**
  @file
  Template code defining data structures used throughout the package.
  Defines 2D arrays and grid cells.

  Richard Barnes (rbarnes@umn.edu), 2012
*/
#ifndef _data_structures_included
#define _data_structures_included

#include <cstdio>
#include <iostream>
#include <iomanip>
#include <string>
#include <vector>

template <class T>
class array2d {
  private:
    typedef std::vector< std::vector<T> > arr2d;
    arr2d data;
  public:
    ///Value corresponding the length of one edge of a square DEM cell
    double cellsize;
    ///Global grid location of lower left x-coordinate
    double xllcorner;
    ///Global grid location of lower left y-coordinate
    double yllcorner;
    ///Number of cells containing data (excludes NO_DATA cells)
    long data_cells;
    ///NO_DATA value. The cell does not contain data, should not be processed.
    T no_data;

    array2d ();
    ///Creates array2d with no data, but invokes copyprops()
    template<class U> array2d (const array2d<U> &copyfrom);
    long width() const  {return data[0].size();}
    long height() const {return data.size();}
    ///Copys everything but the data from another array2d
    template<class U> void copyprops (const array2d<U> &copyfrom);
    ///Prints an estimate of the file size were the array printed in ASCII
    long estimated_output_size();
    ///Sets all the cells of an array2d to "val"
    void init(T val);
    ///Returns true if each cell of the array2d equals its counterpart in "other"
    bool operator==(const array2d<T> &other) const;
    ///Returns true if (x,y) is within the bounds of the array2d
    inline bool in_grid(int x, int y) const
      {return (x>=0 && y>=0 && x<width() && y<height());}
    ///Returns true if (x,y) is not an edge cell
    inline bool interior_grid(int x, int y) const
      {return (x>=1 && y>=1 && x<width()-1 && y<height()-1);}
    ///Returns true if (x,y) lies on the edge of the array2d
    inline bool edge_grid(int x, int y) const
      {return (x==0 || y==0 || x==width()-1 || y==height()-1);}
    ///Returns a reference to (x,y)
    T& operator()(int x, int y)
      {return data[y][x];}
    ///Returns a const reference to (x,y)
    const T& operator()(int x, int y) const
      {return data[y][x];}
    ///Resizes the array2d. May or may not be destructive to existing data.
    void resize(int width, int height);
    ///Destroys all data in the array2d.
    void clear() {data.clear();}
};

template <class T>
array2d<T>::array2d(){
  cellsize=-1;
  xllcorner=-1;
  yllcorner=-1;
  data_cells=-1;
  no_data=-1;
}

template <class T>
void array2d<T>::resize(int width, int height){
  fprintf(
    stderr,
    "\n\tApprox RAM requirement: %lluMB\n",
    (unsigned long long)width * (unsigned long long)height *
      (unsigned long long)sizeof(T) / 1024 / 1024
  );
  data.resize(height, std::vector<T> (width));
}

template<class T>
template<class U>
void array2d<T>::copyprops (const array2d<U> &copyfrom){
  cellsize=copyfrom.cellsize;
  xllcorner=copyfrom.xllcorner;
  yllcorner=copyfrom.yllcorner;
  data_cells=copyfrom.data_cells;
  no_data=copyfrom.no_data;
  resize(copyfrom.width(),copyfrom.height());
}

template <class T>
template <class U>
array2d<T>::array2d(const array2d<U> &copyfrom){
  *this=array2d();
  copyprops(copyfrom);
}

template <class T>
void array2d<T>::init(T val){
  #pragma omp parallel for
  for(int x=0;x<width();x++)
  for(int y=0;y<height();y++)
    operator()(x,y)=val;
}

template <> inline long array2d<float>::estimated_output_size(){
  return 9*this->width()*this->height();
}
template <> inline long array2d<char>::estimated_output_size(){
  return 4*this->width()*this->height();
}
template <> inline long array2d<bool>::estimated_output_size(){
  return 2*this->width()*this->height();
}
template <> inline long array2d<unsigned int>::estimated_output_size(){
  return 9*this->width()*this->height();
}

template <class T>
bool array2d<T>::operator==(const array2d<T> &other) const {
  if(width()!=other.width() || height()!=other.height())
    return false; 
  for(int x=0;x<width();x++)
  for(int y=0;y<height();y++)
    if(operator()(x,y)!=other(x,y))
      return false;
  return true;
}




typedef array2d<double>       double_2d;
typedef array2d<float>        float_2d;
typedef array2d<signed char>  char_2d;
typedef array2d<bool>         bool_2d;
typedef array2d<unsigned int> uint_2d;
typedef array2d<int>          int_2d;


/// Stores the (x,y) coordinates of a grid cell
class grid_cell {
  public:
    int x; ///< Grid cell's x-coordinate
    int y; ///< Grid cell's y-coordinate
    /// Initiate the grid cell without coordinates; should generally be avoided
    grid_cell(){}
    /// Initiate the grid cell to the coordinates (x0,y0)
    grid_cell(int x, int y):x(x),y(y){}
};

#endif

/****************************************************************************
  FileName     [ array.h ]
  PackageName  [ util ]
  Synopsis     [ Define dynamic array package ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2005-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef ARRAY_H
#define ARRAY_H

#include <cassert>
#include <algorithm>

using namespace std;

// NO need to implement class ArrayNode
//
template <class T>
class Array
{
public:
   // TODO: decide the initial value for _isSorted
   Array() : _data(0), _size(0), _capacity(0) {}
   ~Array() { delete []_data; }

   // DO NOT add any more data member or function for class iterator
   class iterator
   {
      friend class Array;

   public:
      iterator(T* n= 0): _node(n) {}
      iterator(const iterator& i): _node(i._node) {}
      ~iterator() {} // Should NOT delete _node

      // TODO: implement these overloaded operators
      const T& operator * () const { return (*_node); }
      T& operator * () { return (*_node); }
      iterator& operator ++ () {
          _node++;
          return (*this); }
      iterator operator ++ (int) {
          iterator temp = *this;
          ++(*this);
          return temp; }
      iterator& operator -- () {
          _node--;
          return (*this); }
      iterator operator -- (int) {
          iterator temp = *this;
          --(*this);
          return temp; }

      iterator operator + (int i) const {
          return (iterator(_node+i)); }
      iterator& operator += (int i) {
          _node = _node+i;
          return (*this); }

      iterator& operator = (const iterator& i) {
          _node = i._node;
          return (*this); }

      bool operator != (const iterator& i) const {
          return _node!=i._node; }
      bool operator == (const iterator& i) const {
          return _node==i._node; }

   private:
      T*    _node;
   };

   // TODO: implement these functions
   iterator begin() const { return iterator(_data); }
   iterator end() const { return iterator(_data+_size); }
   bool empty() const { return _data==0; }
   size_t size() const { return _size; }

   T& operator [] (size_t i) {
       assert(i<_size);
       return _data[i]; }
   const T& operator [] (size_t i) const {
       assert(i<_size);
       return _data[i]; }

   void push_back(const T& x) {
       if(_size < _capacity)
       {
           _data[_size] = x;
           _size++;}
       else if(_size == _capacity)
       {
           expand();
           if(_data !=0)
           {
               T* temp = new T[_capacity];
               for(size_t k = 0; k<_size; k++)
               {
                   temp[k] = _data[k];
               }
               delete []_data;
               _data=temp;
           }
           else if(_data == 0)
           {
               _data = new T[_capacity];
           }
           _data[_size] = x;
           _size++;}}
    
   void pop_front() {
       _data[0] = _data[_size-1];
       _size--;}
    
   void pop_back() {
       _size--;}

   bool erase(iterator pos) {
       int index = pos._node-_data;
       if(index==_size)
           return false;
       else{
           _data[index] = _data[_size-1];
           _size--;
           return true;}}
    
   bool erase(const T& x) {
       size_t index = -1;
       for(size_t k=0; k<_size; k++)
       {
           if(_data[k]==x)
           {
               index = k;
               break;
           }
       }
       if(index==-1)
           return false;
       else{
           _data[index] = _data[_size-1];
           _size--;
           return true;}}

   void clear() {
       size_t num = _size;
       for(size_t k=0; k<num; k++)
           pop_back();
   }

   // [Optional TODO] Feel free to change, but DO NOT change ::sort()
   void sort() const { if (!empty()) ::sort(_data, _data+_size); }

   // Nice to have, but not required in this homework...
   // void reserve(size_t n) { ... }
   // void resize(size_t n) { ... }

private:
    // [NOTE] DO NOT ADD or REMOVE any data member
    T*            _data;
    size_t        _size;       // number of valid elements
    size_t        _capacity;   // max number of elements
    mutable bool  _isSorted;   // (optionally) to indicate the array is sorted

    // [OPTIONAL TODO] Helper functions; called by public member functions
    void expand()
    {
        if(_capacity ==0)
            _capacity = 1;
        else
            _capacity*=2;
    }
    bool isCleared()
    {
        if(_data!=0 && _size==0)
            return true;
        return false;
    }
};

#endif // ARRAY_H

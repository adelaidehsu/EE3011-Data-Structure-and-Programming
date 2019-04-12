/****************************************************************************
  FileName     [ myHashSet.h ]
  PackageName  [ util ]
  Synopsis     [ Define HashSet ADT ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2014-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef MY_HASH_SET_H
#define MY_HASH_SET_H

#include <vector>
using namespace std;

//---------------------
// Define HashSet class
//---------------------
// To use HashSet ADT,
// the class "Data" should at least overload the "()" and "==" operators.
//
// "operator ()" is to generate the hash key (size_t)
// that will be % by _numBuckets to get the bucket number.
// ==> See "bucketNum()"
//
// "operator ==" is to check whether there has already been
// an equivalent "Data" object in the HashSet.
// Note that HashSet does not allow equivalent nodes to be inserted
//
template <class Data>
class HashSet
{
public:
   HashSet(size_t b = 0) : _numBuckets(0), _buckets(0) { if (b != 0) init(b); }
   ~HashSet() { reset(); }

   // TODO: implement the HashSet<Data>::iterator
   // o An iterator should be able to go through all the valid Data
   //   in the Hash
   // o Functions to be implemented:
   //   - constructor(s), destructor
   //   - operator '*': return the HashNode
   //   - ++/--iterator, iterator++/--
   //   - operators '=', '==', !="
   //
   class iterator
   {
       friend class HashSet<Data>;
       
   public:
       iterator(){};
       iterator(typename vector<Data>::iterator d, size_t r, size_t b, vector<Data>* v): _ptr(d), row(r), totalB(b), vbucket(v){};
       ~iterator(){};
       const Data& operator * () const { return *_ptr; }
       iterator& operator -- ()
       {
           if(_ptr==vbucket[row].begin())
           {
               if(row == 0)
                   return (*this);
               else
               {
                   while(vbucket[row-1].empty() && (row-1!=0))
                       --row;
                   _ptr = vbucket[row-1].end()-1;
                   --row;
               }
           }
           else
               --_ptr;
           return (*this);
       }
       
       iterator& operator ++ ()
       {
           ++_ptr;
           if(_ptr!=vbucket[row].end())
               return (*this);
           else
           {
               if(row == totalB-1)
                   return (*this);
               while(vbucket[row+1].empty())
               {
                   if(row+1 == totalB-1)
                       break;
                   ++row;
               }
               _ptr = vbucket[row+1].begin();
               ++row;
           }
           return (*this);
       }
       
       iterator operator ++ (int k) {
           iterator i = *this;
           ++(*this);
           return i;
       }
       iterator operator -- (int k) {
           iterator i = *this;
           --(*this);
           return i;
       }
       iterator& operator = (const iterator& i) {
           _ptr = i._ptr;
           vbucket = i.vbucket;
           row = i.row;
           totalB = i.totalB;
           return *this;
       }
       bool operator == (const iterator& i) const { return _ptr==i._ptr; }
       bool operator != (const iterator& i) const { return !(_ptr==i._ptr); }
       
   private:
       typename vector<Data>::iterator _ptr;
       vector<Data>* vbucket;
       size_t totalB;
       size_t row;

   };

   void init(size_t b) { _numBuckets = b; _buckets = new vector<Data>[b]; }
   void reset() {
      _numBuckets = 0;
      if (_buckets) { delete [] _buckets; _buckets = 0; }
   }
   void clear() {
      for (size_t i = 0; i < _numBuckets; ++i) _buckets[i].clear();
   }
   size_t numBuckets() const { return _numBuckets; }

   vector<Data>& operator [] (size_t i) { return _buckets[i]; }
   const vector<Data>& operator [](size_t i) const { return _buckets[i]; }

   // TODO: implement these functions
   //
   // Point to the first valid data
   iterator begin() const{
       int index;
       bool indexSet = false;
       for(int i=0; i<numBuckets(); ++i)
       {
           if(!_buckets[i].empty())
           {
               index = i;
               indexSet = true;
               break;
           }
       }
       if(!indexSet)
           return iterator(_buckets[numBuckets()-1].end(), numBuckets()-1, _numBuckets, _buckets);
       return iterator(_buckets[index].begin(), index, _numBuckets, _buckets);
   }
   // Pass the end
   iterator end() const {
       return iterator(_buckets[numBuckets()-1].end(), numBuckets()-1, _numBuckets, _buckets); }
   // return true if no valid data
   bool empty() const { return begin()==end();}
   // number of valid data
    size_t size() const {
        size_t s = 0;
        for(int i=0; i<numBuckets(); i++)
            s+=_buckets[i].size();
        return s;
    }

   // check if d is in the hash...
   // if yes, return true;
   // else return false;
   bool check(const Data& d) const {
       int index = bucketNum(d);
       int size = _buckets[index].size();
       for(int i = 0; i<size; ++i)
       {
           if(_buckets[index][i] == d)
               return true;
       }
       return false;
   }
    
    Data findGV(const Data& d) const{
        int index = bucketNum(d);
        int size = _buckets[index].size();
        for(int i = 0; i<size; ++i)
        {
            if(_buckets[index][i] == d)
                return _buckets[index][i];
        }
    }
    
    vector<vector<Data>> GetGrp() const{
        vector<vector<Data>> table;
        for(int i=0; i<numBuckets(); ++i)
        {
            if(!_buckets[i].empty())
                table.push_back(_buckets[i]);
        }
        return table;
    }
    
   // query if d is in the hash...
   // if yes, replace d with the data in the hash and return true;
   // else return false;
   bool query(Data& d) const {
       int index = bucketNum(d);
       int size = _buckets[index].size();
       for(int i=0; i<size; ++i)
       {
           if(_buckets[index][i] == d)
           {
               d = _buckets[index][i];
               return true;
           }
       }
       return false;
   }

   // update the entry in hash that is equal to d (i.e. == return true)
   // if found, update that entry with d and return true;
   // else insert d into hash as a new entry and return false;
   bool update(const Data& d) {
       size_t index = bucketNum(d);
       int size = _buckets[index].size();
       for(int i=0; i<size; ++i)
       {
           if(_buckets[index][i] == d)
           {
               _buckets[index][i] = d;
               return true;
           }
       }
       _buckets[index].push_back(d);
       return false;
   }

   // return true if inserted successfully (i.e. d is not in the hash)
   // return false is d is already in the hash ==> will not insert
   bool insert(const Data& d) {
       if(!empty())
       {
           if(check(d))
               return false;
       }
       size_t index = bucketNum(d);
       _buckets[index].push_back(d);
       return true;
   }

   // return true if removed successfully (i.e. d is in the hash)
   // return fasle otherwise (i.e. nothing is removed)
   bool remove(const Data& d) {
       size_t index = bucketNum(d);
       int size = _buckets[index].size();
       for(int i=0; i<size; ++i)
       {
           if(_buckets[index][i] == d)
           {
               _buckets[index].erase(_buckets[index].begin()+i);
               return true;
           }
       }
       return false;
   }

private:
   // Do not add any extra data member
   size_t            _numBuckets;
   vector<Data>*     _buckets;

   size_t bucketNum(const Data& d) const {
      return (d() % _numBuckets); }
};

#endif // MY_HASH_SET_H

/****************************************************************************
  FileName     [ memMgr.h ]
  PackageName  [ cmd ]
  Synopsis     [ Define Memory Manager ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2007-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef MEM_MGR_H
#define MEM_MGR_H

#include <cassert>
#include <iostream>
#include <iomanip>
#include <stdlib.h>

using namespace std;

// Turn this on for debugging
//#define MEM_DEBUG

//--------------------------------------------------------------------------
// Define MACROs
//--------------------------------------------------------------------------
#define MEM_MGR_INIT(T) \
MemMgr<T>* const T::_memMgr = new MemMgr<T>

#define USE_MEM_MGR(T)                                                      \
public:                                                                     \
   void* operator new(size_t t) { return (void*)(_memMgr->alloc(t)); }      \
   void* operator new[](size_t t) { return (void*)(_memMgr->allocArr(t)); } \
   void  operator delete(void* p) { _memMgr->free((T*)p); }                 \
   void  operator delete[](void* p) { _memMgr->freeArr((T*)p); }            \
   static void memReset(size_t b = 0) { _memMgr->reset(b); }                \
   static void memPrint() { _memMgr->print(); }                             \
private:                                                                    \
   static MemMgr<T>* const _memMgr

// You should use the following two MACROs whenever possible to 
// make your code 64/32-bit platform independent.
// DO NOT use 4 or 8 for sizeof(size_t) in your code
//
#define SIZE_T      sizeof(size_t)
#define SIZE_T_1    (sizeof(size_t) - 1)

// TODO: Define them by SIZE_T and/or SIZE_T_1 MACROs.
//
// To promote 't' to the nearest multiple of SIZE_T; 
// e.g. Let SIZE_T = 8;  toSizeT(7) = 8, toSizeT(12) = 16
#define toSizeT(t)  (t%SIZE_T==0 ?t:SIZE_T*(t/SIZE_T+1)) // TODO
//
// To demote 't' to the nearest multiple of SIZE_T
// e.g. Let SIZE_T = 8;  downtoSizeT(9) = 8, downtoSizeT(100) = 96
#define downtoSizeT(t)  (t%SIZE_T==0 ?SIZE_T:SIZE_T*(t/SIZE_T))  // TODO

// R_SIZE is the size of the recycle list
#define R_SIZE 256

//--------------------------------------------------------------------------
// Forward declarations
//--------------------------------------------------------------------------
template <class T> class MemMgr;


//--------------------------------------------------------------------------
// Class Definitions
//--------------------------------------------------------------------------
// T is the class that use this memory manager
//
// Make it a private class;
// Only friend to MemMgr;
//
template <class T>
class MemBlock
{
    friend class MemMgr<T>;

    // Constructor/Destructor
    MemBlock(MemBlock<T>* n, size_t b) : _nextBlock(n) {
       _begin = _ptr = new char[b]; _end = _begin + b; }
    ~MemBlock() { delete [] _begin; }

    // Member functions
    void reset() { _ptr = _begin; }
    // 1. Get (at least) 't' bytes memory from current block
    //    Promote 't' to a multiple of SIZE_T
    // 2. Update "_ptr" accordingly
    // 3. The return memory address is stored in "ret"
    // 4. Return false if not enough memory
    bool getMem(size_t t, T*& ret)
    {
        // TODO
        size_t needM = toSizeT(t);
        if(getRemainSize() < needM)
            return false;
        else
        {
            ret = (T*)_ptr;
            _ptr=_ptr+needM;
            return true;
        }
    }
    size_t getRemainSize() const { return size_t(_end - _ptr); }
      
    MemBlock<T>* getNextBlock() const { return _nextBlock; }

    // Data members
    char*             _begin;
    char*             _ptr;
    char*             _end;
    MemBlock<T>*      _nextBlock;
};

// Make it a private class;
// Only friend to MemMgr;
//
template <class T>
class MemRecycleList
{
    friend class MemMgr<T>;

    // Constructor/Destructor
    MemRecycleList(size_t a = 0) : _arrSize(a), _first(0), _nextList(0) {}
    ~MemRecycleList() { reset(); }

    // Member functions
    // ----------------
    size_t getArrSize() const { return _arrSize; }
    MemRecycleList<T>* getNextList() const { return _nextList; }
    void setNextList(MemRecycleList<T>* l) { _nextList = l; }
    // pop out the first element in the recycle list
    T* popFront()
    {
        // TODO
        //cout<<"in popFront(), initial _first is: "<<_first<<endl;
        T* p = _first;
        T* next = getNext(_first);
        _first = next;
        //cout<<"in popFront(), after pop _first is now: "<<_first<<endl;
        return p;
    }
    // push the element 'p' to the beginning of the recycle list
    void  pushFront(T* p)
    {
        //cout<<"in pushFront(), initial  _first is"<<_first<<endl;
        T* tmp = p;
        *(size_t*)p = (size_t)_first;
        _first = tmp;
        //cout<<"in pushFront(), after pushed _first is now "<<_first<<endl;
        //cout<<"the first eight byte of _first is "<<getNext(_first)<<endl;
    }
    // Release the memory occupied by the recycle list(s)
    // DO NOT release the memory occupied by MemMgr/MemBlock
    void reset()
    {
        // TODO
        const size_t motherlistL=numElm();
        for(size_t i=0; i<motherlistL; i++)
        {
            popFront();
        }
        while(_nextList!=0)
        {
            const size_t nextlistL=_nextList->numElm();
            for(size_t i=0; i<nextlistL; i++)
            {
                _nextList->popFront();
            }
            _nextList = _nextList->getNextList();
        }
    }

    // Helper functions
    // ----------------
    // Iterate to the next element after 'p' in the recycle list
    T* getNext(T* p) const
    {
        // TODO
        if((size_t*)p!=0)
            return (T*)*(size_t**)p;
        else
            return 0;
    }
    //
    // count the number of elements in the recycle list
    size_t numElm() const
    {
        size_t count = 0;
        T* p = _first;
        while (p) {
           p = getNext(p);
           ++count;
        }
        return count;
    }

    // Data members
    size_t              _arrSize;   // the array size of the recycled data
    T*                  _first;     // the first recycled data
    MemRecycleList<T>*  _nextList;  // next MemRecycleList
                                   //      with _arrSize + x*R_SIZE
};

template <class T>
class MemMgr
{
   #define S sizeof(T)

public:
   MemMgr(size_t b = 65536) : _blockSize(b) {
      assert(b % SIZE_T == 0);
      _activeBlock = new MemBlock<T>(0, _blockSize);
      for (int i = 0; i < R_SIZE; ++i)
         _recycleList[i]._arrSize = i;
   }
   ~MemMgr() { reset(); delete _activeBlock; }

   // 1. Remove the memory of all but the firstly allocated MemBlocks
   //    That is, the last MemBlock searchd from _activeBlock.
   //    reset its _ptr = _begin (by calling MemBlock::reset())
   // 2. reset _recycleList[]
   // 3. 'b' is the new _blockSize; "b = 0" means _blockSize does not change
   //    if (b != _blockSize) reallocate the memory for the first MemBlock
   // 4. Update the _activeBlock pointer
    void reset(size_t b = 0)
    {
        assert(b % SIZE_T == 0);
        #ifdef MEM_DEBUG
        cout << "Resetting memMgr...(" << b << ")" << endl;
        #endif // MEM_DEBUG
        // TODO
        for(int i=0; i< R_SIZE; i++)
        {
            _recycleList[i].reset();
        }
        while(_activeBlock->getNextBlock()!=0)
        {
            MemBlock<T>* a = _activeBlock->getNextBlock();
            delete _activeBlock;
            _activeBlock = a;
        }
        _activeBlock->reset();
        if(b!=0)
        {
            _blockSize = b;
            delete _activeBlock;
            _activeBlock = new MemBlock<T>(0,_blockSize);
        }
    }
   // Called by new
   T* alloc(size_t t) {
      assert(t == S);
      #ifdef MEM_DEBUG
      cout << "Calling alloc...(" << t << ")" << endl;
      #endif // MEM_DEBUG
      return getMem(t);
   }
   // Called by new[]
   T* allocArr(size_t t) {
      #ifdef MEM_DEBUG
      cout << "Calling allocArr...(" << t << ")" << endl;
      #endif // MEM_DEBUG
      // Note: no need to record the size of the array == > system will do
      return getMem(t);
   }
   // Called by delete
   void  free(T* p) {
      #ifdef MEM_DEBUG
      cout << "Calling free...(" << p << ")" << endl;
      #endif // MEM_DEBUG
      getMemRecycleList(0)->pushFront(p);
   }
    // Called by delete[]
    void  freeArr(T* p)
    {
        #ifdef MEM_DEBUG
        cout << "Calling freeArr...(" << p << ")" << endl;
        #endif // MEM_DEBUG
        // TODO
        // Get the array size 'n' stored by system,
        // which is also the _recycleList index
        size_t n = *(size_t*)p;
        #ifdef MEM_DEBUG
        cout << ">> Array size = " << n << endl;
        cout << "Recycling " << p << " to _recycleList[" << n << "]" << endl;
        #endif // MEM_DEBUG
        // add to recycle list...
        getMemRecycleList(n)->pushFront(p);
    }
   void print() const {
      cout << "=========================================" << endl
           << "=              Memory Manager           =" << endl
           << "=========================================" << endl
           << "* Block size            : " << _blockSize << " Bytes" << endl
           << "* Number of blocks      : " << getNumBlocks() << endl
           << "* Free mem in last block: " << _activeBlock->getRemainSize()
           << endl
           << "* Recycle list          : " << endl;
      int i = 0, count = 0;
      while (i < R_SIZE) {
         const MemRecycleList<T>* ll = &(_recycleList[i]);
         while (ll != 0) {
            size_t s = ll->numElm();
            if (s) {
               cout << "[" << setw(3) << right << ll->_arrSize << "] = "
                    << setw(10) << left << s;
               if (++count % 4 == 0) cout << endl;
            }
            ll = ll->_nextList;
         }
         ++i;
      }
      cout << endl;
   }

private:
   size_t                     _blockSize;
   MemBlock<T>*               _activeBlock;
   MemRecycleList<T>          _recycleList[R_SIZE];

   // Private member functions
   //
   // t: #Bytes; MUST be a multiple of SIZE_T
   // return the size of the array with respect to memory size t
   // [Note] t must >= S
   // [NOTE] Use this function in (at least) getMem() to get the size of array
   //        and call getMemRecycleList() later to get the index for
   //        the _recycleList[]
    size_t getArraySize(size_t t) const
    {
        assert(t % SIZE_T == 0);
        assert(t >= S);
        // TODO
        size_t n = (t-sizeof(size_t*))/S;
        return n;
    }
   // Go through _recycleList[m], its _nextList, and _nexList->_nextList, etc,
   //    to find a recycle list whose "_arrSize" == "n"
   // If not found, create a new MemRecycleList with _arrSize = n
   //    and add to the last MemRecycleList
   // So, should never return NULL
   // [Note]: This function will be called by MemMgr->getMem() to get the
   //         recycle list. Therefore, the recycle list is first created
   //         by the MTNew command, not MTDelete.
    MemRecycleList<T>* getMemRecycleList(size_t n)
    {
        size_t m = n % R_SIZE;
        MemRecycleList<T>* ptr = &_recycleList[m];
        MemRecycleList<T>* next = ptr->getNextList();
        // TODO
        if(ptr->getArrSize() == n)
        {
            return ptr;
        }
        else
        {
            while(next!=0)
            {
                if(next->getArrSize() == n)
                {
                    return next;
                }
                ptr = next;
                next = next->getNextList();
            }
        }
        MemRecycleList<T>* l = new MemRecycleList<T>;
        ptr->setNextList(l);
        l->_arrSize = n;
        return l;
    }
   // t is the #Bytes requested from new or new[]
   // Note: Make sure the returned memory is a multiple of SIZE_T
    T* getMem(size_t t)
    {
        T* ret = 0;
        #ifdef MEM_DEBUG
        cout << "Calling MemMgr::getMem...(" << t << ")" << endl;
        #endif // MEM_DEBUG
        // TODO ---
        // 1. Make sure to promote t to a multiple of SIZE_T
        // 2. Check if the requested memory is greater than the block size.
        //    If so, throw a "bad_alloc()" exception.
        //    cerr << "Requested memory (" << t << ") is greater than block size"
        //         << "(" << _blockSize << "). " << "Exception raised...\n";
        // 3. Check the _recycleList first...
        //    #ifdef MEM_DEBUG
        //    cout << "Recycled from _recycleList[" << n << "]..." << ret << endl;
        //    #endif // MEM_DEBUG
        //    => 'n' is the size of array
        //    => "ret" is the return address

        // If no match from recycle list...
        // 4. Get the memory from _activeBlock
        // 5. If not enough, recycle the remained memory and print out ---
        //    Note: recycle to the as biggest array index as possible
        //    Note: rn is the array size
        //    #ifdef MEM_DEBUG
        //    cout << "Recycling " << ret << " to _recycleList[" << rn << "]\n";
        //    #endif // MEM_DEBUG
        //    ==> allocate a new memory block, and print out ---
        //    #ifdef MEM_DEBUG
        //    cout << "New MemBlock... " << _activeBlock << endl;
        //    #endif // MEM_DEBUG
        // 6. At the end, print out the acquired memory address
        //    #ifdef MEM_DEBUG
        //    cout << "Memory acquired... " << ret << endl;
        //    #endif // MEM_DEBUG
        size_t needM = toSizeT(t);
        if(needM > _blockSize)
        {
            cerr << "Requested memory (" << needM << ") is greater than block size"<< "(" << _blockSize << "). " << "Exception raised...\n";
            throw bad_alloc();
        }
        size_t n = getArraySize(needM);
        MemRecycleList<T>* List = getMemRecycleList(n);
        if(List->_first!=0)
        {
            ret = List->popFront();
            #ifdef MEM_DEBUG
            cout << "Recycled from _recycleList[" << n << "]..." << ret << endl;
            #endif // MEM_DEBUG
            return ret;
        }
        bool getM =_activeBlock->getMem(needM, ret);
        if(!getM)
            {
                size_t r = _activeBlock->getRemainSize();
                if(toSizeT(r)>=S)
                {
                    size_t rn = getArraySize(toSizeT(r));
                    ret = (T*)_activeBlock->_ptr;
                    #ifdef MEM_DEBUG
                    cout << "Recycling " << ret << " to _recycleList[" << rn << "]\n";
                    #endif // MEM_DEBUG
                    MemRecycleList<T>* tmpl = getMemRecycleList(rn);
                    tmpl->pushFront(ret);
                }
                MemBlock<T>* nb = new MemBlock<T>(0,_blockSize);
                MemBlock<T>* tmpb = _activeBlock;
                _activeBlock=nb;
                nb->_nextBlock = tmpb;
                #ifdef MEM_DEBUG
                cout << "New MemBlock... " << _activeBlock << endl;
                #endif // MEM_DEBUG
                if(_activeBlock->getMem(needM, ret))
                {
                    #ifdef MEM_DEBUG
                    cout << "Memory acquired... " << ret << endl;
                    #endif // MEM_DEBUG
                    return ret;
                }
            }
        else if(getM)
        {
            #ifdef MEM_DEBUG
            cout << "Memory acquired... " << ret << endl;
            #endif // MEM_DEBUG
            return ret;
        }
    }
    
    // Get the currently allocated number of MemBlock's
     size_t getNumBlocks() const
     {
         // TODO
         size_t count = 0;
         MemBlock<T>* p = _activeBlock;
         while (p) {
             p = p->getNextBlock();
             ++count;
         }
         return count;
     }
};

#endif // MEM_MGR_H

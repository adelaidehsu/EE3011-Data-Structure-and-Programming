/****************************************************************************
  FileName     [ dlist.h ]
  PackageName  [ util ]
  Synopsis     [ Define doubly linked list package ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2005-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef DLIST_H
#define DLIST_H

#include <cassert>

template <class T> class DList;

// DListNode is supposed to be a private class. User don't need to see it.
// Only DList and DList::iterator can access it.
//
// DO NOT add any public data member or function to this class!!
//
template <class T>
class DListNode
{
   friend class DList<T>;
   friend class DList<T>::iterator;

   DListNode(const T& d, DListNode<T>* p = 0, DListNode<T>* n = 0):
      _data(d), _prev(p), _next(n) {}

   // [NOTE] DO NOT ADD or REMOVE any data member
   T              _data;
   DListNode<T>*  _prev;
   DListNode<T>*  _next;
};


template <class T>
class DList
{
public:
    // TODO: decide the initial value for _isSorted
    DList() {
       _head = new DListNode<T>(T());
       _head->_prev = _head->_next = _head; // _head is a dummy node
    }
    ~DList() { clear(); delete _head; }

    // DO NOT add any more data member or function for class iterator
    class iterator
    {
        friend class DList;

    public:
        iterator(DListNode<T>* n= 0): _node(n) {}
        iterator(const iterator& i) : _node(i._node) {}
        ~iterator() {} // Should NOT delete _node

        // TODO: implement these overloaded operators
        const T& operator * () const { return _node->_data; }
        T& operator * () { return _node->_data; }
        iterator& operator ++ (){
            _node = _node -> _next;
            return *(this);
        }
        iterator operator ++ (int) {
            iterator temp = *this;
            ++(*this);
            return temp; }
        iterator& operator -- () {
            _node = _node -> _prev;
            return *(this); }
        iterator operator -- (int){
            iterator temp = *this;
            --(*this);
            return temp; }

        iterator& operator = (const iterator& i) {
            _node = i._node;
            return *(this); }

        bool operator != (const iterator& i) const {
            return _node != i._node; }
        bool operator == (const iterator& i) const {
            return _node == i._node; }

    private:
        DListNode<T>* _node;
    };

   // TODO: implement these functions
    iterator begin() const {
        return iterator(_head->_next); }
    iterator end() const {
        return iterator(_head);}
    bool empty() const {
        if(_head->_prev == _head->_next && _head->_next == _head)
            return true;
        else
            return false;}
    size_t size() const {
        size_t count =0;
        iterator li = begin();
        for (; li != end(); ++li)
        {
            count++;
        }
        return count;}

    void push_back(const T& x) {
        DListNode<T>* newNode = new DListNode<T>(x);
        if(_head->_prev ==_head)
        {
            _head->_prev = newNode;
            newNode->_next = _head;
            newNode->_prev=_head;
            _head->_next=newNode;
            
        }
        else
        {
            DListNode<T>* temp;
            temp = _head->_prev;
            temp->_next = newNode;
            newNode->_prev = temp;
            newNode->_next=_head;
            _head->_prev=newNode;
        }}
    void pop_front() {
        if(_head->_next==_head)
            return;
        else
        {
            DListNode<T>* temp = _head->_next;
            DListNode<T>* newfirst = temp->_next;
            _head->_next = newfirst;
            newfirst->_prev=_head;
            delete temp;
        }}
    void pop_back() {
        if(_head->_prev==_head)
            return;
        else
        {
            DListNode<T>* temp = _head->_prev;
            DListNode<T>* newlast = temp->_prev;
            _head->_prev = newlast;
            newlast->_next=_head;
            delete temp;
        }}

    // return false if nothing to erase
    bool erase(iterator pos) {
        if(pos._node == _head)
            return false;
        else
        {
            DListNode<T>* nbefore = pos._node->_prev;
            DListNode<T>* nafter = pos._node->_next;
            nbefore->_next = nafter;
            nafter->_prev = nbefore;
            delete pos._node;
            return true;
        }}
    bool erase(const T& x) {
        bool found = false;
        DListNode<T>* nerase;
        iterator li = begin();
        for (; li != end(); ++li)
        {
            if(*li == x)
            {
                found = true;
                nerase = li._node;
                break;
            }
        }
        if(found)
        {
            DListNode<T>* nbefore = nerase->_prev;
            DListNode<T>* nafter = nerase->_next;
            nbefore->_next = nafter;
            nafter->_prev = nbefore;
            delete nerase;
            return true;
        }
        else
            return false;}

    void clear() {
        size_t num = size();
        for (size_t i=0; i<num; i++)
        {
            pop_back();
        }
        
    }  // delete all nodes except for the dummy node

    void sort() const {
        iterator li = begin();
        for (; li != iterator(_head->_prev); ++li)
        {
            iterator lj = ++li;
            --li;
            for (; lj != end(); ++lj)
            {
                if(*lj < *li)
                {
                    T temp = *li;
                    *li = *lj;
                    *lj = temp;
                }
            }
        }}

private:
    // [NOTE] DO NOT ADD or REMOVE any data member
    DListNode<T>*  _head;     // = dummy node if list is empty
    mutable bool   _isSorted; // (optionally) to indicate the array is sorted

    // [OPTIONAL TODO] helper functions; called by public member functions
};

#endif // DLIST_H

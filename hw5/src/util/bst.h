/****************************************************************************
  FileName     [ bst.h ]
  PackageName  [ util ]
  Synopsis     [ Define binary search tree package ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2005-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef BST_H
#define BST_H
#include <vector>
#include <cassert>

using namespace std;

template <class T> class BSTree;

// BSTreeNode is supposed to be a private class. User don't need to see it.
// Only BSTree and BSTree::iterator can access it.
//
// DO NOT add any public data member or function to this class!!
//
template <class T>
class BSTreeNode
{
    // TODO: design your own class!!
    friend class BSTree<T>;
    friend class BSTree<T>::iterator;
    
    BSTreeNode(const T& d, BSTreeNode<T>* p = 0, BSTreeNode<T>* l = 0, BSTreeNode<T>* r = 0):
    _data(d), _parent(p), _left(l), _right(r) {}
    
    T _data;
    bool visited = false;
    BSTreeNode<T>* _parent;
    BSTreeNode<T>* _left;
    BSTreeNode<T>* _right;
};


template <class T>
class BSTree
{
    friend class BSTreeNode<T>;
public:
    BSTree() {
        _root = 0;
        _size = 0;
    }
    ~BSTree() { clear();}
    
   // TODO: design your own class!!
    class iterator
    {
        friend class BSTree<T>;
    public:
        iterator(BSTreeNode<T>* n= 0): _node(n) {nodelist.push_back(_node);}
        iterator(const iterator& i) : _node(i._node) {nodelist.push_back(_node);}
        ~iterator() {} // Should NOT delete _node
        
        BSTreeNode<T>* imin(BSTreeNode<T>* node)
        {
            while(node->_left!=0)
                node = node->_left;
            return node;
        }
        
        BSTreeNode<T>* imax(BSTreeNode<T>* node)
        {
            while(node->_right!=0)
                node = node->_right;
            return node;
        }

        const T& operator * () const { return _node->_data; }
        T& operator * () { return _node->_data; }
        
        iterator& operator ++ (){
     
            if(!(_node->_right==0))
            {
                _node = imin(_node->_right);
                nodelist.push_back(_node);
                _node->visited=true;
                return *this;
            }
            
            BSTreeNode<T>* temprt = _node->_parent;
            BSTreeNode<T>* moveto;
            while(temprt!=0)
            {
                if(_node->_data > temprt->_data)
                {
                    temprt = temprt->_parent;
                    continue;
                }
                if(_node->_data <= temprt->_data)
                {
                    if(!(temprt->visited))
                    {
                        moveto = temprt;
                        _node = moveto;
                        nodelist.push_back(_node);
                        _node->visited=true;
                        return *this;
                    }
                    temprt = temprt->_parent;
                }
            }
            if(temprt==0)
            {
                for(int i=0; i<nodelist.size(); i++)
                {
                    nodelist[i]->visited = false;
                }
                _node=0;
                return *this;
            }
        }
        
        iterator operator ++ (int) {
            iterator temp = *this;
            ++(*this);
            return temp; }
        
        iterator& operator -- ()
        {
            if(_node == 0)
            {
                nodelist.clear();
                _node = _tail;
                nodelist.push_back(_node);
                _node->visited=true;
                return *this;
            }
            
            if(!(_node->_left==0))
            {
                _node = imax(_node->_left);
                nodelist.push_back(_node);
                _node->visited=true;
                return *this;
            }
            
            BSTreeNode<T>* temprt = _node->_parent;
            BSTreeNode<T>* moveto;
            while(temprt!=0)
            {
                if(_node->_data < temprt->_data)
                {
                    temprt = temprt->_parent;
                    continue;
                }
                if(_node->_data >= temprt->_data)
                {
                    if(!(temprt->visited))
                    {
                        moveto = temprt;
                        _node = moveto;
                        nodelist.push_back(_node);
                        _node->visited=true;
                        return *this;
                    }
                    temprt = temprt->_parent;
                }
            }
            if(temprt==0)
            {
                for(int i=0; i<nodelist.size(); i++)
                {
                    nodelist[i]->visited = false;
                }
                _node = imin(_node);
                return *this;
            }
        }
        
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
        BSTreeNode<T>* _node;
        vector<BSTreeNode<T>*> nodelist;
        BSTreeNode<T>* _tail = 0;
    };
    
    iterator begin() const
    {
        return iterator(min(_root));}
    
    iterator end() const
    {
        iterator i;
        if(max(_root)!=0)
            i = iterator(max(_root)->_right);
        else
            i = iterator(max(_root));
        i._tail = max(_root);
        return i;
    }
    
    bool empty() const
    {
        return _root == 0;}
    
    size_t size() const
    {
        return _size;}
    
    void insert(const T& x)
    {
        if(_root==0)
        {
            _root = new BSTreeNode<T> (x);
        }
        else{
            BSTreeNode<T>* temprt = _root;
            BSTreeNode<T>* mom = 0;
            while(temprt!=0)
            {
                mom = temprt;
                if(x <= temprt->_data)
                    temprt = temprt->_left;
                else if(x > temprt->_data)
                    temprt = temprt->_right;
            }
            BSTreeNode<T>* child = new BSTreeNode<T> (x);
            child -> _parent = mom;
            if(x <= mom->_data)
                mom->_left = child;
            else
                mom->_right = child;
            
        }
        _size++;
    }
    
    BSTreeNode<T>* max(BSTreeNode<T>* node) const
    {
        if(node!=0)
        {
            if(node->_right == 0)
                return node;
            else
                max(node->_right);
        }
        else
            return 0;
    }
    
    BSTreeNode<T>* min(BSTreeNode<T>* node) const
    {
        if(node!=0)
        {
            if(node->_left == 0)
                return node;
            else
                min(node->_left);
        }
        else
            return 0;}
    
    
    BSTreeNode<T>* succ(BSTreeNode<T>* node)
    {
        if(node->_right == 0)
            return 0;
        else
            return min(node->_right);}
    
    void pop_front()
    {
        if(empty())
            return;
        else
        {
            BSTreeNode<T>* element = min(_root);
            BSTreeNode<T>* parent = element->_parent;
            if(element->_left==0 && element->_right==0)
            {
                if(parent!=0)
                {
                    delete element;
                    parent->_left = 0;
                }
                else
                {
                    delete _root;
                    _root=0;
                }
            }
            else if((element->_left == 0 && element->_right!=0))
            {
                if(parent!=0)
                {
                    parent->_left = element->_right;
                    element->_right->_parent = parent;
                    delete element;
                }
                else
                {
                    BSTreeNode<T>* temp = element->_right;
                    delete _root;
                    _root = temp;
                    _root->_parent=0;
                }
            }
            _size--;
        }
    }
    
    void pop_back()
    {
        if(empty())
            return;
        else
        {
            BSTreeNode<T>* element = max(_root);
            BSTreeNode<T>* parent = element->_parent;
            if(element->_left==0 && element->_right==0)
            {
                if(parent!=0)
                {
                    delete element;
                    parent->_right = 0;
                }
                else
                {
                    delete _root;
                    _root =0 ;
                }
            }
            else if((element->_right == 0 && element->_left!=0))
            {
                if(parent!=0)
                {
                    parent->_right = element->_left;
                    element->_left->_parent = parent;
                    delete element;
                }
                else
                {
                    BSTreeNode<T>* temp = element->_left;
                    delete _root;
                    _root = temp;
                    _root->_parent = 0;
                }
            }
            _size--;
        }
    }
    
    void del(BSTreeNode<T>* element)
    {
        /*
        cout<<"now deleting..."<<element->_data<<endl;
        BSTreeNode<T>* parent = element->_parent;
        cout<<"parent is.."<<parent->_data<<endl;
        if(element->_left==0 && element->_right==0)
        {
            cout<<"0 kid case: deleting.. "<< element->_data<<endl;
            if(parent!=0)
            {
                if(element->_data < parent->_data)
                    parent->_left=0;
                else
                    parent->_right=0;
                element->_parent=0;
                delete element;
                if(parent->_left ==0)
                {
                    cout<<"now the element is 0."<<endl;
                }
            }
            else
            {
                delete _root;
                _root = 0;
            }
        }
        else if(element->_left!=0 && element->_right==0)
        {
            cout<<"1 left kid case: deleting.. "<< element->_data<<endl;
            if(parent!=0)
            {
                if(element->_data < parent->_data)
                    parent->_left=element->_left;
                else
                    parent->_right=element->_left;
                element->_left->_parent = parent;
                delete element;
            }
            else
            {
                BSTreeNode<T>* temp = element->_left;
                temp->_parent = 0;
                delete _root;
                _root = temp;
            }
        }
        else if(element->_left==0 && element->_right!=0)
        {
            cout<<"1 right kid case: deleting.. "<< element->_data<<endl;
            if(parent!=0)
            {
                if(element->_data < parent->_data)
                    parent->_left=element->_right;
                else
                    parent->_right=element->_right;
                element->_right->_parent = parent;
                delete element;
            }
            else
            {
                BSTreeNode<T>* temp = element->_right;
                temp->_parent = 0;
                delete _root;
                _root = temp;
            }
        }
        else
        {
            cout<<"2 kid case: deleting.. "<< element->_data<<endl;
            BSTreeNode<T>* nsucc = succ(element);
            BSTreeNode<T>* succparent = nsucc->_parent;
            if(parent!=0)
            {
                if(nsucc->_data < succparent->_data)
                    succparent->_left = 0;
                else
                    succparent->_right=0;
                if(element->_data < parent->_data)
                    parent->_left = nsucc;
                else
                    parent->_right = nsucc;
                nsucc->_parent = parent;
                nsucc->_left = element->_left;
                element->_left->_parent = nsucc;
                
                if(succparent!=element)
                {
                    nsucc->_right = element->_right;
                    element->_right->_parent = nsucc;
                }
                else
                    nsucc->_right = 0;
                
                delete element;
            }
            else
            {
                if(nsucc->_data < succparent->_data)
                    succparent->_left = 0;
                else
                    succparent->_right=0;
                nsucc->_parent = 0;
                nsucc->_left = element->_left;
                element->_left->_parent = nsucc;
                
                if(succparent!=element)
                {
                    nsucc->_right = element->_right;
                    element->_right->_parent = nsucc;
                }
                else
                    nsucc->_right = 0;
                delete _root;
                _root = nsucc;
            }
        }
         */
        
        BSTreeNode<T> *main = 0;
        BSTreeNode<T> *child = 0;
        
        if (element->_left == 0 || element->_right == 0){
            main = element;}
        else{
            main = succ(element);}
        
        if (main->_left != 0){
            child = main->_left;}
        else{
            child = main->_right;}
        
        if (child != 0){
            child->_parent = main->_parent;}
   
        if (main->_parent == 0){
            this->_root = child;}
        else if (main == main->_parent->_left){
            main->_parent->_left = child;}
        else{
            main->_parent->_right = child;}
        
        if (main != element) {
            element->_data = main->_data;}
        
        delete main;
        main = 0;
        
        _size--;
    }
    //return false if nothing to erase
    bool erase(iterator pos)
    {
        BSTreeNode<T>* element = pos._node;
        for(int i=0; i<pos.nodelist.size();i++)
        {
            pos.nodelist[i]->visited = false;
        }
        if(element==0)
            return false;
        else
        {
            del(element);
            return true;
        }
    }
    
    bool erase(const T& x)
    {
        iterator li = begin();
        BSTreeNode<T>* element = 0;
        
        if(end()!=begin())
        {
            for (; li != end(); ++li)
            {
                if(li._node->_data == x)
                {
                    element = li._node;
                    break;
                }
            }
            for(int i=0; i<li.nodelist.size();i++)
            {
                li.nodelist[i]->visited = false;
            }
        }
        if(element == 0)
            return false;
        else
        {
            del(element);
            return true;
        }
    }
    
    void clear()
    {
        size_t num = _size;
        for(size_t i=0; i<num; i++)
            pop_front();
        _size = 0;
    }
    
    void print() const
    {
        return;}
    
    void sort()
    {
        return;}
    
private:
    // [NOTE] DO NOT ADD or REMOVE any data member
    BSTreeNode<T>*  _root;
    size_t _size;
};

#endif // BST_H

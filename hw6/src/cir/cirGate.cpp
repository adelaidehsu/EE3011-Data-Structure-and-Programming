/****************************************************************************
  FileName     [ cirGate.cpp ]
  PackageName  [ cir ]
  Synopsis     [ Define class CirAigGate member functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#include <iostream>
#include <iomanip>
#include <sstream>
#include <stdarg.h>
#include <cassert>
#include <string>
#include <vector>
#include <cmath>
#include "cirGate.h"
#include "cirMgr.h"
#include "util.h"

using namespace std;

extern CirMgr *cirMgr;

// TODO: Implement memeber functions for class(es) in cirGate.h

/**************************************/
/*   class CirGate member functions   */
/**************************************/
unsigned CirGate::_globalRef = 0;

void
CirGate::reportGate() const
{
    int numt = getTypeStr().length();
    int numid = to_string(_Gid).length();
    int numsym = _sym.length();
    int numl = to_string(_lineNum).length();
    if(_sym!="")
    {
        for(int i=0; i<50; i++)
            cout<<"=";
        cout<<endl;
        cout<<"= "<<getTypeStr()<<"("<<_Gid<<")\""<<_sym<<"\", line "<<_lineNum<<setw(50-numt-numid-numsym-numl-13)<<"="<<endl;
        for(int i=0; i<50; i++)
            cout<<"=";
        cout<<endl;
    }
    else
    {
        for(int i=0; i<50; i++)
            cout<<"=";
        cout<<endl;
        cout<<"= "<<getTypeStr()<<"("<<_Gid<<"), line "<<_lineNum<<setw(50-numt-numid-numl-11)<<"="<<endl;
        for(int i=0; i<50; i++)
            cout<<"=";
        cout<<endl;
    }
}

void
CirGate::reportFanin(int level) const
{
    assert (level >= 0);
    vector<CirGate* > _Itrace;
    vector<CirGate* > _Nsplit;
    bool finish = false;
    //root
    cout<<this->getTypeStr()<<" "<<this->_Gid<<endl;
    if(this->_faninlist.empty() || level==0)
    {
        finish=true;
        return;
    }
    
    for(int i=0; i<this->_faninlist.size(); i++)
    {
        setGlobalRef();
        int index = 1;
        CirGate* next = this->_faninlist[i];
        string invert = "";
        if(abs(this->_tmpfanin1)==INT_MAX && next->_Gid == 0)
        {
            if(this->_tmpfanin1<0)
                invert = "!";
        }
        else if(abs(this->_tmpfanin2)==INT_MAX && next->_Gid == 0)
        {
            if(this->_tmpfanin2<0)
                invert = "!";
        }
        else if(abs(this->_tmpfanin1)!=INT_MAX && (next->_Gid == abs(this->_tmpfanin1)))
        {
            if(this->_tmpfanin1<0)
                invert = "!";
        }
        else if(abs(this->_tmpfanin2)!=INT_MAX && (next->_Gid == abs(this->_tmpfanin2)))
        {
            if(this->_tmpfanin2<0)
                invert = "!";
        }
        
        for(int i=0; i<2*index; i++)
            cout<<" ";
        cout<<invert<<next->getTypeStr()<<" "<<next->_Gid<<endl;
        next->setToGlobalRef();
        _Itrace.push_back(next);
        
        Helpfani(next, index, level, _Nsplit, _Itrace);
    }
    
    for(int k=0; k<_Itrace.size(); k++)
        _Itrace[k]->_ref = 0;
}
bool
CirGate::ManyTRoutes(CirGate*& g, vector<CirGate*>& t)const
{
    int cnt = g->_fanoutlist.size();
    for(int i=0; i<g->_fanoutlist.size(); i++)
    {
        bool find = false;
        for(int j=t.size()-1; j>=0; j--)
        {
            if(g->_fanoutlist[i] == t[j])
            {
                find = true;
                break;
            }
        }
        if(!find)
            cnt--;
    }
    if(cnt>1)
        return true;
    else
        return false;
}

void
CirGate::Helpfani(CirGate* g, int index, int level, vector<CirGate*>& s, vector<CirGate*>& t)const
{
    bool handle = false;
    while(!g->_faninlist.empty())
    {
        if(g->_faninlist.size()>1)
            s.push_back(g);
        CirGate* tmprt = g;
        string invert = "";
        g = g->_faninlist[0];
        index++;
        if(index>level)
            break;
        if(CheckInvert(tmprt,g))
            invert = "!";
        for(int i=0; i<2*index; i++)
            cout<<" ";
        if(g->_ref>0)
        {
            if(!g->_faninlist.empty() && (index<level))
                cout<<invert<<g->getTypeStr()<<" "<<g->_Gid<<" (*)"<<endl;
            else
                cout<<invert<<g->getTypeStr()<<" "<<g->_Gid<<endl;
            g->setToGlobalRef();
            t.push_back(g);
            handle = true;
        }
        else
        {
            cout<<invert<<g->getTypeStr()<<" "<<g->_Gid<<endl;
            g->setToGlobalRef();
            t.push_back(g);
            if(index == level)
                handle = true;
        }
        
        if(g->_faninlist.empty() || handle)
        {
            handle = false;
            bool resume = false;
            bool firstin = true;
            CirGate* root;
            while(!s.empty())
            {
                if(resume)
                {
                    int size;
                    for(int k=t.size()-1; k>=0; k--)
                    {
                        if(t[k]==root)
                        {
                            size = k;
                            break;
                        }
                    }
                    int i=0;
                    while(!(t[size-i]==s.back()))
                    {
                        if(IsChild(t[size-i-1],t[size-i]))
                            index--;
                        i++;
                    }
                    resume = false;
                }
                else
                {
                    int i=1;
                    while(!(t[t.size()-i]==s.back()))
                    {
                        if(IsChild(t[t.size()-i-1],t[t.size()-i]))
                            index--;
                        i++;
                    }
                }
                
                root = s.back();
                int tmp = index;
                int cnt = 0;
                bool samechild = false;
                bool beenboth = false;
                for(int k=0; k<root->_faninlist.size(); k++)
                {
                    string invert = "";
                    if(!samechild)
                    {
                        if(CheckInvert(root, root->_faninlist[k]))
                            invert = "!";
                    }

                    if(root->_faninlist[k]->_ref==0)
                    {
                        index = tmp+1;
                        if(index>level)
                            break;
                        for(int i=0; i<2*index; i++)
                            cout<<" ";
                        cout<<invert<<root->_faninlist[k]->getTypeStr()<<" "<<root->_faninlist[k]->_Gid<<endl;
                        root->_faninlist[k]->setToGlobalRef();
                        t.push_back(root->_faninlist[k]);
                        Helpfani(root->_faninlist[k], index, level, s, t);
                        cnt++;
                        
                    }
                    else if((root->_faninlist[k]->_ref>0 && (!root->_faninlist[k]->isGlobalRef())) || samechild|| (beenboth&&firstin) || ((root->_faninlist[k]->_ref>0)&&(ManyTRoutes(root->_faninlist[k], t))))
                    {
                        index = tmp+1;
                        if(index>level)
                            break;
                        for(int i=0; i<2*index; i++)
                            cout<<" ";
                        if(samechild)
                        {
                            if(root->_tmpfanin2<0)
                                invert = "!";
                        }
                        if(!root->_faninlist[k]->_faninlist.empty() && (index<level))
                            cout<<invert<<root->_faninlist[k]->getTypeStr()<<" "<<root->_faninlist[k]->_Gid<<" (*)"<<endl;
                        else
                            cout<<invert<<root->_faninlist[k]->getTypeStr()<<" "<<root->_faninlist[k]->_Gid<<endl;
                        root->_faninlist[k]->setToGlobalRef();
                        cnt++;
                        samechild = false;
                        beenboth = false;
                    }
                    if(root->_faninlist[0]->_Gid == root->_faninlist[1]->_Gid)
                        samechild  = true;
                     if(root->_faninlist[0]->isGlobalRef() && root->_faninlist[1]->isGlobalRef())
                         beenboth = true;
                }
                if(cnt == 0)
                    resume = true;
                if(s.back() == root && !s.empty())
                    s.pop_back();
                firstin = false;
            }
            return;
        }
    }
}

void
CirGate::reportFanout(int level) const
{
    assert (level >= 0);
    vector<CirGate* > _Otrace;
    vector<CirGate* > _Nsplit;
    bool finish = false;
    //root
    cout<<this->getTypeStr()<<" "<<this->_Gid<<endl;
    if(this->_fanoutlist.empty() || level==0)
    {
        finish=true;
        return;
    }
    
    for(int i=0; i<this->_fanoutlist.size(); i++)
    {
        setGlobalRef();
        int index = 1;
        CirGate* next = this->_fanoutlist[i];
        string invert = "";
        if(abs(next->_tmpfanin1)==INT_MAX && this->_Gid == 0)
        {
            if(next->_tmpfanin1<0)
                invert = "!";
        }
        else if(abs(next->_tmpfanin2)==INT_MAX && this->_Gid == 0)
        {
            if(next->_tmpfanin2<0)
                invert = "!";
        }
        else if(abs(next->_tmpfanin1)!=INT_MAX && (this->_Gid == abs(next->_tmpfanin1)))
        {
            if(next->_tmpfanin1<0)
                invert = "!";
        }
        else if(abs(next->_tmpfanin2)!=INT_MAX && (this->_Gid == abs(next->_tmpfanin2)))
        {
            if(next->_tmpfanin2<0)
                invert = "!";
        }
        
        for(int i=0; i<2*index; i++)
            cout<<" ";
        if(next->_ref>0)
        {
            if(!next->_fanoutlist.empty() && (index<level))
                cout<<invert<<next->getTypeStr()<<" "<<next->_Gid<<" (*)"<<endl;
            else
                cout<<invert<<next->getTypeStr()<<" "<<next->_Gid<<endl;
            next->setToGlobalRef();
             _Otrace.push_back(next);
            continue;
        }
        else
            cout<<invert<<next->getTypeStr()<<" "<<next->_Gid<<endl;
        next->setToGlobalRef();
        _Otrace.push_back(next);
        
        Helpfano(next, index, level, _Nsplit, _Otrace);
    }
    
    for(int k=0; k<_Otrace.size(); k++)
        _Otrace[k]->_ref = 0;
}

bool
CirGate::FromOne(CirGate* mother, CirGate* child)const
{
    int succ1 = abs(mother->_tmpfanin1);
    int succ2 = abs(mother->_tmpfanin2);
    if(succ1 == INT_MAX && child->_Gid == 0)
        return true;
    else if(succ2 == INT_MAX && child->_Gid == 0)
        return false;
    else
    {
        if(succ1 == child->_Gid)
            return true;
        else if(succ2 == child->_Gid)
            return false;}
}
bool
CirGate::CheckInvert(CirGate* mother, CirGate* child)const
{
    if(FromOne(mother, child))
    {
        if(mother->_tmpfanin1<0)
            return true;}
    else
    {
        if(mother->_tmpfanin2<0)
            return true;}
    return false;
}
bool
CirGate::IsChild(CirGate* mother, CirGate* child)const
{
    if(abs(mother->_tmpfanin1) == INT_MAX && child->_Gid == 0)
        return true;
    else if(abs(mother->_tmpfanin1) != INT_MAX && (child->_Gid == abs(mother->_tmpfanin1)))
        return true;
    if(mother->_faninlist.size()>1)
    {
        if(abs(mother->_tmpfanin2) == INT_MAX && child->_Gid == 0)
            return true;
        else if(abs(mother->_tmpfanin2) != INT_MAX && (child->_Gid == abs(mother->_tmpfanin2)))
            return true;
    }
    return false;
}


void
CirGate::Helpfano(CirGate* g, int index, int level, vector<CirGate*>& s, vector<CirGate*>& t)const
{
    bool handle = false;
    while(!g->_fanoutlist.empty())
    {
        if(g->_fanoutlist.size()>1)
            s.push_back(g);
        CirGate* tmprt = g;
        string invert = "";
        g = g->_fanoutlist[0];
        index++;
        if(index>level)
            break;
        if(CheckInvert(g,tmprt))
            invert = "!";
        for(int i=0; i<2*index; i++)
            cout<<" ";
        if(g->_ref>0 && !g->isGlobalRef())
        {
            if(!g->_fanoutlist.empty() && (index<level))
                cout<<invert<<g->getTypeStr()<<" "<<g->_Gid<<" (*)"<<endl;
            else
                cout<<invert<<g->getTypeStr()<<" "<<g->_Gid<<endl;
            g->setToGlobalRef();
            t.push_back(g);
            handle = true;
        }
        else
        {
            cout<<invert<<g->getTypeStr()<<" "<<g->_Gid<<endl;
            g->setToGlobalRef();
            t.push_back(g);
            if(index == level)
                handle = true;
        }
        
        if(g->_fanoutlist.empty() || handle)
        {
            handle = false;
            CirGate* root;
            while(!s.empty())
            {
                int i=1;
                while(!(t[t.size()-i]==s.back()))
                {
                    if(IsChild(t[t.size()-i],t[t.size()-i-1]))
                        index--;
                    i++;
                }
                root = s.back();
                int tmp = index;
                for(int k=0; k<root->_fanoutlist.size(); k++)
                {
                    string invert = "";
                    if(CheckInvert(root->_fanoutlist[k], root))
                        invert = "!";
                    if(root->_fanoutlist[k]->_ref==0)
                    {
                        index = tmp+1;
                        for(int i=0; i<2*index; i++)
                            cout<<" ";
                        cout<<invert<<root->_fanoutlist[k]->getTypeStr()<<" "<<root->_fanoutlist[k]->_Gid<<endl;
                        root->_fanoutlist[k]->setToGlobalRef();
                        t.push_back(root->_fanoutlist[k]);
                        Helpfano(root->_fanoutlist[k], index, level, s, t);
                    }
                    else if(root->_fanoutlist[k]->_ref>0 && (!root->_fanoutlist[k]->isGlobalRef()))
                    {
                        index = tmp+1;
                        for(int i=0; i<2*index; i++)
                            cout<<" ";
                        if(!root->_fanoutlist[k]->_fanoutlist.empty() && (index<level))
                            cout<<invert<<root->_fanoutlist[k]->getTypeStr()<<" "<<root->_fanoutlist[k]->_Gid<<" (*)"<<endl;
                        else
                            cout<<invert<<root->_fanoutlist[k]->getTypeStr()<<" "<<root->_fanoutlist[k]->_Gid<<endl;
                        root->_fanoutlist[k]->setToGlobalRef();
                    }
                }
                if(s.back() == root)
                    s.pop_back();
            }
            return;
        }
    }
}


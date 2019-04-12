/****************************************************************************
  FileName     [ cirMgr.h ]
  PackageName  [ cir ]
  Synopsis     [ Define circuit manager ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef CIR_MGR_H
#define CIR_MGR_H

#include <vector>
#include <string>
#include <fstream>
#include <iostream>

using namespace std;

#include "cirDef.h"
#include "cirGate.h"

extern CirMgr *cirMgr;

// TODO: Define your own data members and member functions
class CirMgr
{
public:
    CirMgr(){}
    ~CirMgr() {}

    // Access functions
    // return '0' if "gid" corresponds to an undefined gate.
    CirGate* getGate(unsigned gid) const
    {
        for(int i=0; i<_OAIGlist.size(); i++)
        {
            if(_OAIGlist[i]->_Gid == gid)
                return _OAIGlist[i];
        }
        for(int i=0; i<_PIlist.size(); i++)
        {
            if(_PIlist[i]->_Gid == gid)
                return _PIlist[i];
        }
        for(int i=0; i<_POlist.size(); i++)
        {
            if(_POlist[i]->_Gid == gid)
                return _POlist[i];
        }
        return 0;
    }

    // Member functions about circuit construction
    int str2ID(const string&);
    int aagFormat(int &result)const;
    vector<int> str2IDv(const string&);
    bool readCircuit(const string&);
    CirGate* decideG(int num);
    CirGate* findmin(CirGate* g);
    void traverse(CirGate* g);
    void setDFS();

    // Member functions about circuit reporting
    void printSummary() const;
    void printNetlist() const;
    void printPIs() const;
    void printPOs() const;
    void printFloatGates() const;
    void writeAag(ostream&) const;

private:
    unsigned _M, _I, _L, _O, _A;
    vector<CirGate*> _PIlist;
    vector<CirGate*> _POlist;
    vector<CirGate*> _OAIGlist;
    vector<CirGate*> _Trace;            //clear handled
    vector<CirGate*> _DFSGlist;
};

#endif // CIR_MGR_H

/****************************************************************************
  FileName     [ cirGate.h ]
  PackageName  [ cir ]
  Synopsis     [ Define basic gate data structures ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef CIR_GATE_H
#define CIR_GATE_H

#include <string>
#include <vector>
#include <iostream>
#include "cirDef.h"
using namespace std;

class CirGate;

//------------------------------------------------------------------------
//   Define classes
//------------------------------------------------------------------------
// TODO: Define your own data members and member functions, or classes
class CirGate
{
    friend class CirMgr;
public:
    CirGate() {}
    virtual ~CirGate() {}

    // Basic access methods
    virtual string getTypeStr() const = 0;
    unsigned getGateID() const { return _Gid; }
    unsigned getLineNo() const { return _lineNum; }
    string getSym() const {return _sym;}
    bool isGlobalRef(){return _ref == _globalRef;}
    void setToGlobalRef(){_ref = _globalRef;}
    static void setGlobalRef(){_globalRef++;}
    
    bool FromOne(CirGate* mother, CirGate* child)const;
    bool CheckInvert(CirGate* mother, CirGate* child)const;
    bool IsChild(CirGate* mother, CirGate* child)const;
    void Helpfano(CirGate* g, int index, int level, vector<CirGate*>& s, vector<CirGate*>& t)const;
    void Helpfani(CirGate* g, int index, int level, vector<CirGate*>& s, vector<CirGate*>& t)const;
    bool ManyTRoutes(CirGate*& main, vector<CirGate*>& t)const;

    // Printing functions
    virtual void printGate() const = 0;
    void reportGate() const;
    void reportFanin(int level) const;
    void reportFanout(int level) const;
    
    string _type;
    static unsigned _globalRef;
    unsigned _ref = 0;
    unsigned _Gid;
    unsigned _lineNum;
    unsigned _colNum;
    int _tmpfanin1 = 0;
    int _tmpfanin2 = 0;
    bool visited = false;
    string _sym = ""; //PI, PO
    vector<CirGate*> _faninlist;
    vector<CirGate*> _fanoutlist;

private:

protected:

};

class PI: public CirGate
{
public:
    string getTypeStr() const {return "PI";}
    void printGate() const{
        if(_sym !="")
            cout<<"PI  "<<_Gid<<" ("<<_sym<<")"<<endl;
        else
            cout<<"PI  "<<_Gid<<endl;
    }
private:
    string _type = "PI";
};

class PO: public CirGate
{
public:
    string getTypeStr() const {return "PO";}
    void printGate() const{
        string invert = "";
        string undef = "";
        if(_tmpfanin1 < 0)
            invert = "!";
        if(_faninlist[0]->getTypeStr() == "UNDEF")
            undef = "*";
        if(_sym !="")
            cout<<"PO  "<<_Gid<<" "<<undef<<invert<<_faninlist[0]->_Gid<<" ("<<_sym<<")"<<endl;
        else
            cout<<"PO  "<<_Gid<<" "<<undef<<invert<<_faninlist[0]->_Gid<<endl;
    }
private:
    string _type = "PO";
};

class AIG: public CirGate
{
public:
    string getTypeStr() const {return "AIG";}
    void printGate() const{
        string invert1 = "";
        string invert2 = "";
        string undef1 = "";
        string undef2 = "";
        if(_tmpfanin1 < 0)
            invert1 = "!";
        if(_tmpfanin2 < 0)
            invert2 = "!";
        if(_faninlist[0]->getTypeStr() == "UNDEF")
            undef1 = "*";
        if(_faninlist[1]->getTypeStr()=="UNDEF")
            undef2 = "*";
        cout<<"AIG "<<_Gid<<" "<<undef1<<invert1<<_faninlist[0]->_Gid<<" "<<undef2<<invert2<<_faninlist[1]->_Gid<<endl;
    }
private:
    string _type = "AIG";
};

class Constzero: public CirGate
{
public:
    string getTypeStr() const {return "CONST";}
    unsigned getGateID() const { return 0; }
    void printGate() const{
        cout<<"CONST0"<<endl;
    }
private:
    string _type = "CONST";
    unsigned _Gid = 0;
};

class Undef: public CirGate
{
public:
    string getTypeStr() const {return "UNDEF";}
    void printGate() const{
        cout<<""<<endl;
    }
private:
    string _type = "UNDEF";
};
#endif // CIR_GATE_H

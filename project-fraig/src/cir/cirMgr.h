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
#include <map>
#include <string>
#include <fstream>
#include <iostream>

using namespace std;

// TODO: Feel free to define your own classes, variables, or functions.

#include "cirDef.h"
#include "cirGate.h"
extern CirMgr *cirMgr;

class CirGateV
{
public:
    CirGateV(CirGate* g): _gateV(g){;}
    ~CirGateV(){}
    CirGate* getGateV()const {return _gateV;}
    
    size_t operator() () const
    {
        int in0 = _gateV -> getInput1Id();
        int in1 = _gateV -> getInput2Id();
        if(_gateV -> getInput1Invered())
            in0+=10;
        if(_gateV -> getInput2Invered())
            in1+=10;
        return in0*in1;
    }
    
    bool operator == (const CirGateV& k) const {
        if((k.getGateV() -> getInput1Id() == _gateV -> getInput1Id())&&
           (k.getGateV() -> getInput2Id() == _gateV -> getInput2Id())&&
           (k.getGateV() -> getInput1Invered() == _gateV -> getInput1Invered())&&
           (k.getGateV() -> getInput2Invered() == _gateV -> getInput2Invered()))
            return true;
        else if((k.getGateV() -> getInput1Id() == _gateV -> getInput2Id())&&
                (k.getGateV() -> getInput2Id() == _gateV -> getInput1Id())&&
                (k.getGateV() -> getInput1Invered() == _gateV -> getInput2Invered())&&
                (k.getGateV() -> getInput2Invered() == _gateV -> getInput1Invered()))
            return true;
        else
            return false;
    }
private:
    CirGate* _gateV;
};

class CirGateFEC
{
public:
    CirGateFEC(CirGate* g): _gateV(g){;}
    ~CirGateFEC(){}
    
    CirGate* getGateFEC()const {return _gateV;}
    
    size_t operator() () const
    {
        bitset<64> b = _gateV -> getGateBit();
        return b.to_ulong();
    }
    
    bool operator == (const CirGateFEC& k) const {
        if(k.getGateFEC() == _gateV)
            return true;
        else
            return false;
    }
private:
    CirGate* _gateV;
};

class CirMgr
{
public:
    CirMgr() {}
    ~CirMgr() {}

    // Access functions
    // return '0' if "gid" corresponds to an undefined gate.
    CirGate* getGate(unsigned gid) const {
        for(int i=0;i<gate_list.size();++i)
        {
            if(gate_list[i]!=0)
            {
                if((gate_list[i]->getId())==gid){return gate_list[i];}
            }
        }
        return 0;
    }
    
    vector<vector<CirGate*>> getFECGL() {return fec_g_list;}
    
    // Member functions about circuit construction
    bool readCircuit(const string&);

    // Member functions about circuit optimization
    void sweep();
    void u_Output(int, int);
    void u_Input(int, int);
    void optimize();

    // Member functions about simulation
    void randomSim();
    void fileSim(ifstream&);
    void setSimLog(ofstream *logFile) { _simLog = logFile; }
    vector<CirGate*> Convert2Gate(vector<CirGateFEC>& a)const;

    // Member functions about fraig
    void strash();
    void printFEC() const;
    void fraig();

    // Member functions about circuit reporting
    void printSummary() const;
    void printNetlist() const;
    void printPIs() const;
    void printPOs() const;
    void printFloatGates() const;
    void printFECPairs() const;
    void writeAag(ostream&) const;
    void writeGate(ostream&, CirGate*) const;
    
    void cir_dfs(int i);
    void ciro_dfs(int i);
    void cirs_dfs(int i);
    void merge(CirGate* mergeG, CirGate* itemG);
    
    GateList gate_list;
    vector<vector<CirGate*>> fec_g_list;
    map<int,int>trace;
    int traverse_num =0;

private:
    ofstream           *_simLog;
    
    bool has_const = false;
    int net_constructed = false;
    int max_variable;
    int input_number;
    int latch_number;
    int output_number;
    int gate_number;
    
    map<int,int>input_id;
    vector<int>output_file;
    vector<int>input_file;
    map<int,int>gate_id_map;
    map<int,int>undef_id;
    IdList output_id;
    vector <bool> output_id_inverted;
    
    IdList gate_id;
    vector<int>net_list;
    int net_list_index=0;

};

#endif // CIR_MGR_H

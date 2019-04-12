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
#include <utility>
#include <map>
#include <bitset>
#include <algorithm>
#include "cirDef.h"
#include "sat.h"

using namespace std;

class CirGate;
class CirAndGate;
class CirPiGate;
class CirPoGate;
class CirUnGate;
// TODO: Feel free to define your own classes, variables, or functions.

class CirGate;

//------------------------------------------------------------------------
//   Define classes
//------------------------------------------------------------------------
class CirGate
{
public:
    CirGate() {}
    virtual ~CirGate() {}
    
    
    string getTypeStr() const {
        if(gate_type==PI_GATE){return "PI";}
        else if(gate_type==PO_GATE) {return "PO";}
        else if(gate_type==AIG_GATE) {return "AIG";}
        else if(gate_type==UNDEF_GATE) {return "UNDEF";}
        else if(gate_type==CONST_GATE) {return "CONST";}
        
        return "";
    }
    
    // Basic access methods
    bool getFECTrace()const{return fec_trace;}
    bitset<64> getGateBit() const {return gate_bit;}
    unsigned getLineNo() const { return line_id; }
    unsigned getLineId() const { return line_id; }
    unsigned getId()const{return gate_id;}
    unsigned getInput1Id()const{return gate_input1;}
    unsigned getInput2Id()const{return gate_input2;}
    unsigned getInput1Pos()const{return input1_pos;}
    unsigned getInput2Pos()const{return input2_pos;}
    GateType getGateType() const{return gate_type;}
    bool getInput1Invered ()const{ return input1_inverted;}
    bool getInput2Invered ()const{ return input2_inverted;}
    string getGateName()const{return gate_name;}
    vector<pair<unsigned,bool>>getOutput(){return output;}
    
    void setFECTrace(bool a){fec_trace = a;}
    void setGateBit(bitset<64> a){gate_bit = a;}
    void setGateId(unsigned gi){gate_id = gi;}
    void setOutput(unsigned output_id,bool inverted){
        pair<unsigned,bool>p=make_pair(output_id,inverted);
        output.push_back(p);
    }
    void setOutputV(vector<pair<unsigned,bool>> V){
        output = V;
    }
    void setLineId(unsigned lineid){line_id = lineid;}
    void setInput1(unsigned i1){ gate_input1 =i1;}
    void setInput2(unsigned i2){ gate_input2 =i2;}
    void setInput1Pos(unsigned i1){ input1_pos =i1;}
    void setInput2Pos(unsigned i2){ input2_pos =i2;}
    void setInput1Invered(bool inverted){ input1_inverted = inverted;}
    void setInput2Invered(bool inverted){ input2_inverted = inverted;}
    void setGateType(GateType g){gate_type = g;}
    void setGateName(string g){gate_name = g;}
    void gate_in_traverse(int i,int l,bool v,int levelnum)const;
    void gate_out_traverse(int i,int l,bool v,int levelnum)const;
    

    virtual void printGate() const = 0;
    virtual bool isAig() const { return false; }
    void reportGate() const;
    void reportFanin(int level) const;
    void reportFanout(int level) const;
    
private:
    unsigned gate_id;
    unsigned gate_input1=-10;
    unsigned gate_input2=-10;
    unsigned input1_pos=-2;
    unsigned input2_pos=-2;
    bitset<64> gate_bit;
    bool input1_inverted;
    bool input2_inverted;
    bool ignored = false;
    bool fec_trace = false;
    GateType gate_type;
    vector<pair<unsigned,bool>>output;
    string gate_name="000";
    unsigned line_id;
protected:
    
};

class CirUnGate : public CirGate
{
public:
    CirUnGate() {}
    ~CirUnGate(){}
    
    void printGate()const{
        cout<<"UNDEF "<<getId();
    }
    
    
};
class CirPiGate : public CirGate
{
public:
    CirPiGate() {}
    ~CirPiGate(){}
    
    void printGate()const{
        cout<<"PI "<<getId();
    }
    
    
};
class CirPoGate : public CirGate
{
public:
    CirPoGate() {}
    ~CirPoGate(){}
    
    void printGate()const{
        cout<<"PO "<<getId();
    }
    
    
};
class CirAndGate : public CirGate
{
public:
    CirAndGate() {}
    ~CirAndGate(){}
    
    
    void printGate()const{
        cout<<"AIG "<<getId();
    }
    bool isAig()const{
        return true;
    }
};

#endif // CIR_GATE_H

/****************************************************************************
  FileName     [ cirFraig.cpp ]
  PackageName  [ cir ]
  Synopsis     [ Define cir FRAIG functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2012-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#include <cassert>
#include "cirMgr.h"
#include "cirGate.h"
#include "sat.h"
#include "myHashSet.h"
#include "util.h"

using namespace std;

// TODO: Please keep "CirMgr::strash()" and "CirMgr::fraig()" for cir cmd.
//       Feel free to define your own variables or functions

/*******************************/
/*   Global variable and enum  */
/*******************************/

/**************************************/
/*   Static varaibles and functions   */
/**************************************/

/*******************************************/
/*   Public member functions about fraig   */
/*******************************************/
// _floatList may be changed.
// _unusedList and _undefList won't be changed
void
CirMgr::strash()
{
    bool reset_netlist = false;
    HashSet<CirGateV> _hashset(5*net_list.size());
    
    for(int i=0; i<net_list.size(); ++i)
    {
        if(gate_list[net_list[i]] -> getGateType() == AIG_GATE)
        {
            CirGateV item(gate_list[net_list[i]]);
            
            if(_hashset.check(item))
            {
                CirGateV mergeGV = _hashset.findGV(item);
                CirGate* mergeG = mergeGV.getGateV();
                CirGate* itemG = item.getGateV();
                cout<<"Strashing: "<<mergeG->getId()<<" merging "<<itemG->getId()<<"..."<<endl;
                merge(mergeG, itemG);
                reset_netlist = true;
            }
            else
                _hashset.insert(item);
        }
    }
    
    if(reset_netlist)
    {
        trace.clear();
        net_list.clear();
        for(int j=0; j<gate_list.size(); ++j)
        {
            if(gate_list[j]!=0)
            {
                if(gate_list[j] -> getGateType() == PO_GATE)
                    cirs_dfs(j);
            }
        }
    }
}
void
CirMgr::merge(CirGate* mergeG, CirGate* itemG)
{
    int in0_pos = itemG -> getInput1Pos();
    int in1_pos = itemG -> getInput2Pos();
    u_Input(in0_pos, itemG -> getId());
    u_Input(in1_pos, itemG -> getId());
    
    vector<pair<unsigned,bool>> vec = itemG -> getOutput();
    int Mindex;
    int Iindex;
    for(int k=0; k<gate_list.size(); ++k)
    {
        if(gate_list[k]!=0)
        {
            if(gate_list[k] == mergeG)
            {
                Mindex = k;
                break;
            }
        }
    }
 
    for(auto&i:vec)
    {
        if(gate_list[i.first] -> getInput1Id() == itemG -> getId())
        {
            Iindex = gate_list[i.first] -> getInput1Pos();
            gate_list[i.first] -> setInput1(mergeG -> getId());
            gate_list[i.first] -> setInput1Pos(Mindex);
        }
        else if(gate_list[i.first] -> getInput2Id() == itemG -> getId())
        {
            Iindex = gate_list[i.first] -> getInput2Pos();
            gate_list[i.first] -> setInput2(mergeG -> getId());
            gate_list[i.first] -> setInput2Pos(Mindex);
        }
        mergeG -> setOutput(i.first,i.second);
    }
    
    gate_list[Iindex] = 0;
    delete itemG;
}
void
CirMgr::cirs_dfs(int i)
{
    if(trace.count(i)==0){
        trace[i]=0;
        
        if(gate_list[i] -> getGateType() == PI_GATE)
        {
            
            net_list.push_back(i);
            
        }
        else if(gate_list[i] -> getGateType() == CONST_GATE)
        {
            net_list.push_back(i);
        }
        else if(gate_list[i] -> getGateType() == PO_GATE)
        {
            
            ciro_dfs(gate_list[i]->getInput1Pos());
            net_list.push_back(i);
        }
        else if(gate_list[i] -> getGateType() == AIG_GATE){
            
            int input1 = gate_list[i] -> getInput1Id();
            int input2 = gate_list[i] -> getInput2Id();
            
            if(gate_list[i]->getInput1Pos()>=0) {ciro_dfs(gate_list[i]->getInput1Pos());}
            if(gate_list[i]->getInput2Pos()>=0) {ciro_dfs(gate_list[i]->getInput2Pos());}
            
            net_list.push_back(i);
        }
    }
}

void
CirMgr::fraig()
{
}

/********************************************/
/*   Private member functions about fraig   */
/********************************************/

/****************************************************************************
  FileName     [ cirSim.cpp ]
  PackageName  [ cir ]
  Synopsis     [ Define cir optimization functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#include <cassert>
#include <string>
#include <vector>
#include <iostream>
#include <utility>
#include <map>
#include <algorithm>
#include "cirMgr.h"
#include "cirGate.h"
#include "util.h"

using namespace std;

// TODO: Please keep "CirMgr::sweep()" and "CirMgr::optimize()" for cir cmd.
//       Feel free to define your own variables or functions

/*******************************/
/*   Global variable and enum  */
/*******************************/

/**************************************/
/*   Static varaibles and functions   */
/**************************************/

/**************************************************/
/*   Public member functions about optimization   */
/**************************************************/
// Remove unused gates
// DFS list should NOT be changed
// UNDEF, float and unused list may be changed
void
CirMgr::u_Output(int m, int c)
{
    if(gate_list[m]->getInput1Id() == c)
    {
        gate_list[m]->setInput1(-10);
        //cout<<gate_list[m]->getId()<<"'s input has changed"<<endl;
    }
    if(gate_list[m]->getInput2Id() == c)
    {
        gate_list[m]->setInput2(-10);
        //cout<<gate_list[m]->getId()<<"'s input has changed"<<endl;
    }
        
}
void
CirMgr::u_Input(int c, int m)
{
    vector<pair<unsigned,bool>> vec = gate_list[c] -> getOutput();
    int n=0;
    if(!vec.empty())
    {
        for(int i=0; i<vec.size(); ++i)
        {
            //cout<<gate_list[c] -> getId()<<"'s output is "<<gate_list[vec[i].first]->getId()<<endl;
            if(gate_list[vec[i].first]->getId() == m)
            {
                vec.erase(vec.begin()+n);
                --i;
                --n;
            }
            n++;
        }
        gate_list[c] -> setOutputV(vec);
    }
}

void
CirMgr::sweep()
{
    trace.clear();
    for(int i=0; i<net_list.size(); ++i)
    {
        if(gate_list[net_list[i]]!=0)
        {
            if(gate_list[net_list[i]]->getId())
            {
                trace[net_list[i]]=1;
            }
        }
    }

    for(int j=0; j<gate_list.size(); ++j)
    {
        if(trace.count(j)==0 &&
           gate_list[j]!=0 &&
           gate_list[j]->getGateType() != CONST_GATE &&
           gate_list[j]->getGateType() != PI_GATE)
        {
            int item_Id  = gate_list[j] -> getId();
            vector<pair<unsigned,bool>> vec = gate_list[j] -> getOutput();
            if(!vec.empty())
            {
                for(auto&x:vec)
                {
                    if(gate_list[x.first]!=0)
                    {
                        u_Output(x.first,item_Id);
                    }
                }
            }
            
            if(gate_list[j]->getInput1Id()>=0)
            {
                int p1 = gate_list[j]->getInput1Pos();
                if(p1>=0 && gate_list[p1]!=0)
                {
                    u_Input(p1, item_Id);
                }
            }
            if(gate_list[j]->getInput2Id()>=0)
            {
                int p2 = gate_list[j]->getInput2Pos();
                if(p2>=0 && gate_list[p2]!=0)
                {
                    u_Input(p2, item_Id);
                }
            }
            
            cout<<"Sweeping: "<<gate_list[j]->getTypeStr()<<"("<<gate_list[j]->getId()<<") removed..."<<endl;
                
            if(gate_list[j]->getGateType() == AIG_GATE)
                --gate_number;
                
            CirGate* item = gate_list[j];
            gate_list[j] = 0;
            delete item;
        }
    }
}

// Recursively simplifying from POs;
// _dfsList needs to be reconstructed afterwards
// UNDEF gates may be delete if its fanout becomes empty...
void
CirMgr::optimize()
{
    bool reset_dfs = false;
    for(int i=0; i<net_list.size(); ++i)
    {
        if(gate_list[net_list[i]] -> getGateType() == AIG_GATE)
        {
            //the other fanin
            if(gate_list[net_list[i]] -> getInput1Id() == 0 && gate_list[net_list[i]] -> getInput1Invered())
            {
                vector<pair<unsigned, bool>> vec = gate_list[net_list[i]] -> getOutput();
                for(auto&j:vec)
                {
                    bool phase_sh = false;
                    if(gate_list[j.first] -> getInput1Id() == gate_list[net_list[i]] -> getId())
                    {
                        gate_list[j.first] -> setInput1(gate_list[net_list[i]]->getInput2Id());
                        gate_list[j.first] -> setInput1Pos(gate_list[net_list[i]]->getInput2Pos());
                        if((gate_list[j.first] -> getInput1Invered() && gate_list[net_list[i]]->getInput2Invered()) ||
                           (!gate_list[j.first] -> getInput1Invered() && !gate_list[net_list[i]]->getInput2Invered()) )
                            gate_list[j.first] -> setInput1Invered(phase_sh);
                        else
                        {
                            phase_sh = true;
                            gate_list[j.first] -> setInput1Invered(phase_sh);
                        }
                    }
                    else if(gate_list[j.first] -> getInput2Id() == gate_list[net_list[i]] -> getId())
                    {
                        gate_list[j.first] -> setInput2(gate_list[net_list[i]]->getInput2Id());
                        gate_list[j.first] -> setInput2Pos(gate_list[net_list[i]]->getInput2Pos());
                        if((gate_list[j.first] -> getInput2Invered() && gate_list[net_list[i]]->getInput2Invered()) ||
                           (!gate_list[j.first] -> getInput2Invered() && !gate_list[net_list[i]]->getInput2Invered()) )
                            gate_list[j.first] -> setInput2Invered(phase_sh);
                        else
                        {
                            phase_sh = true;
                            gate_list[j.first] -> setInput2Invered(phase_sh);
                        }
                    }
                    gate_list[gate_list[net_list[i]]->getInput2Pos()] -> setOutput(j.first,phase_sh);
                }
                
                u_Input(gate_list[net_list[i]]->getInput2Pos(), gate_list[net_list[i]]->getId());
                u_Input(gate_list[net_list[i]]->getInput1Pos(), gate_list[net_list[i]]->getId());
    
                if(gate_list[net_list[i]]->getInput2Invered())
                    cout<<"Simplifying: "<<gate_list[gate_list[net_list[i]]->getInput2Pos()]->getId()<<" merging "<<"!"<<gate_list[net_list[i]]->getId()<<"..."<<endl;
                else
                    cout<<"Simplifying: "<<gate_list[gate_list[net_list[i]]->getInput2Pos()]->getId()<<" merging "<<gate_list[net_list[i]]->getId()<<"..."<<endl;
               
                CirGate* item = gate_list[net_list[i]];
                gate_list[net_list[i]] = 0;
                delete item;
                reset_dfs = true;
            }
            else if(gate_list[net_list[i]] -> getInput2Id() == 0 && gate_list[net_list[i]] -> getInput2Invered())
            {
                vector<pair<unsigned, bool>> vec = gate_list[net_list[i]] -> getOutput();
                for(auto&j:vec)
                {
                    bool phase_sh = false;
                    if(gate_list[j.first] -> getInput1Id() == gate_list[net_list[i]] -> getId())
                    {
                        gate_list[j.first] -> setInput1(gate_list[net_list[i]]->getInput1Id());
                        gate_list[j.first] -> setInput1Pos(gate_list[net_list[i]]->getInput1Pos());
                        if((gate_list[j.first] -> getInput1Invered() && gate_list[net_list[i]]->getInput1Invered()) ||
                           (!gate_list[j.first] -> getInput1Invered() && !gate_list[net_list[i]]->getInput1Invered()) )
                            gate_list[j.first] -> setInput1Invered(phase_sh);
                        else
                        {
                            phase_sh = true;
                            gate_list[j.first] -> setInput1Invered(phase_sh);
                        }
                        
                    }
                    else if(gate_list[j.first] -> getInput2Id() == gate_list[net_list[i]] -> getId())
                    {
                        gate_list[j.first] -> setInput2(gate_list[net_list[i]]->getInput1Id());
                        gate_list[j.first] -> setInput2Pos(gate_list[net_list[i]]->getInput1Pos());
                        if((gate_list[j.first] -> getInput2Invered() && gate_list[net_list[i]]->getInput1Invered()) ||
                           (!gate_list[j.first] -> getInput2Invered() && !gate_list[net_list[i]]->getInput1Invered()) )
                            gate_list[j.first] -> setInput2Invered(phase_sh);
                        else
                        {
                            phase_sh = true;
                            gate_list[j.first] -> setInput2Invered(phase_sh);
                        }

                    }
                     gate_list[gate_list[net_list[i]]->getInput1Pos()] -> setOutput(j.first,phase_sh);
                }
                u_Input(gate_list[net_list[i]]->getInput2Pos(), gate_list[net_list[i]]->getId());
                u_Input(gate_list[net_list[i]]->getInput1Pos(), gate_list[net_list[i]]->getId());
                
                if(gate_list[net_list[i]]->getInput1Invered())
                    cout<<"Simplifying: "<<gate_list[gate_list[net_list[i]]->getInput1Pos()]->getId()<<" merging "<<"!"<<gate_list[net_list[i]]->getId()<<"..."<<endl;
                else
                    cout<<"Simplifying: "<<gate_list[gate_list[net_list[i]]->getInput1Pos()]->getId()<<" merging "<<gate_list[net_list[i]]->getId()<<"..."<<endl;
                
                CirGate* item = gate_list[net_list[i]];
                gate_list[net_list[i]] = 0;
                delete item;
                reset_dfs = true;
            }
            //const 0
            else if((gate_list[net_list[i]] -> getInput1Id() == 0 && !gate_list[net_list[i]] -> getInput1Invered()) ||(gate_list[net_list[i]] -> getInput2Id() == 0 && !gate_list[net_list[i]] -> getInput2Invered()))
            {
                bool do1 = false;
                bool do2 = false;
                if(gate_list[net_list[i]] -> getInput1Id() == 0)
                    do1 = true;
                else if(gate_list[net_list[i]] -> getInput2Id() == 0)
                    do2 = true;
                
                vector<pair<unsigned, bool>> vec = gate_list[net_list[i]] -> getOutput();
                for(auto&j:vec)
                {
                    if(gate_list[j.first] -> getInput1Id() == gate_list[net_list[i]] -> getId())
                    {
                        gate_list[j.first] -> setInput1(0);
                        if(do1)
                            gate_list[j.first] -> setInput1Pos(gate_list[net_list[i]]->getInput1Pos());
                        if(do2)
                            gate_list[j.first] -> setInput1Pos(gate_list[net_list[i]]->getInput2Pos());
                        
                    }
                    else if(gate_list[j.first] -> getInput2Id() == gate_list[net_list[i]] -> getId())
                    {
                        gate_list[j.first] -> setInput2(0);
                        if(do1)
                        {
                            gate_list[j.first] -> setInput2Pos(gate_list[net_list[i]]->getInput1Pos());
                        }
                        if(do2)
                        {
                            gate_list[j.first] -> setInput2Pos(gate_list[net_list[i]]->getInput2Pos());
                        }
                    }
                    if(do1)
                        gate_list[gate_list[net_list[i]]->getInput1Pos()] -> setOutput(j.first,j.second);
                    if(do2)
                        gate_list[gate_list[net_list[i]]->getInput2Pos()] -> setOutput(j.first,j.second);
                    
                }
                u_Input(gate_list[net_list[i]]->getInput2Pos(), gate_list[net_list[i]]->getId());
                u_Input(gate_list[net_list[i]]->getInput1Pos(), gate_list[net_list[i]]->getId());
                cout<<"Simplifying: 0 merging "<<gate_list[net_list[i]]->getId()<<"..."<<endl;
                
                CirGate* item = gate_list[net_list[i]];
                gate_list[net_list[i]] = 0;
                delete item;
                reset_dfs = true;

            }
            //fanin + phase
            else if((gate_list[net_list[i]] -> getInput1Id() == gate_list[net_list[i]] -> getInput2Id()) &&
               (gate_list[net_list[i]] -> getInput1Invered() == gate_list[net_list[i]] -> getInput2Invered()))
            {
                vector<pair<unsigned, bool>> vec = gate_list[net_list[i]] -> getOutput();
                for(auto&j:vec)
                {
                    bool phase_sh = false;
                    if(gate_list[j.first] -> getInput1Id() == gate_list[net_list[i]] -> getId())
                    {
                        gate_list[j.first] -> setInput1(gate_list[net_list[i]]->getInput1Id());
                        gate_list[j.first] -> setInput1Pos(gate_list[net_list[i]]->getInput1Pos());
                        if((gate_list[j.first] -> getInput1Invered() && gate_list[net_list[i]]->getInput1Invered()) ||
                           (!gate_list[j.first] -> getInput1Invered() && !gate_list[net_list[i]]->getInput1Invered()) )
                            gate_list[j.first] -> setInput1Invered(phase_sh);
                        else
                        {
                            phase_sh = true;
                            gate_list[j.first] -> setInput1Invered(phase_sh);
                        }
                    }
                    else if(gate_list[j.first] -> getInput2Id() == gate_list[net_list[i]] -> getId())
                    {
                        gate_list[j.first] -> setInput2(gate_list[net_list[i]]->getInput1Id());
                        gate_list[j.first] -> setInput2Pos(gate_list[net_list[i]]->getInput1Pos());
                        if((gate_list[j.first] -> getInput2Invered() && gate_list[net_list[i]]->getInput1Invered()) ||
                           (!gate_list[j.first] -> getInput2Invered() && !gate_list[net_list[i]]->getInput1Invered()) )
                            gate_list[j.first] -> setInput2Invered(phase_sh);
                        else
                        {
                            phase_sh = true;
                            gate_list[j.first] -> setInput2Invered(phase_sh);
                        }
                    }
                   
                    gate_list[gate_list[net_list[i]]->getInput1Pos()] -> setOutput(j.first,phase_sh);
                }
                u_Input(gate_list[net_list[i]]->getInput1Pos(), gate_list[net_list[i]]->getId());
                
                if(gate_list[net_list[i]]->getInput1Invered())
                    cout<<"Simplifying: "<<gate_list[net_list[i]]->getInput1Id()<<" merging "<<"!"<<gate_list[net_list[i]]->getId()<<"..."<<endl;
                else
                   cout<<"Simplifying: "<<gate_list[net_list[i]]->getInput1Id()<<" merging "<<gate_list[net_list[i]]->getId()<<"..."<<endl;
                
                CirGate* item = gate_list[net_list[i]];
                gate_list[net_list[i]] = 0;
                delete item;
                reset_dfs = true;
   
            }
            //const 0
            else if((gate_list[net_list[i]] -> getInput1Id() == gate_list[net_list[i]] -> getInput2Id()) &&
               (gate_list[net_list[i]] -> getInput1Invered() == !gate_list[net_list[i]] -> getInput2Invered()))
            {
                int Zindex;
                for(int k=0; k<gate_list.size(); ++k)
                {
                    if(gate_list[k]!=0 && gate_list[k] -> getGateType() == CONST_GATE)
                    {
                        Zindex = k;
                        break;
                    }
                }
                vector<pair<unsigned, bool>> vec = gate_list[net_list[i]] -> getOutput();
                for(auto&j:vec)
                {
                    if(gate_list[j.first] -> getInput1Id() == gate_list[net_list[i]] -> getId())
                    {
                        gate_list[j.first] -> setInput1(0);
                        gate_list[j.first] -> setInput1Pos(Zindex);
                    }
                    else if(gate_list[j.first] -> getInput2Id() == gate_list[net_list[i]] -> getId())
                    {
                        gate_list[j.first] -> setInput2(0);
                        gate_list[j.first] -> setInput2Pos(Zindex);
                    }
                    gate_list[Zindex] -> setOutput(j.first,j.second);
                    
                }
                u_Input(gate_list[net_list[i]]->getInput1Pos(), gate_list[net_list[i]]->getId());
                cout<<"Simplifying: 0 merging "<<gate_list[net_list[i]]->getId()<<"..."<<endl;
                
                CirGate* item = gate_list[net_list[i]];
                gate_list[net_list[i]] = 0;
                delete item;
                reset_dfs = true;
            }
        }
    }
    for(int i=0; i<gate_list.size(); ++i)
    {
        if(gate_list[i]!=0)
        {
            if(gate_list[i] -> getGateType() == UNDEF_GATE)
            {
                vector<pair<unsigned,bool>> vec = gate_list[i] -> getOutput();
                if(vec.empty())
                {
                    CirGate* item = gate_list[net_list[i]];
                    gate_list[net_list[i]] = 0;
                    delete item;
                }
            }
        }
    }

    if(reset_dfs)
    {
        trace.clear();
        net_list.clear();
        for(int j=0; j<gate_list.size(); ++j)
        {
            if(gate_list[j]!=0)
            {
                if(gate_list[j] -> getGateType() == PO_GATE)
                    ciro_dfs(j);
            }
        }
    }
}
void
CirMgr::ciro_dfs(int i)
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
/***************************************************/
/*   Private member functions about optimization   */
/***************************************************/

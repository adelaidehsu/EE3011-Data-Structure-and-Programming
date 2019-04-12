/****************************************************************************
  FileName     [ cirSim.cpp ]
  PackageName  [ cir ]
  Synopsis     [ Define cir simulation functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#include <fstream>
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <cassert>
#include <bitset>
#include <string>
#include <vector>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include "cirMgr.h"
#include "cirGate.h"
#include "myHashSet.h"
#include "util.h"

using namespace std;

// TODO: Keep "CirMgr::randimSim()" and "CirMgr::fileSim()" for cir cmd.
//       Feel free to define your own variables or functions

/*******************************/
/*   Global variable and enum  */
/*******************************/

/**************************************/
/*   Static varaibles and functions   */
/**************************************/

/************************************************/
/*   Public member functions about Simulation   */
/************************************************/
void
CirMgr::randomSim()
{
    int sim_Count = 0;
    bool init = true;
    bool find_Z = false;
    int net_list_size = net_list.size();
    
    vector<CirGate*> InitGrp;

    for(int i=0; i<net_list_size; ++i)
    {
        if(gate_list[net_list[i]] -> getGateType()== AIG_GATE)
            InitGrp.push_back(gate_list[net_list[i]]);
        if(gate_list[net_list[i]] -> getGateType()== CONST_GATE)
        {
            InitGrp.push_back(gate_list[net_list[i]]);
            find_Z = true;
        }
    }
    
    if(!find_Z)
    {
        InitGrp.push_back(gate_list[gate_id_map[0]]);
        gate_list[gate_id_map[0]] -> setGateBit(bitset<64>(0));
    }
    
    fec_g_list.push_back(InitGrp);
    int FECG_prev = 0;
    int FECG_after = 0;
    
    while(init || FECG_prev!= FECG_after)
    {
        //All-gate sim
        FECG_prev = fec_g_list.size();
        for(int i=0; i<input_number; ++i)
        {
            
            string instr = "";
            for(int i=0; i<64; ++i)
            {
                string inc = to_string(rand()%2);
                instr+=inc;
            }
                
            bitset<64> bin(instr);
            gate_list[input_id[input_file[i]/2]] -> setGateBit(bin);
        }
        
        for(int i=0; i<net_list_size; ++i)
        {
            if(gate_list[net_list[i]] -> getGateType() == CONST_GATE ||
               gate_list[net_list[i]] -> getGateType() == UNDEF_GATE)
                gate_list[net_list[i]] -> setGateBit(bitset<64>(0));
            
            if(gate_list[net_list[i]] -> getGateType() == AIG_GATE)
            {
                bitset<64> in0 = gate_list[gate_list[net_list[i]] -> getInput1Pos()] -> getGateBit();
                bitset<64> in1 = gate_list[gate_list[net_list[i]] -> getInput2Pos()] -> getGateBit();

                if(gate_list[net_list[i]] -> getInput1Invered() &&
                   gate_list[net_list[i]] -> getInput2Invered())
                    gate_list[net_list[i]] -> setGateBit((~in0)&(~in1));
                else if(!gate_list[net_list[i]] -> getInput1Invered() &&
                        gate_list[net_list[i]] -> getInput2Invered())
                    gate_list[net_list[i]] -> setGateBit((in0)&(~in1));
                else if(gate_list[net_list[i]] -> getInput1Invered() &&
                        !gate_list[net_list[i]] -> getInput2Invered())
                    gate_list[net_list[i]] -> setGateBit((~in0)&(in1));
                else if((!gate_list[net_list[i]] -> getInput1Invered()) &&
                        (!gate_list[net_list[i]] -> getInput2Invered()))
                    gate_list[net_list[i]] -> setGateBit(in0&in1);
            }
            if(gate_list[net_list[i]] -> getGateType() == PO_GATE)
            {
                bitset<64> in0 = gate_list[gate_list[net_list[i]] -> getInput1Pos()] -> getGateBit();
                if(gate_list[net_list[i]] -> getInput1Invered())
                    gate_list[net_list[i]] -> setGateBit(~in0);
                else
                    gate_list[net_list[i]] -> setGateBit(in0);
            }
        }
        
        for(int i=0; i<FECG_prev; ++i)
        {
            if(fec_g_list[i].size() > 1)
            {
                vector<CirGate*> fec_gates = fec_g_list[i];
                HashSet<CirGateFEC> _myHashSet(9*net_list_size);
                for(int j=0; j<fec_gates.size(); ++j)
                {
                    CirGateFEC item(fec_gates[j]);
                    if(!_myHashSet.check(item))
                    {
                        _myHashSet.insert(item);
                    }
                }
                
                fec_g_list.erase(fec_g_list.begin()+i);
                vector<vector<CirGateFEC>> table = _myHashSet.GetGrp();
                for(int k=0; k<table.size(); ++k)
                    fec_g_list.push_back(Convert2Gate(table[k]));
            }
        }
        
        sim_Count++;
        init = false;
        FECG_after = fec_g_list.size();
    }
        
    cout<<64*sim_Count<<" patterns simulated."<<endl;
    
}

vector<CirGate*>
CirMgr::Convert2Gate(vector<CirGateFEC>& a)const
{
    vector<CirGate*> result;
    for(int i=0; i<a.size(); ++i)
    {
        result.push_back(a[i].getGateFEC());
    }
    return result;
}
    
void
CirMgr::fileSim(ifstream& patternFile)
{
    string inStr;
    bool find_Z = false;
    
    vector<CirGate*> InitGrp;
    vector<string> Output;
    int netL_size = net_list.size();
    
    for(int i=0; i<netL_size; ++i)
    {
        if(gate_list[net_list[i]] -> getGateType()== AIG_GATE)
            InitGrp.push_back(gate_list[net_list[i]]);
        if(gate_list[net_list[i]] -> getGateType()== CONST_GATE)
        {
            InitGrp.push_back(gate_list[net_list[i]]);
            find_Z = true;
        }
    }
    
    if(!find_Z)
    {
        InitGrp.push_back(gate_list[gate_id_map[0]]);
        gate_list[gate_id_map[0]] -> setGateBit(bitset<64>(0));
    }
    
    fec_g_list.push_back(InitGrp);
    bool need_pad = false;
    int tail_num = 0;
    int sim_Count = 0;

    while(!need_pad)
    {
        vector<string> inputs;
        for(int i=0; i<input_number; ++i)
        {
            inputs.push_back("");
        }
        for(int i=0; i<64; ++i)
        {
            if(getline(patternFile, inStr))
            {
                if(inStr == "")
                    continue;
                
                auto f = [](unsigned char const c) { return std::isspace(c); };
                inStr.erase(std::remove_if(inStr.begin(), inStr.end(), f),inStr.end());
                
                if(inStr.length()!=input_number)
                {
                    cerr<<"Error: Pattern("<<inStr<<") length("<<inStr.length()<<") does not match the number of inputs ("<<input_number<<") in a circuit!!"<<endl;
                    cout<<64*sim_Count + tail_num<<" patterns simulated."<<endl;
                    return;
                }
                for(int i=0; i<inStr.length(); ++i)
                {
                    if(inStr[i]!= '0' && inStr[i]!= '1')
                    {
                        cerr<<"Error: Pattern("<<inStr<<") contains a non-0/1 character ('"<<inStr[i]<<"')."<<endl;
                        cout<<64*sim_Count + tail_num<<" patterns simulated."<<endl;
                        return;
                    }
                }
                Output.push_back(inStr);
                for(int i=0; i<inStr.length(); ++i)
                {
                    inputs[i]+=inStr[i];
                }
            }
            else
            {
                if(inputs[0].length()==0)
                {
                    cout<<64*sim_Count + tail_num<<" patterns simulated."<<endl;
                    return;
                }
                
                if(inputs[0].length()!=64)
                    tail_num = inputs[0].length();
                need_pad = true;
                break;
            }
        }
        
        int index = 0;
        for(int i=0; i<input_number; ++i)
        {
            if(need_pad)
            {
                for(int i=0; i<(64-tail_num); ++i)
                    inputs[index]+="0";
            }
                
            bitset<64> bin(inputs[index]);
            gate_list[input_id[input_file[i]/2]] -> setGateBit(bin);
            index++;
        }
            
        vector<bitset<64>> Output2;
        for(int i=0; i<netL_size; ++i)
        {
            if(gate_list[net_list[i]] -> getGateType() == CONST_GATE ||
               gate_list[net_list[i]] -> getGateType() == UNDEF_GATE)
                gate_list[net_list[i]] -> setGateBit(bitset<64>(0));
            
            if(gate_list[net_list[i]] -> getGateType() == AIG_GATE)
            {
                bitset<64> in0 = gate_list[gate_list[net_list[i]] -> getInput1Pos()] -> getGateBit();
                bitset<64> in1 = gate_list[gate_list[net_list[i]] -> getInput2Pos()] -> getGateBit();
                    
                if(gate_list[net_list[i]] -> getInput1Invered() &&
                    gate_list[net_list[i]] -> getInput2Invered())
                    gate_list[net_list[i]] -> setGateBit((~in0)&(~in1));
                else if(!gate_list[net_list[i]] -> getInput1Invered() &&
                        gate_list[net_list[i]] -> getInput2Invered())
                    gate_list[net_list[i]] -> setGateBit((in0)&(~in1));
                else if(gate_list[net_list[i]] -> getInput1Invered() &&
                        !gate_list[net_list[i]] -> getInput2Invered())
                    gate_list[net_list[i]] -> setGateBit((~in0)&(in1));
                else if((!gate_list[net_list[i]] -> getInput1Invered()) &&
                        (!gate_list[net_list[i]] -> getInput2Invered()))
                    gate_list[net_list[i]] -> setGateBit(in0&in1);
            }
            if(gate_list[net_list[i]] -> getGateType() == PO_GATE)
            {
                bitset<64> in0 = gate_list[gate_list[net_list[i]] -> getInput1Pos()] -> getGateBit();
                if(gate_list[net_list[i]] -> getInput1Invered())
                    gate_list[net_list[i]] -> setGateBit(~in0);
                else
                    gate_list[net_list[i]] -> setGateBit(in0);
                Output2.push_back(gate_list[net_list[i]] -> getGateBit());
            }
        }
        for(int i=0; i<Output.size(); ++i)
        {
            Output[i]+=" ";
        }
        for(int i=0; i<output_number; ++i)
        {
            bitset<64> b = Output2[i];
            string bit = b.to_string();

            for(int j = 0; j<bit.length(); ++j)
            {
                if(64*sim_Count+(j) < Output.size())
                {
                    Output[64*sim_Count+(j)]+=bit[j];
                }
                else
                    break;
            }
        }

        int FECG_prev = fec_g_list.size();
        
        for(int i=0; i<FECG_prev; ++i)
        {
            vector<CirGate*> fec_gates = fec_g_list[i];
            int gateL_size = fec_g_list[i].size();
            
            if(gateL_size > 1)
            {
                HashSet<CirGateFEC> _myHashSet(9*netL_size);
                for(int j=0; j<gateL_size; ++j)
                {
                    CirGateFEC item(fec_gates[j]);
                    _myHashSet.insert(item);
                }
                
                fec_g_list.erase(fec_g_list.begin()+i);
                vector<vector<CirGateFEC>> table = _myHashSet.GetGrp();
                for(int k=0; k<table.size(); ++k)
                    fec_g_list.push_back(Convert2Gate(table[k]));
            }
        }
        
        if(!need_pad)
            sim_Count++;
    }
    cout<<64*sim_Count + tail_num<<" patterns simulated."<<endl;
    
    if(_simLog!=0)
    {
        for(int i=0; i<Output.size(); ++i)
        {
            (*_simLog) <<Output[i]<<endl;
        }
    }
}

/*************************************************/
/*   Private member functions about Simulation   */
/*************************************************/

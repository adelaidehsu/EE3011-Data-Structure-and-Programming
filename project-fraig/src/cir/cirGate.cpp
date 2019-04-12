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
#include "cirGate.h"
#include "cirMgr.h"
#include "util.h"

using namespace std;

// TODO: Keep "CirGate::reportGate()", "CirGate::reportFanin()" and
//       "CirGate::reportFanout()" for cir cmds. Feel free to define
//       your own variables and functions.

extern CirMgr *cirMgr;

/**************************************/
/*   class CirGate member functions   */
/**************************************/
 bool BiggerFunc(CirGate* a, CirGate* b) { return (a -> getId()) < (b -> getId());}
void
CirGate::reportGate() const
{
    for(int i=0; i<80; ++i)
    {
        cout<<"=";
    }
    cout<<endl;
    
    cout<<"= ";
    int eid=0;
    if(gate_type==PI_GATE)
    {
        cout<<"PI("<<gate_id<<")";
        
        if(gate_name!="000"){
            cout<<"\"";
            for(auto&i:gate_name)
            {
                cout<<i;
            }
            cout<<"\"";
        }
        
        cout<<", line "<<line_id<<endl;
        
    }
    else if(gate_type==PO_GATE)
    {
        cout<<"PO("<<gate_id<<")";
        if(gate_name!="000"){
            cout<<"\"";
            for(auto&i:gate_name)
            {
                cout<<i;
            }
            cout<<"\"";
        }
        
        cout<<", line "<<line_id<<endl;
        
    }
    else if(gate_type==AIG_GATE)
    {
        cout<<"AIG("<<gate_id<<"), line "<<line_id<<endl;
    }
    else if(gate_type==UNDEF_GATE)
    {
        cout<<"UNDEF("<<gate_id<<"), line "<<0<<endl;
    }
    else if(gate_type==CONST_GATE)
    {
        cout<<"CONST("<<gate_id<<"), line "<<0<<endl;
    }

    cout<<"= FECS: ";
    bool find_fec = false;
    bool find_inv_fec = false;
    vector<CirGate*> FEC_grp;
    vector<CirGate*> Inv_FEC_grp;
    
    if(gate_type == AIG_GATE || gate_type == CONST_GATE)
    {
        for(int i=0; i<cirMgr -> fec_g_list.size(); ++i)
        {
            if(cirMgr -> fec_g_list[i][0] -> getGateBit() == gate_bit)
            {
                for(int j=0; j<cirMgr ->fec_g_list[i].size(); ++j)
                {
                    if(cirMgr ->fec_g_list[i][j]->getId() == gate_id)
                    {
                        vector<CirGate*> a(cirMgr -> fec_g_list[i]);
                        find_fec = true;
                        FEC_grp = a;
                        break;
                    }
                }
            }
            else if(cirMgr -> fec_g_list[i][0] -> getGateBit() == (~gate_bit))
            {
                vector<CirGate*> b(cirMgr -> fec_g_list[i]);
                find_inv_fec = true;
                Inv_FEC_grp = b;
            }
            if((find_fec && find_inv_fec) || i == cirMgr -> fec_g_list.size()-1)
                break;
        }
        vector<CirGate*> combined;
        for(int i=0; i<FEC_grp.size(); ++i)
        {
            if(FEC_grp[i] -> getId() != gate_id)
                combined.push_back(FEC_grp[i]);
        }
        
        for(int i=0; i<Inv_FEC_grp.size(); ++i)
            combined.push_back(Inv_FEC_grp[i]);
        
        sort(combined.begin(), combined.end(), BiggerFunc);
        
        for(int i=0; i<combined.size(); ++i)
        {
            if(combined[i] -> getGateBit() == gate_bit)
                cout<<combined[i] -> getId()<<" ";
            if(combined[i] -> getGateBit() == (~gate_bit))
                cout<<"!"<<combined[i] -> getId()<<" ";
        }
    }
    cout<<endl;
    
    cout<<"= Value: ";
    for(int i=0; i<gate_bit.size(); ++i)
    {
        cout<<gate_bit[i];
        if((i+1)%8 == 0 && i!=gate_bit.size()-1)
            cout<<"_";
    }
    cout<<endl;
    
    for(int i=0; i<80; ++i)
    {
        cout<<"=";
    }
    cout<<endl;
}

void
CirGate::reportFanin(int level) const
{
    assert (level >= 0);
    cirMgr->trace.clear();
    cirMgr->traverse_num=0;
    for(int i=0;i<cirMgr->gate_list.size();++i)
    {
        if(cirMgr -> gate_list[i]!=0)
        {
            if(cirMgr->gate_list[i]->getId()==gate_id)
            {
                gate_in_traverse(i,0,false,level);
                
                break;
            }
        }
    }
}
void
CirGate::gate_in_traverse(int i,int level,bool inv,int levelnum)const
{
    
    if(level<=levelnum){
        if(cirMgr->gate_list[i] -> getGateType() == UNDEF_GATE)
        {
            
            for(int j=0;j<level;++j)
            {
                cout<<"  ";
            }
            if(inv==true){cout<<"!";}
            cirMgr->gate_list[i]->printGate();
            cout<<endl;
            
        }
        else if(cirMgr->gate_list[i] -> getGateType() == PI_GATE)
        {
            
            for(int j=0;j<level;++j)
            {
                cout<<"  ";
            }
            if(inv==true){cout<<"!";}
            cirMgr->gate_list[i]->printGate();
            cout<<endl;
        }
        else if(cirMgr->gate_list[i] -> getGateType() == CONST_GATE)
        {
            
            for(int j=0;j<level;++j)
            {
                cout<<"  ";
            }
            if(inv==true){cout<<"!";}
            cout<<"CONST "<<cirMgr->gate_list[i]->getId()<<endl;
            
        }
        else if(cirMgr->gate_list[i] -> getGateType() == PO_GATE)
        {
            
            for(int j=0;j<level;++j)
            {
                cout<<"  ";
            }
            if(inv==true){cout<<"!";}
            cirMgr->gate_list[i]->printGate();
            cout<<endl;
            int input1 = cirMgr->gate_list[i] -> getInput1Id();
            bool input1inverted = cirMgr->gate_list[i] -> getInput1Invered();
            int input1id;
            for(int j=0;j<cirMgr->gate_list.size();++j)
            {
                if(cirMgr -> gate_list[j]!=0)
                {
                    if(cirMgr->gate_list[j]->getId()==input1&&(cirMgr->trace.count(j)==0))
                    {
                        input1id = j;
                    }
                }
            }
            if(input1id>=0)
            {
                cirMgr->traverse_num++;
                gate_in_traverse(input1id,level+1,input1inverted,levelnum);
            }
            
            
        }
        else{
            if(cirMgr->trace.count(i)==1&&level+1<=levelnum)
            {
                for(int j=0;j<level;++j)
                {
                    cout<<"  ";
                }
                if(inv==true){cout<<"!";}
                cirMgr->gate_list[i]->printGate();
                cout<<" (*)"<<endl;
            }
            
            else
            {
                for(int j=0;j<level;++j)
                {
                    cout<<"  ";
                }
                if(inv==true){cout<<"!";}
                cirMgr->gate_list[i]->printGate();
                cout<<endl;
                int input1 = cirMgr->gate_list[i] -> getInput1Id();
                int input2 = cirMgr->gate_list[i] -> getInput2Id();
                bool input1inverted = cirMgr->gate_list[i] -> getInput1Invered();
                bool input2inverted = cirMgr->gate_list[i] -> getInput2Invered();
                int input1id=-2;
                int input2id=-2;
                for(int j=0;j<cirMgr->gate_list.size();++j)
                {
                    if(cirMgr -> gate_list[j]!=0)
                    {
                        if(cirMgr->gate_list[j]->getId()==input1&&(cirMgr->trace.count(j)>=0))
                        {
                            input1id = j;
                            
                        }
                        else if(cirMgr->gate_list[j]->getId()==input1&&(cirMgr->trace.count(j)==1))
                        {
                            input1id=-1;
                        }
                        if(cirMgr->gate_list[j]->getId()==input2&&(cirMgr->trace.count(j)>=0))
                        {
                            input2id = j;
                            
                        }
                        else if(cirMgr->gate_list[j]->getId()==input2&&(cirMgr->trace.count(j)==1))
                        {
                            input2id=-1;
                        }
                    }
           
                }
                
                if(input1id>=0) {cirMgr->traverse_num++;gate_in_traverse(input1id,level+1,input1inverted,levelnum);}
                if(input2id>=0) {cirMgr->traverse_num++;gate_in_traverse(input2id,level+1,input2inverted,levelnum);}
                if(level+1<=levelnum){
                    cirMgr->trace[i]=0;
                }
                
            }
        }
        
    }
    
}

void
CirGate::reportFanout(int level) const
{
    assert (level >= 0);
    cirMgr->trace.clear();
    cirMgr->traverse_num=0;
    
    for(int i=0;i<cirMgr->gate_list.size();++i)
    {
        if(cirMgr -> gate_list[i]!=0)
        {
            if(cirMgr->gate_list[i]->getId()==gate_id)
            {
                gate_out_traverse(i,0,false,level);
                break;
            }
        }
    }
}
bool sortfunc(const pair<int,int> &a,
              const pair<int,int> &b)
{
    return (cirMgr->gate_list[a.first] -> getId() < cirMgr->gate_list[b.first] -> getId());
}
void
CirGate::gate_out_traverse(int i,int level,bool inv,int levelnum)const
{
    if(level<=levelnum){
        if(cirMgr->gate_list[i] -> getGateType() == UNDEF_GATE)
        {
            for(int j=0;j<level;++j)
            {
                cout<<"  ";
            }
            if(inv==true){cout<<"!";}
            cirMgr->gate_list[i]->printGate();
            cout<<endl;
            vector<pair<unsigned,bool>> vec=cirMgr -> gate_list[i] ->getOutput();
            sort(vec.begin(),vec.end(),sortfunc);
            
            
            for(auto&j:vec)
            {
                if(cirMgr -> gate_list[j.first]!=0)
                    gate_out_traverse(j.first,level+1,j.second,levelnum);
            }
            
        }
        else if(cirMgr->gate_list[i] -> getGateType() == PI_GATE)
        {
            
            for(int j=0;j<level;++j)
            {
                cout<<"  ";
            }
            if(inv==true){cout<<"!";}
            cirMgr->gate_list[i]->printGate();
            cout<<endl;
            vector<pair<unsigned,bool>> vec=cirMgr -> gate_list[i] ->getOutput();
            sort(vec.begin(),vec.end(),sortfunc);
            
            
            for(auto&j:vec)
            {
                if(cirMgr -> gate_list[j.first]!=0)
                    gate_out_traverse(j.first,level+1,j.second,levelnum);
            }
        }
        else if(cirMgr->gate_list[i] -> getGateType() == CONST_GATE)
        {
            
            for(int j=0;j<level;++j)
            {
                cout<<"  ";
            }
            if(inv==true){cout<<"!";}
            cout<<"CONST "<<cirMgr->gate_list[i]->getId()<<endl;
            vector<pair<unsigned,bool>> vec=cirMgr -> gate_list[i] ->getOutput();
            sort(vec.begin(),vec.end(),sortfunc);
            
            for(auto&j:vec)
            {
                if(cirMgr -> gate_list[j.first]!=0)
                    gate_out_traverse(j.first,level+1,j.second,levelnum);
            }
            
        }
        else if(cirMgr->gate_list[i] -> getGateType() == PO_GATE)
        {
            
            for(int j=0;j<level;++j)
            {
                cout<<"  ";
            }
            if(inv==true){cout<<"!";}
            cirMgr->gate_list[i]->printGate();
            cout<<endl;
            
        }
        else if(cirMgr->gate_list[i] -> getGateType() == AIG_GATE){
            if(cirMgr->trace.count(i)==1&&level+1<=levelnum)
            {
                for(int j=0;j<level;++j)
                {
                    cout<<"  ";
                }
                if(inv==true){cout<<"!";}
                cirMgr->gate_list[i]->printGate();
                cout<<" (*)"<<endl;
            }
            
            else
            {
                for(int j=0;j<level;++j)
                {
                    cout<<"  ";
                }
                if(inv==true){cout<<"!";}
                cirMgr->gate_list[i]->printGate();
                cout<<endl;
                bool has_o=false;
                vector<pair<unsigned,bool>> vec=cirMgr -> gate_list[i] ->getOutput();
                sort(vec.begin(),vec.end(),sortfunc);
                
                
                for(auto&j:vec)
                {
                    has_o=true;
                    if(cirMgr -> gate_list[j.first]!=0)
                        gate_out_traverse(j.first,level+1,j.second,levelnum);
                }
                if(level+1<=levelnum &&  has_o==true){
                    
                    cirMgr->trace[i]=0;
                }
                
            }
        }
        
    }
}


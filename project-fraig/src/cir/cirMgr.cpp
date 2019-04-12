/****************************************************************************
  FileName     [ cirMgr.cpp ]
  PackageName  [ cir ]
  Synopsis     [ Define cir manager functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#include <iostream>
#include <fstream>
#include <iomanip>
#include <cstdio>
#include <ctype.h>
#include <string>
#include <map>
#include <algorithm>
#include <cassert>
#include <bitset>
#include <vector>
#include <cstring>
#include "cirMgr.h"
#include "cirGate.h"
#include "util.h"

using namespace std;

// TODO: Implement memeber functions for class CirMgr

/*******************************/
/*   Global variable and enum  */
/*******************************/
CirMgr* cirMgr = 0;

enum CirParseError {
   EXTRA_SPACE,
   MISSING_SPACE,
   ILLEGAL_WSPACE,
   ILLEGAL_NUM,
   ILLEGAL_IDENTIFIER,
   ILLEGAL_SYMBOL_TYPE,
   ILLEGAL_SYMBOL_NAME,
   MISSING_NUM,
   MISSING_IDENTIFIER,
   MISSING_NEWLINE,
   MISSING_DEF,
   CANNOT_INVERTED,
   MAX_LIT_ID,
   REDEF_GATE,
   REDEF_SYMBOLIC_NAME,
   REDEF_CONST,
   NUM_TOO_SMALL,
   NUM_TOO_BIG,

   DUMMY_END
};

/**************************************/
/*   Static varaibles and functions   */
/**************************************/
static unsigned lineNo = 0;  // in printint, lineNo needs to ++
static unsigned colNo  = 0;  // in printing, colNo needs to ++
static char buf[1024];
static string errMsg;
static int errInt;
static CirGate *errGate;

static bool
parseError(CirParseError err)
{
   switch (err) {
      case EXTRA_SPACE:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Extra space character is detected!!" << endl;
         break;
      case MISSING_SPACE:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Missing space character!!" << endl;
         break;
      case ILLEGAL_WSPACE: // for non-space white space character
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Illegal white space char(" << errInt
              << ") is detected!!" << endl;
         break;
      case ILLEGAL_NUM:
         cerr << "[ERROR] Line " << lineNo+1 << ": Illegal "
              << errMsg << "!!" << endl;
         break;
      case ILLEGAL_IDENTIFIER:
         cerr << "[ERROR] Line " << lineNo+1 << ": Illegal identifier \""
              << errMsg << "\"!!" << endl;
         break;
      case ILLEGAL_SYMBOL_TYPE:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Illegal symbol type (" << errMsg << ")!!" << endl;
         break;
      case ILLEGAL_SYMBOL_NAME:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Symbolic name contains un-printable char(" << errInt
              << ")!!" << endl;
         break;
      case MISSING_NUM:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Missing " << errMsg << "!!" << endl;
         break;
      case MISSING_IDENTIFIER:
         cerr << "[ERROR] Line " << lineNo+1 << ": Missing \""
              << errMsg << "\"!!" << endl;
         break;
      case MISSING_NEWLINE:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": A new line is expected here!!" << endl;
         break;
      case MISSING_DEF:
         cerr << "[ERROR] Line " << lineNo+1 << ": Missing " << errMsg
              << " definition!!" << endl;
         break;
      case CANNOT_INVERTED:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": " << errMsg << " " << errInt << "(" << errInt/2
              << ") cannot be inverted!!" << endl;
         break;
      case MAX_LIT_ID:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Literal \"" << errInt << "\" exceeds maximum valid ID!!"
              << endl;
         break;
      case REDEF_GATE:
         cerr << "[ERROR] Line " << lineNo+1 << ": Literal \"" << errInt
              << "\" is redefined, previously defined as "
              << errGate->getTypeStr() << " in line " << errGate->getLineNo()
              << "!!" << endl;
         break;
      case REDEF_SYMBOLIC_NAME:
         cerr << "[ERROR] Line " << lineNo+1 << ": Symbolic name for \""
              << errMsg << errInt << "\" is redefined!!" << endl;
         break;
      case REDEF_CONST:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Cannot redefine const (" << errInt << ")!!" << endl;
         break;
      case NUM_TOO_SMALL:
         cerr << "[ERROR] Line " << lineNo+1 << ": " << errMsg
              << " is too small (" << errInt << ")!!" << endl;
         break;
      case NUM_TOO_BIG:
         cerr << "[ERROR] Line " << lineNo+1 << ": " << errMsg
              << " is too big (" << errInt << ")!!" << endl;
         break;
      default: break;
   }
   return false;
}

/**************************************************************/
/*   class CirMgr member functions for circuit construction   */
/**************************************************************/
bool
CirMgr::readCircuit(const string& fileName)
{
    lineNo = 0; colNo =0;
    
    string buffer; int column=0;
    ifstream file;
    file.open(fileName, ifstream::in);
    if(file.is_open()==false){
        cerr<<"Cannot open design \""<<fileName<<"\"!!"<<endl;
        return false;
    }
    getline(file, buffer);
    if(buffer.empty())
    {
        errMsg = "aag";
        parseError(MISSING_IDENTIFIER);return false;
    }
    int first_space = buffer.find(" ");
    
    column = first_space;
    
    if(first_space==0){parseError(EXTRA_SPACE); return false;}
    if(char(buffer[0])==9){
        errInt = char(buffer[0]);
        parseError(ILLEGAL_WSPACE);return false;
    }
    if(first_space==-1)
    {
        colNo = buffer.size();
        errMsg = "number of variables";
        parseError( MISSING_NUM);return false;
    }
    if((buffer[0]!='a')||(buffer[0]=='a'&&buffer[1]!='a')||(buffer[0]=='a'&&buffer[1]=='a'&&buffer[2]!='g')
       ||(first_space!=3))
    {
        errMsg.assign(buffer.begin(),buffer.begin()+first_space);
        parseError(ILLEGAL_IDENTIFIER);return false;
    }
    
    
    vector<int>space_id;
    for(int i=0;i<buffer.size();++i)
    {
        if(buffer[i]==' '){space_id.push_back(i);}
        if(char(buffer[i])==9){
            colNo = i;
            errInt = char(buffer[i]);
            parseError(ILLEGAL_WSPACE); return false;
        }
    }
    
    if(space_id.size()==1){ colNo=4;
        errMsg = "number of variables";
        parseError(MISSING_NUM);return false;}
    for(int i=0;i<space_id.size()-1;++i)
    {
        if(space_id[i+1]==space_id[i]+1)
        {
            colNo = space_id[i+1];
            parseError (EXTRA_SPACE);return false;
        }
    }
    
    string buf=buffer; int second_space;
    for(int i=first_space+1;i<buf.size();++i)
    {
        if(buf[i]==' '){second_space=i;break;}
    }
    if(!myStr2Int(buf.substr(first_space+1,second_space-first_space-1), max_variable))
    {
        errMsg = "number of AIGs(";
        errMsg.append(buf.substr(first_space+1,second_space-first_space-1));
        errMsg.append(")");
        parseError(ILLEGAL_NUM); return false;
    }
    
    buf.erase(buf.begin(),buf.begin()+first_space+(to_string(max_variable)).size()+1);
    
    first_space = buf.find(" ");
    for(int i=first_space+1;i<buf.size();++i)
    {
        if(buf[i]==' '){second_space=i;break;}
    }
    if(!myStr2Int(buf.substr(first_space+1,second_space-first_space-1), input_number))
    {
        errMsg = "number of MAXs(";
        errMsg.append(buf.substr(first_space+1,second_space-first_space-1));
        errMsg.append(")");
        parseError(ILLEGAL_NUM); return false;
    }
    
    buf.erase(buf.begin(),buf.begin()+first_space+1+(to_string(input_number)).size());
    
    first_space = buf.find(" ");
    for(int i=first_space+1;i<buf.size();++i)
    {
        if(buf[i]==' '){second_space=i;break;}
    }
    if(!myStr2Int(buf.substr(first_space+1,second_space-first_space-1), latch_number))
    {
        errMsg = "number of LATCHs(";
        errMsg.append(buf.substr(first_space+1,second_space-first_space-1));
        errMsg.append(")");
        parseError(ILLEGAL_NUM); return false;
    }
    
    if(latch_number%2!=0){
        errMsg = "latches";
        parseError(ILLEGAL_NUM); return false;
    }
    buf.erase(buf.begin(),buf.begin()+first_space+1+(to_string(latch_number)).size());
    
    first_space = buf.find(" ");
    for(int i=first_space+1;i<buf.size();++i)
    {
        if(buf[i]==' '){second_space=i;break;}
    }
    if(!myStr2Int(buf.substr(first_space+1,second_space-first_space-1), output_number))
    {
        errMsg = "number of POs(";
        errMsg.append(buf.substr(first_space+1,second_space-first_space-1));
        errMsg.append(")");
        parseError(ILLEGAL_NUM); return false;
    }
    
    buf.erase(buf.begin(),buf.begin()+first_space+1+(to_string(output_number)).size());
    second_space = -1;
    first_space = buf.find(" ");
    for(int i=first_space+1;i<buf.size();++i)
    {
        if(buf[i]==' '){second_space=i;break;}
    }
    if(second_space!=-1)
    {
        if(!myStr2Int(buf.substr(first_space+1,second_space-first_space-1), gate_number))
        {
            errMsg = "number of AIGs(";
            errMsg.append(buf.substr(first_space+1,second_space-first_space-1));
            errMsg.append(")");
            parseError(ILLEGAL_NUM); return false;
        }
    }
    else{
        
        if(!myStr2Int(buf.substr(first_space+1), gate_number))
        {
            errMsg = "number of AIGs(";
            errMsg.append(buf.substr(first_space+1));
            errMsg.append(")");
            parseError(ILLEGAL_NUM); return false;
        }
    }
    
    
    buf.erase(buf.begin(),buf.begin()+first_space+1+(to_string(gate_number)).size());
    
    
    if((!buf.empty())&&buf.size()>0){
        
        colNo = buffer.size()-buf.size();
        parseError(MISSING_NEWLINE);
        return false;
    }
    
    if(max_variable<input_number+latch_number+gate_number)
    {
        errMsg = "Number of variables";  errInt = max_variable;
        parseError(NUM_TOO_SMALL); return false;
    }
    int b;
    for(int i=0;i<input_number;++i)
    {
        
        b=buffer.size(); colNo=b;
        
        getline(file, buffer);
        lineNo++;
        if(buffer.empty()){
            errMsg = "PI";
            parseError(MISSING_DEF);
            
            return false;
        }
        
        if(buffer[0]==' '&&buffer.size()==1)
        {
            errMsg = "PI literal ID";
            parseError(MISSING_NUM);return false;
        }
        for(int j=0;j<buffer.size();++j)
        {
            if(buffer[j]==' '){
                colNo = j;
                parseError(EXTRA_SPACE); return false;
            }
            if(char(buffer[j])==9){
                colNo = j;
                errInt = char(buffer[j]);
                parseError(ILLEGAL_WSPACE); return false;
            }
        }
        colNo = 0;
        
        if(stoi(buffer.substr(0))==1 || stoi(buffer.substr(0))==0)
        {
            errInt=stoi(buffer.substr(0));
            parseError(REDEF_CONST);return false;
        }
        if(stoi(buffer.substr(0))%2!=0)
        {
            errMsg = "PI"; errInt = stoi(buffer.substr(0));
            parseError(CANNOT_INVERTED);return false;
        }
        
        unsigned input = (stoi(buffer.substr(0)))/2;
        
        if(input_id.count(input)==true)
        {
            
            errInt = input*2;
            for(int j=0;j<gate_list.size();++j)
            {
                if(gate_list[j]->getId()==input){errGate = gate_list[j]; break;}
            }
            parseError(REDEF_GATE);return false;
        }
        if(input>max_variable)
        {
            errInt = input*2;
            parseError(MAX_LIT_ID);return false;
        }
        input_file.push_back(input*2);
        
        input_id[input]=i;
        
        CirGate* cir_gate = new CirPiGate;
        cir_gate -> setGateId(input);
        cir_gate -> setGateType(PI_GATE);
        cir_gate -> setLineId(i+2);
        gate_list.push_back(cir_gate);
        
        
    }
    if(has_const==false)
    {
        CirGate* cir_constgate = new CirPiGate;
        cir_constgate -> setGateId(0);
        cir_constgate -> setGateType(CONST_GATE);
        gate_list.push_back(cir_constgate);
        gate_id_map[0]=input_number;
        has_const = true;
    }
    for(int i=0;i<output_number;++i)
    {
        bool www=false;
        b=buffer.size(); colNo=b;
        
        getline(file, buffer);
        lineNo++;
        if(buffer.empty()){
            errMsg = "PO";
            parseError(MISSING_DEF);
            
            return false;
        }
        for(int j=0;j<buffer.size();++j)
        {
            if(buffer[j]==' '){
                colNo = j;
                parseError(EXTRA_SPACE); return false;
            }
            if(char(buffer[j])==9){
                colNo = j;
                errInt = char(buffer[j]);
                parseError(ILLEGAL_WSPACE); return false;
            }
        }
        colNo = 0;
        unsigned output = stoi(buffer);
        if(output%2==0&&output>max_variable*2)
        {
            errInt = output;
            parseError(MAX_LIT_ID);return false;
        }
        else if(output%2==1&&output>max_variable*2+1)
        {
            errInt = output;
            parseError(MAX_LIT_ID);return false;
        }
        CirGate* cir_gate = new CirPoGate;
        output_file.push_back(output);
        if(output%2==0){
            output_id.push_back(max_variable+1+i); output_id_inverted.push_back(false);
            
            cir_gate -> setInput1(output/2);
            cir_gate ->setInput1Invered(false);
            
        }
        else{
            output_id.push_back(max_variable+1+i); output_id_inverted.push_back(true);
            cir_gate -> setInput1((output-1)/2);
            cir_gate ->setInput1Invered(true);
            
        }
        
        cir_gate -> setGateId(max_variable+1+i);
        cir_gate -> setGateType(PO_GATE);
        cir_gate -> setLineId(i+2+input_number);
        gate_list.push_back(cir_gate);
        
    }
    int space_index; bool has_sym=false;
    
    for(int i=0;i<gate_number;++i)
    {
        b=buffer.size(); colNo=b;
        getline(file, buffer);
        if(!file.eof())
        {
            if(file.peek()!=EOF){
                
                if(file.peek()=='\n'&&i==gate_number-1){
                    if(file.eof()||file.bad()){file.clear();}
                    else{has_sym=true;}
                }
            }
            else{file.clear();}
        }
        
        lineNo++;
        if(buffer.empty()){
            errMsg = "AIG";
            parseError(MISSING_DEF);
            
            return false;
        }
        bool has_space=false; int space_count=0;
        for(int j=0;j<buffer.size();++j)
        {
            
            if(space_count==3)
            {
                parseError(EXTRA_SPACE);return false;
            }
            if(j==0&&buffer[j]==' '){
                colNo = j;
                parseError(EXTRA_SPACE); return false;
            }
            else if(buffer[j]==' ')
            {
                colNo = j;
                has_space = true; space_count++;
            }
            else if(char(buffer[j])==9){
                colNo = j;
                errInt = char(buffer[j]);
                parseError(ILLEGAL_WSPACE); return false;
            }
            colNo = j;
        }
        if(has_space==false)
        {
            colNo+=1;
            errMsg = "space character";
            parseError(MISSING_NUM);return false;
        }
        colNo=0;
        if(stoi(buffer.substr(0))==1 || stoi(buffer.substr(0))==0)
        {
            errInt=stoi(buffer.substr(0));
            parseError(REDEF_CONST);return false;
        }
        unsigned gateid = (stoi(buffer.substr(0)))/2;
        if(gateid>max_variable)
        {
            errInt = gateid*2;
            parseError(MAX_LIT_ID);return false;
        }
        if(input_id.count(gateid)==true)
        {
            errInt = gateid*2;
            for(int j=0;j<gate_list.size();++j)
            {
                if(gate_list[j]->getId()==gateid){errGate = gate_list[j]; break;}
            }
            parseError(REDEF_GATE);return false;
        }
        if(gate_id_map.count(gateid)==true)
        {
            errInt = gateid*2;
            for(int j=0;j<gate_list.size();++j)
            {
                if(gate_list[j]->getId()==gateid){errGate = gate_list[j]; break;}
            }
            parseError(REDEF_GATE);return false;
        }
        CirGate* cir_gate = new CirAndGate;
        cir_gate -> setGateId(gateid);
        cir_gate -> setLineId(i+2+input_number+output_number);
        gate_id_map[gateid]=i+output_number+input_number+1;
        space_index = buffer.find(" ");
        int input1 = stoi(buffer.substr(space_index+1));
        colNo = space_index+1;
        if(input1%2==0){
            if(input1>2*max_variable)
            {
                errInt = input1*2;
                parseError(MAX_LIT_ID);return false;
            }
            cir_gate -> setInput1(input1/2) ;
            cir_gate ->setInput1Invered(false);
            
            
        }
        else{
            if(input1>2*max_variable+1)
            {
                errInt = input1;
                parseError(MAX_LIT_ID);return false;
            }
            cir_gate -> setInput1((input1-1)/2) ;
            cir_gate -> setInput1Invered(true);
            
            
        }
        
        buffer.erase(buffer.begin(),buffer.begin()+space_index+1);
        space_index = buffer.find(" ");
        colNo = space_index+1;
        int input2 = stoi(buffer.substr(space_index+1));
        if(input2%2==0){
            if(input2>2*max_variable)
            {
                errInt = input2*2;
                parseError(MAX_LIT_ID);return false;
            }
            cir_gate -> setInput2(input2/2) ;
            cir_gate -> setInput2Invered(false);
            
        }
        else{
            if(input2>2*max_variable+1)
            {
                errInt = input2;
                parseError(MAX_LIT_ID);return false;
            }
            cir_gate -> setInput2((input2-1)/2) ;
            cir_gate -> setInput2Invered(true);
            
            
        }
        cir_gate -> setGateType(AIG_GATE);
        gate_list.push_back(cir_gate);
    }
    
    
    
    int ki=-1;
    
    map<int,int>mi;
    map<int,int>mo;
    while(1)
    {
        getline(file, buffer);
        ki++;
        lineNo++; colNo=0;
        
        if((buffer.empty()&&ki>0&&ki<input_number+output_number)||(buffer.empty()&&has_sym==true&&ki==0))
        {
            errMsg = "";
            parseError(ILLEGAL_SYMBOL_TYPE);return false;
        }
        else if((buffer.empty()&&ki==0)||(buffer.empty()&&ki>=input_number+output_number))
        {
            break;
        }
        if(!buffer.empty())
        {
            if(buffer[0]==' ')
            {
                parseError(EXTRA_SPACE);return false;
            }
            if(char(buffer[0])==9)
            {
                errInt = char(buffer[0]);
                parseError(ILLEGAL_WSPACE); return false;
            }
        }
        
        if(buffer[0]=='c'){
            if(buffer.size()>1)
            {
                colNo =1;
                parseError(MISSING_NEWLINE); return false;
            }
            
            break;
        }
        
        else if(buffer[0]=='i')
        {
            
            space_index = buffer.find(" ");
            if(buffer[0]==' ')
            {
                colNo=0;
                parseError(EXTRA_SPACE);return false;
            }
            if(char(buffer[0])==9)
            {
                colNo=0;
                errInt = char(buffer[0]);
                parseError(ILLEGAL_WSPACE); return false;
            }
            if(buffer[1]==' ')
            {
                colNo=1;
                parseError(EXTRA_SPACE);return false;
            }
            if(char(buffer[1])==9)
            {
                colNo=1;
                errInt = char(buffer[1]);
                parseError(ILLEGAL_WSPACE); return false;
            }
            int num;
            if(space_index==-1 || space_index==buffer.size()-1)
            {
                errMsg = "symbolic name";
                parseError( MISSING_IDENTIFIER);return false;
            }
            if(!myStr2Int(buffer.substr(1,space_index-1), num))
            {
                errMsg = "symbol index(";
                errMsg.append(buffer.substr(1,space_index-1));
                errMsg.append(")");
                parseError(ILLEGAL_NUM); return false;
            }
            else{
                if(num>input_number-1)
                {
                    errMsg = "PI index"; errInt = num;
                    parseError(NUM_TOO_BIG);return false;
                }
                if(mi.count(num)==true)
                {
                    errMsg = "i"; errInt = num;
                    parseError(REDEF_SYMBOLIC_NAME); return false;
                }
            }
            
            
            
            string gatename = buffer.substr(space_index+1);
            for(int i=0;i<gatename.size();++i)
            {
                if(!isprint(gatename[i])){
                    colNo = i+space_index+1;
                    errInt = int(gatename[i]);
                    parseError(ILLEGAL_SYMBOL_NAME);return false;
                }
            }
            gate_list[num]->setGateName (gatename);
            mi[num]=1;
            
        }
        else if(buffer[0]=='o')
        {
            space_index = buffer.find(" ");
            if(buffer[1]==' ')
            {
                colNo=1;
                parseError(EXTRA_SPACE);return false;
            }
            if(char(buffer[1])==9)
            {
                colNo=1;
                errInt = char(buffer[1]);
                parseError(ILLEGAL_WSPACE); return false;
            }
            int num;
            if(space_index==-1 || space_index==buffer.size()-1)
            {
                errMsg = "symbolic name";
                parseError( MISSING_IDENTIFIER);return false;
            }
            if(!myStr2Int(buffer.substr(1,space_index-1), num))
            {
                errMsg = "symbol index(";
                errMsg.append(buffer.substr(1,space_index-1));
                errMsg.append(")");
                parseError(ILLEGAL_NUM); return false;
            }
            else{
                if(num>output_number-1)
                {
                    errMsg = "PO index"; errInt = num;
                    parseError(NUM_TOO_BIG);return false;
                }
                if(mo.count(num)==true)
                {
                    errMsg = "o"; errInt = num;
                    parseError(REDEF_SYMBOLIC_NAME); return false;
                }
            }
            
            string gatename = buffer.substr(space_index+1);
            for(int i=0;i<gatename.size();++i)
            {
                if(!isprint(gatename[i])){
                    colNo = i+space_index+1;
                    errInt = int(gatename[i]);
                    parseError(ILLEGAL_SYMBOL_NAME);return false;
                }
            }
            gate_list[num+input_number+1]->setGateName (gatename);
            mo[num]=1;
            
        }
        else{
            int ss=buffer.find(" ");
            
            if(ss==-1){errMsg = buffer;}
            else{
                errMsg = buffer.substr(0,1);
            }
            
            parseError(ILLEGAL_SYMBOL_TYPE);return false;
        }
        
        
    }
    int unid=0;
    for(int i=0;i<gate_list.size();++i)
    {
        
        if(gate_list[i]->getInput1Id()!=0&&
           gate_list[i] -> getGateType() == AIG_GATE&&
           input_id.count( gate_list[i]->getInput1Id() )==0&&
           gate_id_map.count( gate_list[i]->getInput1Id() )==0)
        {
            
            CirGate* cir_ungate = new CirUnGate;
            cir_ungate -> setGateId( gate_list[i]->getInput1Id() );
            undef_id[gate_list[i]->getInput1Id()]=0;
            cir_ungate -> setGateType(UNDEF_GATE);
            gate_list.push_back(cir_ungate);
            gate_id_map[gate_list[i]->getInput1Id()]=input_number+output_number+1+gate_number+unid;
            unid++;
        }
        if(gate_list[i]->getInput2Id()!=0&&
           gate_list[i] -> getGateType() == AIG_GATE&&
           input_id.count( gate_list[i]->getInput2Id() )==0&&
           gate_id_map.count( gate_list[i]->getInput2Id() )==0)
        {
            
            CirGate* cir_ungate = new CirUnGate;
            cir_ungate -> setGateId( gate_list[i]->getInput2Id() );
            undef_id[gate_list[i]->getInput2Id()]=0;
            cir_ungate -> setGateType(UNDEF_GATE);
            gate_list.push_back(cir_ungate);
            gate_id_map[gate_list[i]->getInput2Id()]=input_number+output_number+1+gate_number+unid;
            unid++;
        }
        if(gate_list[i] -> getGateType() == PO_GATE&&
           gate_list[i]->getInput1Id()!=0&&
           input_id.count( gate_list[i]->getInput1Id() )==0&&
           gate_id_map.count( gate_list[i]->getInput1Id() )==0)
        {
            
            CirGate* cir_ungate = new CirUnGate;
            cir_ungate -> setGateId( gate_list[i]->getInput1Id() );
            undef_id[gate_list[i]->getInput1Id()]=0;
            cir_ungate -> setGateType(UNDEF_GATE);
            gate_list.push_back(cir_ungate);
            gate_id_map[gate_list[i]->getInput1Id()]=input_number+output_number+1+gate_number+unid;
            unid++;
        }
    }
    
    for(int i=0;i<gate_list.size();++i)
    {
        if(gate_list[i] -> getGateType() == AIG_GATE)
        {
            int input1 = gate_list[i]->getInput1Id();
            int input2 = gate_list[i]->getInput2Id();
            if(input_id.count(input1)==true){
                gate_list[input_id[input1]]->setOutput( i ,gate_list[i]->getInput1Invered());
                gate_list[i]->setInput1Pos(input_id[input1]);
            }
            else if(gate_id_map.count(input1)==true)
            {
                gate_list[gate_id_map[input1]]->setOutput( i ,gate_list[i]->getInput1Invered());
                gate_list[i]->setInput1Pos(gate_id_map[input1]);
            }
            
            if(input_id.count(input2)==true){
                gate_list[input_id[input2]]->setOutput( i ,gate_list[i]->getInput2Invered());
                gate_list[i]->setInput2Pos(input_id[input2]);
            }
            else if(gate_id_map.count(input2)==true)
            {
                gate_list[gate_id_map[input2]]->setOutput( i ,gate_list[i]->getInput2Invered());
                gate_list[i]->setInput2Pos(gate_id_map[input2]);
            }
            
        }
        else if(gate_list[i] -> getGateType() == PO_GATE)
        {
            int input1 = gate_list[i]->getInput1Id();
            if(input_id.count(input1)==true){
                gate_list[input_id[input1]]->setOutput( i ,gate_list[i]->getInput1Invered());
                gate_list[i]->setInput1Pos(input_id[input1]);
            }
            else if(gate_id_map.count(input1)==true)
            {
                gate_list[gate_id_map[input1]]->setOutput( i ,gate_list[i]->getInput1Invered());
                gate_list[i]->setInput1Pos(gate_id_map[input1]);
            }
            
        }
        
    }
    for(int i=0;i<gate_list.size();++i)
    {
        if(gate_list[i]->getGateType() == PO_GATE)
        {
            
            cir_dfs(i);
        }
    }
    
    
    return true;

}

/**********************************************************/
/*   class CirMgr member functions for circuit printing   */
/**********************************************************/
/*********************
Circuit Statistics
==================
  PI          20
  PO          12
  AIG        130
------------------
  Total      162
*********************/
void
CirMgr::printSummary() const
{
    cout<<endl;
    cout<<"Circuit Statistics"<<endl;
    cout<<"=================="<<endl;
    cout<<"  PI"<<right<<setw(12)<<input_number<<endl;
    cout<<"  PO"<<right<<setw(12)<< output_number<<endl;
    cout<<"  AIG"<<right<<setw(11)<< gate_number<<endl;
    cout<<"------------------"<<endl;
    cout<<"  Total"<< setw(9) << right << input_number+output_number+gate_number<<endl;
}

void
CirMgr::printNetlist() const
{
    cout<<endl;
    for(int i=0;i<net_list.size();++i)
    {
        
        if(gate_list[net_list[i]] -> getGateType() == PI_GATE)
        {
            cout<<"["<<i<<"] "<<"PI  "<<gate_list[net_list[i]]->getId();
            
            if(gate_list[net_list[i]] -> getGateName()!="000"){
                cout<<" (";
                for(auto&j:gate_list[net_list[i]] -> getGateName())
                {
                    cout<<j;
                }
                cout<<")";
            }
            cout<<endl;
        }
        else if(gate_list[net_list[i]] -> getGateType() == CONST_GATE)
        {
            
            
            cout<<"["<<i<<"] "<<"CONST"<<gate_list[net_list[i]]->getId()<<endl;
            
        }
        else if(gate_list[net_list[i]] -> getGateType() == PO_GATE)
        {
            
            
            cout<<"["<<i<<"] "<<"PO  "<<gate_list[net_list[i]]->getId()<<" ";
            if(undef_id.count( gate_list[net_list[i]] -> getInput1Id() )==true){cout<<"*";}
            if(gate_list[net_list[i]] -> getInput1Invered()==true){cout<<"!";}
            cout<<gate_list[net_list[i]] -> getInput1Id();
            
            if(gate_list[net_list[i]] -> getGateName()!="000"){
                cout<<" (";
                for(auto&j:gate_list[net_list[i]] -> getGateName())
                {
                    cout<<j;
                }
                cout<<")";
            }
            cout<<endl;
            
        }
        else if(gate_list[net_list[i]] -> getGateType() == AIG_GATE){
            cout<<"["<<i<<"] "<<"AIG "<<gate_list[net_list[i]]->getId()<<" ";
            if(undef_id.count( gate_list[net_list[i]] -> getInput1Id() )==true){cout<<"*";}
            if(gate_list[net_list[i]] -> getInput1Invered()==true){cout<<"!";}
            cout<<gate_list[net_list[i]] -> getInput1Id()<<" ";
            if(undef_id.count( gate_list[net_list[i]] -> getInput2Id() )==true){cout<<"*";}
            if(gate_list[net_list[i]] -> getInput2Invered()==true){cout<<"!";}
            cout<<gate_list[net_list[i]] -> getInput2Id()<<endl;
        }
    }

}
void
CirMgr::cir_dfs(int i)
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
            
            cir_dfs(gate_list[i]->getInput1Pos());
            net_list.push_back(i);
        }
        else if(gate_list[i] -> getGateType() == AIG_GATE){
            
            int input1 = gate_list[i] -> getInput1Id();
            int input2 = gate_list[i] -> getInput2Id();
            
            if(gate_list[i]->getInput1Pos()>=0) {cir_dfs(gate_list[i]->getInput1Pos());}
            if(gate_list[i]->getInput2Pos()>=0) {cir_dfs(gate_list[i]->getInput2Pos());}
            
            
            net_list.push_back(i);
        }
    }
}
void
CirMgr::printPIs() const
{
    cout << "PIs of the circuit:";
    for(auto&i:input_file)
    {
        cout<<" "<<(i/2);
    }
    cout << endl;
}

void
CirMgr::printPOs() const
{
    cout << "POs of the circuit:";
    for(auto&i:output_id)
    {
        cout<<" "<<i;
    }
    cout << endl;
}

void
CirMgr::printFloatGates() const
{
    vector<unsigned>floating_id;  vector<unsigned>no_fanout_id;
    for(int i=0;i<gate_list.size();++i)
    {
        if(gate_list[i]!=0)
        {
            if(gate_list[i]->getGateType() == AIG_GATE)
            {
                if(gate_list[i]->getInput1Id()==-10 || gate_list[i]->getInput2Id()==-10)
                {
                    floating_id.push_back(gate_list[i]->getId());
                }
                else{
                    bool nn=false;
                    if(undef_id.count(gate_list[i]->getInput1Id())==true)
                    {
                        floating_id.push_back(gate_list[i]->getId());
                        nn=true;
                    }
                    if(nn==false){
                        if(undef_id.count(gate_list[i]->getInput2Id())==true)
                        {
                            floating_id.push_back(gate_list[i]->getId());
                        }
                    }
                }
                if(gate_list[i]->getOutput().empty()==true)
                {
                    no_fanout_id.push_back(gate_list[i]->getId());
                }
                
            }
            else if(gate_list[i]->getGateType() == PI_GATE)
            {
                if(gate_list[i]->getOutput().empty()==true)
                { no_fanout_id.push_back(gate_list[i]->getId());}
            }
            else if(gate_list[i]->getGateType() == PO_GATE)
            {
                if(gate_list[i]->getInput1Id()==-10||undef_id.count(gate_list[i]->getInput1Id())==true)
                { floating_id.push_back(gate_list[i]->getId());}
            }
        }
    }
    if(floating_id.empty()==false){
        sort(floating_id.begin(),floating_id.end());
        cout<<"Gates with floating fanin(s):";
        for(auto&i:floating_id)
        {
            cout<<" "<<i;
        }
        cout<<endl;
    }
    if(no_fanout_id.empty()==false){
        sort(no_fanout_id.begin(),no_fanout_id.end());
        cout<<"Gates defined but not used  :";
        for(auto&i:no_fanout_id)
        {
            cout<<" "<<i;
        }
        cout<<endl;
    }

}

bool BiggerFunc1(CirGate* a, CirGate* b) { return (a -> getId()) < (b -> getId());}
bool BiggerFunc2(const vector<CirGate*>& a, const vector<CirGate*>& b) { return (a[0] -> getId()) < (b[0] -> getId());}

void
CirMgr::printFECPairs() const
{
    bool no_fec_found = false;
    int index = 0;
    map<int, int> trace;
    map<int, int> fec;
    map<int, int> inv;
    
    if(!fec_g_list.empty())
    {
        vector<vector<CirGate*>> total = fec_g_list;
        for(int i=0; i<total.size(); ++i)
        {
            sort(total[i].begin(), total[i].end(), BiggerFunc1);
        }
        sort(total.begin(), total.end(), BiggerFunc2);
        
        while(!no_fec_found)
        {
            vector<CirGate*> Fec;
            vector<CirGate*> Inv_fec;
            vector<CirGate*> Combined;
            bitset<64> bit;
            
            for(int i=0; i<total.size(); ++i)
            {
                if(trace.count(total[i][0]->getId())==1)
                {
                    if(i!=total.size()-1)
                        continue;
                    else
                    {
                        no_fec_found = true;
                        break;
                    }
                }
                else
                {
                    vector<CirGate*> a(total[i]);
                    Fec = a;
                    for(int j=0; j<Fec.size(); ++j)
                    {
                        trace[Fec[j]->getId()] = 1;
                        fec[Fec[j]->getId()] = 1;
                    }
                    bit = Fec[0] -> getGateBit();
                    break;
                }
            }
            
            if(!no_fec_found)
            {
                for(int i=0; i<total.size(); ++i)
                {
                    if(trace.count(total[i][0] -> getId()) == 1)
                        continue;
                    else if(total[i][0] -> getGateBit() == (~bit) &&
                            trace.count(total[i][0] -> getId()) == 0)
                    {
                        vector<CirGate*> b(total[i]);
                        Inv_fec = b;
                        for(int j=0; j<Inv_fec.size(); ++j)
                        {
                            trace[Inv_fec[j]->getId()] = 1;
                            inv[Inv_fec[j]->getId()] = 1;
                        }
                        break;
                    }
                }
                
                for(int i=0; i<Fec.size(); ++i)
                {
                    Combined.push_back(Fec[i]);
                }
                
                if(!Inv_fec.empty())
                {
                    for(int i=0; i<Inv_fec.size(); ++i)
                    {
                        Combined.push_back(Inv_fec[i]);
                    }
                }

                if(Combined.size()>1)
                {
                    sort(Combined.begin(), Combined.end(), BiggerFunc1);
                    
                    cout<<"["<<index<<"] ";
                    for(int i=0; i<Combined.size(); ++i)
                    {
                        if(fec.count(Combined[i] -> getId()) == 1)
                            cout<<Combined[i] -> getId()<<" ";
                        if(inv.count(Combined[i] -> getId()) == 1)
                            cout<<"!"<<Combined[i] -> getId()<<" ";
                    }
                    cout<<endl;
                    index++;
                }
            }
        }
    }
}
void
CirMgr::writeGate(ostream& outfile, CirGate *g) const
{
    int aig=0;
    int in1=0; int in2=0;
    vector<string> input_name;
    
        
    outfile<<"aag "<<g->getId()<<" "<<2<<" "<<0<<" "<<1<<" "<<1<<endl;
    
    if(g -> getInput1Id() != -10 && g-> getInput1Invered())
    {
        outfile<< g->getInput1Id()*2+1<<endl;
        in1 = g->getInput1Id()*2+1;
        if(gate_list[g -> getInput1Pos()]->getGateName() != "000")
            input_name.push_back(gate_list[g -> getInput1Pos()]->getGateName());
    }
    else if(g -> getInput1Id() != -10 && (!g-> getInput1Invered()))
    {
        outfile<< g->getInput1Id()*2<<endl;
        in1 =  g->getInput1Id()*2;
        if(gate_list[g -> getInput1Pos()]->getGateName() != "000")
            input_name.push_back(gate_list[g -> getInput1Pos()]->getGateName());
    }
    else if(g -> getInput2Id() != -10 && (g-> getInput2Invered()))
    {
        outfile<< g->getInput2Id()*2+1<<endl;
        in2 = g->getInput2Id()*2+1;
        if(gate_list[g -> getInput2Pos()]->getGateName() != "000")
            input_name.push_back(gate_list[g -> getInput2Pos()]->getGateName());
    }
    else if(g -> getInput2Id() != -10 && (!g-> getInput2Invered()))
    {
        outfile<< g->getInput2Id()*2<<endl;
        in2 =  g->getInput2Id()*2;
        if(gate_list[g -> getInput2Pos()]->getGateName() != "000")
            input_name.push_back(gate_list[g -> getInput2Pos()]->getGateName());
    }
    
        if(in1!=0 || in2!=0)
        {
            if(in1 > 0 && in2 > 0)
                outfile<< g->getId()*2 <<" "<<in1<<" "<<in2<<endl;
            else if(in1 > 0 && in2 == 0)
                outfile<< g->getId()*2 <<" "<<in1<<endl;
        }
        
        for(int i=0; i<input_name.size(); ++i)
        {
            outfile<< "i"<<i<<" "<<input_name[i]<<endl;
        }
        
        outfile<< "o"<<0<<" "<<g->getId()<<endl<<"c"<<endl;

}
void
CirMgr::writeAag(ostream& outfile) const
{
    vector<int>new_output_id;
    vector<int>new_gate_id;
    vector<pair<int,int>>new_gate_io;
    map<int,string>inputsymbol; map<int,string>outputsymbol;
    int sym_i=0; int sym_o=0;
    for(int i=0;i<gate_list.size();++i)
    {
        if(gate_list[i]!=0)
        {
            if(gate_list[i] -> getGateType() == PI_GATE)
            {
                
                if(gate_list[i] -> getGateName()!="000"){
                    
                    inputsymbol[sym_i] =gate_list[i]-> getGateName();
                    sym_i++;
                }
            }
            else if(gate_list[i] -> getGateType() == PO_GATE)
            {
                
                if(gate_list[i] -> getGateName()!="000"){
                    if(gate_list[i] -> getInput1Invered()==true)
                    {
                        outputsymbol[gate_list[i] -> getId()] = gate_list[i]-> getGateName();
                    }
                    else{
                        outputsymbol[gate_list[i] -> getId()] = gate_list[i]-> getGateName();
                    }
                    
                }
            }
        }
    }
    for(int i=0;i<net_list.size();++i)
    {
        if(gate_list[net_list[i]] -> getGateType() == AIG_GATE){
            new_gate_id.push_back(2*(gate_list[net_list[i]]->getId()));
            int i1=0; int i2=0;
            if(gate_list[net_list[i]] -> getInput1Invered()==true)
            {
                i1=2*(gate_list[net_list[i]] -> getInput1Id())+1;
            }
            else{
                i1=2*(gate_list[net_list[i]] -> getInput1Id());
            }
            if(gate_list[net_list[i]] -> getInput2Invered()==true)
            {
                i2=2*(gate_list[net_list[i]] -> getInput2Id())+1;
            }
            else{
                i2=2*(gate_list[net_list[i]] -> getInput2Id());
            }
            pair<int,int>pp;
            pp = make_pair(i1,i2);
            new_gate_io.push_back(pp);
        }
    }
    outfile<<"aag "<<max_variable<<" "<<input_id.size()<<" "<<0<<" "<<output_id.size()<<" "<<new_gate_id.size()<<endl;
    for(auto&i:input_file)
    {
        outfile<<i<<endl;
    }
    
    for(int i=0;i<output_file.size();++i)
    {
        outfile<<output_file[i]<<endl;
    }
    for(int i=0;i<new_gate_id.size();++i)
    {
        outfile<<new_gate_id[i]<<" "<<new_gate_io[i].first<<" "<<new_gate_io[i].second<<endl;
    }
    int n1=0; int n2=0;
    for(auto&i:inputsymbol)
    {
        outfile<<"i"<<n1<<" ";
        for(auto&j:i.second)
        {
            outfile<<j;
        }
        outfile<<endl;
        n1++;
    }
    for(auto&i:outputsymbol)
    {
        outfile<<"o"<<n2<<" ";
        for(auto&j:i.second)
        {
            outfile<<j;
        }
        outfile<<endl;
        n2++;
    }

}



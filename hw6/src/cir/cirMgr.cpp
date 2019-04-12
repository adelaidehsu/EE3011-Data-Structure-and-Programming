/****************************************************************************
  FileName     [ cirMgr.cpp ]
  PackageName  [ cir ]
  Synopsis     [ Define cir manager functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#include <iostream>
#include <iomanip>
#include <cstdio>
#include <ctype.h>
#include <cassert>
#include <cstring>
#include <sstream>
#include <vector>
#include <string>
#include <cmath>
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
              << ": Cannot redefine constant (" << errInt << ")!!" << endl;
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
int
CirMgr::str2ID(const string& content)
{
    int num;
    stringstream ss(content);
    ss>>num;
    //0's inverted case strored as -INT_MAX
    if(num==1)
        return -INT_MAX;
    else if(num%2!=0)
        return (-num/2);
    return num/2;
    ss.str("");
    ss.clear();
}

vector<int>
CirMgr::str2IDv(const string& content)
{
    int num;
    vector<int> vect;
    stringstream ss(content);
    while(ss>>num)
    {
        vect.push_back(num);
        if(ss.peek() == ' ')
            ss.ignore();
    }
    ss.str("");
    ss.clear();
    //0's inverted case strored as -INT_MAX
    for(int i=0; i<vect.size(); i++)
    {
        if(vect[i]==1)
            vect[i] = -INT_MAX;
        else if(vect[i]%2!=0)
            vect[i] = (-vect[i]/2);
        else
            vect[i]/=2;
    }
    return vect;
}

CirGate*
CirMgr::decideG(int num)
{
    CirGate* g;
    if(num==0 || num==-INT_MAX)
        g = new Constzero;
    else
    {
        g = getGate(abs(num));
        if(g==0)
        {
            g = new Undef;
            g->_Gid = abs(num);
        }
    }
    return g;
}


CirGate*
CirMgr::findmin(CirGate* g)
{
    while(g->_faninlist.size()!=0)
    {
        _Trace.push_back(g);
        g = g->_faninlist[0];
    }
    return g;
}

void
CirMgr::traverse(CirGate* _node)
{
    //_node = min
    if(_node->visited == false)
    {
        _DFSGlist.push_back(_node);
        _node->visited = true;
    }
    
    while(_node->_fanoutlist.size()!=0)
    {
        CirGate* temprt = _Trace.back();
        if(_node == temprt->_faninlist[1])
        {
            _node = temprt;
            _Trace.pop_back();
            if(_node->visited == false)
            {
                _DFSGlist.push_back(_node);
                _node->visited = true;
            }
        }
        else if(_node == temprt->_faninlist[0] && !temprt->_fanoutlist.empty())
        {
            _node = findmin(temprt->_faninlist[1]);
            if(_node->visited == false)
            {
                _DFSGlist.push_back(_node);
                _node->visited=true;
            }
        }
        else
        {
            _DFSGlist.push_back(temprt);
            temprt->visited = true;
            _Trace.clear();
            bool findzero = false;
            for(int i=0; i<_DFSGlist.size(); i++)
            {
                if(!findzero)
                {
                    if(_DFSGlist[i]->getTypeStr() == "CONST")
                        findzero = true;
                }
                else
                {
                    if(_DFSGlist[i]->getTypeStr() == "CONST")
                        _DFSGlist.erase(_DFSGlist.begin()+i);
                }
            }
            break;
        }
    }
}

void
CirMgr::setDFS()
{
    for(int i=0; i<_POlist.size(); i++)
    {
        CirGate* seed = findmin(_POlist[i]);
        traverse(seed);
    }
    for(int i=0; i<_DFSGlist.size(); i++)
    {
        _DFSGlist[i]->visited=false;
    }
}

bool
CirMgr::readCircuit(const string& fileName)
{
    ifstream fin(fileName);
    vector<string> hlist;
    vector<int> collist;
    int m, i, l, o, a, row, col;
    
    string instr;
    string delimiter = " ";
    string token;
    
    size_t start = 0;
    size_t end = 0;
    size_t len = 0;
    row = 1;
    getline(fin, instr);
    do{ end = instr.find(delimiter,start);
        len = end - start;
        token = instr.substr(start, len);
        hlist.push_back(token);
        collist.push_back(start+1);
        start += len + delimiter.length();
    }while ( end != std::string::npos );
    
    //cout<< hlist[0]<<"- aag ("<<row<<", "<<collist[0]<<")"<<endl;
    //cout<< hlist[1]<<"- M ("<<row<<", "<<collist[1]<<")"<<endl;
    //cout<< hlist[2]<<"- I ("<<row<<", "<<collist[2]<<")"<<endl;
    //cout<< hlist[3]<<"- L ("<<row<<", "<<collist[3]<<")"<<endl;
    //cout<< hlist[4]<<"- O ("<<row<<", "<<collist[4]<<")"<<endl;
    //cout<< hlist[5]<<"- A ("<<row<<", "<<collist[5]<<")"<<endl;
    
    stringstream ss;
    //L
    ss<<hlist[3];
    ss>>_L;
    ss.str("");
    ss.clear();
    //PI
    ss<<hlist[2];
    ss>>_I;
    ss.str("");
    ss.clear();
    for(int k=0; k<_I; k++)
    {
        CirGate* g = new PI;
        getline(fin, instr);
        row++;
        g->_Gid = str2ID(instr);
        g->_lineNum = row;
        g->_colNum = 1;
        _PIlist.push_back(g);
        //cout<<instr<<" - PI "<<str2ID(instr)<<"("<<row<<", "<<1<<")"<<endl;
    }
    //PO
    ss<<hlist[4];
    ss>>_O;
    ss.str("");
    ss.clear();
    ss<<hlist[1];
    ss>>_M;
    ss.str("");
    ss.clear();
    int max = _M;
    for(int k=0; k<_O; k++)
    {
        CirGate* g = new PO;
        getline(fin, instr);
        row++;
        g->_Gid = ++max;
        g->_lineNum = row;
        g->_colNum = 1;
        g->_tmpfanin1 = str2ID(instr);
        _POlist.push_back(g);
        //cout<<instr<<" - PO "<<str2ID(instr)<<"("<<row<<", "<<1<<")"<<endl; //PO connected to
    }
    //aig
    ss<<hlist[5];
    ss>>_A;
    ss.str("");
    ss.clear();
    for(int k=0; k<_A; k++)
    {
        CirGate* g = new AIG;
        getline(fin, instr);
        row++;
        vector<int> param = str2IDv(instr);
        g->_Gid = param[0];
        g->_tmpfanin1 = param[1];
        g->_tmpfanin2 = param[2];
        g->_lineNum = row;
        g->_colNum = 1;
        _OAIGlist.push_back(g);
        //cout<<instr<<" - aig "<<param[0]<<param[1]<<param[2]<<"("<<row<<", "<<1<<")"<<endl;
    }
    //symbol
    while(getline(fin, instr) && instr!="c")
    {
        string sym, def;
        size_t start = 0;
        if(instr[0] == 'i')
        {
            row++;
            start = instr.find(delimiter,0);
            def = instr.substr(0,start);
            sym = instr.substr(start+1,instr.length());
            string newh = def.substr(1,def.length());
            stringstream bb(newh);
            int index;
            bb>>index;
            bb.str("");
            bb.clear();
            _PIlist[index]->_sym = sym;
            //cout<<def<<"("<<row<<", "<<1<<")"<<endl;
            //cout<<sym<<"("<<row<<", "<<start+2<<")"<<endl;
        }
        else if(instr[0] == 'o')
        {
            row++;
            start = instr.find(delimiter,0);
            def = instr.substr(0,start);
            sym = instr.substr(start+1,instr.length());
            string newh = def.substr(1,def.length());
            stringstream bb(newh);
            int index;
            bb>>index;
            bb.str("");
            bb.clear();
            _POlist[index]->_sym = sym;
            //cout<<def<<"("<<row<<", "<<1<<")"<<endl;
            //cout<<sym<<"("<<row<<", "<<start+2<<")"<<endl;
        }
    }
    //connect circuit (PO-> tmpfanin1, AIG->tmpfanin1/2)
    for(int i=0; i<_OAIGlist.size(); i++)
    {
        CirGate* child1;
        child1 = decideG(_OAIGlist[i]->_tmpfanin1);
        _OAIGlist[i]->_faninlist.push_back(child1);
        child1->_fanoutlist.push_back(_OAIGlist[i]);
        
        CirGate* child2;
        child2 = decideG(_OAIGlist[i]->_tmpfanin2);
        _OAIGlist[i]->_faninlist.push_back(child2);
        child2->_fanoutlist.push_back(_OAIGlist[i]);
    }
    
    for(int i=0; i<_POlist.size(); i++)
    {
        CirGate* child1;
        child1 = decideG(_POlist[i]->_tmpfanin1);
        _POlist[i]->_faninlist.push_back(child1);
        child1->_fanoutlist.push_back(_POlist[i]);
    }
    
    setDFS();
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
    cout<<"Circuit Statistics"<<endl;
    cout<<"=================="<<endl;
    cout<<"  PI           "<<_PIlist.size()<<endl;
    cout<<"  PO           "<<_POlist.size()<<endl;
    cout<<"  AIG          "<<_OAIGlist.size()<<endl;
    cout<<"------------------"<<endl;
    cout<<"  Total        "<<_PIlist.size()+_POlist.size()+_OAIGlist.size()<<endl;
}

void
CirMgr::printNetlist() const
{
    int index = 0;
    for(int i=0; i<_DFSGlist.size(); i++)
    {
        if(_DFSGlist[i]->getTypeStr() == "UNDEF")
            continue;
        else
        {
            cout<<"["<<index<<"] ";
            _DFSGlist[i]->printGate();
            index++;
        }
    }
}

void
CirMgr::printPIs() const
{
    cout << "PIs of the circuit: ";
    for(int i=0; i<_PIlist.size(); i++)
        cout<<_PIlist[i]->_Gid<<' ';
    cout << endl;
}

void
CirMgr::printPOs() const
{
   cout << "POs of the circuit: ";
    for(int i=0; i<_POlist.size(); i++)
        cout<<_POlist[i]->_Gid<<' ';
   cout << endl;
}

void
CirMgr::printFloatGates() const
{
    vector<CirGate*> fl;
    vector<CirGate*> unused;
    
    for(int i=0; i<_OAIGlist.size(); i++)
    {
        for(int j=0; j<_OAIGlist[i]->_faninlist.size(); j++)
        {
            if(_OAIGlist[i]->_faninlist[j]->getTypeStr() == "UNDEF")
            {
                fl.push_back(_OAIGlist[i]);
                break;
            }
                
        }
    }
    for(int i=0; i<_POlist.size(); i++)
    {
        for(int j=0; j<_POlist[i]->_faninlist.size(); j++)
        {
            if(_POlist[i]->_faninlist[j]->getTypeStr() == "UNDEF")
            {
                fl.push_back(_POlist[i]);
                break;
            }
        }
    }
    
    for(int i=0; i<_OAIGlist.size(); i++)
    {
        if(_OAIGlist[i]->_fanoutlist.size() == 0)
            unused.push_back(_OAIGlist[i]);
    }
    for(int i=0; i<_PIlist.size(); i++)
    {
        if(_PIlist[i]->_fanoutlist.size() == 0)
            unused.push_back(_PIlist[i]);
    }
    
    if(fl.size()!=0)
    {
        cout << "Gates with floating fanin(s): ";
        for(int i=0; i<fl.size(); i++)
            cout<<fl[i]->_Gid<<' ';
        cout << endl;
    }
 
    if(unused.size()!=0)
    {
        cout << "Gates defined but not used : ";
        for(int i=0; i<unused.size(); i++)
            cout<<unused[i]->_Gid<<' ';
        cout << endl;
    }
}
int
CirMgr::aagFormat(int &result)const
{
    if(result<0)
    {
        if(abs(result)==INT_MAX)
            return 2*0+1;
        else
            return 2*abs(result)+1;
    }
    else
        return 2*result;
}

void
CirMgr::writeAag(ostream& outfile) const
{
    int index1 = 0;
    int index2 = 0;
    outfile<<"aag "<<_M<<" "<<_I<<" "<<_L<<" "<<_O<<" "<<_A<<endl;
    for(int i=0; i<_PIlist.size(); i++)
        outfile<< 2*(_PIlist[i]->_Gid)<<endl;
    for(int i=0; i<_POlist.size(); i++)
    {
        int result = _POlist[i]->_tmpfanin1;
        outfile<<aagFormat(result)<<endl;
    }
    for(int i=0; i<_DFSGlist.size(); i++)
    {
        if(_DFSGlist[i]->getTypeStr()=="AIG")
        {
            int id = _DFSGlist[i]->_Gid;
            int ch1 = _DFSGlist[i]->_tmpfanin1;
            int ch2 = _DFSGlist[i]->_tmpfanin2;
            outfile<<aagFormat(id)<<" "<<aagFormat(ch1)<<" "<<aagFormat(ch2)<<endl;
        }
    }
    for(int i=0; i<_PIlist.size(); i++)
    {
        if(_PIlist[i]->_sym!="")
        {
            outfile<<"i"<<index1<<" "<<_PIlist[i]->_sym<<endl;
            index1++;
        }
    }
    for(int i=0; i<_POlist.size(); i++)
    {
        if(_POlist[i]->_sym!="")
        {
            outfile<<"o"<<index2<<" "<<_POlist[i]->_sym<<endl;
            index2++;
        }
    }
    outfile<<"c"<<endl;
}

/****************************************************************************
  FileName     [ memCmd.cpp ]
  PackageName  [ mem ]
  Synopsis     [ Define memory test commands ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2007-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/
#include <iostream>
#include <iomanip>
#include "memCmd.h"
#include "memTest.h"
#include "cmdParser.h"
#include "util.h"

using namespace std;

extern MemTest mtest;  // defined in memTest.cpp

bool
initMemCmd()
{
   if (!(cmdMgr->regCmd("MTReset", 3, new MTResetCmd) &&
         cmdMgr->regCmd("MTNew", 3, new MTNewCmd) &&
         cmdMgr->regCmd("MTDelete", 3, new MTDeleteCmd) &&
         cmdMgr->regCmd("MTPrint", 3, new MTPrintCmd)
      )) {
      cerr << "Registering \"mem\" commands fails... exiting" << endl;
      return false;
   }
   return true;
}


//----------------------------------------------------------------------
//    MTReset [(size_t blockSize)]
//----------------------------------------------------------------------
CmdExecStatus
MTResetCmd::exec(const string& option)
{
   // check option
   string token;
   if (!CmdExec::lexSingleOption(option, token))
      return CMD_EXEC_ERROR;
   if (token.size()) {
      int b;
      if (!myStr2Int(token, b) || b < int(toSizeT(sizeof(MemTestObj)))) {
         cerr << "Illegal block size (" << token << ")!!" << endl;
         return CmdExec::errorOption(CMD_OPT_ILLEGAL, token);
      }
      #ifdef MEM_MGR_H
      mtest.reset(toSizeT(b));
      #else
      mtest.reset();
      #endif // MEM_MGR_H
   }
   else
      mtest.reset();
   return CMD_EXEC_DONE;
}

void
MTResetCmd::usage(ostream& os) const
{  
   os << "Usage: MTReset [(size_t blockSize)]" << endl;
}

void
MTResetCmd::help() const
{  
   cout << setw(15) << left << "MTReset: " 
        << "(memory test) reset memory manager" << endl;
}


//----------------------------------------------------------------------
//    MTNew <(size_t numObjects)> [-Array (size_t arraySize)]
//----------------------------------------------------------------------
CmdExecStatus
MTNewCmd::exec(const string& option)
{
    // TODO
    int num, size, firstArrp;
    bool doArray = false;
    vector<string> options;
    if (!CmdExec::lexOptions(option, options))
        return CMD_EXEC_ERROR;
    if(options.size()==0)
    {
        cerr<<"Error: MIssing option!!"<<endl;
        return CMD_EXEC_ERROR;
    }
    if(options.size()>3)
    {
        cerr<<"Extra option!!"<<" ("<<options[3]<<")"<<endl;
        return CMD_EXEC_ERROR;
    }
    
    for(int i=0; i<options.size(); i++)
    {
        if(myStrNCmp("-Array", options[i], 2) == 0)
        {
            if(!doArray)
                firstArrp = i;
            doArray = true;
        }
    }
    if(options.size()==1 && !doArray)
    {
        if(!myStr2Int(options[0], num)||num<=0)
        {
            cerr<<"Error: Illegal option!!"<<" ("<<options[0]<<")"<<endl;
            return CMD_EXEC_ERROR;
        }
        else
        {
            try {
                mtest.newObjs(num);
                return CMD_EXEC_DONE;
            } catch (std::bad_alloc) {
                return CMD_EXEC_ERROR;
            }
        }
    }
    else if(options.size()==1&&doArray)
    {
        cerr<<"Error: MIssing option after"<<" ("<<options[0]<<")!!"<<endl;
        return CMD_EXEC_ERROR;
    }
    else if(options.size()==2&&!doArray)
    {
        cerr<<"Error: Extra option!!"<<" ("<<options[1]<<")"<<endl;
        return CMD_EXEC_ERROR;
    }
    else if(options.size()==2&&doArray)
    {
        if(firstArrp == 1)
        {
            cerr<<"Error: Missing option after"<<" ("<<options[firstArrp]<<")!!"<<endl;
            return CMD_EXEC_ERROR;
        }
        else
        {
            if(!myStr2Int(options[1], size) || size<=0)
            {
                cerr<<"Error: Illegal option!!"<<" ("<<options[1]<<")"<<endl;
                return CMD_EXEC_ERROR;
            }
            else
            {
                cerr<<"Error: Missing option!!"<<endl;
                return CMD_EXEC_ERROR;
            }
        }
    }
    else if(options.size()==3&&!doArray)
    {
        if(!myStr2Int(options[0], num)||num<=0)
        {
            cerr<<"Error: Illegal option!!"<<" ("<<options[0]<<")"<<endl;
            return CMD_EXEC_ERROR;
        }
        else
        {
            cerr<<"Error: Extra option!!"<<" ("<<options[1]<<")"<<endl;
            return CMD_EXEC_ERROR;
        }
    }
    else if(options.size()==3&&doArray)
    {
        if(firstArrp == 2)
        {
            if(!myStr2Int(options[0], num)||num<=0)
            {
                cerr<<"Error: Illegal option!!"<<" ("<<options[0]<<")"<<endl;
                return CMD_EXEC_ERROR;
            }
            else
            {
                cerr<<"Error: Extra option!!"<<" ("<<options[1]<<")"<<endl;
                return CMD_EXEC_ERROR;
            }
        }
        else if(!myStr2Int(options[firstArrp+1], size) || size<=0)
        {
            cerr<<"Error: Illegal option!!"<<" ("<<options[firstArrp+1]<<")"<<endl;
            return CMD_EXEC_ERROR;
        }
        else if(firstArrp-1==0)
        {
            if(!myStr2Int(options[0], num)||num<=0)
            {
                cerr<<"Error: Illegal option!!"<<" ("<<options[0]<<")"<<endl;
                return CMD_EXEC_ERROR;
            }
            else
            {
                try {
                    mtest.newArrs(num,size);
                    return CMD_EXEC_DONE;
                } catch (std::bad_alloc) {
                    return CMD_EXEC_ERROR;
                }
            }
        }
        else if(firstArrp-1<0)
        {
            if(!myStr2Int(options[2], num)||num<=0)
            {
                cerr<<"Error: Illegal option!!"<<" ("<<options[2]<<")"<<endl;
                return CMD_EXEC_ERROR;
            }
            else
            {
                try {
                    mtest.newArrs(num,size);
                    return CMD_EXEC_DONE;
                } catch (std::bad_alloc) {
                    return CMD_EXEC_ERROR;
                }
            }
        }
    }
}

void
MTNewCmd::usage(ostream& os) const
{  
   os << "Usage: MTNew <(size_t numObjects)> [-Array (size_t arraySize)]\n";
}

void
MTNewCmd::help() const
{  
   cout << setw(15) << left << "MTNew: " 
        << "(memory test) new objects" << endl;
}


//----------------------------------------------------------------------
//    MTDelete <-Index (size_t objId) | -Random (size_t numRandId)> [-Array]
//----------------------------------------------------------------------
CmdExecStatus
MTDeleteCmd::exec(const string& option)
{
    // TODO
    bool doIdx = false;
    bool doRan = false;
    bool doArray = false;
    int objId, ranNum, firstIndexp, firstRanp, firstArrp;
    vector<string> options;
    if (!CmdExec::lexOptions(option, options))
        return CMD_EXEC_ERROR;
    if(options.size()==0)
    {
        cerr<<"Error: Missing option!!"<<endl;
        return CMD_EXEC_ERROR;
    }
    if(options.size()>3)
    {
        cerr<<"Error: Extra option"<<" ("<<options[3]<<")"<<endl;
        return CMD_EXEC_ERROR;
    }
    for(int i=0; i<options.size(); i++)
    {
        if(myStrNCmp("-Index", options[i], 2) == 0)
        {
            if(!doIdx)
                firstIndexp = i;
            doIdx = true;
        }
        
        if(myStrNCmp("-Random", options[i], 2) == 0)
        {
            if(!doRan)
                firstRanp = i;
            doRan = true;
        }
        if(myStrNCmp("-Array", options[i], 2) == 0)
        {
            if(!doArray)
                firstArrp = i;
            doArray = true;
        }
    }

    if(options.size()==1)
    {
        if(myStrNCmp("-Index", options[0], 2) == 0 || myStrNCmp("-Index", options[0], 2) == 0)
        {
            cerr<<"Error: Missing option after"<<" ("<<options[0]<<")"<<endl;
            return CMD_EXEC_ERROR;
        }
        else if(myStrNCmp("-Array", options[0], 2) == 0)
        {
            cerr<<"Error: Missing option!!"<<endl;
            return CMD_EXEC_ERROR;
        }
        else
        {
            cerr<<"Error: Illegal option!!"<<" ("<<options[0]<<")"<<endl;
            return CMD_EXEC_ERROR;
        }
    }
    else if(options.size() == 2&&!doArray)
    {
        if(doIdx)
        {
            if(firstIndexp == 1)
            {
                cerr<<"Error: Illegal option!!"<<" ("<<options[0]<<")"<<endl;
                return CMD_EXEC_ERROR;
                
            }
            else if(!myStr2Int(options[firstIndexp+1], objId) || objId<0)
            {
                cerr<<"Error: Illegal option!!"<<" ("<<options[firstIndexp+1]<<")"<<endl;
                return CMD_EXEC_ERROR;
            }
            else if(objId >= mtest.getObjListSize())
            {
                cerr<<"Error: Size of object list"<<" ("<<mtest.getObjListSize()<<") "<<"is <= "<<objId<<"!!"<<endl;
                return CMD_EXEC_ERROR;
            }
            else
            {
                mtest.deleteObj(objId);
                return CMD_EXEC_DONE;
            }
        }
        else if(doRan)
        {
            if(firstRanp == 1)
            {
                cerr<<"Error: Illegal option!!"<<" ("<<options[0]<<")"<<endl;
                return CMD_EXEC_ERROR;
                
            }
            else if(!myStr2Int(options[firstRanp+1], ranNum) || ranNum<0)
            {
                cerr<<"Error: Illegal option!!"<<" ("<<options[firstRanp+1]<<")"<<endl;
                return CMD_EXEC_ERROR;
            }
            else if(mtest.getObjListSize()==0)
            {
                cerr<<"Error: Size of object list"<<" ("<<mtest.getObjListSize()<<")!!"<<endl;
                return CMD_EXEC_ERROR;
            }
            else
            {
                for(int i=0; i<ranNum; i++)
                {
                    int ranIdx = rnGen(mtest.getObjListSize());
                    mtest.deleteObj(ranIdx);
                }
                return CMD_EXEC_DONE;
            }
        }
        else
        {
            cerr<<"Error: Illegal option"<<"("<<options[0]<<")"<<endl;
            return CMD_EXEC_ERROR;
        }
    }
    else if(options.size()==2&&doArray)
    {
        if(doRan)
        {
            cerr<<"Error: Missing option after"<<" ("<<options[firstRanp]<<")"<<endl;
            return CMD_EXEC_ERROR;
        }
        else if(doIdx)
        {
            cerr<<"Error: Missing option after"<<" ("<<options[firstIndexp]<<")"<<endl;
            return CMD_EXEC_ERROR;
        }
        else if(firstArrp == 0)
        {
            cerr<<"Error: Illegal option!!"<<" ("<<options[1]<<")"<<endl;
            return CMD_EXEC_ERROR;
        }
        else
        {
            cerr<<"Error: Illegal option!!"<<" ("<<options[0]<<")"<<endl;
            return CMD_EXEC_ERROR;
        }
    }
    else if(options.size()==3)
    {
        if(doIdx)
        {
            if(firstIndexp == 2)
            {
                cerr<<"Error: Illegal option!!"<<" ("<<options[0]<<")"<<endl;
                return CMD_EXEC_ERROR;
            }
            else if(!myStr2Int(options[firstIndexp+1], objId) || objId<0 || objId >= mtest.getArrListSize())
            {
                cerr<<"Error: Illegal option!!"<<" ("<<options[firstIndexp+1]<<")"<<endl;
                return CMD_EXEC_ERROR;
            }
            else if(firstIndexp==0 && !doArray)
            {
                cerr<<"Error: Illegal option!!"<<" ("<<options[2]<<")"<<endl;
                return CMD_EXEC_ERROR;
            }
            else if(firstIndexp==1 && !doArray)
            {
                cerr<<"Error: Illegal option!!"<<" ("<<options[0]<<")"<<endl;
                return CMD_EXEC_ERROR;
            }
            else if(doArray)
            {
                mtest.deleteArr(objId);
                return CMD_EXEC_DONE;
            }
        }
        else if(doRan)
        {
            if(firstRanp == 2)
            {
                cerr<<"Error: Illegal option!!"<<" ("<<options[0]<<")"<<endl;
                return CMD_EXEC_ERROR;
            }
            else if(!myStr2Int(options[firstRanp+1], ranNum) || ranNum<0)
            {
                cerr<<"Error: Illegal option!!"<<" ("<<options[firstRanp+1]<<")"<<endl;
                return CMD_EXEC_ERROR;
            }
            else if(mtest.getArrListSize()==0)
            {
                cerr<<"Error: Size of array list"<<" ("<<mtest.getArrListSize()<<")!!"<<endl;
                return CMD_EXEC_ERROR;
            }
            else if(firstRanp==0 && !doArray)
            {
                cerr<<"Error: Illegal option!!"<<" ("<<options[2]<<")"<<endl;
                return CMD_EXEC_ERROR;
            }
            else if(firstRanp==1 && !doArray)
            {
                cerr<<"Error: Illegal option!!"<<" ("<<options[0]<<")"<<endl;
                return CMD_EXEC_ERROR;
            }
            else if(doArray)
            {
                for(int i=0; i<ranNum; i++)
                {
                    int ranIdx = rnGen(mtest.getArrListSize());
                    mtest.deleteArr(ranIdx);
                }
                return CMD_EXEC_DONE;
            }
        }
        else
        {
            cerr<<"Error: Illegal option:"<<" ("<<options[0]<<")"<<endl;
            return CMD_EXEC_ERROR;
        }
    }
}

void
MTDeleteCmd::usage(ostream& os) const
{  
   os << "Usage: MTDelete <-Index (size_t objId) | "
      << "-Random (size_t numRandId)> [-Array]" << endl;
}

void
MTDeleteCmd::help() const
{  
   cout << setw(15) << left << "MTDelete: " 
        << "(memory test) delete objects" << endl;
}


//----------------------------------------------------------------------
//    MTPrint
//----------------------------------------------------------------------
CmdExecStatus
MTPrintCmd::exec(const string& option)
{
   // check option
   if (option.size())
      return CmdExec::errorOption(CMD_OPT_EXTRA, option);
   mtest.print();

   return CMD_EXEC_DONE;
}

void
MTPrintCmd::usage(ostream& os) const
{  
   os << "Usage: MTPrint" << endl;
}

void
MTPrintCmd::help() const
{  
   cout << setw(15) << left << "MTPrint: " 
        << "(memory test) print memory manager info" << endl;
}



/****************************************************************************
  FileName     [ cmdReader.cpp ]
  PackageName  [ cmd ]
  Synopsis     [ Define command line reader member functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2007-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/
#include <cassert>
#include <cstring>
#include "cmdParser.h"

using namespace std;

//----------------------------------------------------------------------
//    External funcitons
//----------------------------------------------------------------------
void mybeep();
char mygetc(istream&);
ParseChar getChar(istream&);


//----------------------------------------------------------------------
//    Member Function for class Parser
//----------------------------------------------------------------------
void
CmdParser::readCmd()
{
    if (_dofile.is_open()) {
       readCmdInt(_dofile);
       _dofile.close();
    }
    else
       readCmdInt(cin);
}

void
CmdParser::readCmdInt(istream& istr)
{
    resetBufAndPrintPrompt();

    while (1) {
       ParseChar pch = getChar(istr);
       if (pch == INPUT_END_KEY) break;
       switch (pch) {
          case LINE_BEGIN_KEY :
          case HOME_KEY       : moveBufPtr(_readBuf); break;
          case LINE_END_KEY   :
          case END_KEY        : moveBufPtr(_readBufEnd); break;
          case BACK_SPACE_KEY :
           {
               /* TODO */
               if(_readBufPtr == _readBuf)
               {
                   mybeep();
                   break;
               }
               else
               {
                   int lcnt=0;
                   for(int i=0; i<sizeof(_readBuf)/sizeof(char); i++)
                   {
                       if(_readBuf[i] != 0)
                       {
                           lcnt++;
                       }
                       else
                           break;
                   }
                   
                   if(lcnt > 1)
                   {
                       *(_readBufPtr-1) = 0;
                       
                       for(int i = 0; i < lcnt-1; i++)
                       {
                           if(&_readBuf[i] >= _readBufPtr-1)
                           {
                               _readBuf[i] = _readBuf[i+1];
                           }
                       }
                       _readBuf[lcnt-1] = 0;
                   }
                   
                   int nlcnt = lcnt-1;
                   _readBufEnd = _readBuf + nlcnt;
                   *_readBufEnd = 0;
                   moveBufPtr(_readBufPtr-1);
                   char* temp = _readBufPtr;
                   moveBufPtr(_readBufEnd);
                   cout<<" ";
                   cout<<'\b';
                   moveBufPtr(temp);
                   break;
               }
           }
          case DELETE_KEY     : deleteChar(); break;
          case NEWLINE_KEY    :
           {
               addHistory();
               cout << char(NEWLINE_KEY);
               resetBufAndPrintPrompt();
               break;
           }
          case ARROW_UP_KEY   : moveToHistory(_historyIdx - 1); break;
          case ARROW_DOWN_KEY : moveToHistory(_historyIdx + 1); break;
          case ARROW_RIGHT_KEY: /* TODO */ moveBufPtr(_readBufPtr+1); break;
          case ARROW_LEFT_KEY : /* TODO */ moveBufPtr(_readBufPtr-1); break;
          case PG_UP_KEY      : moveToHistory(_historyIdx - PG_OFFSET); break;
          case PG_DOWN_KEY    : moveToHistory(_historyIdx + PG_OFFSET); break;
          case TAB_KEY        :
           {
               /* TODO */
               int hindex;
               int lcnt=0;
               for(int i=0; i<sizeof(_readBuf)/sizeof(char); i++)
               {
                   if(_readBuf[i] != 0)
                   {
                       lcnt++;
                   }
                   else
                       break;
               }
               
               for(int i=0; i<READ_BUF_SIZE; i++)
               {
                   if(&_readBuf[i] == _readBufPtr)
                   {
                       hindex = i;
                       break;
                   }
               }
               
               int numtab = ((hindex/TAB_POSITION)+1)*TAB_POSITION;
               int move = numtab - hindex;
               char* temp = _readBufPtr;

               for(int i= lcnt -1; i>=0 ; i--)
               {
                   if(&_readBuf[i] >= _readBufPtr)
                   {
                       _readBuf[i+move] = _readBuf[i];
                   }
               }

               for(int i=0; i<move; i++)
               {
                   _readBuf[hindex+i] = ' ';

               }

               _readBufEnd = _readBufEnd + move;
               moveBufPtr(_readBufEnd);
               moveBufPtr(temp+move);
               
               break;
           }
          case INSERT_KEY     : // not yet supported; fall through to UNDEFINE
          case UNDEFINED_KEY:   mybeep(); break;
          default:  // printable character
             insertChar(char(pch)); break;
       }
       #ifdef TA_KB_SETTING
       taTestOnly();
       #endif
    }
}


// This function moves _readBufPtr to the "ptr" pointer
// It is used by left/right arrowkeys, home/end, etc.
//
// Suggested steps:
// 1. Make sure ptr is within [_readBuf, _readBufEnd].
//    If not, make a beep sound and return false. (DON'T MOVE)
// 2. Move the cursor to the left or right, depending on ptr
// 3. Update _readBufPtr accordingly. The content of the _readBuf[] will
//    not be changed
//
// [Note] This function can also be called by other member functions below
//        to move the _readBufPtr to proper position.
bool
CmdParser::moveBufPtr(char* const ptr)
{
    // TODO...
    if(ptr <= _readBufEnd && ptr >= _readBuf)
    {
        if(_readBufPtr > ptr)
        {
            for(int i=0; i<(_readBufPtr-ptr)/sizeof(char);i++)
            {
                cout << '\b';
            }
        }
        else
        {
            for(int i=0; i<(ptr-_readBufPtr)/sizeof(char); i++)
            {
                cout<<*(_readBufPtr+i);
            }
        }
        _readBufPtr = ptr;
        return true;
    }
    else
    {
        mybeep();
        return false;
    }
}


// [Notes]
// 1. Delete the char at _readBufPtr
// 2. mybeep() and return false if at _readBufEnd
// 3. Move the remaining string left for one character
// 4. The cursor should stay at the same position
// 5. Remember to update _readBufEnd accordingly.
// 6. Don't leave the tailing character.
// 7. Call "moveBufPtr(...)" if needed.
//
// For example,
//
// cmd> This is the command
//              ^                (^ is the cursor position)
//
// After calling deleteChar()---
//
// cmd> This is he command
//              ^
//
bool
CmdParser::deleteChar()
{
    // TODO...
    if(_readBufPtr == _readBufEnd)
    {
        mybeep();
        return false;
    }
    else
    {
        int lcnt=0;
        for(int i=0; i<sizeof(_readBuf)/sizeof(char); i++)
        {
            if(_readBuf[i] != 0)
            {
                lcnt++;
            }
            else
                break;
        }
        
        if(lcnt > 1)
        {
            *_readBufPtr = 0;
            
            for(int i = 0; i < lcnt-1; i++)
            {
                if(&_readBuf[i] >= _readBufPtr)
                {
                    _readBuf[i] = _readBuf[i+1];
                }
            }
            _readBuf[lcnt-1] = 0;
        }
        else
        {
            *_readBufPtr = 0;
        }

        int nlcnt = lcnt-1;
        _readBufEnd = _readBuf + nlcnt;
        *_readBufEnd = 0;
        char* temp = _readBufPtr;
        moveBufPtr(_readBufEnd);
        cout<<" ";
        cout<<'\b';
        moveBufPtr(temp);
        return true;
    }
}

// 1. Insert character 'ch' for "repeat" times at _readBufPtr
// 2. Move the remaining string right for "repeat" characters
// 3. The cursor should move right for "repeats" positions afterwards
// 4. Default value for "repeat" is 1. You should assert that (repeat >= 1).
//
// For example,
//
// cmd> This is the command
//              ^                (^ is the cursor position)
//
// After calling insertChar('k', 3) ---
//
// cmd> This is kkkthe command
//                 ^
//
void
CmdParser::insertChar(char ch, int repeat)
{
    // TODO...
    char temp[READ_BUF_SIZE];
    int tempsize = 0;
    char* tempptr;
    
    for(int i=0; i<repeat; i++)
    {
        temp[tempsize] = *_readBufPtr;
        *_readBufPtr = ch;
        cout<<ch;
        _readBufPtr++;
        tempsize++;
    }
    
    int lcnt=0;
    
    for(int i=0; i<sizeof(_readBuf)/sizeof(char); i++)
    {
        if(_readBuf[i] != 0)
        {
            lcnt++;
        }
        else
            break;
    }
    
    for(int i=lcnt-1; i>=0; i--)
    {
        if(&_readBuf[i] >= _readBufPtr && &_readBuf[i] < _readBufEnd)
        {
            _readBuf[i+repeat] = _readBuf[i];
        }
    }
    tempptr = _readBufPtr;
    
    for(int i=0; i<sizeof(_readBuf)/sizeof(char); i++)
    {
        if(&_readBuf[i] >= _readBufPtr)
        {
            int k = 0;
            while(k < tempsize)
            {
                _readBuf[i] = temp[k];
                k++;
            }
            break;
        }
    }
    
    _readBufEnd = _readBufEnd+repeat;
    moveBufPtr(_readBufEnd);
    moveBufPtr(tempptr);
    
    assert(repeat >= 1);
    

}

// 1. Delete the line that is currently shown on the screen
// 2. Reset _readBufPtr and _readBufEnd to _readBuf
// 3. Make sure *_readBufEnd = 0
//
// For example,
//
// cmd> This is the command
//              ^                (^ is the cursor position)
//
// After calling deleteLine() ---
//
// cmd>
//      ^
//
void
CmdParser::deleteLine()
{
    // TODO...
    int lcnt=0;
    for(int i=0; i<sizeof(_readBuf)/sizeof(char); i++)
    {
        if(_readBuf[i] != 0)
        {
            lcnt++;
        }
        else
            break;
    }
    moveBufPtr(_readBuf);
    for(int i=0; i<lcnt; i++)
    {
        _readBuf[i] = 0;
        cout<<" ";
    }
    for(int i=0; i<lcnt; i++)
    {
        cout<<'\b';
    }
    moveBufPtr(_readBuf);
    _readBufEnd = _readBuf;
    *_readBufPtr = 0;
}


// This functions moves _historyIdx to index and display _history[index]
// on the screen.
//
// Need to consider:
// If moving up... (i.e. index < _historyIdx)
// 1. If already at top (i.e. _historyIdx == 0), beep and do nothing.
// 2. If at bottom, temporarily record _readBuf to history.
//    (Do not remove spaces, and set _tempCmdStored to "true")
// 3. If index < 0, let index = 0.
//
// If moving down... (i.e. index > _historyIdx)
// 1. If already at bottom, beep and do nothing
// 2. If index >= _history.size(), let index = _history.size() - 1.
//
// Assign _historyIdx to index at the end.
//
// [Note] index should not = _historyIdx
//
void
CmdParser::moveToHistory(int index)
{
    // TODO...
    for(int i=0; i<1; i++)
    {
        if(index < _historyIdx)
        {
            if (_historyIdx == 0)
            {
                mybeep();
                break;
            }
            if(index < 0)
            {
                index = 0;
            }
            if(_historyIdx == _history.size())
            {
                string currentline(_readBuf);
                _history.push_back(currentline);
                _tempCmdStored = true;
            }
        }
        
        if(index > _historyIdx)
        {
            
            if(_historyIdx == _history.size())
            {
                mybeep();
                break;
            }
            
            if(index >= _history.size())
            {
                index = _history.size() -1;
                
            }
            if(index == _history.size()-1 && _tempCmdStored == true)
            {
                _historyIdx = index;
                retrieveHistory();
                _history.pop_back();
                _tempCmdStored = false;
                break;
            }
        }
        
        _historyIdx = index;
        retrieveHistory();
    }


}


// This function adds the string in _readBuf to the _history.
// The size of _history may or may not change. Depending on whether 
// there is a temp history string.
//
// 1. Remove ' ' at the beginning and end of _readBuf
// 2. If not a null string, add string to _history.
//    Be sure you are adding to the right entry of _history.
// 3. If it is a null string, don't add anything to _history.
// 4. Make sure to clean up "temp recorded string" (added earlier by up/pgUp,
//    and reset _tempCmdStored to false
// 5. Reset _historyIdx to _history.size() // for future insertion
//
void
CmdParser::addHistory()
{
    // TODO...
 
        string currentline(_readBuf);
        int size = currentline.length();
        for(int j=0; j<size; j++)
        {
            for(int i=0; i<=j; i++)
            {
                
                if(currentline[0] == ' ')
                {
                    if(currentline[i] == ' ' && currentline[i+1] == ' ')
                    {
                        currentline.erase(currentline.begin()+ i);
                    }
                    
                    else if(currentline[0] == ' ')
                    {
                        currentline.erase(currentline.begin());
                    }
                }
            }
        }
        int size2 = currentline.length();
        for(int j=size2-1; j>=0; j--)
        {
            for(int i=size2-1; i>=j; i--)
            {
                
                if(currentline[currentline.length()-1] == ' ')
                {
                    if(currentline[i] == ' ' && currentline[i-1] == ' ')
                    {
                        currentline.erase(currentline.begin()+(i));
                    }
                    
                    else if(currentline[currentline.length()-1] == ' ')
                    {
                        currentline.erase(currentline.end()-1);
                    }
                }
            }
        }
        
    if(_tempCmdStored == true)
    {
        _history.pop_back();
        _tempCmdStored = false;

    }
    
    if(!currentline.empty())
    {
        _history.push_back(currentline);
    }
    _historyIdx = _history.size();
    
}


// 1. Replace current line with _history[_historyIdx] on the screen
// 2. Set _readBufPtr and _readBufEnd to end of line
//
// [Note] Do not change _history.size().
//
void
CmdParser::retrieveHistory()
{
    deleteLine();
    strcpy(_readBuf, _history[_historyIdx].c_str());
    cout << _readBuf;
    _readBufPtr = _readBufEnd = _readBuf + _history[_historyIdx].size();
}
